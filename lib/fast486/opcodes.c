/*
 * Fast486 386/486 CPU Emulation Library
 * opcodes.c
 *
 * Copyright (C) 2013 Aleksandar Andrejevic <theflash AT sdf DOT lonestar DOT org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* INCLUDES *******************************************************************/

#include <windef.h>

// #define NDEBUG
#include <debug.h>

#include <fast486.h>
#include "opcodes.h"
#include "opgroups.h"
#include "extraops.h"
#include "common.h"
#include "fpu.h"

/* PUBLIC VARIABLES ***********************************************************/

FAST486_OPCODE_HANDLER_PROC
Fast486OpcodeHandlers[FAST486_NUM_OPCODE_HANDLERS] =
{
    Fast486OpcodeAddByteModrm,
    Fast486OpcodeAddModrm,
    Fast486OpcodeAddByteModrm,
    Fast486OpcodeAddModrm,
    Fast486OpcodeAddAl,
    Fast486OpcodeAddEax,
    Fast486OpcodePushEs,
    Fast486OpcodePopEs,
    Fast486OpcodeOrByteModrm,
    Fast486OpcodeOrModrm,
    Fast486OpcodeOrByteModrm,
    Fast486OpcodeOrModrm,
    Fast486OpcodeOrAl,
    Fast486OpcodeOrEax,
    Fast486OpcodePushCs,
    Fast486OpcodeExtended,
    Fast486OpcodeAdcByteModrm,
    Fast486OpcodeAdcModrm,
    Fast486OpcodeAdcByteModrm,
    Fast486OpcodeAdcModrm,
    Fast486OpcodeAdcAl,
    Fast486OpcodeAdcEax,
    Fast486OpcodePushSs,
    Fast486OpcodePopSs,
    Fast486OpcodeSbbByteModrm,
    Fast486OpcodeSbbModrm,
    Fast486OpcodeSbbByteModrm,
    Fast486OpcodeSbbModrm,
    Fast486OpcodeSbbAl,
    Fast486OpcodeSbbEax,
    Fast486OpcodePushDs,
    Fast486OpcodePopDs,
    Fast486OpcodeAndByteModrm,
    Fast486OpcodeAndModrm,
    Fast486OpcodeAndByteModrm,
    Fast486OpcodeAndModrm,
    Fast486OpcodeAndAl,
    Fast486OpcodeAndEax,
    Fast486OpcodePrefix,
    Fast486OpcodeDaa,
    Fast486OpcodeCmpSubByteModrm,
    Fast486OpcodeCmpSubModrm,
    Fast486OpcodeCmpSubByteModrm,
    Fast486OpcodeCmpSubModrm,
    Fast486OpcodeCmpSubAl,
    Fast486OpcodeCmpSubEax,
    Fast486OpcodePrefix,
    Fast486OpcodeDas,
    Fast486OpcodeXorByteModrm,
    Fast486OpcodeXorModrm,
    Fast486OpcodeXorByteModrm,
    Fast486OpcodeXorModrm,
    Fast486OpcodeXorAl,
    Fast486OpcodeXorEax,
    Fast486OpcodePrefix,
    Fast486OpcodeAaa,
    Fast486OpcodeCmpSubByteModrm,
    Fast486OpcodeCmpSubModrm,
    Fast486OpcodeCmpSubByteModrm,
    Fast486OpcodeCmpSubModrm,
    Fast486OpcodeCmpSubAl,
    Fast486OpcodeCmpSubEax,
    Fast486OpcodePrefix,
    Fast486OpcodeAas,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeIncrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodeDecrement,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePushReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePopReg,
    Fast486OpcodePushAll,
    Fast486OpcodePopAll,
    Fast486OpcodeBound,
    Fast486OpcodeArpl,
    Fast486OpcodePrefix,
    Fast486OpcodePrefix,
    Fast486OpcodePrefix,
    Fast486OpcodePrefix,
    Fast486OpcodePushImm,
    Fast486OpcodeImulModrmImm,
    Fast486OpcodePushByteImm,
    Fast486OpcodeImulModrmImm,
    Fast486OpcodeIns,
    Fast486OpcodeIns,
    Fast486OpcodeOuts,
    Fast486OpcodeOuts,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeShortConditionalJmp,
    Fast486OpcodeGroup8082,
    Fast486OpcodeGroup81,
    Fast486OpcodeGroup8082,
    Fast486OpcodeGroup83,
    Fast486OpcodeTestByteModrm,
    Fast486OpcodeTestModrm,
    Fast486OpcodeXchgByteModrm,
    Fast486OpcodeXchgModrm,
    Fast486OpcodeMovByteModrm,
    Fast486OpcodeMovModrm,
    Fast486OpcodeMovByteModrm,
    Fast486OpcodeMovModrm,
    Fast486OpcodeMovStoreSeg,
    Fast486OpcodeLea,
    Fast486OpcodeMovLoadSeg,
    Fast486OpcodeGroup8F,
    Fast486OpcodeNop,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeExchangeEax,
    Fast486OpcodeCwde,
    Fast486OpcodeCdq,
    Fast486OpcodeCallAbs,
    Fast486OpcodeWait,
    Fast486OpcodePushFlags,
    Fast486OpcodePopFlags,
    Fast486OpcodeSahf,
    Fast486OpcodeLahf,
    Fast486OpcodeMovAlOffset,
    Fast486OpcodeMovEaxOffset,
    Fast486OpcodeMovOffsetAl,
    Fast486OpcodeMovOffsetEax,
    Fast486OpcodeMovs,
    Fast486OpcodeMovs,
    Fast486OpcodeCmps,
    Fast486OpcodeCmps,
    Fast486OpcodeTestAl,
    Fast486OpcodeTestEax,
    Fast486OpcodeStos,
    Fast486OpcodeStos,
    Fast486OpcodeLods,
    Fast486OpcodeLods,
    Fast486OpcodeScas,
    Fast486OpcodeScas,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovByteRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeMovRegImm,
    Fast486OpcodeGroupC0,
    Fast486OpcodeGroupC1,
    Fast486OpcodeRet,
    Fast486OpcodeRet,
    Fast486OpcodeLdsLes,
    Fast486OpcodeLdsLes,
    Fast486OpcodeGroupC6,
    Fast486OpcodeGroupC7,
    Fast486OpcodeEnter,
    Fast486OpcodeLeave,
    Fast486OpcodeRetFar,
    Fast486OpcodeRetFar,
    Fast486OpcodeInt,
    Fast486OpcodeInt,
    Fast486OpcodeInt,
    Fast486OpcodeIret,
    Fast486OpcodeGroupD0,
    Fast486OpcodeGroupD1,
    Fast486OpcodeGroupD2,
    Fast486OpcodeGroupD3,
    Fast486OpcodeAam,
    Fast486OpcodeAad,
    Fast486OpcodeSalc,
    Fast486OpcodeXlat,
    Fast486FpuOpcodeD8,
    Fast486FpuOpcodeD9,
    Fast486FpuOpcodeDA,
    Fast486FpuOpcodeDB,
    Fast486FpuOpcodeDC,
    Fast486FpuOpcodeDD,
    Fast486FpuOpcodeDE,
    Fast486FpuOpcodeDF,
    Fast486OpcodeLoop,
    Fast486OpcodeLoop,
    Fast486OpcodeLoop,
    Fast486OpcodeJecxz,
    Fast486OpcodeInByte,
    Fast486OpcodeIn,
    Fast486OpcodeOutByte,
    Fast486OpcodeOut,
    Fast486OpcodeCall,
    Fast486OpcodeJmp,
    Fast486OpcodeJmpAbs,
    Fast486OpcodeShortJump,
    Fast486OpcodeInByte,
    Fast486OpcodeIn,
    Fast486OpcodeOutByte,
    Fast486OpcodeOut,
    Fast486OpcodePrefix,
    NULL, // Invalid
    Fast486OpcodePrefix,
    Fast486OpcodePrefix,
    Fast486OpcodeHalt,
    Fast486OpcodeComplCarry,
    Fast486OpcodeGroupF6,
    Fast486OpcodeGroupF7,
    Fast486OpcodeClearCarry,
    Fast486OpcodeSetCarry,
    Fast486OpcodeClearInt,
    Fast486OpcodeSetInt,
    Fast486OpcodeClearDir,
    Fast486OpcodeSetDir,
    Fast486OpcodeGroupFE,
    Fast486OpcodeGroupFF,
};

/* PUBLIC FUNCTIONS ***********************************************************/

FAST486_OPCODE_HANDLER(Fast486OpcodePrefix)
{
    BOOLEAN Valid = FALSE;

    switch (Opcode)
    {
        /* ES: */
        case 0x26:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_ES;
                Valid = TRUE;
            }

            break;
        }

        /* CS: */
        case 0x2E:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_CS;
                Valid = TRUE;
            }

            break;
        }

        /* SS: */
        case 0x36:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_SS;
                Valid = TRUE;
            }

            break;
        }

        /* DS: */
        case 0x3E:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_DS;
                Valid = TRUE;
            }

            break;
        }

        /* FS: */
        case 0x64:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_FS;
                Valid = TRUE;
            }

            break;
        }

        /* GS: */
        case 0x65:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_SEG))
            {
                State->PrefixFlags |= FAST486_PREFIX_SEG;
                State->SegmentOverride = FAST486_REG_GS;
                Valid = TRUE;
            }

            break;
        }

        /* OPSIZE */
        case 0x66:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_OPSIZE))
            {
                State->PrefixFlags |= FAST486_PREFIX_OPSIZE;
                Valid = TRUE;
            }

            break;
        }

        /* ADSIZE */
        case 0x67:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_ADSIZE))
            {
                State->PrefixFlags |= FAST486_PREFIX_ADSIZE;
                Valid = TRUE;
            }
            break;
        }

        /* LOCK */
        case 0xF0:
        {
            if (!(State->PrefixFlags & FAST486_PREFIX_LOCK))
            {
                State->PrefixFlags |= FAST486_PREFIX_LOCK;
                Valid = TRUE;
            }

            break;
        }

        /* REPNZ */
        case 0xF2:
        {
            /* Mutually exclusive with REP */
            if (!(State->PrefixFlags
                & (FAST486_PREFIX_REPNZ | FAST486_PREFIX_REP)))
            {
                State->PrefixFlags |= FAST486_PREFIX_REPNZ;
                Valid = TRUE;
            }

            break;
        }

        /* REP / REPZ */
        case 0xF3:
        {
            /* Mutually exclusive with REPNZ */
            if (!(State->PrefixFlags
                & (FAST486_PREFIX_REPNZ | FAST486_PREFIX_REP)))
            {
                State->PrefixFlags |= FAST486_PREFIX_REP;
                Valid = TRUE;
            }

            break;
        }
    }

    if (!Valid)
    {
        /* Clear all prefixes */
        State->PrefixFlags = 0;

        /* Throw an exception */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeIncrement)
{
    ULONG Value;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0x40);

    if (Size)
    {
        Value = ++State->GeneralRegs[Opcode & 0x07].Long;

        State->Flags.Of = (Value == SIGN_FLAG_LONG);
        State->Flags.Sf = ((Value & SIGN_FLAG_LONG) != 0);
    }
    else
    {
        Value = ++State->GeneralRegs[Opcode & 0x07].LowWord;

        State->Flags.Of = (Value == SIGN_FLAG_WORD);
        State->Flags.Sf = ((Value & SIGN_FLAG_WORD) != 0);
    }

    State->Flags.Zf = (Value == 0);
    State->Flags.Af = ((Value & 0x0F) == 0);
    State->Flags.Pf = Fast486CalculateParity(LOBYTE(Value));

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeDecrement)
{
    ULONG Value;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0x48);

    if (Size)
    {
        Value = --State->GeneralRegs[Opcode & 0x07].Long;

        State->Flags.Of = (Value == (SIGN_FLAG_LONG - 1));
        State->Flags.Sf = ((Value & SIGN_FLAG_LONG) != 0);
    }
    else
    {
        Value = --State->GeneralRegs[Opcode & 0x07].LowWord;

        State->Flags.Of = (Value == (SIGN_FLAG_WORD - 1));
        State->Flags.Sf = ((Value & SIGN_FLAG_WORD) != 0);
    }

    State->Flags.Zf = (Value == 0);
    State->Flags.Af = ((Value & 0x0F) == 0x0F);
    State->Flags.Pf = Fast486CalculateParity(LOBYTE(Value));

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushReg)
{
    NO_LOCK_PREFIX();

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0x50);

    /* Call the internal function */
    return Fast486StackPush(State, State->GeneralRegs[Opcode & 0x07].Long);
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopReg)
{
    ULONG Value;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0x58);

    /* Call the internal function */
    if (!Fast486StackPop(State, &Value)) return FALSE;

    /* Store the value */
    if (Size) State->GeneralRegs[Opcode & 0x07].Long = Value;
    else State->GeneralRegs[Opcode & 0x07].LowWord = Value;

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeNop)
{
    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        /* Idle cycle */
        State->IdleCallback(State);
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeExchangeEax)
{
    INT Reg = Opcode & 0x07;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0x90);

    /* Exchange the values */
    if (Size)
    {
        ULONG Value;

        Value = State->GeneralRegs[Reg].Long;
        State->GeneralRegs[Reg].Long = State->GeneralRegs[FAST486_REG_EAX].Long;
        State->GeneralRegs[FAST486_REG_EAX].Long = Value;
    }
    else
    {
        USHORT Value;

        Value = State->GeneralRegs[Reg].LowWord;
        State->GeneralRegs[Reg].LowWord = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Value;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeShortConditionalJmp)
{
    BOOLEAN Jump = FALSE;
    CHAR Offset = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF0) == 0x70);

    TOGGLE_OPSIZE(Size);

    /* Fetch the offset */
    if (!Fast486FetchByte(State, (PUCHAR)&Offset))
    {
        /* An exception occurred */
        return FALSE;
    }

    switch ((Opcode & 0x0F) >> 1)
    {
        /* JO / JNO */
        case 0:
        {
            Jump = State->Flags.Of;
            break;
        }

        /* JC / JNC */
        case 1:
        {
            Jump = State->Flags.Cf;
            break;
        }

        /* JZ / JNZ */
        case 2:
        {
            Jump = State->Flags.Zf;
            break;
        }

        /* JBE / JNBE */
        case 3:
        {
            Jump = State->Flags.Cf || State->Flags.Zf;
            break;
        }

        /* JS / JNS */
        case 4:
        {
            Jump = State->Flags.Sf;
            break;
        }

        /* JP / JNP */
        case 5:
        {
            Jump = State->Flags.Pf;
            break;
        }

        /* JL / JNL */
        case 6:
        {
            Jump = State->Flags.Sf != State->Flags.Of;
            break;
        }

        /* JLE / JNLE */
        case 7:
        {
            Jump = (State->Flags.Sf != State->Flags.Of) || State->Flags.Zf;
            break;
        }
    }

    if (Opcode & 1)
    {
        /* Invert the result */
        Jump = !Jump;
    }

    if (Jump)
    {
        /* Move the instruction pointer */
        State->InstPtr.Long += Offset;

        if (!Size)
        {
            /* Clear the top half of EIP */
            State->InstPtr.Long &= 0xFFFF;
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeClearCarry)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xF8);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Clear CF and return success */
    State->Flags.Cf = FALSE;
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSetCarry)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xF9);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Set CF and return success*/
    State->Flags.Cf = TRUE;
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeComplCarry)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xF5);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Toggle CF and return success */
    State->Flags.Cf = !State->Flags.Cf;
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeClearInt)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xFA);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Check for protected mode */
    if (State->ControlRegisters[FAST486_REG_CR0] & FAST486_CR0_PE)
    {
        /* Check IOPL */
        if (State->Flags.Iopl >= State->SegmentRegs[FAST486_REG_CS].Dpl)
        {
            /* Clear the interrupt flag */
            State->Flags.If = FALSE;
        }
        else
        {
            /* General Protection Fault */
            Fast486Exception(State, FAST486_EXCEPTION_GP);
            return FALSE;
        }
    }
    else
    {
        /* Just clear the interrupt flag */
        State->Flags.If = FALSE;
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSetInt)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xFB);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Check for protected mode */
    if (State->ControlRegisters[FAST486_REG_CR0] & FAST486_CR0_PE)
    {
        /* Check IOPL */
        if (State->Flags.Iopl >= State->SegmentRegs[FAST486_REG_CS].Dpl)
        {
            /* Set the interrupt flag */
            State->Flags.If = TRUE;
        }
        else
        {
            /* General Protection Fault */
            Fast486Exception(State, FAST486_EXCEPTION_GP);
            return FALSE;
        }
    }
    else
    {
        /* Just set the interrupt flag */
        State->Flags.If = TRUE;
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeClearDir)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xFC);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Clear DF and return success */
    State->Flags.Df = FALSE;
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSetDir)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xFD);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Set DF and return success*/
    State->Flags.Df = TRUE;
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeHalt)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xF4);

    /* No prefixes allowed */
    if (State->PrefixFlags)
    {
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Privileged instructions can only be executed under CPL = 0 */
    if (State->SegmentRegs[FAST486_REG_CS].Dpl != 0)
    {
        Fast486Exception(State, FAST486_EXCEPTION_GP);
        return FALSE;
    }

    /* Halt */
    // TODO: Halt the CPU until an interrupt occurs, using IdleCallback if needed.

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeInByte)
{
    UCHAR Data;
    ULONG Port;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF7) == 0xE4);

    if (Opcode == 0xE4)
    {
        /* Fetch the parameter */
        if (!Fast486FetchByte(State, &Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set the port number to the parameter */
        Port = Data;
    }
    else
    {
        /* The port number is in DX */
        Port = State->GeneralRegs[FAST486_REG_EDX].LowWord;
    }

    /* Read a byte from the I/O port */
    State->IoReadCallback(State, Port, &Data, 1, sizeof(UCHAR));

    /* Store the result in AL */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Data;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeIn)
{
    ULONG Port;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF7) == 0xE5);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Opcode == 0xE5)
    {
        UCHAR Data;

        /* Fetch the parameter */
        if (!Fast486FetchByte(State, &Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set the port number to the parameter */
        Port = Data;
    }
    else
    {
        /* The port number is in DX */
        Port = State->GeneralRegs[FAST486_REG_EDX].LowWord;
    }

    if (Size)
    {
        ULONG Data;

        /* Read a dword from the I/O port */
        State->IoReadCallback(State, Port, &Data, 1, sizeof(ULONG));

        /* Store the value in EAX */
        State->GeneralRegs[FAST486_REG_EAX].Long = Data;
    }
    else
    {
        USHORT Data;

        /* Read a word from the I/O port */
        State->IoReadCallback(State, Port, &Data, 1, sizeof(USHORT));

        /* Store the value in AX */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Data;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOutByte)
{
    UCHAR Data;
    ULONG Port;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF7) == 0xE6);

    if (Opcode == 0xE6)
    {
        /* Fetch the parameter */
        if (!Fast486FetchByte(State, &Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set the port number to the parameter */
        Port = Data;
    }
    else
    {
        /* The port number is in DX */
        Port = State->GeneralRegs[FAST486_REG_EDX].LowWord;
    }

    /* Read the value from AL */
    Data = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    /* Write the byte to the I/O port */
    State->IoWriteCallback(State, Port, &Data, 1, sizeof(UCHAR));

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOut)
{
    ULONG Port;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF7) == 0xE7);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Opcode == 0xE7)
    {
        UCHAR Data;

        /* Fetch the parameter */
        if (!Fast486FetchByte(State, &Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set the port number to the parameter */
        Port = Data;
    }
    else
    {
        /* The port number is in DX */
        Port = State->GeneralRegs[FAST486_REG_EDX].LowWord;
    }

    if (Size)
    {
        /* Get the value from EAX */
        ULONG Data = State->GeneralRegs[FAST486_REG_EAX].Long;

        /* Write a dword to the I/O port */
        State->IoWriteCallback(State, Port, &Data, 1, sizeof(ULONG));
    }
    else
    {
        /* Get the value from AX */
        USHORT Data = State->GeneralRegs[FAST486_REG_EAX].LowWord;

        /* Write a word to the I/O port */
        State->IoWriteCallback(State, Port, &Data, 1, sizeof(USHORT));
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeShortJump)
{
    CHAR Offset = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(Size);

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xEB);

    /* Fetch the offset */
    if (!Fast486FetchByte(State, (PUCHAR)&Offset))
    {
        /* An exception occurred */
        return FALSE;
    }

    /* Move the instruction pointer */
    State->InstPtr.Long += Offset;

    if (!Size)
    {
        /* Clear the top half of EIP */
        State->InstPtr.Long &= 0xFFFF;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovRegImm)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0xB8);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Size)
    {
        ULONG Value;

        /* Fetch the dword */
        if (!Fast486FetchDword(State, &Value))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Store the value in the register */
        State->GeneralRegs[Opcode & 0x07].Long = Value;
    }
    else
    {
        USHORT Value;

        /* Fetch the word */
        if (!Fast486FetchWord(State, &Value))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Store the value in the register */
        State->GeneralRegs[Opcode & 0x07].LowWord = Value;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovByteRegImm)
{
    UCHAR Value;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xF8) == 0xB0);

    if (State->PrefixFlags != 0)
    {
        /* Invalid prefix */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Fetch the byte */
    if (!Fast486FetchByte(State, &Value))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (Opcode & 0x04)
    {
        /* AH, CH, DH or BH */
        State->GeneralRegs[Opcode & 0x03].HighByte = Value;
    }
    else
    {
        /* AL, CL, DL or BL */
        State->GeneralRegs[Opcode & 0x03].LowByte = Value;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAddByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x00);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue + SecondValue;

    /* Update the flags */
    State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) == (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAddModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x01);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue;

        /* Update the flags */
        State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) == (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue;

        /* Update the flags */
        State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) == (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAddAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x04);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue + SecondValue;

    /* Update the flags */
    State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) == (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAddEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x05);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue;

        /* Update the flags */
        State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) == (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue;

        /* Update the flags */
        State->Flags.Cf = (Result < FirstValue) && (Result < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) == (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((((FirstValue & 0x0F) + (SecondValue & 0x0F)) & 0x10) != 0);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOrByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x08);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue | SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOrModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x09);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue | SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue | SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOrAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x0C);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue | SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOrEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x0D);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue | SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue | SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAndByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x20);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue & SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAndModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x21);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAndAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x24);

    NO_LOCK_PREFIX();

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue & SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAndEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x25);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXorByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x30);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue ^ SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXorModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x31);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue ^ SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue ^ SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXorAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x34);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue ^ SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXorEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x35);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue ^ SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue ^ SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeTestByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x84);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }
    /* Calculate the result */
    Result = FirstValue & SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* The result is discarded */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeTestModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x85);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);
    }

    /* The result is discarded */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeTestAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA8);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue & SecondValue;

    /* Update the flags */
    State->Flags.Cf = FALSE;
    State->Flags.Of = FALSE;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* The result is discarded */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeTestEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA9);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue & SecondValue;

        /* Update the flags */
        State->Flags.Cf = FALSE;
        State->Flags.Of = FALSE;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);
    }

    /* The result is discarded */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXchgByteModrm)
{
    UCHAR FirstValue, SecondValue;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x86);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Write the value from the register to the R/M */
    if (!Fast486WriteModrmByteOperands(State,
                                       &ModRegRm,
                                       FALSE,
                                       FirstValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Write the value from the R/M to the register */
    if (!Fast486WriteModrmByteOperands(State,
                                       &ModRegRm,
                                       TRUE,
                                       SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXchgModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x87);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write the value from the register to the R/M */
        if (!Fast486WriteModrmDwordOperands(State,
                                            &ModRegRm,
                                            FALSE,
                                            FirstValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write the value from the R/M to the register */
        if (!Fast486WriteModrmDwordOperands(State,
                                            &ModRegRm,
                                            TRUE,
                                            SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }
    }
    else
    {
        USHORT FirstValue, SecondValue;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write the value from the register to the R/M */
        if (!Fast486WriteModrmWordOperands(State,
                                           &ModRegRm,
                                           FALSE,
                                           FirstValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write the value from the R/M to the register */
        if (!Fast486WriteModrmWordOperands(State,
                                           &ModRegRm,
                                           TRUE,
                                           SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }
    }

    /* The result is discarded */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushEs)
{
    /* Call the internal API */
    return Fast486StackPush(State, State->SegmentRegs[FAST486_REG_ES].Selector);
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopEs)
{
    ULONG NewSelector;

    if (!Fast486StackPop(State, &NewSelector))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Call the internal API */
    return Fast486LoadSegment(State, FAST486_REG_ES, LOWORD(NewSelector));
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushCs)
{
    /* Call the internal API */
    return Fast486StackPush(State, State->SegmentRegs[FAST486_REG_CS].Selector);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAdcByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x10);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue + SecondValue + State->Flags.Cf;

    /* Special exception for CF */
    State->Flags.Cf = State->Flags.Cf
                      && ((FirstValue == 0xFF) || (SecondValue == 0xFF));

    /* Update the flags */
    State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) == (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAdcModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x11);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue + State->Flags.Cf;

        /* Special exception for CF */
        State->Flags.Cf = State->Flags.Cf
                          && ((FirstValue == 0xFFFFFFFF) || (SecondValue == 0xFFFFFFFF));

        /* Update the flags */
        State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) == (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue + State->Flags.Cf;

        /* Special exception for CF */
        State->Flags.Cf = State->Flags.Cf
                          && ((FirstValue == 0xFFFF) || (SecondValue == 0xFFFF));

        /* Update the flags */
        State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) == (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }

}

FAST486_OPCODE_HANDLER(Fast486OpcodeAdcAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x14);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue + SecondValue + State->Flags.Cf;

    /* Special exception for CF */
    State->Flags.Cf = State->Flags.Cf &&
                      ((FirstValue == 0xFF) || (SecondValue == 0xFF));

    /* Update the flags */
    State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) == (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAdcEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x15);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue + State->Flags.Cf;

        /* Special exception for CF */
        State->Flags.Cf = State->Flags.Cf &&
                          ((FirstValue == 0xFFFFFFFF) || (SecondValue == 0xFFFFFFFF));

        /* Update the flags */
        State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) == (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue + SecondValue + State->Flags.Cf;

        /* Special exception for CF */
        State->Flags.Cf = State->Flags.Cf &&
                          ((FirstValue == 0xFFFF) || (SecondValue == 0xFFFF));

        /* Update the flags */
        State->Flags.Cf = State->Flags.Cf || ((Result < FirstValue) && (Result < SecondValue));
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) == (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushSs)
{
    /* Call the internal API */
    return Fast486StackPush(State, State->SegmentRegs[FAST486_REG_SS].Selector);
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopSs)
{
    ULONG NewSelector;

    if (!Fast486StackPop(State, &NewSelector))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Call the internal API */
    return Fast486LoadSegment(State, FAST486_REG_SS, LOWORD(NewSelector));
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSbbByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;
    INT Carry = State->Flags.Cf ? 1 : 0;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x18);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check if this is the instruction that writes to R/M */
    if (!(Opcode & FAST486_OPCODE_WRITE_REG))
    {
        /* Swap the order */
        SWAP(FirstValue, SecondValue);
    }

    /* Calculate the result */
    Result = FirstValue - SecondValue - Carry;

    /* Update the flags */
    State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) != (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSbbModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;
    INT Carry = State->Flags.Cf ? 1 : 0;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x19);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                           &ModRegRm,
                                           &FirstValue,
                                           &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Check if this is the instruction that writes to R/M */
        if (!(Opcode & FAST486_OPCODE_WRITE_REG))
        {
            /* Swap the order */
            SWAP(FirstValue, SecondValue);
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue - Carry;

        /* Update the flags */
        State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) != (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Check if this is the instruction that writes to R/M */
        if (!(Opcode & FAST486_OPCODE_WRITE_REG))
        {
            /* Swap the order */
            SWAP(FirstValue, SecondValue);
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue - Carry;

        /* Update the flags */
        State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) != (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                             &ModRegRm,
                                             Opcode & FAST486_OPCODE_WRITE_REG,
                                             Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSbbAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;
    INT Carry = State->Flags.Cf ? 1 : 0;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x1C);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue - SecondValue - Carry;

    /* Update the flags */
    State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) != (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Write back the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;

    return TRUE;

}

FAST486_OPCODE_HANDLER(Fast486OpcodeSbbEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    INT Carry = State->Flags.Cf ? 1 : 0;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x1D);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue - Carry;

        /* Update the flags */
        State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) != (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].Long = Result;
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue - Carry;

        /* Update the flags */
        State->Flags.Cf = Carry ? (FirstValue <= SecondValue) : (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) != (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = ((FirstValue ^ SecondValue ^ Result) & 0x10) != 0;
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
    }

    return TRUE;

}

FAST486_OPCODE_HANDLER(Fast486OpcodePushDs)
{
    /* Call the internal API */
    return Fast486StackPush(State, State->SegmentRegs[FAST486_REG_DS].Selector);
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopDs)
{
    ULONG NewSelector;

    if (!Fast486StackPop(State, &NewSelector))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Call the internal API */
    return Fast486LoadSegment(State, FAST486_REG_DS, LOWORD(NewSelector));
}

FAST486_OPCODE_HANDLER(Fast486OpcodeDaa)
{
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    BOOLEAN Carry = State->Flags.Cf;

    /* Clear the carry flag */
    State->Flags.Cf = FALSE;

    /* Check if the first BCD digit is invalid or there was a carry from it */
    if (((Value & 0x0F) > 9) || State->Flags.Af)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowByte += 0x06;
        if (State->GeneralRegs[FAST486_REG_EAX].LowByte < 0x06)
        {
            /* A carry occurred */
            State->Flags.Cf = TRUE;
        }

        /* Set the adjust flag */
        State->Flags.Af = TRUE;
    }

    /* Check if the second BCD digit is invalid or there was a carry from it */
    if ((Value > 0x99) || Carry)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowByte += 0x60;

        /* There was a carry */
        State->Flags.Cf = TRUE;
    }

    Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    /* Update the flags */
    State->Flags.Sf = (Value & SIGN_FLAG_BYTE) != 0;
    State->Flags.Zf = (Value == 0);
    State->Flags.Pf = Fast486CalculateParity(Value);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCmpSubByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xED) == 0x28);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check if this is the instruction that writes to R/M */
    if (!(Opcode & FAST486_OPCODE_WRITE_REG))
    {
        /* Swap the order */
        SWAP(FirstValue, SecondValue);
    }

    /* Calculate the result */
    Result = FirstValue - SecondValue;

    /* Update the flags */
    State->Flags.Cf = (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) != (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Check if this is not a CMP */
    if (!(Opcode & 0x10))
    {
        /* Write back the result */
        return Fast486WriteModrmByteOperands(State,
                                             &ModRegRm,
                                             Opcode & FAST486_OPCODE_WRITE_REG,
                                             Result);
    }
    else
    {
        /* Discard the result */
        return TRUE;
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCmpSubModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xED) == 0x29);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Check if this is the instruction that writes to R/M */
        if (!(Opcode & FAST486_OPCODE_WRITE_REG))
        {
            /* Swap the order */
            SWAP(FirstValue, SecondValue);
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue;

        /* Update the flags */
        State->Flags.Cf = (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) != (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Check if this is not a CMP */
        if (!(Opcode & 0x10))
        {
            /* Write back the result */
            return Fast486WriteModrmDwordOperands(State,
                                                  &ModRegRm,
                                                  Opcode & FAST486_OPCODE_WRITE_REG,
                                                  Result);
        }
        else
        {
            /* Discard the result */
            return TRUE;
        }
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Check if this is the instruction that writes to R/M */
        if (!(Opcode & FAST486_OPCODE_WRITE_REG))
        {
            /* Swap the order */
            SWAP(FirstValue, SecondValue);
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue;

        /* Update the flags */
        State->Flags.Cf = (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) != (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Check if this is not a CMP */
        if (!(Opcode & 0x10))
        {
            /* Write back the result */
            return Fast486WriteModrmWordOperands(State,
                                                 &ModRegRm,
                                                 Opcode & FAST486_OPCODE_WRITE_REG,
                                                 Result);
        }
        else
        {
            /* Discard the result */
            return TRUE;
        }
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCmpSubAl)
{
    UCHAR FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    UCHAR SecondValue, Result;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xEF) == 0x2C);

    if (State->PrefixFlags)
    {
        /* This opcode doesn't take any prefixes */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486FetchByte(State, &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    Result = FirstValue - SecondValue;

    /* Update the flags */
    State->Flags.Cf = (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SIGN_FLAG_BYTE) != (SecondValue & SIGN_FLAG_BYTE))
                      && ((FirstValue & SIGN_FLAG_BYTE) != (Result & SIGN_FLAG_BYTE));
    State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Check if this is not a CMP */
    if (!(Opcode & 0x10))
    {
        /* Write back the result */
        State->GeneralRegs[FAST486_REG_EAX].LowByte = Result;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCmpSubEax)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xEF) == 0x2D);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
        ULONG SecondValue, Result;

        if (!Fast486FetchDword(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue;

        /* Update the flags */
        State->Flags.Cf = (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_LONG) != (SecondValue & SIGN_FLAG_LONG))
                          && ((FirstValue & SIGN_FLAG_LONG) != (Result & SIGN_FLAG_LONG));
        State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_LONG) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Check if this is not a CMP */
        if (!(Opcode & 0x10))
        {
            /* Write back the result */
            State->GeneralRegs[FAST486_REG_EAX].Long = Result;
        }
    }
    else
    {
        USHORT FirstValue = State->GeneralRegs[FAST486_REG_EAX].LowWord;
        USHORT SecondValue, Result;

        if (!Fast486FetchWord(State, &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Calculate the result */
        Result = FirstValue - SecondValue;

        /* Update the flags */
        State->Flags.Cf = (FirstValue < SecondValue);
        State->Flags.Of = ((FirstValue & SIGN_FLAG_WORD) != (SecondValue & SIGN_FLAG_WORD))
                          && ((FirstValue & SIGN_FLAG_WORD) != (Result & SIGN_FLAG_WORD));
        State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
        State->Flags.Zf = (Result == 0);
        State->Flags.Sf = ((Result & SIGN_FLAG_WORD) != 0);
        State->Flags.Pf = Fast486CalculateParity(Result);

        /* Check if this is not a CMP */
        if (!(Opcode & 0x10))
        {
            /* Write back the result */
            State->GeneralRegs[FAST486_REG_EAX].LowWord = Result;
        }
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeDas)
{
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;
    BOOLEAN Carry = State->Flags.Cf;

    /* Clear the carry flag */
    State->Flags.Cf = FALSE;

    /* Check if the first BCD digit is invalid or there was a borrow */
    if (((Value & 0x0F) > 9) || State->Flags.Af)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowByte -= 0x06;
        if (State->GeneralRegs[FAST486_REG_EAX].LowByte > 0xFB)
        {
            /* A borrow occurred */
            State->Flags.Cf = TRUE;
        }

        /* Set the adjust flag */
        State->Flags.Af = TRUE;
    }

    /* Check if the second BCD digit is invalid or there was a borrow */
    if ((Value > 0x99) || Carry)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowByte -= 0x60;

        /* There was a borrow */
        State->Flags.Cf = TRUE;
    }

    Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    /* Update the flags */
    State->Flags.Sf = (Value & SIGN_FLAG_BYTE) != 0;
    State->Flags.Zf = (Value == 0);
    State->Flags.Pf = Fast486CalculateParity(Value);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAaa)
{
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    /*
     * Check if the value in AL is not a valid BCD digit,
     * or there was a carry from the lowest 4 bits of AL
     */
    if (((Value & 0x0F) > 9) || State->Flags.Af)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowWord += 0x06;
        State->GeneralRegs[FAST486_REG_EAX].HighByte++;

        /* Set CF and AF */
        State->Flags.Cf = State->Flags.Af = TRUE;
    }
    else
    {
        /* Clear CF and AF */
        State->Flags.Cf = State->Flags.Af = FALSE;
    }

    /* Keep only the lowest 4 bits of AL */
    State->GeneralRegs[FAST486_REG_EAX].LowByte &= 0x0F;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAas)
{
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    /*
     * Check if the value in AL is not a valid BCD digit,
     * or there was a borrow from the lowest 4 bits of AL
     */
    if (((Value & 0x0F) > 9) || State->Flags.Af)
    {
        /* Correct it */
        State->GeneralRegs[FAST486_REG_EAX].LowWord -= 0x06;
        State->GeneralRegs[FAST486_REG_EAX].HighByte--;

        /* Set CF and AF */
        State->Flags.Cf = State->Flags.Af = TRUE;
    }
    else
    {
        /* Clear CF and AF */
        State->Flags.Cf = State->Flags.Af = FALSE;
    }

    /* Keep only the lowest 4 bits of AL */
    State->GeneralRegs[FAST486_REG_EAX].LowByte &= 0x0F;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushAll)
{
    INT i;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    FAST486_REG SavedEsp = State->GeneralRegs[FAST486_REG_ESP];

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x60);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Push all the registers in order */
    for (i = 0; i < FAST486_NUM_GEN_REGS; i++)
    {
        if (i == FAST486_REG_ESP)
        {
            /* Use the saved ESP instead */
            if (!Fast486StackPush(State, Size ? SavedEsp.Long : SavedEsp.LowWord))
            {
                /* Exception occurred */
                return FALSE;
            }
        }
        else
        {
            /* Push the register */
            if (!Fast486StackPush(State, Size ? State->GeneralRegs[i].Long
                                              : State->GeneralRegs[i].LowWord))
            {
                /* Exception occurred */
                return FALSE;
            }
        }
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopAll)
{
    INT i;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    ULONG Value;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x61);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Pop all the registers in reverse order */
    for (i = FAST486_NUM_GEN_REGS - 1; i >= 0; i--)
    {
        /* Pop the value */
        if (!Fast486StackPop(State, &Value))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Don't modify ESP */
        if (i != FAST486_REG_ESP)
        {
            if (Size) State->GeneralRegs[i].Long = Value;
            else State->GeneralRegs[i].LowWord = LOWORD(Value);
        }
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeBound)
{
    BOOLEAN OperandSize, AddressSize;
    FAST486_MOD_REG_RM ModRegRm;
    FAST486_SEG_REGS Segment = FAST486_REG_DS;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!ModRegRm.Memory)
    {
        /* Invalid */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Check for the segment override */
    if (State->PrefixFlags & FAST486_PREFIX_SEG)
    {
        /* Use the override segment instead */
        Segment = State->SegmentOverride;
    }

    if (OperandSize)
    {
        LONG Index, LowerBound, UpperBound;

        /* Read the operands */
        if (!Fast486ReadModrmDwordOperands(State,
                                           &ModRegRm,
                                           (PULONG)&Index,
                                           (PULONG)&LowerBound))
        {
            /* Exception occurred */
            return FALSE;
        }

        if (!Fast486ReadMemory(State,
                               Segment,
                               ModRegRm.MemoryAddress + sizeof(ULONG),
                               FALSE,
                               &UpperBound,
                               sizeof(ULONG)))
        {
            /* Exception occurred */
            return FALSE;
        }

        if ((Index < LowerBound) || (Index > UpperBound))
        {
            /* Out of bounds */
            Fast486Exception(State, FAST486_EXCEPTION_BR);
            return FALSE;
        }
    }
    else
    {
        SHORT Index, LowerBound, UpperBound;

        /* Read the operands */
        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          (PUSHORT)&Index,
                                          (PUSHORT)&LowerBound))
        {
            /* Exception occurred */
            return FALSE;
        }

        if (!Fast486ReadMemory(State,
                               Segment,
                               ModRegRm.MemoryAddress + sizeof(USHORT),
                               FALSE,
                               &UpperBound,
                               sizeof(USHORT)))
        {
            /* Exception occurred */
            return FALSE;
        }

        if ((Index < LowerBound) || (Index > UpperBound))
        {
            /* Out of bounds */
            Fast486Exception(State, FAST486_EXCEPTION_BR);
            return FALSE;
        }
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeArpl)
{
    USHORT FirstValue, SecondValue;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    if (!(State->ControlRegisters[FAST486_REG_CR0] & FAST486_CR0_PE)
        || State->Flags.Vm
        || (State->PrefixFlags & FAST486_PREFIX_LOCK))
    {
        /* Cannot be used in real mode or with a LOCK prefix */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Read the operands */
    if (!Fast486ReadModrmWordOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check if the RPL needs adjusting */
    if ((SecondValue & 3) < (FirstValue & 3))
    {
        /* Adjust the RPL */
        SecondValue &= ~3;
        SecondValue |= FirstValue & 3;

        /* Set ZF */
        State->Flags.Zf = TRUE;

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State, &ModRegRm, FALSE, SecondValue);
    }
    else
    {
        /* Clear ZF */
        State->Flags.Zf = FALSE;
        return TRUE;
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushImm)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x68);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        ULONG Data;

        if (!Fast486FetchDword(State, &Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Call the internal API */
        return Fast486StackPush(State, Data);
    }
    else
    {
        SHORT Data;

        if (!Fast486FetchWord(State, (PUSHORT)&Data))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Call the internal API */
        return Fast486StackPush(State, Data);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeImulModrmImm)
{
    BOOLEAN OperandSize, AddressSize;
    FAST486_MOD_REG_RM ModRegRm;
    LONG Multiplier;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x69);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Fetch the parameters */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (Opcode == 0x6B)
    {
        CHAR Byte;

        /* Fetch the immediate operand */
        if (!Fast486FetchByte(State, (PUCHAR)&Byte))
        {
            /* Exception occurred */
            return FALSE;
        }

        Multiplier = (LONG)Byte;
    }
    else
    {
        if (OperandSize)
        {
            LONG Dword;

            /* Fetch the immediate operand */
            if (!Fast486FetchDword(State, (PULONG)&Dword))
            {
                /* Exception occurred */
                return FALSE;
            }

            Multiplier = Dword;
        }
        else
        {
            SHORT Word;

            /* Fetch the immediate operand */
            if (!Fast486FetchWord(State, (PUSHORT)&Word))
            {
                /* Exception occurred */
                return FALSE;
            }

            Multiplier = (LONG)Word;
        }
    }

    if (OperandSize)
    {
        LONG RegValue, Multiplicand;
        LONGLONG Product;

        /* Read the operands */
        if (!Fast486ReadModrmDwordOperands(State,
                                           &ModRegRm,
                                           (PULONG)&RegValue,
                                           (PULONG)&Multiplicand))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Multiply */
        Product = (LONGLONG)Multiplicand * (LONGLONG)Multiplier;

        /* Check for carry/overflow */
        State->Flags.Cf = State->Flags.Of = ((Product < MINLONG) || (Product > MAXLONG));

        /* Write-back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              TRUE,
                                              (ULONG)((LONG)Product));
    }
    else
    {
        SHORT RegValue, Multiplicand;
        LONG Product;

        /* Read the operands */
        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          (PUSHORT)&RegValue,
                                          (PUSHORT)&Multiplicand))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Multiply */
        Product = (LONG)Multiplicand * (LONG)Multiplier;

        /* Check for carry/overflow */
        State->Flags.Cf = State->Flags.Of = ((Product < MINSHORT) || (Product > MAXSHORT));

        /* Write-back the result */
        return Fast486WriteModrmWordOperands(State,
                                             &ModRegRm,
                                             TRUE,
                                             (USHORT)((SHORT)Product));
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushByteImm)
{
    CHAR Data;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x6A);

    if (!Fast486FetchByte(State, (PUCHAR)&Data))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Call the internal API */
    return Fast486StackPush(State, Data);
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovByteModrm)
{
    UCHAR FirstValue, SecondValue, Result;
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x88);

    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486ReadModrmByteOperands(State,
                                      &ModRegRm,
                                      &FirstValue,
                                      &SecondValue))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (Opcode & FAST486_OPCODE_WRITE_REG) Result = SecondValue;
    else Result = FirstValue;

    /* Write back the result */
    return Fast486WriteModrmByteOperands(State,
                                         &ModRegRm,
                                         Opcode & FAST486_OPCODE_WRITE_REG,
                                         Result);

}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovModrm)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFD) == 0x89);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check the operand size */
    if (OperandSize)
    {
        ULONG FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmDwordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        if (Opcode & FAST486_OPCODE_WRITE_REG) Result = SecondValue;
        else Result = FirstValue;

        /* Write back the result */
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              Opcode & FAST486_OPCODE_WRITE_REG,
                                              Result);
    }
    else
    {
        USHORT FirstValue, SecondValue, Result;

        if (!Fast486ReadModrmWordOperands(State,
                                          &ModRegRm,
                                          &FirstValue,
                                          &SecondValue))
        {
            /* Exception occurred */
            return FALSE;
        }

        if (Opcode & FAST486_OPCODE_WRITE_REG) Result = SecondValue;
        else Result = FirstValue;

        /* Write back the result */
        return Fast486WriteModrmWordOperands(State,
                                             &ModRegRm,
                                             Opcode & FAST486_OPCODE_WRITE_REG,
                                             Result);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovStoreSeg)
{
    BOOLEAN OperandSize, AddressSize;
    FAST486_MOD_REG_RM ModRegRm;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x8C);

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (ModRegRm.Register >= FAST486_NUM_SEG_REGS)
    {
        /* Invalid */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (OperandSize)
    {
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              FALSE,
                                              State->SegmentRegs[ModRegRm.Register].Selector);
    }
    else
    {
        return Fast486WriteModrmWordOperands(State,
                                             &ModRegRm,
                                             FALSE,
                                             State->SegmentRegs[ModRegRm.Register].Selector);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLea)
{
    FAST486_MOD_REG_RM ModRegRm;
    BOOLEAN OperandSize, AddressSize;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x8D);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* The second operand must be memory */
    if (!ModRegRm.Memory)
    {
        /* Invalid */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    /* Write the address to the register */
    if (OperandSize)
    {
        return Fast486WriteModrmDwordOperands(State,
                                              &ModRegRm,
                                              TRUE,
                                              ModRegRm.MemoryAddress);
    }
    else
    {
        return Fast486WriteModrmWordOperands(State,
                                             &ModRegRm,
                                             TRUE,
                                             ModRegRm.MemoryAddress);

    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovLoadSeg)
{
    BOOLEAN OperandSize, AddressSize;
    FAST486_MOD_REG_RM ModRegRm;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x8E);

    TOGGLE_ADSIZE(AddressSize);
    TOGGLE_OPSIZE(OperandSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if ((ModRegRm.Register >= FAST486_NUM_SEG_REGS)
        || ((FAST486_SEG_REGS)ModRegRm.Register == FAST486_REG_CS))
    {
        /* Invalid */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (OperandSize)
    {
        ULONG Selector;

        if (!Fast486ReadModrmDwordOperands(State, &ModRegRm, NULL, &Selector))
        {
            /* Exception occurred */
            return FALSE;
        }

        return Fast486LoadSegment(State, ModRegRm.Register, LOWORD(Selector));
    }
    else
    {
        USHORT Selector;

        if (!Fast486ReadModrmWordOperands(State, &ModRegRm, NULL, &Selector))
        {
            /* Exception occurred */
            return FALSE;
        }

        return Fast486LoadSegment(State, ModRegRm.Register, Selector);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCwde)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x98);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Size)
    {
        /* Sign extend AX to EAX */
        State->GeneralRegs[FAST486_REG_EAX].Long = MAKELONG
        (
            State->GeneralRegs[FAST486_REG_EAX].LowWord,
            (State->GeneralRegs[FAST486_REG_EAX].LowWord & SIGN_FLAG_WORD)
            ? 0xFFFF : 0x0000
        );
    }
    else
    {
        /* Sign extend AL to AX */
        State->GeneralRegs[FAST486_REG_EAX].HighByte =
        (State->GeneralRegs[FAST486_REG_EAX].LowByte & SIGN_FLAG_BYTE)
        ? 0xFF : 0x00;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCdq)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x99);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Size)
    {
        /* Sign extend EAX to EDX:EAX */
        State->GeneralRegs[FAST486_REG_EDX].Long =
        (State->GeneralRegs[FAST486_REG_EAX].Long & SIGN_FLAG_LONG)
        ? 0xFFFFFFFF : 0x00000000;
    }
    else
    {
        /* Sign extend AX to DX:AX */
        State->GeneralRegs[FAST486_REG_EDX].LowWord =
        (State->GeneralRegs[FAST486_REG_EAX].LowWord & SIGN_FLAG_WORD)
        ? 0xFFFF : 0x0000;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCallAbs)
{
    USHORT Segment = 0;
    ULONG Offset = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x9A);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Fetch the offset */
    if (Size)
    {
        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }
    else
    {
        if (!Fast486FetchWord(State, (PUSHORT)&Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }

    /* Fetch the segment */
    if (!Fast486FetchWord(State, &Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Push the current code segment selector */
    if (!Fast486StackPush(State, State->SegmentRegs[FAST486_REG_CS].Selector))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Push the current value of the instruction pointer */
    if (!Fast486StackPush(State, State->InstPtr.Long))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load the new CS */
    if (!Fast486LoadSegment(State, FAST486_REG_CS, Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load new (E)IP */
    if (Size) State->InstPtr.Long = Offset;
    else State->InstPtr.LowWord = LOWORD(Offset);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeWait)
{
    // TODO: NOT IMPLEMENTED
    UNIMPLEMENTED;

    return FALSE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodePushFlags)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    /* Check for VM86 mode when IOPL is not 3 */
    if (State->Flags.Vm && (State->Flags.Iopl != 3))
    {
        /* Call the VM86 monitor */
        Fast486ExceptionWithErrorCode(State, FAST486_EXCEPTION_GP, 0);
        return FALSE;
    }

    /* Push the flags */
    if (Size) return Fast486StackPush(State, State->Flags.Long);
    else return Fast486StackPush(State, LOWORD(State->Flags.Long));
}

FAST486_OPCODE_HANDLER(Fast486OpcodePopFlags)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    INT Cpl = Fast486GetCurrentPrivLevel(State);
    FAST486_FLAGS_REG NewFlags;

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    /* Pop the new flags */
    if (!Fast486StackPop(State, &NewFlags.Long))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check for VM86 mode when IOPL is not 3 */
    if (State->Flags.Vm && (State->Flags.Iopl != 3))
    {
        /* Call the VM86 monitor */
        Fast486ExceptionWithErrorCode(State, FAST486_EXCEPTION_GP, 0);
        return FALSE;
    }

    State->Flags.Cf = NewFlags.Cf;
    State->Flags.Pf = NewFlags.Pf;
    State->Flags.Af = NewFlags.Af;
    State->Flags.Zf = NewFlags.Zf;
    State->Flags.Sf = NewFlags.Sf;
    State->Flags.Tf = NewFlags.Tf;
    State->Flags.Df = NewFlags.Df;
    State->Flags.Of = NewFlags.Of;
    State->Flags.Nt = NewFlags.Nt;
    State->Flags.Ac = NewFlags.Ac;

    if (Cpl == 0) State->Flags.Iopl = NewFlags.Iopl;
    if (Cpl <= State->Flags.Iopl) State->Flags.If = NewFlags.If;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSahf)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x9E);

    /* Set the low-order byte of FLAGS to AH */
    State->Flags.Long &= 0xFFFFFF00;
    State->Flags.Long |= State->GeneralRegs[FAST486_REG_EAX].HighByte;

    /* Restore the reserved bits of FLAGS */
    State->Flags.AlwaysSet = TRUE;
    State->Flags.Reserved0 = State->Flags.Reserved1 = FALSE;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLahf)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0x9F);

    /* Set AH to the low-order byte of FLAGS */
    State->GeneralRegs[FAST486_REG_EAX].HighByte = LOBYTE(State->Flags.Long);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeRet)
{
    ULONG ReturnAddress;
    USHORT BytesToPop = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xC2);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Opcode == 0xC2)
    {
        /* Fetch the number of bytes to pop after the return */
        if (!Fast486FetchWord(State, &BytesToPop)) return FALSE;
    }

    /* Pop the return address */
    if (!Fast486StackPop(State, &ReturnAddress)) return FALSE;

    /* Return to the calling procedure, and if necessary, pop the parameters */
    if (Size)
    {
        State->InstPtr.Long = ReturnAddress;
        State->GeneralRegs[FAST486_REG_ESP].Long += BytesToPop;
    }
    else
    {
        State->InstPtr.LowWord = LOWORD(ReturnAddress);
        State->GeneralRegs[FAST486_REG_ESP].LowWord += BytesToPop;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLdsLes)
{
    UCHAR FarPointer[6];
    BOOLEAN OperandSize, AddressSize;
    FAST486_MOD_REG_RM ModRegRm;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xC4);

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    /* Get the operands */
    if (!Fast486ParseModRegRm(State, AddressSize, &ModRegRm))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!ModRegRm.Memory)
    {
        /* Check if this is a BOP and the host supports BOPs */
        if ((Opcode == 0xC4)
            && (ModRegRm.Register == FAST486_REG_EAX)
            && (ModRegRm.SecondRegister == FAST486_REG_ESP)
            && (State->BopCallback != NULL))
        {
            UCHAR BopCode;

            /* Fetch the BOP code */
            if (!Fast486FetchByte(State, &BopCode))
            {
                /* Exception occurred */
                return FALSE;
            }

            /* Call the BOP handler */
            State->BopCallback(State, BopCode);

            /*
             * If an interrupt should occur at this time, delay it.
             * We must do this because if an interrupt begins and the BOP callback
             * changes the CS:IP, the interrupt handler won't execute and the
             * stack pointer will never be restored.
             */
            if (State->IntStatus == FAST486_INT_EXECUTE)
            {
                State->IntStatus = FAST486_INT_DELAYED;
            }

            /* Return success */
            return TRUE;
        }

        /* Invalid */
        Fast486Exception(State, FAST486_EXCEPTION_UD);
        return FALSE;
    }

    if (!Fast486ReadMemory(State,
                           (State->PrefixFlags & FAST486_PREFIX_SEG)
                           ? State->SegmentOverride : FAST486_REG_DS,
                           ModRegRm.MemoryAddress,
                           FALSE,
                           FarPointer,
                           OperandSize ? 6 : 4))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (OperandSize)
    {
        ULONG Offset = *((PULONG)FarPointer);
        USHORT Segment = *((PUSHORT)&FarPointer[sizeof(ULONG)]);

        /* Set the register to the offset */
        State->GeneralRegs[ModRegRm.Register].Long = Offset;

        /* Load the segment */
        return Fast486LoadSegment(State,
                                  (Opcode == 0xC4)
                                  ? FAST486_REG_ES : FAST486_REG_DS,
                                  Segment);
    }
    else
    {
        USHORT Offset = *((PUSHORT)FarPointer);
        USHORT Segment = *((PUSHORT)&FarPointer[sizeof(USHORT)]);

        /* Set the register to the offset */
        State->GeneralRegs[ModRegRm.Register].LowWord = Offset;

        /* Load the segment */
        return Fast486LoadSegment(State,
                                  (Opcode == 0xC4)
                                  ? FAST486_REG_ES : FAST486_REG_DS,
                                  Segment);
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeEnter)
{
    INT i;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    USHORT FrameSize;
    UCHAR NestingLevel;
    FAST486_REG FramePointer;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xC8);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (!Fast486FetchWord(State, &FrameSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    if (!Fast486FetchByte(State, &NestingLevel))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Push EBP */
    if (!Fast486StackPush(State, State->GeneralRegs[FAST486_REG_EBP].Long))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Save ESP */
    FramePointer = State->GeneralRegs[FAST486_REG_ESP];

    /* Set up the nested procedure stacks */
    for (i = 1; i < NestingLevel; i++)
    {
        if (Size)
        {
            State->GeneralRegs[FAST486_REG_EBP].Long -= 4;
            Fast486StackPush(State, State->GeneralRegs[FAST486_REG_EBP].Long);
        }
        else
        {
            State->GeneralRegs[FAST486_REG_EBP].LowWord -= 2;
            Fast486StackPush(State, State->GeneralRegs[FAST486_REG_EBP].LowWord);
        }
    }

    if (NestingLevel > 0) Fast486StackPush(State, FramePointer.Long);

    /* Set EBP to the frame pointer */
    State->GeneralRegs[FAST486_REG_EBP] = FramePointer;

    /* Reserve space for the frame */
    if (Size) State->GeneralRegs[FAST486_REG_ESP].Long -= (ULONG)FrameSize;
    else State->GeneralRegs[FAST486_REG_ESP].LowWord -= FrameSize;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLeave)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xC9);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    if (Size)
    {
        /* Set the stack pointer (ESP) to the base pointer (EBP) */
        State->GeneralRegs[FAST486_REG_ESP].Long = State->GeneralRegs[FAST486_REG_EBP].Long;

        /* Pop the saved base pointer from the stack */
        return Fast486StackPop(State, &State->GeneralRegs[FAST486_REG_EBP].Long);
    }
    else
    {
        ULONG Value;

        /* Set the stack pointer (SP) to the base pointer (BP) */
        State->GeneralRegs[FAST486_REG_ESP].LowWord = State->GeneralRegs[FAST486_REG_EBP].LowWord;

        /* Pop the saved base pointer from the stack */
        if (Fast486StackPop(State, &Value))
        {
            State->GeneralRegs[FAST486_REG_EBP].LowWord = LOWORD(Value);
            return TRUE;
        }
        else return FALSE;
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeRetFar)
{
    ULONG Segment = 0;
    ULONG Offset = 0;
    USHORT BytesToPop = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xCA);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Opcode == 0xCA)
    {
        /* Fetch the number of bytes to pop after the return */
        if (!Fast486FetchWord(State, &BytesToPop)) return FALSE;
    }

    /* Pop the offset */
    if (!Fast486StackPop(State, &Offset))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Pop the segment */
    if (!Fast486StackPop(State, &Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load the new CS */
    if (!Fast486LoadSegment(State, FAST486_REG_CS, Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load new (E)IP, and if necessary, pop the parameters */
    if (Size)
    {
        State->InstPtr.Long = Offset;
        State->GeneralRegs[FAST486_REG_ESP].Long += BytesToPop;
    }
    else
    {
        State->InstPtr.LowWord = LOWORD(Offset);
        State->GeneralRegs[FAST486_REG_ESP].LowWord += BytesToPop;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeInt)
{
    UCHAR IntNum;
    FAST486_IDT_ENTRY IdtEntry;

    switch (Opcode)
    {
        case 0xCC:
        {
            /* This is the INT3 instruction */
            IntNum = 3;
            break;
        }

        case 0xCD:
        {
            /* Fetch the interrupt number */
            if (!Fast486FetchByte(State, &IntNum))
            {
                /* Exception occurred */
                return FALSE;
            }

            break;
        }

        case 0xCE:
        {
            /* Don't do anything if OF is cleared */
            if (!State->Flags.Of) return TRUE;

            /* Exception #OF */
            IntNum = FAST486_EXCEPTION_OF;

            break;
        }

        default:
        {
            /* Should not happen */
            ASSERT(FALSE);
        }
    }

    /* Get the interrupt vector */
    if (!Fast486GetIntVector(State, IntNum, &IdtEntry))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Perform the interrupt */
    if (!Fast486InterruptInternal(State,
                                  IdtEntry.Selector,
                                  MAKELONG(IdtEntry.Offset, IdtEntry.OffsetHigh),
                                  IdtEntry.Type))
    {
        /* Exception occurred */
        return FALSE;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeIret)
{
    FAST486_SEG_REGS i;
    ULONG InstPtr, CodeSel, StackPtr, StackSel;
    FAST486_FLAGS_REG NewFlags;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xCF);

    NO_LOCK_PREFIX();
    TOGGLE_OPSIZE(Size);

    /* Pop EIP */
    if (!Fast486StackPop(State, &InstPtr))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Pop CS */
    if (!Fast486StackPop(State, &CodeSel))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Pop EFLAGS */
    if (!Fast486StackPop(State, &NewFlags.Long))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check for protected mode */
    if (State->ControlRegisters[FAST486_REG_CR0] & FAST486_CR0_PE)
    {
        INT Cpl = Fast486GetCurrentPrivLevel(State);

        if (State->Flags.Vm)
        {
            /* Return from VM86 mode */

            /* Check the IOPL */
            if (State->Flags.Iopl == 3)
            {
                /* Set new EIP */
                State->InstPtr.Long = LOWORD(InstPtr);

                /* Load new CS */
                if (!Fast486LoadSegment(State, FAST486_REG_CS, CodeSel))
                {
                    /* Exception occurred */
                    return FALSE;
                }

                /* Set the new flags */
                if (Size) State->Flags.Long = NewFlags.Long & REAL_MODE_FLAGS_MASK;
                else State->Flags.LowWord = NewFlags.LowWord & REAL_MODE_FLAGS_MASK;
                State->Flags.AlwaysSet = State->Flags.Vm = TRUE;
                State->Flags.Iopl = 3;
            }
            else
            {
                /* Call the VM86 monitor */
                Fast486ExceptionWithErrorCode(State, FAST486_EXCEPTION_GP, 0);
                return FALSE;
            }

            return TRUE;
        }

        if (State->Flags.Nt)
        {
            /* Nested task return */

            UNIMPLEMENTED;
            return FALSE;
        }

        if (NewFlags.Vm)
        {
            /* Return to VM86 mode */
            ULONG Es, Ds, Fs, Gs;

            /* Pop ESP, SS, ES, FS, GS */
            if (!Fast486StackPop(State, &StackPtr)) return FALSE;
            if (!Fast486StackPop(State, &StackSel)) return FALSE;
            if (!Fast486StackPop(State, &Es)) return FALSE;
            if (!Fast486StackPop(State, &Ds)) return FALSE;
            if (!Fast486StackPop(State, &Fs)) return FALSE;
            if (!Fast486StackPop(State, &Gs)) return FALSE;

            /* Set the new IP */
            State->InstPtr.Long = LOWORD(InstPtr);

            /* Set the new flags */
            if (Size) State->Flags.Long = NewFlags.Long & REAL_MODE_FLAGS_MASK;
            else State->Flags.LowWord = NewFlags.LowWord & REAL_MODE_FLAGS_MASK;
            State->Flags.AlwaysSet = State->Flags.Vm = TRUE;

            /* Load the new segments */
            if (!Fast486LoadSegment(State, FAST486_REG_CS, CodeSel)) return FALSE;
            if (!Fast486LoadSegment(State, FAST486_REG_SS, StackSel)) return FALSE;
            if (!Fast486LoadSegment(State, FAST486_REG_ES, Es)) return FALSE;
            if (!Fast486LoadSegment(State, FAST486_REG_DS, Ds)) return FALSE;
            if (!Fast486LoadSegment(State, FAST486_REG_FS, Fs)) return FALSE;
            if (!Fast486LoadSegment(State, FAST486_REG_GS, Gs)) return FALSE;

            return TRUE;
        }

        /* Load the new CS */
        if (!Fast486LoadSegment(State, FAST486_REG_CS, CodeSel))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set EIP */
        if (Size) State->InstPtr.Long = InstPtr;
        else State->InstPtr.LowWord = LOWORD(InstPtr);

        if (GET_SEGMENT_RPL(CodeSel) > Cpl)
        {
            /* Pop ESP */
            if (!Fast486StackPop(State, &StackPtr))
            {
                /* Exception */
                return FALSE;
            }

            /* Pop SS */
            if (!Fast486StackPop(State, &StackSel))
            {
                /* Exception */
                return FALSE;
            }

            /* Load new SS */
            if (!Fast486LoadSegment(State, FAST486_REG_SS, StackSel))
            {
                /* Exception */
                return FALSE;
            }

            /* Set ESP */
            if (Size) State->GeneralRegs[FAST486_REG_ESP].Long = StackPtr;
            else State->GeneralRegs[FAST486_REG_ESP].LowWord = LOWORD(StackPtr);
        }

        /* Set the new flags */
        if (Size) State->Flags.Long = NewFlags.Long & PROT_MODE_FLAGS_MASK;
        else State->Flags.LowWord = NewFlags.LowWord & PROT_MODE_FLAGS_MASK;
        State->Flags.AlwaysSet = TRUE;

        /* Set additional flags */
        if (Cpl <= State->Flags.Iopl) State->Flags.If = NewFlags.If;
        if (Cpl == 0) State->Flags.Iopl = NewFlags.Iopl;

        if (GET_SEGMENT_RPL(CodeSel) > Cpl)
        {
            /* Update the CPL */
            Cpl = Fast486GetCurrentPrivLevel(State);

            /* Check segment security */
            for (i = 0; i < FAST486_NUM_SEG_REGS; i++)
            {
                /* Don't check CS or SS */
                if ((i == FAST486_REG_CS) || (i == FAST486_REG_SS)) continue;

                if ((Cpl > State->SegmentRegs[i].Dpl)
                    && (!State->SegmentRegs[i].Executable
                    || !State->SegmentRegs[i].DirConf))
                {
                    /* Load the NULL descriptor in the segment */
                    if (!Fast486LoadSegment(State, i, 0)) return FALSE;
                }
            }
        }
    }
    else
    {
        if (Size && (InstPtr & 0xFFFF0000))
        {
            /* Invalid */
            Fast486ExceptionWithErrorCode(State, FAST486_EXCEPTION_GP, 0);
            return FALSE;
        }

        /* Set new EIP */
        State->InstPtr.Long = InstPtr;

        /* Load new CS */
        if (!Fast486LoadSegment(State, FAST486_REG_CS, CodeSel))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Set the new flags */
        if (Size) State->Flags.Long = NewFlags.Long & REAL_MODE_FLAGS_MASK;
        else State->Flags.LowWord = NewFlags.LowWord & REAL_MODE_FLAGS_MASK;
        State->Flags.AlwaysSet = TRUE;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAam)
{
    UCHAR Base;
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    NO_LOCK_PREFIX();

    /* Fetch the base */
    if (!Fast486FetchByte(State, &Base))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Check if the base is zero */
    if (Base == 0)
    {
        /* Divide error */
        Fast486Exception(State, FAST486_EXCEPTION_DE);
        return FALSE;
    }

    /* Adjust */
    State->GeneralRegs[FAST486_REG_EAX].HighByte = Value / Base;
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Value %= Base;

    /* Update flags */
    State->Flags.Af = FALSE;
    State->Flags.Zf = (Value == 0);
    State->Flags.Sf = ((Value & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Value);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeAad)
{
    UCHAR Base;
    UCHAR Value = State->GeneralRegs[FAST486_REG_EAX].LowByte;

    NO_LOCK_PREFIX();

    /* Fetch the base */
    if (!Fast486FetchByte(State, &Base))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Adjust */
    Value += State->GeneralRegs[FAST486_REG_EAX].HighByte * Base;
    State->GeneralRegs[FAST486_REG_EAX].LowWord = Value;

    /* Update flags */
    State->Flags.Af = FALSE;
    State->Flags.Zf = (Value == 0);
    State->Flags.Sf = ((Value & SIGN_FLAG_BYTE) != 0);
    State->Flags.Pf = Fast486CalculateParity(Value);

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeXlat)
{
    UCHAR Value;
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    TOGGLE_ADSIZE(AddressSize);

    /* Read a byte from DS:[(E)BX + AL] */
    if (!Fast486ReadMemory(State,
                           FAST486_REG_DS,
                           (AddressSize ? State->GeneralRegs[FAST486_REG_EBX].Long
                                        : State->GeneralRegs[FAST486_REG_EBX].LowWord)
                           + State->GeneralRegs[FAST486_REG_EAX].LowByte,
                           FALSE,
                           &Value,
                           sizeof(UCHAR)))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Set AL to the result */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = Value;

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLoop)
{
    BOOLEAN Condition;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    CHAR Offset = 0;

    /* Make sure this is the right instruction */
    ASSERT((Opcode >= 0xE0) && (Opcode <= 0xE2));

    NO_LOCK_PREFIX();
    TOGGLE_ADSIZE(Size);

    if (Size) Condition = ((--State->GeneralRegs[FAST486_REG_ECX].Long) != 0);
    else Condition = ((--State->GeneralRegs[FAST486_REG_ECX].LowWord) != 0);

    if (Opcode == 0xE0)
    {
        /* Additional rule for LOOPNZ */
        if (State->Flags.Zf) Condition = FALSE;
    }

    if (Opcode == 0xE1)
    {
        /* Additional rule for LOOPZ */
        if (!State->Flags.Zf) Condition = FALSE;
    }

    /* Fetch the offset */
    if (!Fast486FetchByte(State, (PUCHAR)&Offset))
    {
        /* An exception occurred */
        return FALSE;
    }

    if (Condition)
    {
        /* Move the instruction pointer */
        if (Size) State->InstPtr.Long += Offset;
        else State->InstPtr.LowWord += Offset;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeJecxz)
{
    BOOLEAN Condition;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;
    CHAR Offset = 0;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xE3);

    NO_LOCK_PREFIX();
    TOGGLE_ADSIZE(Size);

    if (Size) Condition = (State->GeneralRegs[FAST486_REG_ECX].Long == 0);
    else Condition = (State->GeneralRegs[FAST486_REG_ECX].LowWord == 0);

    /* Fetch the offset */
    if (!Fast486FetchByte(State, (PUCHAR)&Offset))
    {
        /* An exception occurred */
        return FALSE;
    }

    if (Condition)
    {
        /* Move the instruction pointer */
        if (Size) State->InstPtr.Long += Offset;
        else State->InstPtr.LowWord += Offset;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCall)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xE8);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Size)
    {
        LONG Offset = 0;

        /* Fetch the offset */
        if (!Fast486FetchDword(State, (PULONG)&Offset))
        {
            /* An exception occurred */
            return FALSE;
        }

        /* Push the current value of the instruction pointer */
        if (!Fast486StackPush(State, State->InstPtr.Long))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Move the instruction pointer */
        State->InstPtr.Long += Offset;
    }
    else
    {
        SHORT Offset = 0;

        /* Fetch the offset */
        if (!Fast486FetchWord(State, (PUSHORT)&Offset))
        {
            /* An exception occurred */
            return FALSE;
        }

        /* Push the current value of the instruction pointer */
        if (!Fast486StackPush(State, State->InstPtr.Long))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Move the instruction pointer */
        State->InstPtr.LowWord += Offset;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeJmp)
{
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xE9);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    if (Size)
    {
        LONG Offset = 0;

        /* Fetch the offset */
        if (!Fast486FetchDword(State, (PULONG)&Offset))
        {
            /* An exception occurred */
            return FALSE;
        }

        /* Move the instruction pointer */
        State->InstPtr.Long += Offset;
    }
    else
    {
        SHORT Offset = 0;

        /* Fetch the offset */
        if (!Fast486FetchWord(State, (PUSHORT)&Offset))
        {
            /* An exception occurred */
            return FALSE;
        }

        /* Move the instruction pointer */
        State->InstPtr.Long += Offset;

        /* Clear the top half of EIP */
        State->InstPtr.Long &= 0xFFFF;
    }

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeJmpAbs)
{
    USHORT Segment = 0;
    ULONG Offset = 0;
    BOOLEAN Size = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xEA);

    TOGGLE_OPSIZE(Size);
    NO_LOCK_PREFIX();

    /* Fetch the offset */
    if (Size)
    {
        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }
    else
    {
        if (!Fast486FetchWord(State, (PUSHORT)&Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }

    /* Fetch the segment */
    if (!Fast486FetchWord(State, &Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load the new CS */
    if (!Fast486LoadSegment(State, FAST486_REG_CS, Segment))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Load new EIP */
    State->InstPtr.Long = Offset;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovAlOffset)
{
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;
    ULONG Offset;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA0);

    TOGGLE_ADSIZE(AddressSize);

    if (AddressSize)
    {
        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }
    else
    {
        USHORT WordOffset;

        if (!Fast486FetchWord(State, &WordOffset))
        {
            /* Exception occurred */
            return FALSE;
        }

        Offset = (ULONG)WordOffset;
    }

    /* Read from memory */
    return Fast486ReadMemory(State,
                             (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                             State->SegmentOverride : FAST486_REG_DS,
                             Offset,
                             FALSE,
                             &State->GeneralRegs[FAST486_REG_EAX].LowByte,
                             sizeof(UCHAR));
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovEaxOffset)
{
    BOOLEAN OperandSize, AddressSize;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA1);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (AddressSize)
    {
        ULONG Offset;

        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Read from memory */
        if (OperandSize)
        {
            return Fast486ReadMemory(State,
                                     (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                     State->SegmentOverride : FAST486_REG_DS,
                                     Offset,
                                     FALSE,
                                     &State->GeneralRegs[FAST486_REG_EAX].Long,
                                     sizeof(ULONG));
        }
        else
        {
            return Fast486ReadMemory(State,
                                     (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                     State->SegmentOverride : FAST486_REG_DS,
                                     Offset,
                                     FALSE,
                                     &State->GeneralRegs[FAST486_REG_EAX].LowWord,
                                     sizeof(USHORT));
        }
    }
    else
    {
        USHORT Offset;

        if (!Fast486FetchWord(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Read from memory */
        if (OperandSize)
        {
            return Fast486ReadMemory(State,
                                     (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                     State->SegmentOverride : FAST486_REG_DS,
                                     Offset,
                                     FALSE,
                                     &State->GeneralRegs[FAST486_REG_EAX].Long,
                                     sizeof(ULONG));
        }
        else
        {
            return Fast486ReadMemory(State,
                                     (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                     State->SegmentOverride : FAST486_REG_DS,
                                     Offset,
                                     FALSE,
                                     &State->GeneralRegs[FAST486_REG_EAX].LowWord,
                                     sizeof(USHORT));
        }
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovOffsetAl)
{
    BOOLEAN AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;
    ULONG Offset;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA2);

    TOGGLE_ADSIZE(AddressSize);

    if (AddressSize)
    {
        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }
    }
    else
    {
        USHORT WordOffset;

        if (!Fast486FetchWord(State, &WordOffset))
        {
            /* Exception occurred */
            return FALSE;
        }

        Offset = (ULONG)WordOffset;
    }

    /* Write to memory */
    return Fast486WriteMemory(State,
                             (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                             State->SegmentOverride : FAST486_REG_DS,
                             Offset,
                             &State->GeneralRegs[FAST486_REG_EAX].LowByte,
                             sizeof(UCHAR));
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovOffsetEax)
{
    BOOLEAN OperandSize, AddressSize;
    
    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xA3);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (AddressSize)
    {
        ULONG Offset;

        if (!Fast486FetchDword(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write to memory */
        if (OperandSize)
        {
            return Fast486WriteMemory(State,
                                      (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                      State->SegmentOverride : FAST486_REG_DS,
                                      Offset,
                                      &State->GeneralRegs[FAST486_REG_EAX].Long,
                                      sizeof(ULONG));
        }
        else
        {
            return Fast486WriteMemory(State,
                                      (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                      State->SegmentOverride : FAST486_REG_DS,
                                      Offset,
                                      &State->GeneralRegs[FAST486_REG_EAX].LowWord,
                                      sizeof(USHORT));
        }
    }
    else
    {
        USHORT Offset;

        if (!Fast486FetchWord(State, &Offset))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write to memory */
        if (OperandSize)
        {
            return Fast486WriteMemory(State,
                                      (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                      State->SegmentOverride : FAST486_REG_DS,
                                      Offset,
                                      &State->GeneralRegs[FAST486_REG_EAX].Long,
                                      sizeof(ULONG));
        }
        else
        {
            return Fast486WriteMemory(State,
                                      (State->PrefixFlags & FAST486_PREFIX_SEG) ?
                                      State->SegmentOverride : FAST486_REG_DS,
                                      Offset,
                                      &State->GeneralRegs[FAST486_REG_EAX].LowWord,
                                      sizeof(USHORT));
        }
    }
}

FAST486_OPCODE_HANDLER(Fast486OpcodeSalc)
{
    /* Make sure this is the right instruction */
    ASSERT(Opcode == 0xD6);

    NO_LOCK_PREFIX();

    /* Set all the bits of AL to CF */
    State->GeneralRegs[FAST486_REG_EAX].LowByte = State->Flags.Cf ? 0xFF : 0x00;

    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeMovs)
{
    ULONG Data, DataSize;
    BOOLEAN OperandSize, AddressSize;
    FAST486_SEG_REGS Segment = FAST486_REG_DS;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xA4);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (State->PrefixFlags & FAST486_PREFIX_SEG)
    {
        /* Use the override segment instead of DS */
        Segment = State->SegmentOverride;
    }

    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        if ((AddressSize && (State->GeneralRegs[FAST486_REG_ECX].Long == 0))
            || (!AddressSize && (State->GeneralRegs[FAST486_REG_ECX].LowWord == 0)))
        {
            /* Do nothing */
            return TRUE;
        }
    }

    /* Calculate the size */
    if (Opcode == 0xA4) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    /* Read from the source operand */
    if (!Fast486ReadMemory(State,
                           Segment,
                           AddressSize ? State->GeneralRegs[FAST486_REG_ESI].Long
                                       : State->GeneralRegs[FAST486_REG_ESI].LowWord,
                           FALSE,
                           &Data,
                           DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Write to the destination operand */
    if (!Fast486WriteMemory(State,
                            FAST486_REG_ES,
                            AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                        : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                            &Data,
                            DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Increment/decrement ESI and EDI */
    if (AddressSize)
    {
        if (!State->Flags.Df)
        {
            State->GeneralRegs[FAST486_REG_ESI].Long += DataSize;
            State->GeneralRegs[FAST486_REG_EDI].Long += DataSize;
        }
        else
        {
            State->GeneralRegs[FAST486_REG_ESI].Long -= DataSize;
            State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
        }
    }
    else
    {
        if (!State->Flags.Df)
        {
            State->GeneralRegs[FAST486_REG_ESI].LowWord += DataSize;
            State->GeneralRegs[FAST486_REG_EDI].LowWord += DataSize;
        }
        else
        {
            State->GeneralRegs[FAST486_REG_ESI].LowWord -= DataSize;
            State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
        }
    }

    // FIXME: This method is slow!
    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        if (AddressSize)
        {
            if (--State->GeneralRegs[FAST486_REG_ECX].Long)
            {
                /* Repeat the instruction */
                State->InstPtr = State->SavedInstPtr;
            }
        }
        else
        {
            if (--State->GeneralRegs[FAST486_REG_ECX].LowWord)
            {
                /* Repeat the instruction */
                State->InstPtr = State->SavedInstPtr;
            }
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeCmps)
{
    ULONG FirstValue = 0, SecondValue = 0, Result;
    ULONG DataSize, DataMask, SignFlag;
    BOOLEAN OperandSize, AddressSize;
    FAST486_SEG_REGS Segment = FAST486_REG_DS;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xA6);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (State->PrefixFlags & FAST486_PREFIX_SEG)
    {
        /* Use the override segment instead of DS */
        Segment = State->SegmentOverride;
    }

    if ((State->PrefixFlags & FAST486_PREFIX_REP)
        || (State->PrefixFlags & FAST486_PREFIX_REPNZ))
    {
        if ((AddressSize && (State->GeneralRegs[FAST486_REG_ECX].Long == 0))
            || (!AddressSize && (State->GeneralRegs[FAST486_REG_ECX].LowWord == 0)))
        {
            /* Do nothing */
            return TRUE;
        }
    }

    /* Calculate the size */
    if (Opcode == 0xA6) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    /* Calculate the mask and sign flag */
    SignFlag = 1 << ((DataSize * 8) - 1);
    DataMask = SignFlag | (SignFlag - 1);

    /* Read from the first source operand */
    if (!Fast486ReadMemory(State,
                           Segment,
                           AddressSize ? State->GeneralRegs[FAST486_REG_ESI].Long
                                       : State->GeneralRegs[FAST486_REG_ESI].LowWord,
                           FALSE,
                           &FirstValue,
                           DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Read from the second source operand */
    if (!Fast486ReadMemory(State,
                           FAST486_REG_ES,
                           AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                       : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                           FALSE,
                           &SecondValue,
                           DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    FirstValue &= DataMask;
    SecondValue &= DataMask;
    Result = (FirstValue - SecondValue) & DataMask;

    /* Update the flags */
    State->Flags.Cf = (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SignFlag) != (SecondValue & SignFlag))
                      && ((FirstValue & SignFlag) != (Result & SignFlag));
    State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SignFlag) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Increment/decrement ESI and EDI */
    if (AddressSize)
    {
        if (!State->Flags.Df)
        {
            State->GeneralRegs[FAST486_REG_ESI].Long += DataSize;
            State->GeneralRegs[FAST486_REG_EDI].Long += DataSize;
        }
        else
        {
            State->GeneralRegs[FAST486_REG_ESI].Long -= DataSize;
            State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
        }
    }
    else
    {
        if (!State->Flags.Df)
        {
            State->GeneralRegs[FAST486_REG_ESI].LowWord += DataSize;
            State->GeneralRegs[FAST486_REG_EDI].LowWord += DataSize;
        }
        else
        {
            State->GeneralRegs[FAST486_REG_ESI].LowWord -= DataSize;
            State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
        }
    }

    // FIXME: This method is slow!
    if ((State->PrefixFlags & FAST486_PREFIX_REP)
        || (State->PrefixFlags & FAST486_PREFIX_REPNZ))
    {
        BOOLEAN Repeat = TRUE;

        if (AddressSize)
        {
            if ((--State->GeneralRegs[FAST486_REG_ECX].Long) == 0)
            {
                /* ECX is 0 */
                Repeat = FALSE;
            }
        }
        else
        {
            if ((--State->GeneralRegs[FAST486_REG_ECX].LowWord) == 0)
            {
                /* CX is 0 */
                Repeat = FALSE;
            }
        }

        if (((State->PrefixFlags & FAST486_PREFIX_REP) && !State->Flags.Zf)
            || ((State->PrefixFlags & FAST486_PREFIX_REPNZ) && State->Flags.Zf))
        {
            /* REPZ with ZF = 0 or REPNZ with ZF = 1 */
            Repeat = FALSE;
        }

        if (Repeat)
        {
            /* Repeat the instruction */
            State->InstPtr = State->SavedInstPtr;
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeStos)
{
    ULONG DataSize;
    BOOLEAN OperandSize, AddressSize;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xAA);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    /* Calculate the size */
    if (Opcode == 0xAA) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        UCHAR Block[STRING_BLOCK_SIZE];
        ULONG Count = AddressSize ? State->GeneralRegs[FAST486_REG_ECX].Long
                                  : State->GeneralRegs[FAST486_REG_ECX].LowWord;

        /* Fill the memory block with the data */
        if (DataSize == sizeof(UCHAR))
        {
            RtlFillMemory(Block, sizeof(Block), State->GeneralRegs[FAST486_REG_EAX].LowByte);
        }
        else
        {
            ULONG i;

            for (i = 0; i < STRING_BLOCK_SIZE / DataSize; i++)
            {
                if (DataSize == sizeof(USHORT))
                {
                    ((PUSHORT)Block)[i] = State->GeneralRegs[FAST486_REG_EAX].LowWord;
                }
                else
                {
                    ((PULONG)Block)[i] = State->GeneralRegs[FAST486_REG_EAX].Long;
                }
            }
        }

        /* Transfer until finished */
        while (Count)
        {
            ULONG Processed = min(Count, STRING_BLOCK_SIZE / DataSize);

            /* Simulate the 16-bit wrap-around of DI in 16-bit address mode */
            if (!AddressSize)
            {
                ULONG MaxBytes = State->Flags.Df
                                 ? (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord
                                 : (0x10000 - (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord);

                Processed = min(Processed, MaxBytes / DataSize);
                if (Processed == 0) Processed = 1;
            }

            if (State->Flags.Df)
            {
                /* Set EDI to the starting location */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long -= (Processed - 1) * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord -= (Processed - 1) * DataSize;
            }

            /* Write to memory */
            if (!Fast486WriteMemory(State,
                                    FAST486_REG_ES,
                                    AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                                : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                                    Block,
                                    Processed * DataSize))
            {
                /* Set ECX */
                if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = Count;
                else State->GeneralRegs[FAST486_REG_ECX].LowWord = LOWORD(Count);

                /* Exception occurred */
                return FALSE;
            }

            if (!State->Flags.Df)
            {
                /* Increase EDI by the number of bytes transfered */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long += Processed * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord += Processed * DataSize;
            }
            else
            {
                /* Reduce EDI */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
            }

            /* Reduce the total count by the number processed in this run */
            Count -= Processed;
        }

        /* Clear ECX */
        if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = 0;
        else State->GeneralRegs[FAST486_REG_ECX].LowWord = 0;
    }
    else
    {
        /* Write to the destination operand */
        if (!Fast486WriteMemory(State,
                                FAST486_REG_ES,
                                AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                            : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                                &State->GeneralRegs[FAST486_REG_EAX].Long,
                                DataSize))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Increment/decrement EDI */
        if (AddressSize)
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].Long += DataSize;
            else State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
        }
        else
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].LowWord += DataSize;
            else State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeLods)
{
    ULONG DataSize;
    BOOLEAN OperandSize, AddressSize;
    FAST486_SEG_REGS Segment = FAST486_REG_DS;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xAC);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if (State->PrefixFlags & FAST486_PREFIX_SEG)
    {
        /* Use the override segment instead of DS */
        Segment = State->SegmentOverride;
    }

    /* Calculate the size */
    if (Opcode == 0xAC) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        ULONG Count = AddressSize ? State->GeneralRegs[FAST486_REG_ECX].Long
                                  : State->GeneralRegs[FAST486_REG_ECX].LowWord;

        /* If the count is 0, do nothing */
        if (Count == 0) return TRUE;

        /* Only the last entry will be loaded */
        if (!State->Flags.Df)
        {
            if (AddressSize) State->GeneralRegs[FAST486_REG_ESI].Long += (Count - 1) * DataSize;
            else State->GeneralRegs[FAST486_REG_ESI].LowWord += (Count - 1) * DataSize;
        }
        else
        {
            if (AddressSize) State->GeneralRegs[FAST486_REG_ESI].Long -= (Count - 1) * DataSize;
            else State->GeneralRegs[FAST486_REG_ESI].LowWord -= (Count - 1) * DataSize;
        }

        /* Clear ECX */
        if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = 0;
        else State->GeneralRegs[FAST486_REG_ECX].LowWord = 0;
    }

    /* Read from the source operand */
    if (!Fast486ReadMemory(State,
                           Segment,
                           AddressSize ? State->GeneralRegs[FAST486_REG_ESI].Long
                                       : State->GeneralRegs[FAST486_REG_ESI].LowWord,
                           FALSE,
                           &State->GeneralRegs[FAST486_REG_EAX].Long,
                           DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Increment/decrement ESI */
    if (AddressSize)
    {
        if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_ESI].Long += DataSize;
        else State->GeneralRegs[FAST486_REG_ESI].Long -= DataSize;
    }
    else
    {
        if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_ESI].LowWord += DataSize;
        else State->GeneralRegs[FAST486_REG_ESI].LowWord -= DataSize;
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeScas)
{
    ULONG FirstValue = State->GeneralRegs[FAST486_REG_EAX].Long;
    ULONG SecondValue = 0;
    ULONG Result;
    ULONG DataSize, DataMask, SignFlag;
    BOOLEAN OperandSize, AddressSize;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0xAE);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    if ((State->PrefixFlags & FAST486_PREFIX_REP)
        || (State->PrefixFlags & FAST486_PREFIX_REPNZ))
    {
        if ((AddressSize && (State->GeneralRegs[FAST486_REG_ECX].Long == 0))
            || (!AddressSize && (State->GeneralRegs[FAST486_REG_ECX].LowWord == 0)))
        {
            /* Do nothing */
            return TRUE;
        }
    }

    /* Calculate the size */
    if (Opcode == 0xAE) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    /* Calculate the mask and sign flag */
    SignFlag = 1 << ((DataSize * 8) - 1);
    DataMask = SignFlag | (SignFlag - 1);

    /* Read from the source operand */
    if (!Fast486ReadMemory(State,
                           FAST486_REG_ES,
                           AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                       : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                           FALSE,
                           &SecondValue,
                           DataSize))
    {
        /* Exception occurred */
        return FALSE;
    }

    /* Calculate the result */
    FirstValue &= DataMask;
    SecondValue &= DataMask;
    Result = (FirstValue - SecondValue) & DataMask;

    /* Update the flags */
    State->Flags.Cf = (FirstValue < SecondValue);
    State->Flags.Of = ((FirstValue & SignFlag) != (SecondValue & SignFlag))
                      && ((FirstValue & SignFlag) != (Result & SignFlag));
    State->Flags.Af = (FirstValue & 0x0F) < (SecondValue & 0x0F);
    State->Flags.Zf = (Result == 0);
    State->Flags.Sf = ((Result & SignFlag) != 0);
    State->Flags.Pf = Fast486CalculateParity(Result);

    /* Increment/decrement EDI */
    if (AddressSize)
    {
        if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].Long += DataSize;
        else State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
    }
    else
    {
        if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].LowWord += DataSize;
        else State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
    }

    // FIXME: This method is slow!
    if ((State->PrefixFlags & FAST486_PREFIX_REP)
        || (State->PrefixFlags & FAST486_PREFIX_REPNZ))
    {
        BOOLEAN Repeat = TRUE;

        if (AddressSize)
        {
            if ((--State->GeneralRegs[FAST486_REG_ECX].Long) == 0)
            {
                /* ECX is 0 */
                Repeat = FALSE;
            }
        }
        else
        {
            if ((--State->GeneralRegs[FAST486_REG_ECX].LowWord) == 0)
            {
                /* CX is 0 */
                Repeat = FALSE;
            }
        }

        if (((State->PrefixFlags & FAST486_PREFIX_REP) && !State->Flags.Zf)
            || ((State->PrefixFlags & FAST486_PREFIX_REPNZ) && State->Flags.Zf))
        {
            /* REPZ with ZF = 0 or REPNZ with ZF = 1 */
            Repeat = FALSE;
        }

        if (Repeat)
        {
            /* Repeat the instruction */
            State->InstPtr = State->SavedInstPtr;
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeIns)
{
    ULONG DataSize;
    BOOLEAN OperandSize, AddressSize;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0x6C);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    /* Calculate the size */
    if (Opcode == 0x6C) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        UCHAR Block[STRING_BLOCK_SIZE];
        ULONG Count = AddressSize ? State->GeneralRegs[FAST486_REG_ECX].Long
                                  : State->GeneralRegs[FAST486_REG_ECX].LowWord;

        /* Clear the memory block */
        RtlZeroMemory(Block, sizeof(Block));

        /* Transfer until finished */
        while (Count)
        {
            ULONG Processed = min(Count, STRING_BLOCK_SIZE / DataSize);

            /* Simulate the 16-bit wrap-around of DI in 16-bit address mode */
            if (!AddressSize)
            {
                ULONG MaxBytes = State->Flags.Df
                                 ? (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord
                                 : (0x10000 - (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord);

                Processed = min(Processed, MaxBytes / DataSize);
                if (Processed == 0) Processed = 1;
            }

            /* Read from the I/O port */
            State->IoReadCallback(State,
                                  State->GeneralRegs[FAST486_REG_EDX].LowWord,
                                  Block,
                                  Processed,
                                  DataSize);

            if (State->Flags.Df)
            {
                ULONG i, j;

                /* Reduce EDI by the number of bytes to transfer */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long -= Processed * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord -= Processed * DataSize;

                /* Reverse the block data */
                for (i = 0; i < Processed / 2; i++)
                {
                    /* Swap the values */
                    for (j = 0; j < DataSize; j++)
                    {
                        UCHAR Temp = Block[i * DataSize + j];
                        Block[i * DataSize + j] = Block[(Processed - i - 1) * DataSize + j];
                        Block[(Processed - i - 1) * DataSize + j] = Temp;
                    }
                }
            }

            /* Write to memory */
            if (!Fast486WriteMemory(State,
                                    FAST486_REG_ES,
                                    AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                                : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                                    Block,
                                    Processed * DataSize))
            {
                /* Set ECX */
                if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = Count;
                else State->GeneralRegs[FAST486_REG_ECX].LowWord = LOWORD(Count);

                /* Exception occurred */
                return FALSE;
            }

            if (!State->Flags.Df)
            {
                /* Increase EDI by the number of bytes transfered */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long += Processed * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord += Processed * DataSize;
            }

            /* Reduce the total count by the number processed in this run */
            Count -= Processed;
        }

        /* Clear ECX */
        if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = 0;
        else State->GeneralRegs[FAST486_REG_ECX].LowWord = 0;
    }
    else
    {
        ULONG Data = 0;

        /* Read from the I/O port */
        State->IoReadCallback(State,
                              State->GeneralRegs[FAST486_REG_EDX].LowWord,
                              &Data,
                              1,
                              DataSize);

        /* Write to the destination operand */
        if (!Fast486WriteMemory(State,
                                FAST486_REG_ES,
                                AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                            : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                                &Data,
                                DataSize))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Increment/decrement EDI */
        if (AddressSize)
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].Long += DataSize;
            else State->GeneralRegs[FAST486_REG_EDI].Long -= DataSize;
        }
        else
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_EDI].LowWord += DataSize;
            else State->GeneralRegs[FAST486_REG_EDI].LowWord -= DataSize;
        }
    }

    /* Return success */
    return TRUE;
}

FAST486_OPCODE_HANDLER(Fast486OpcodeOuts)
{
    ULONG DataSize;
    BOOLEAN OperandSize, AddressSize;

    OperandSize = AddressSize = State->SegmentRegs[FAST486_REG_CS].Size;

    /* Make sure this is the right instruction */
    ASSERT((Opcode & 0xFE) == 0x6E);

    TOGGLE_OPSIZE(OperandSize);
    TOGGLE_ADSIZE(AddressSize);

    /* Calculate the size */
    if (Opcode == 0x6E) DataSize = sizeof(UCHAR);
    else DataSize = OperandSize ? sizeof(ULONG) : sizeof(USHORT);

    if (State->PrefixFlags & FAST486_PREFIX_REP)
    {
        UCHAR Block[STRING_BLOCK_SIZE];
        ULONG Count = AddressSize ? State->GeneralRegs[FAST486_REG_ECX].Long
                                  : State->GeneralRegs[FAST486_REG_ECX].LowWord;

        /* Clear the memory block */
        RtlZeroMemory(Block, sizeof(Block));

        /* Transfer until finished */
        while (Count)
        {
            ULONG Processed = min(Count, STRING_BLOCK_SIZE / DataSize);

            /* Simulate the 16-bit wrap-around of DI in 16-bit address mode */
            if (!AddressSize)
            {
                ULONG MaxBytes = State->Flags.Df
                                 ? (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord
                                 : (0x10000 - (ULONG)State->GeneralRegs[FAST486_REG_EDI].LowWord);

                Processed = min(Processed, MaxBytes / DataSize);
                if (Processed == 0) Processed = 1;
            }

            /* Read from memory */
            if (!Fast486ReadMemory(State,
                                   FAST486_REG_ES,
                                   AddressSize ? State->GeneralRegs[FAST486_REG_EDI].Long
                                               : State->GeneralRegs[FAST486_REG_EDI].LowWord,
                                   FALSE,
                                   Block,
                                   Processed * DataSize))
            {
                /* Set ECX */
                if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = Count;
                else State->GeneralRegs[FAST486_REG_ECX].LowWord = LOWORD(Count);

                /* Exception occurred */
                return FALSE;
            }

            if (State->Flags.Df)
            {
                ULONG i, j;

                /* Reduce EDI by the number of bytes to transfer */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long -= Processed * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord -= Processed * DataSize;

                /* Reverse the block data */
                for (i = 0; i < Processed / 2; i++)
                {
                    /* Swap the values */
                    for (j = 0; j < DataSize; j++)
                    {
                        UCHAR Temp = Block[i * DataSize + j];
                        Block[i * DataSize + j] = Block[(Processed - i - 1) * DataSize + j];
                        Block[(Processed - i - 1) * DataSize + j] = Temp;
                    }
                }
            }

            /* Write to the I/O port */
            State->IoWriteCallback(State,
                                   State->GeneralRegs[FAST486_REG_EDX].LowWord,
                                   Block,
                                   Processed,
                                   DataSize);

            if (!State->Flags.Df)
            {
                /* Increase EDI by the number of bytes transfered */
                if (AddressSize) State->GeneralRegs[FAST486_REG_EDI].Long += Processed * DataSize;
                else State->GeneralRegs[FAST486_REG_EDI].LowWord += Processed * DataSize;
            }

            /* Reduce the total count by the number processed in this run */
            Count -= Processed;
        }

        /* Clear ECX */
        if (AddressSize) State->GeneralRegs[FAST486_REG_ECX].Long = 0;
        else State->GeneralRegs[FAST486_REG_ECX].LowWord = 0;
    }
    else
    {
        ULONG Data = 0;

        /* Read from the source operand */
        if (!Fast486ReadMemory(State,
                               FAST486_REG_DS,
                               AddressSize ? State->GeneralRegs[FAST486_REG_ESI].Long
                                           : State->GeneralRegs[FAST486_REG_ESI].LowWord,
                               FALSE,
                               &Data,
                               DataSize))
        {
            /* Exception occurred */
            return FALSE;
        }

        /* Write to the I/O port */
        State->IoWriteCallback(State,
                               State->GeneralRegs[FAST486_REG_EDX].LowWord,
                               &Data,
                               1,
                               DataSize);

        /* Increment/decrement ESI */
        if (AddressSize)
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_ESI].Long += DataSize;
            else State->GeneralRegs[FAST486_REG_ESI].Long -= DataSize;
        }
        else
        {
            if (!State->Flags.Df) State->GeneralRegs[FAST486_REG_ESI].LowWord += DataSize;
            else State->GeneralRegs[FAST486_REG_ESI].LowWord -= DataSize;
        }
    }

    /* Return success */
    return TRUE;
}
