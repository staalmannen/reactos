/*
 * PROJECT:         ReactOS Kernel
 * COPYRIGHT:       GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/cm/registry.c
 * PURPOSE:         Registry functions
 *
 * PROGRAMMERS:     Hartmut Birr
 *                  Alex Ionescu
 *                  Rex Jolliff
 *                  Eric Kohl
 *                  Matt Pyne
 *                  Jean Michault
 *                  Art Yerkes
 */

#include <ntoskrnl.h>
#define NDEBUG
#include <internal/debug.h>

#include "cm.h"

#if defined (ALLOC_PRAGMA)
#pragma alloc_text(INIT, CmInitSystem1)
#endif

/* GLOBALS ******************************************************************/

extern BOOLEAN ExpInTextModeSetup;

POBJECT_TYPE  CmpKeyObjectType = NULL;
PCMHIVE  CmiVolatileHive = NULL;

LIST_ENTRY CmpHiveListHead;

ERESOURCE CmpRegistryLock;

LIST_ENTRY CmiKeyObjectListHead;
LIST_ENTRY CmiConnectedHiveList;

extern LIST_ENTRY CmiCallbackHead;
extern FAST_MUTEX CmiCallbackLock;

PVOID
NTAPI
CmpRosGetHardwareHive(OUT PULONG Length)
{
    PLIST_ENTRY ListHead, NextEntry;
    PMEMORY_ALLOCATION_DESCRIPTOR MdBlock = NULL;

    /* Loop the memory descriptors */
    ListHead = &KeLoaderBlock->MemoryDescriptorListHead;
    NextEntry = ListHead->Flink;
    while (NextEntry != ListHead)
    {
        /* Get the current block */
        MdBlock = CONTAINING_RECORD(NextEntry,
                                    MEMORY_ALLOCATION_DESCRIPTOR,
                                    ListEntry);

        /* Check if this is an registry block */
        if (MdBlock->MemoryType == LoaderRegistryData)
        {
            /* Check if it's not the SYSTEM hive that we already initialized */
            if ((MdBlock->BasePage) !=
                (((ULONG_PTR)KeLoaderBlock->RegistryBase &~ KSEG0_BASE) >> PAGE_SHIFT))
            {
                /* Hardware hive break out */
                break;
            }
        }

        /* Go to the next block */
        NextEntry = MdBlock->ListEntry.Flink;
    }

    /* We need a hardware hive */
    ASSERT(MdBlock);
    *Length = MdBlock->PageCount << PAGE_SHIFT;
    return (PVOID)((MdBlock->BasePage << PAGE_SHIFT) | KSEG0_BASE);
}

/* Precondition: Must not hold the hive lock CmpRegistryLock */
VOID
NTAPI
EnlistKeyBodyWithKeyObject(IN PKEY_OBJECT KeyObject,
                           IN ULONG Flags)
{
    /* Acquire hive lock */
    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&CmpRegistryLock, TRUE);

    /* Insert it into the global list (we don't have KCBs here) */
    InsertTailList(&CmiKeyObjectListHead, &KeyObject->KeyBodyList);

    /* Release hive lock */
    ExReleaseResourceLite(&CmpRegistryLock);
    KeLeaveCriticalRegion();
}

NTSTATUS
NTAPI
CmpLinkHiveToMaster(IN PUNICODE_STRING LinkName,
                    IN HANDLE RootDirectory,
                    IN PCMHIVE RegistryHive,
                    IN BOOLEAN Allocate,
                    IN PSECURITY_DESCRIPTOR SecurityDescriptor)
{
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING RemainingPath;
    PKEY_OBJECT ParentKey;
    PKEY_OBJECT NewKey;
    NTSTATUS Status;
    UNICODE_STRING ObjectName;
    OBJECT_CREATE_INFORMATION ObjectCreateInfo;
    CM_PARSE_CONTEXT ParseContext = {0};
    PAGED_CODE();

    /* Setup the object attributes */
    InitializeObjectAttributes(&ObjectAttributes,
                               LinkName,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               RootDirectory,
                               SecurityDescriptor);
    
    /* Setup the parse context */
    ParseContext.CreateLink = TRUE;
    ParseContext.CreateOperation = TRUE;
    ParseContext.ChildHive.KeyHive = &RegistryHive->Hive;
    
    /* Check if we have a root keycell or if we need to create it */
    if (Allocate)
    {
        /* Create it */
        ParseContext.ChildHive.KeyCell = HCELL_NIL;
    }
    else
    {
        /* We have one */
        ParseContext.ChildHive.KeyCell = RegistryHive->Hive.BaseBlock->RootCell;   
    }
    
    /* Capture all the info */
    Status = ObpCaptureObjectAttributes(&ObjectAttributes,
                                        KernelMode,
                                        FALSE,
                                        &ObjectCreateInfo,
                                        &ObjectName);
    if (!NT_SUCCESS(Status)) return Status;
    
    /* Do the parse */
    Status = CmFindObject(&ObjectCreateInfo,
                          &ObjectName,
                          (PVOID*)&ParentKey,
                          &RemainingPath,
                          CmpKeyObjectType,
                          NULL,
                          NULL);
    
    /* Let go of captured attributes and name */
    ObpReleaseCapturedAttributes(&ObjectCreateInfo);   
    if (ObjectName.Buffer) ObpFreeObjectNameBuffer(&ObjectName);
    
    /* Get out of here if we failed */
    if (!NT_SUCCESS(Status)) return Status;
    
    /* Scan for no name */
    if (!(RemainingPath.Length) || (RemainingPath.Buffer[0] == UNICODE_NULL))
    {
        /* Fail */
        ObDereferenceObject(ParentKey);
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }
    
    /* Scan for leading backslash */
    while ((RemainingPath.Length) &&
           (*RemainingPath.Buffer == OBJ_NAME_PATH_SEPARATOR))
    {
        /* Ignore it */
        RemainingPath.Length -= sizeof(WCHAR);
        RemainingPath.MaximumLength -= sizeof(WCHAR);
        RemainingPath.Buffer++;
    }
    
    /* Create the link node */
    Status = CmpCreateLinkNode(ParentKey->KeyControlBlock->KeyHive,
                               ParentKey->KeyControlBlock->KeyCell,
                               NULL,
                               RemainingPath,
                               KernelMode,
                               0,
                               &ParseContext,
                               ParentKey->KeyControlBlock,
                               (PVOID*)&NewKey);
    if (!NT_SUCCESS(Status))
    {
        /* Failed */
        DPRINT1("CmpLinkHiveToMaster failed: %lx\n", Status);
        ObDereferenceObject(ParentKey);
        return Status;
    }
    
    /* Free the create information */
    ObpFreeAndReleaseCapturedAttributes(OBJECT_TO_OBJECT_HEADER(NewKey)->ObjectCreateInfo);
    OBJECT_TO_OBJECT_HEADER(NewKey)->ObjectCreateInfo = NULL;
    
    /* Mark the hive as clean */
    RegistryHive->Hive.DirtyFlag = FALSE;
    
    /* Update KCB information */
    NewKey->KeyControlBlock->KeyCell = RegistryHive->Hive.BaseBlock->RootCell;
    NewKey->KeyControlBlock->KeyHive = &RegistryHive->Hive;
    
    /* Build the key name */
    RtlDuplicateUnicodeString(RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE, 
                              &RemainingPath,
                              &NewKey->Name);
    
    /* Reference the new key */
    ObReferenceObject(NewKey);
    
    /* Link this key to the parent */
    CmiAddKeyToList(ParentKey, NewKey);
    return STATUS_SUCCESS;    
}

VOID
NTAPI
CmShutdownRegistry(VOID)
{
    CmShutdownSystem();
}

/* EOF */
