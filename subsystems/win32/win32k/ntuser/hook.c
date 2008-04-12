/*
 *  ReactOS W32 Subsystem
 *  Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 ReactOS Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          ReactOS kernel
 * PURPOSE:          Window hooks
 * FILE:             subsys/win32k/ntuser/hook.c
 * PROGRAMER:        Casper S. Hornstrup (chorns@users.sourceforge.net)
 * REVISION HISTORY:
 *       06-06-2001  CSH  Created
 * NOTE:             Most of this code was adapted from Wine,
 *                   Copyright (C) 2002 Alexandre Julliard
 */

#include <w32k.h>

#define NDEBUG
#include <debug.h>

#define HOOKID_TO_INDEX(HookId) (HookId - WH_MINHOOK)
#define HOOKID_TO_FLAG(HookId) (1 << ((HookId) + 1))

static PHOOKTABLE GlobalHooks;
DWORD Bogus_SrvEventActivity = 0;


/* PRIVATE FUNCTIONS *********************************************************/

static
DWORD
FASTCALL
GetMaskFromEvent(DWORD Event)
{
  DWORD Ret = 0;

  if ( Event > EVENT_OBJECT_STATECHANGE )
  {
    if ( Event == EVENT_OBJECT_LOCATIONCHANGE ) return SRV_EVENT_LOCATIONCHANGE;
    if ( Event == EVENT_OBJECT_NAMECHANGE )     return SRV_EVENT_NAMECHANGE;
    if ( Event == EVENT_OBJECT_VALUECHANGE )    return SRV_EVENT_VALUECHANGE;
    return SRV_EVENT_CREATE;
  }

  if ( Event == EVENT_OBJECT_STATECHANGE ) return SRV_EVENT_STATECHANGE;

  Ret = SRV_EVENT_RUNNING;

  if ( Event < EVENT_SYSTEM_MENUSTART )    return SRV_EVENT_CREATE;

  if ( Event <= EVENT_SYSTEM_MENUPOPUPEND )
  {
    Ret = SRV_EVENT_MENU;
  }
  else
  {
    if ( Event <= EVENT_CONSOLE_CARET-1 )         return SRV_EVENT_CREATE;
    if ( Event <= EVENT_CONSOLE_END_APPLICATION ) return SRV_EVENT_END_APPLICATION;
    if ( Event != EVENT_OBJECT_FOCUS )            return SRV_EVENT_CREATE;
  }
  return Ret;
}

/* create a new hook table */
static PHOOKTABLE
IntAllocHookTable(void)
{
   PHOOKTABLE Table;
   UINT i;

   Table = ExAllocatePoolWithTag(PagedPool, sizeof(HOOKTABLE), TAG_HOOK);
   if (NULL != Table)
   {
      for (i = 0; i < NB_HOOKS; i++)
      {
         InitializeListHead(&Table->Hooks[i]);
         Table->Counts[i] = 0;
      }
   }

   return Table;
}


PHOOK FASTCALL IntGetHookObject(HHOOK hHook)
{
   PHOOK Hook;

   if (!hHook)
   {
      SetLastWin32Error(ERROR_INVALID_HOOK_HANDLE);
      return NULL;
   }

   Hook = (PHOOK)UserGetObject(gHandleTable, hHook, otHook);
   if (!Hook)
   {
      SetLastWin32Error(ERROR_INVALID_HOOK_HANDLE);
      return NULL;
   }

   ASSERT(USER_BODY_TO_HEADER(Hook)->RefCount >= 0);

   USER_BODY_TO_HEADER(Hook)->RefCount++;

   return Hook;
}



/* create a new hook and add it to the specified table */
static PHOOK
IntAddHook(PETHREAD Thread, int HookId, BOOLEAN Global, PWINSTATION_OBJECT WinStaObj)
{
   PW32THREAD W32Thread;
   PHOOK Hook;
   PHOOKTABLE Table = Global ? GlobalHooks : MsqGetHooks(((PW32THREAD)Thread->Tcb.Win32Thread)->MessageQueue);
   HANDLE Handle;

   if (NULL == Table)
   {
      Table = IntAllocHookTable();
      if (NULL == Table)
      {
         return NULL;
      }
      if (Global)
      {
         GlobalHooks = Table;
      }
      else
      {
         MsqSetHooks(((PW32THREAD)Thread->Tcb.Win32Thread)->MessageQueue, Table);
      }
   }

   Hook = UserCreateObject(gHandleTable, &Handle, otHook, sizeof(HOOK));
   if (NULL == Hook)
   {
      return NULL;
   }

   Hook->Self = Handle;
   Hook->Thread = Thread;
   Hook->HookId = HookId;

   W32Thread = ((PW32THREAD)Thread->Tcb.Win32Thread);
   ASSERT(W32Thread != NULL);
   W32Thread->Hooks |= HOOKID_TO_FLAG(HookId);
   if (W32Thread->ThreadInfo != NULL)
       W32Thread->ThreadInfo->Hooks = W32Thread->Hooks;

   RtlInitUnicodeString(&Hook->ModuleName, NULL);

   InsertHeadList(&Table->Hooks[HOOKID_TO_INDEX(HookId)], &Hook->Chain);

   return Hook;
}

/* get the hook table that a given hook belongs to */
static PHOOKTABLE FASTCALL
IntGetTable(PHOOK Hook)
{
   if (NULL == Hook->Thread || WH_KEYBOARD_LL == Hook->HookId ||
         WH_MOUSE_LL == Hook->HookId)
   {
      return GlobalHooks;
   }

   return MsqGetHooks(((PW32THREAD)Hook->Thread->Tcb.Win32Thread)->MessageQueue);
}

/* get the first hook in the chain */
static PHOOK FASTCALL
IntGetFirstHook(PHOOKTABLE Table, int HookId)
{
   PLIST_ENTRY Elem = Table->Hooks[HOOKID_TO_INDEX(HookId)].Flink;
   return Elem == &Table->Hooks[HOOKID_TO_INDEX(HookId)]
          ? NULL : CONTAINING_RECORD(Elem, HOOK, Chain);
}

/* find the first non-deleted hook in the chain */
static PHOOK FASTCALL
IntGetFirstValidHook(PHOOKTABLE Table, int HookId)
{
   PHOOK Hook;
   PLIST_ENTRY Elem;

   Hook = IntGetFirstHook(Table, HookId);
   while (NULL != Hook && NULL == Hook->Proc)
   {
      Elem = Hook->Chain.Flink;
      Hook = (Elem == &Table->Hooks[HOOKID_TO_INDEX(HookId)]
              ? NULL : CONTAINING_RECORD(Elem, HOOK, Chain));
   }

   return Hook;
}

/* find the next hook in the chain, skipping the deleted ones */
static PHOOK FASTCALL
IntGetNextHook(PHOOK Hook)
{
   PHOOKTABLE Table = IntGetTable(Hook);
   int HookId = Hook->HookId;
   PLIST_ENTRY Elem;

   Elem = Hook->Chain.Flink;
   while (Elem != &Table->Hooks[HOOKID_TO_INDEX(HookId)])
   {
      Hook = CONTAINING_RECORD(Elem, HOOK, Chain);
      if (NULL != Hook->Proc)
      {
         return Hook;
      }
   }

   if (NULL != GlobalHooks && Table != GlobalHooks)  /* now search through the global table */
   {
      return IntGetFirstValidHook(GlobalHooks, HookId);
   }

   return NULL;
}

/* free a hook, removing it from its chain */
static VOID FASTCALL
IntFreeHook(PHOOKTABLE Table, PHOOK Hook, PWINSTATION_OBJECT WinStaObj)
{
   RemoveEntryList(&Hook->Chain);
   RtlFreeUnicodeString(&Hook->ModuleName);

   /* Dereference thread if required */
   if (Hook->Flags & HOOK_THREAD_REFERENCED)
   {
      ObDereferenceObject(Hook->Thread);
   }

   /* Close handle */
   UserDeleteObject(Hook->Self, otHook);
}

/* remove a hook, freeing it if the chain is not in use */
static VOID
IntRemoveHook(PHOOK Hook, PWINSTATION_OBJECT WinStaObj, BOOL TableAlreadyLocked)
{
   PW32THREAD W32Thread;
   PHOOKTABLE Table = IntGetTable(Hook);

   ASSERT(NULL != Table);
   if (NULL == Table)
   {
      return;
   }

   W32Thread = ((PW32THREAD)Hook->Thread->Tcb.Win32Thread);
   ASSERT(W32Thread != NULL);
   W32Thread->Hooks &= ~HOOKID_TO_FLAG(Hook->HookId);
   if (W32Thread->ThreadInfo != NULL)
       W32Thread->ThreadInfo->Hooks = W32Thread->Hooks;

   if (0 != Table->Counts[HOOKID_TO_INDEX(Hook->HookId)])
   {
      Hook->Proc = NULL; /* chain is in use, just mark it and return */
   }
   else
   {
      IntFreeHook(Table, Hook, WinStaObj);
   }
}

/* release a hook chain, removing deleted hooks if the use count drops to 0 */
static VOID FASTCALL
IntReleaseHookChain(PHOOKTABLE Table, int HookId, PWINSTATION_OBJECT WinStaObj)
{
   PLIST_ENTRY Elem;
   PHOOK HookObj;

   if (NULL == Table)
   {
      return;
   }

   /* use count shouldn't already be 0 */
   ASSERT(0 != Table->Counts[HOOKID_TO_INDEX(HookId)]);
   if (0 == Table->Counts[HOOKID_TO_INDEX(HookId)])
   {
      return;
   }
   if (0 == --Table->Counts[HOOKID_TO_INDEX(HookId)])
   {
      Elem = Table->Hooks[HOOKID_TO_INDEX(HookId)].Flink;
      while (Elem != &Table->Hooks[HOOKID_TO_INDEX(HookId)])
      {
         HookObj = CONTAINING_RECORD(Elem, HOOK, Chain);
         Elem = Elem->Flink;
         if (NULL == HookObj->Proc)
         {
            IntFreeHook(Table, HookObj, WinStaObj);
         }
      }
   }
}

static LRESULT FASTCALL
IntCallLowLevelHook(INT HookId, INT Code, WPARAM wParam, LPARAM lParam, PHOOK Hook)
{
   NTSTATUS Status;
   ULONG_PTR uResult;

   /* FIXME should get timeout from
    * HKEY_CURRENT_USER\Control Panel\Desktop\LowLevelHooksTimeout */
   Status = co_MsqSendMessage(((PW32THREAD)Hook->Thread->Tcb.Win32Thread)->MessageQueue, (HWND) Code, HookId,
                              wParam, lParam, 5000, TRUE, TRUE, &uResult);

   return NT_SUCCESS(Status) ? uResult : 0;
}

LRESULT FASTCALL
co_HOOK_CallHooks(INT HookId, INT Code, WPARAM wParam, LPARAM lParam)
{
   PHOOK Hook;
   PW32THREAD Win32Thread;
   PHOOKTABLE Table;
   LRESULT Result;
   PWINSTATION_OBJECT WinStaObj;
   NTSTATUS Status;

   ASSERT(WH_MINHOOK <= HookId && HookId <= WH_MAXHOOK);

   Win32Thread = PsGetCurrentThreadWin32Thread();
   if (NULL == Win32Thread)
   {
      Table = NULL;
   }
   else
   {
      Table = MsqGetHooks(Win32Thread->MessageQueue);
   }

   if (NULL == Table || ! (Hook = IntGetFirstValidHook(Table, HookId)))
   {
      /* try global table */
      Table = GlobalHooks;
      if (NULL == Table || ! (Hook = IntGetFirstValidHook(Table, HookId)))
      {
         return 0;  /* no hook set */
      }
   }

   if (Hook->Thread != PsGetCurrentThread()
         && (WH_KEYBOARD_LL == HookId || WH_MOUSE_LL == HookId))
   {
      DPRINT("Calling hook in owning thread\n");
      return IntCallLowLevelHook(HookId, Code, wParam, lParam, Hook);
   }

   if (Hook->Thread != PsGetCurrentThread())
   {
      DPRINT1("Calling hooks in other threads not implemented yet");
      return 0;
   }

   Table->Counts[HOOKID_TO_INDEX(HookId)]++;
   if (Table != GlobalHooks && GlobalHooks != NULL)
   {
      GlobalHooks->Counts[HOOKID_TO_INDEX(HookId)]++;
   }

   Result = co_IntCallHookProc(HookId, Code, wParam, lParam, Hook->Proc,
                               Hook->Ansi, &Hook->ModuleName);

   Status = IntValidateWindowStationHandle(PsGetCurrentProcess()->Win32WindowStation,
                                           KernelMode,
                                           0,
                                           &WinStaObj);

   if (! NT_SUCCESS(Status))
   {
      DPRINT1("Invalid window station????\n");
   }
   else
   {
      IntReleaseHookChain(MsqGetHooks(PsGetCurrentThreadWin32Thread()->MessageQueue), HookId, WinStaObj);
      IntReleaseHookChain(GlobalHooks, HookId, WinStaObj);
      ObDereferenceObject(WinStaObj);
   }

   return Result;
}

VOID FASTCALL
HOOK_DestroyThreadHooks(PETHREAD Thread)
{
   int HookId;
   PLIST_ENTRY Elem;
   PHOOK HookObj;
   PWINSTATION_OBJECT WinStaObj;
   NTSTATUS Status;

   if (NULL != GlobalHooks)
   {
      Status = IntValidateWindowStationHandle(PsGetCurrentProcess()->Win32WindowStation,
                                              KernelMode,
                                              0,
                                              &WinStaObj);

      if (! NT_SUCCESS(Status))
      {
         DPRINT1("Invalid window station????\n");
         return;
      }

      for (HookId = WH_MINHOOK; HookId <= WH_MAXHOOK; HookId++)
      {
         /* only low-level keyboard/mouse global hooks can be owned by a thread */
         switch(HookId)
         {
            case WH_KEYBOARD_LL:
            case WH_MOUSE_LL:
               Elem = GlobalHooks->Hooks[HOOKID_TO_INDEX(HookId)].Flink;
               while (Elem != &GlobalHooks->Hooks[HOOKID_TO_INDEX(HookId)])
               {
                  HookObj = CONTAINING_RECORD(Elem, HOOK, Chain);
                  Elem = Elem->Flink;
                  if (HookObj->Thread == Thread)
                  {
                     IntRemoveHook(HookObj, WinStaObj, TRUE);
                  }
               }
               break;
         }
      }

      ObDereferenceObject(WinStaObj);
   }
}

LRESULT
STDCALL
NtUserCallNextHookEx(
   HHOOK Hook,
   int Code,
   WPARAM wParam,
   LPARAM lParam)
{
   PHOOK HookObj, NextObj;
   PWINSTATION_OBJECT WinStaObj;
   NTSTATUS Status;
   DECLARE_RETURN(LRESULT);

   DPRINT("Enter NtUserCallNextHookEx\n");
   UserEnterExclusive();

   Status = IntValidateWindowStationHandle(PsGetCurrentProcess()->Win32WindowStation,
                                           KernelMode,
                                           0,
                                           &WinStaObj);

   if (! NT_SUCCESS(Status))
   {
      SetLastNtError(Status);
      RETURN( FALSE);
   }

   //Status = UserReferenceObjectByHandle(gHandleTable, Hook,
   //                             otHookProc, (PVOID *) &HookObj);
   ObDereferenceObject(WinStaObj);

   //  if (! NT_SUCCESS(Status))
   //    {
   //      DPRINT1("Invalid handle passed to NtUserCallNextHookEx\n");
   //      SetLastNtError(Status);
   //      RETURN( 0);
   //    }

   if (!(HookObj = IntGetHookObject(Hook)))
   {
      RETURN(0);
   }

   ASSERT(Hook == HookObj->Self);

   if (NULL != HookObj->Thread && (HookObj->Thread != PsGetCurrentThread()))
   {
      DPRINT1("Thread mismatch\n");
      UserDereferenceObject(HookObj);
      SetLastWin32Error(ERROR_INVALID_HANDLE);
      RETURN( 0);
   }

   NextObj = IntGetNextHook(HookObj);
   UserDereferenceObject(HookObj);
   if (NULL != NextObj)
   {
      DPRINT1("Calling next hook not implemented\n");
      UNIMPLEMENTED
      SetLastWin32Error(ERROR_NOT_SUPPORTED);
      RETURN( 0);
   }

   RETURN( 0);

CLEANUP:
   DPRINT("Leave NtUserCallNextHookEx, ret=%i\n",_ret_);
   UserLeave();
   END_CLEANUP;
}

HHOOK
STDCALL
NtUserSetWindowsHookAW(
   int idHook, 
   HOOKPROC lpfn,
   BOOL Ansi)
{
   UNICODE_STRING USModuleName;
   RtlInitUnicodeString(&USModuleName, NULL);
   return NtUserSetWindowsHookEx(NULL, &USModuleName, 0, idHook, lpfn, Ansi);
}

HHOOK
STDCALL
NtUserSetWindowsHookEx(
   HINSTANCE Mod,
   PUNICODE_STRING UnsafeModuleName,
   DWORD ThreadId,
   int HookId,
   HOOKPROC HookProc,
   BOOL Ansi)
{
   PWINSTATION_OBJECT WinStaObj;
   BOOLEAN Global;
   PETHREAD Thread;
   PHOOK Hook;
   UNICODE_STRING ModuleName;
   NTSTATUS Status;
   HHOOK Handle;
   DECLARE_RETURN(HHOOK);

   DPRINT("Enter NtUserSetWindowsHookEx\n");
   UserEnterExclusive();

   if (HookId < WH_MINHOOK || WH_MAXHOOK < HookId || NULL == HookProc)
   {
      SetLastWin32Error(ERROR_INVALID_PARAMETER);
      RETURN( NULL);
   }

   if (ThreadId)  /* thread-local hook */
   {
      if (HookId == WH_JOURNALRECORD ||
            HookId == WH_JOURNALPLAYBACK ||
            HookId == WH_KEYBOARD_LL ||
            HookId == WH_MOUSE_LL ||
            HookId == WH_SYSMSGFILTER)
      {
         /* these can only be global */
         SetLastWin32Error(ERROR_INVALID_PARAMETER);
         RETURN( NULL);
      }
      Mod = NULL;
      Global = FALSE;
      if (! NT_SUCCESS(PsLookupThreadByThreadId((HANDLE) ThreadId, &Thread)))
      {
         DPRINT1("Invalid thread id 0x%x\n", ThreadId);
         SetLastWin32Error(ERROR_INVALID_PARAMETER);
         RETURN( NULL);
      }
      if (Thread->ThreadsProcess != PsGetCurrentProcess())
      {
         ObDereferenceObject(Thread);
         DPRINT1("Can't specify thread belonging to another process\n");
         SetLastWin32Error(ERROR_INVALID_PARAMETER);
         RETURN( NULL);
      }
   }
   else  /* system-global hook */
   {
      if (HookId == WH_KEYBOARD_LL || HookId == WH_MOUSE_LL)
      {
         Mod = NULL;
         Thread = PsGetCurrentThread();
         Status = ObReferenceObjectByPointer(Thread,
                                             THREAD_ALL_ACCESS,
                                             PsThreadType,
                                             KernelMode);

         if (! NT_SUCCESS(Status))
         {
            SetLastNtError(Status);
            RETURN( (HANDLE) NULL);
         }
      }
      else if (NULL ==  Mod)
      {
         SetLastWin32Error(ERROR_INVALID_PARAMETER);
         RETURN( NULL);
      }
      else
      {
         Thread = NULL;
      }
      Global = TRUE;
   }

   /* We only (partially) support local WH_CBT hooks and
    * WH_KEYBOARD_LL/WH_MOUSE_LL hooks for now */
   if ((WH_CBT != HookId || Global)
         && WH_KEYBOARD_LL != HookId && WH_MOUSE_LL != HookId)
   {
#if 0 /* Removed to get winEmbed working again */
      UNIMPLEMENTED
#else
      DPRINT1("Not implemented: HookId %d Global %s\n", HookId, Global ? "TRUE" : "FALSE");
#endif

      if (NULL != Thread)
      {
         ObDereferenceObject(Thread);
      }
      SetLastWin32Error(ERROR_NOT_SUPPORTED);
      RETURN( NULL);
   }

   Status = IntValidateWindowStationHandle(PsGetCurrentProcess()->Win32WindowStation,
                                           KernelMode,
                                           0,
                                           &WinStaObj);

   if (! NT_SUCCESS(Status))
   {
      if (NULL != Thread)
      {
         ObDereferenceObject(Thread);
      }
      SetLastNtError(Status);
      RETURN( (HANDLE) NULL);
   }

   Hook = IntAddHook(Thread, HookId, Global, WinStaObj);
   if (NULL == Hook)
   {
      if (NULL != Thread)
      {
         ObDereferenceObject(Thread);
      }
      ObDereferenceObject(WinStaObj);
      RETURN( NULL);
   }

   if (NULL != Thread)
   {
      Hook->Flags |= HOOK_THREAD_REFERENCED;
   }

   if (NULL != Mod)
   {
      Status = MmCopyFromCaller(&ModuleName, UnsafeModuleName, sizeof(UNICODE_STRING));
      if (! NT_SUCCESS(Status))
      {
         UserDereferenceObject(Hook);
         IntRemoveHook(Hook, WinStaObj, FALSE);
         if (NULL != Thread)
         {
            ObDereferenceObject(Thread);
         }
         ObDereferenceObject(WinStaObj);
         SetLastNtError(Status);
         RETURN( NULL);
      }
      Hook->ModuleName.Buffer = ExAllocatePoolWithTag(PagedPool,
                                ModuleName.MaximumLength,
                                TAG_HOOK);
      if (NULL == Hook->ModuleName.Buffer)
      {
         UserDereferenceObject(Hook);
         IntRemoveHook(Hook, WinStaObj, FALSE);
         if (NULL != Thread)
         {
            ObDereferenceObject(Thread);
         }
         ObDereferenceObject(WinStaObj);
         SetLastWin32Error(ERROR_NOT_ENOUGH_MEMORY);
         RETURN( NULL);
      }
      Hook->ModuleName.MaximumLength = ModuleName.MaximumLength;
      Status = MmCopyFromCaller(Hook->ModuleName.Buffer,
                                ModuleName.Buffer,
                                ModuleName.MaximumLength);
      if (! NT_SUCCESS(Status))
      {
	     ExFreePool(Hook->ModuleName.Buffer);
         UserDereferenceObject(Hook);
         IntRemoveHook(Hook, WinStaObj, FALSE);
         if (NULL != Thread)
         {
            ObDereferenceObject(Thread);
         }
         ObDereferenceObject(WinStaObj);
         SetLastNtError(Status);
         RETURN( NULL);
      }
      Hook->ModuleName.Length = ModuleName.Length;
   }

   Hook->Proc = HookProc;
   Hook->Ansi = Ansi;
   Handle = Hook->Self;

   UserDereferenceObject(Hook);
   ObDereferenceObject(WinStaObj);

   RETURN( Handle);

CLEANUP:
   DPRINT("Leave NtUserSetWindowsHookEx, ret=%i\n",_ret_);
   UserLeave();
   END_CLEANUP;
}

HWINEVENTHOOK
STDCALL
NtUserSetWinEventHook(
   UINT eventMin,
   UINT eventMax,
   HMODULE hmodWinEventProc,
   PUNICODE_STRING puString,
   WINEVENTPROC lpfnWinEventProc,
   DWORD idProcess,
   DWORD idThread,
   UINT dwflags)
{

   Bogus_SrvEventActivity |= GetMaskFromEvent(eventMin); // Fake it out for now.
   Bogus_SrvEventActivity &= ~GetMaskFromEvent(eventMin);
   UNIMPLEMENTED

   return 0;
}

BOOL
STDCALL
NtUserUnhookWindowsHookEx(
   HHOOK Hook)
{
   PWINSTATION_OBJECT WinStaObj;
   PHOOK HookObj;
   NTSTATUS Status;
   DECLARE_RETURN(BOOL);

   DPRINT("Enter NtUserUnhookWindowsHookEx\n");
   UserEnterExclusive();

   Status = IntValidateWindowStationHandle(PsGetCurrentProcess()->Win32WindowStation,
                                           KernelMode,
                                           0,
                                           &WinStaObj);

   if (! NT_SUCCESS(Status))
   {
      SetLastNtError(Status);
      RETURN( FALSE);
   }

   //  Status = UserReferenceObjectByHandle(gHandleTable, Hook,
   //                                      otHookProc, (PVOID *) &HookObj);
   if (!(HookObj = IntGetHookObject(Hook)))
   {
      DPRINT1("Invalid handle passed to NtUserUnhookWindowsHookEx\n");
      ObDereferenceObject(WinStaObj);
      //      SetLastNtError(Status);
      RETURN( FALSE);
   }
   ASSERT(Hook == HookObj->Self);

   IntRemoveHook(HookObj, WinStaObj, FALSE);

   UserDereferenceObject(HookObj);
   ObDereferenceObject(WinStaObj);

   RETURN( TRUE);

CLEANUP:
   DPRINT("Leave NtUserUnhookWindowsHookEx, ret=%i\n",_ret_);
   UserLeave();
   END_CLEANUP;
}

DWORD
STDCALL
NtUserUnhookWinEvent(
   HWINEVENTHOOK hWinEventHook)
{
   UNIMPLEMENTED

   return 0;
}

/* EOF */
