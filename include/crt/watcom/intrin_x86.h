/*
        Compatibility <intrin_x86.h> header for Watcom equivalents of intrinsic
        Microsoft Visual C++ functions. Originally developed for GCC on the ReactOS
        (<http://www.reactos.org/>) and TinyKrnl (<http://www.tinykrnl.org/>)
        projects. GCC header edited for the Watcom compiler and ASM.

        GCC header: Copyright (c) 2006 KJK::Hyperion <hackbunny@reactos.com>

        Permission is hereby granted, free of charge, to any person obtaining a
        copy of this software and associated documentation files (the "Software"),
        to deal in the Software without restriction, including without limitation
        the rights to use, copy, modify, merge, publish, distribute, sublicense,
        and/or sell copies of the Software, and to permit persons to whom the
        Software is furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in
        all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
        FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
        DEALINGS IN THE SOFTWARE.
*/

#ifndef KJK_INTRIN_X86_H_
#define KJK_INTRIN_X86_H_

/*
        FIXME: review all "memory" clobbers, add/remove to match Visual C++
        behavior: some "obvious" memory barriers are not present in the Visual C++
        implementation - e.g. __stosX; on the other hand, some memory barriers that
        *are* present could have been missed
*/

/*
        NOTE: this is a *compatibility* header. Some functions may look wrong at
        first, but they're only "as wrong" as they would be on Visual C++. Our
        priority is compatibility

        NOTE: unlike most people who write inline asm for GCC, I didn't pull the
        constraints and the uses of __volatile__ out of my... hat. Do not touch
        them. I hate cargo cult programming: WATCOM remarkark OK hopefully I am 
        doing this correctly... 

        NOTE: be very careful with declaring "memory" clobbers. Some "obvious"
        barriers aren't there in Visual C++ (e.g. __stosX)

        NOTE: review all intrinsics with a return value, add/remove __volatile__
        where necessary. If an intrinsic whose value is ignored generates a no-op
        under Visual C++, __volatile__ must be omitted; if it always generates code
        (for example, if it has side effects), __volatile__ must be specified. GCC
        will only optimize out non-volatile asm blocks with outputs, so input-only
        blocks are safe. Oddities such as the non-volatile 'rdmsr' are intentional
        and follow Visual C++ behavior

        Pay attention to the type of barrier. Make it match with what Visual C++
        would use in the same case
*/

#ifdef __cplusplus
extern "C" {
#endif

/*** memcopy must be memmove ***/
__INTRIN_INLINE void* memcpy(void* dest, const void* source, size_t num)
{
    return memmove(dest, source, num);
}


/*** Stack frame juggling ***/
#define _ReturnAddress() (__builtin_return_address(0))
#define _AddressOfReturnAddress() (&(((void **)(__builtin_frame_address(0)))[1]))
/* TODO: __getcallerseflags but how??? */


/*** Memory barriers ***/

/* Open Watcom doesn't have possibility to handle this */
/* because barriers are not supported by compiler */

#define _ReadWriteBarrier()
#define _ReadBarrier _ReadWriteBarrier
#define _WriteBarrier _ReadWriteBarrier

__INTRIN_INLINE void _mm_mfence(void);
__INTRIN_INLINE void _mm_lfence(void);
__INTRIN_INLINE void _mm_sfence(void);
#ifdef __x86_64__
__INTRIN_INLINE void __faststorefence(void);
#endif

#pragma aux _mm_mfence = "mfence";
#pragma aux _mm_lfence = "lfence";
#pragma aux _mm_sfence = "sfence";

#ifdef __x86_64__
__INTRIN_INLINE void __faststorefence(void)
#pragma intrinsic(__faststorefence)
#endif


/*** Atomic operations ***/

__INTRIN_INLINE long _InterlockedAddLargeStatistic(volatile long long * const Addend, const long Value);
__INTRIN_INLINE unsigned char _interlockedbittestandreset(volatile long * a, const long b);
__INTRIN_INLINE unsigned char _interlockedbittestandset(volatile long * a, const long b);
#if defined(_M_AMD64)
__INTRIN_INLINE unsigned char _interlockedbittestandreset64(volatile long long * a, const long long b);
__INTRIN_INLINE unsigned char _interlockedbittestandset64(volatile long long * a, const long long b);
#endif

__INTRIN_INLINE char _InterlockedCompareExchange8(volatile char * const Destination, const char Exchange, const char Comperand);
__INTRIN_INLINE short _InterlockedCompareExchange16(volatile short * const Destination, const short Exchange, const short Comperand);
__INTRIN_INLINE long _InterlockedCompareExchange(volatile long * const Destination, const long Exchange, const long Comperand);
__INTRIN_INLINE void * _InterlockedCompareExchangePointer(void * volatile * const Destination, void * const Exchange, void * const Comperand);
__INTRIN_INLINE long _InterlockedExchange(volatile long * const Target, const long Value);
__INTRIN_INLINE void * _InterlockedExchangePointer(void * volatile * const Target, void * const Value);
__INTRIN_INLINE long _InterlockedExchangeAdd16(volatile short * const Addend, const short Value);
__INTRIN_INLINE long _InterlockedExchangeAdd(volatile long * const Addend, const long Value);
__INTRIN_INLINE char _InterlockedAnd8(volatile char * const value, const char mask);
__INTRIN_INLINE short _InterlockedAnd16(volatile short * const value, const short mask);
__INTRIN_INLINE long _InterlockedAnd(volatile long * const value, const long mask);
__INTRIN_INLINE char _InterlockedOr8(volatile char * const value, const char mask);
__INTRIN_INLINE short _InterlockedOr16(volatile short * const value, const short mask);
__INTRIN_INLINE long _InterlockedOr(volatile long * const value, const long mask);
__INTRIN_INLINE char _InterlockedXor8(volatile char * const value, const char mask);
__INTRIN_INLINE short _InterlockedXor16(volatile short * const value, const short mask);
__INTRIN_INLINE long _InterlockedXor(volatile long * const value, const long mask);
__INTRIN_INLINE long _InterlockedDecrement(volatile long * const lpAddend);
__INTRIN_INLINE long _InterlockedIncrement(volatile long * const lpAddend);
__INTRIN_INLINE short _InterlockedDecrement16(volatile short * const lpAddend);
__INTRIN_INLINE short _InterlockedIncrement16(volatile short * const lpAddend);
#if defined(_M_AMD64)
__INTRIN_INLINE long long _InterlockedDecrement64(volatile long long * const lpAddend);
__INTRIN_INLINE long long _InterlockedIncrement64(volatile long long * const lpAddend);
#endif

#pragma aux _InterlockedCompareExchange8 = \
        "lock cmpxchg [edx],cl" \
        parm [edx] [cl] [al];

#pragma aux _InterlockedCompareExchange16 = \
        "lock cmpxchg [edx],cx" \
        parm [edx] [cx] [ax];

#pragma aux _InterlockedCompareExchange = \
        "lock cmpxchg [edx],ecx" \
        parm [edx] [ecx] [eax];

#pragma aux _InterlockedCompareExchangePointer = \
        "lock cmpxchg [edx],ecx" \
        parm [edx] [ecx] [eax];

#pragma aux _InterlockedExchange = \
        "xchg [edx],eax" \
        parm [edx] [eax];

#pragma aux _InterlockedExchange = \
        "xchg [edx],eax" \
        parm [edx] [eax];

#pragma aux _InterlockedExchangeAdd16 = \
        "lock xadd [edx],ax" \
        parm [edx] [ax];

#pragma aux _InterlockedExchangeAdd = \
        "lock xadd [edx],eax" \
        parm [edx] [eax];

__INTRIN_INLINE char _InterlockedAnd8(volatile char * const value, const char mask)
{
        char x;
        char y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange8(value, x & mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE short _InterlockedAnd16(volatile short * const value, const short mask)
{
        short x;
        short y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange16(value, x & mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE long _InterlockedAnd(volatile long * const value, const long mask)
{
        long x;
        long y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange(value, x & mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE char _InterlockedOr8(volatile char * const value, const char mask)
{
        char x;
        char y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange8(value, x | mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE short _InterlockedOr16(volatile short * const value, const short mask)
{
        short x;
        short y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange16(value, x | mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE long _InterlockedOr(volatile long * const value, const long mask)
{
        long x;
        long y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange(value, x | mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE char _InterlockedXor8(volatile char * const value, const char mask)
{
        char x;
        char y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange8(value, x ^ mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE short _InterlockedXor16(volatile short * const value, const short mask)
{
        short x;
        short y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange16(value, x ^ mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE long _InterlockedXor(volatile long * const value, const long mask)
{
        long x;
        long y;

        y = *value;

        do
        {
                x = y;
                y = _InterlockedCompareExchange(value, x ^ mask, x);
        }
        while(y != x);

        return y;
}

__INTRIN_INLINE long _InterlockedDecrement(volatile long * const lpAddend)
{
        return _InterlockedExchangeAdd(lpAddend, -1) - 1;
}

__INTRIN_INLINE long _InterlockedIncrement(volatile long * const lpAddend)
{
        return _InterlockedExchangeAdd(lpAddend, 1) + 1;
}

__INTRIN_INLINE short _InterlockedDecrement16(volatile short * const lpAddend)
{
        return _InterlockedExchangeAdd16(lpAddend, -1) - 1;
}

__INTRIN_INLINE short _InterlockedIncrement16(volatile short * const lpAddend)
{
        return _InterlockedExchangeAdd16(lpAddend, 1) + 1;
}

#if defined(_M_AMD64)
__INTRIN_INLINE long long _InterlockedDecrement64(volatile long long * const lpAddend)
{
        return _InterlockedExchangeAdd64(lpAddend, -1) - 1;
}

__INTRIN_INLINE long long _InterlockedIncrement64(volatile long long * const lpAddend)
{
        return _InterlockedExchangeAdd64(lpAddend, 1) + 1;
}
#endif

#pragma aux _InterlockedCompareExchange64 = \
        "lock cmpxchg8b [edi]" \
        parm [edi] [ecx ebx] [edx eax];

#pragma aux _InterlockedAddLargeStatistic = \
        "lock add [edx],eax" \
        "jae short L1" \
        "lock adc [edx+4],0" \
     "L1:" \
        parm [edx] [eax] \
        value [eax];

#pragma aux _interlockedbittestandreset = \
        "lock btr [edx],eax" \
        "setb al" \
        parm [edx] [eax] \
        value [al];

#if defined(_M_AMD64)
__INTRIN_INLINE unsigned char _interlockedbittestandreset64(volatile long long * a, const long long b)
{
        unsigned char retval;
        __asm__("lock; btrq %[b], %[a]; setb %b[retval]" : [retval] "=r" (retval), [a] "+m" (*a) : [b] "Ir" (b) : "memory");
        return retval;
}
#endif

#pragma aux _interlockedbittestandset = \
        "lock bts [edx],eax" \
        "setc al" \
        parm [edx] [eax] \
        value [al];

#if defined(_M_AMD64)
__INTRIN_INLINE unsigned char _interlockedbittestandset64(volatile long long * a, const long long b)
{
        unsigned char retval;
        __asm__("lock; btsq %[b], %[a]; setc %b[retval]" : [retval] "=r" (retval), [a] "+m" (*a) : [b] "Ir" (b) : "memory");
        return retval;
}
#endif

/*** String operations ***/

__INTRIN_INLINE void __stosb(unsigned char * Dest, const unsigned char Data, size_t Count);
__INTRIN_INLINE void __stosw(unsigned short * Dest, const unsigned short Data, size_t Count);
__INTRIN_INLINE void __stosd(unsigned long * Dest, const unsigned long Data, size_t Count);
__INTRIN_INLINE void __movsb(unsigned char * Destination, const unsigned char * Source, size_t Count);
__INTRIN_INLINE void __movsw(unsigned short * Destination, const unsigned short * Source, size_t Count);
__INTRIN_INLINE void __movsd(unsigned long * Destination, const unsigned long * Source, size_t Count);
#ifdef _M_AMD64
__INTRIN_INLINE void __stosq(unsigned __int64 * Dest, const unsigned __int64 Data, size_t Count);
__INTRIN_INLINE void __movsq(unsigned long * Destination, const unsigned long * Source, size_t Count);
#endif


#pragma aux __stosb = \
        "rep stosb" \
        parm [edi] [esi] [ecx];

#pragma aux __stosw = \
        "rep stosw" \
        parm [edi] [esi] [ecx];

#pragma aux __stosd = \
        "rep stosd" \
        parm [edi] [esi] [ecx];

#ifdef _M_AMD64
__INTRIN_INLINE void __stosq(unsigned __int64 * Dest, const unsigned __int64 Data, size_t Count)
{
        __asm__ __volatile__
        (
                "rep; stosq" :
                [Dest] "=D" (Dest), [Count] "=c" (Count) :
                "[Dest]" (Dest), "a" (Data), "[Count]" (Count)
        );
}
#endif

#pragma aux __movsb = \
        "rep movsb" \
        parm [edi] [esi] [ecx];

#pragma aux __movsw = \
        "rep movsw" \
        parm [edi] [esi] [ecx];

#pragma aux __movsd = \
        "rep movsd" \
        parm [edi] [esi] [ecx];

#ifdef _M_AMD64
__INTRIN_INLINE void __movsq(unsigned long * Destination, const unsigned long * Source, size_t Count)
{
        __asm__ __volatile__
        (
                "rep; movsq" :
                [Destination] "=D" (Destination), [Source] "=S" (Source), [Count] "=c" (Count) :
                "[Destination]" (Destination), "[Source]" (Source), "[Count]" (Count)
        );
}
#endif

#if defined(_M_AMD64)

/*** GS segment addressing ***/

__INTRIN_INLINE void __writegsbyte(const unsigned long Offset, const unsigned char Data);
__INTRIN_INLINE void __writegsword(const unsigned long Offset, const unsigned short Data);
__INTRIN_INLINE void __writegsdword(const unsigned long Offset, const unsigned long Data);
__INTRIN_INLINE void __writegsqword(const unsigned long Offset, const unsigned __int64 Data);
__INTRIN_INLINE unsigned char __readgsbyte(const unsigned long Offset);
__INTRIN_INLINE unsigned short __readgsword(const unsigned long Offset);
__INTRIN_INLINE unsigned long __readgsdword(const unsigned long Offset);
__INTRIN_INLINE unsigned __int64 __readgsqword(const unsigned long Offset);
__INTRIN_INLINE void __incgsbyte(const unsigned long Offset);
__INTRIN_INLINE void __incgsword(const unsigned long Offset);
__INTRIN_INLINE void __incgsdword(const unsigned long Offset);
__INTRIN_INLINE void __addgsbyte(const unsigned long Offset, const unsigned char Data);
__INTRIN_INLINE void __addgsword(const unsigned long Offset, const unsigned short Data);
__INTRIN_INLINE void __addgsdword(const unsigned long Offset, const unsigned int Data);
__INTRIN_INLINE void __addgsqword(const unsigned long Offset, const unsigned __int64 Data);


__INTRIN_INLINE void __writegsbyte(const unsigned long Offset, const unsigned char Data)
{
        __asm__ __volatile__("movb %b[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __writegsword(const unsigned long Offset, const unsigned short Data)
{
        __asm__ __volatile__("movw %w[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __writegsdword(const unsigned long Offset, const unsigned long Data)
{
        __asm__ __volatile__("movl %k[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __writegsqword(const unsigned long Offset, const unsigned __int64 Data)
{
        __asm__ __volatile__("movq %q[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE unsigned char __readgsbyte(const unsigned long Offset)
{
        unsigned char value;
        __asm__ __volatile__("movb %%gs:%a[Offset], %b[value]" : [value] "=r" (value) : [Offset] "ir" (Offset));
        return value;
}

__INTRIN_INLINE unsigned short __readgsword(const unsigned long Offset)
{
        unsigned short value;
        __asm__ __volatile__("movw %%gs:%a[Offset], %w[value]" : [value] "=r" (value) : [Offset] "ir" (Offset));
        return value;
}

__INTRIN_INLINE unsigned long __readgsdword(const unsigned long Offset)
{
        unsigned long value;
        __asm__ __volatile__("movl %%gs:%a[Offset], %k[value]" : [value] "=r" (value) : [Offset] "ir" (Offset));
        return value;
}

__INTRIN_INLINE unsigned __int64 __readgsqword(const unsigned long Offset)
{
        unsigned __int64 value;
        __asm__ __volatile__("movq %%gs:%a[Offset], %q[value]" : [value] "=r" (value) : [Offset] "ir" (Offset));
        return value;
}

__INTRIN_INLINE void __incgsbyte(const unsigned long Offset)
{
        __asm__ __volatile__("incb %%gs:%a[Offset]" : : [Offset] "ir" (Offset) : "memory");
}

__INTRIN_INLINE void __incgsword(const unsigned long Offset)
{
        __asm__ __volatile__("incw %%gs:%a[Offset]" : : [Offset] "ir" (Offset) : "memory");
}

__INTRIN_INLINE void __incgsdword(const unsigned long Offset)
{
        __asm__ __volatile__("incl %%gs:%a[Offset]" : : [Offset] "ir" (Offset) : "memory");
}

__INTRIN_INLINE void __addgsbyte(const unsigned long Offset, const unsigned char Data)
{
        __asm__ __volatile__("addb %b[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __addgsword(const unsigned long Offset, const unsigned short Data)
{
        __asm__ __volatile__("addw %w[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __addgsdword(const unsigned long Offset, const unsigned int Data)
{
        __asm__ __volatile__("addl %k[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

__INTRIN_INLINE void __addgsqword(const unsigned long Offset, const unsigned __int64 Data)
{
        __asm__ __volatile__("addq %k[Data], %%gs:%a[Offset]" : : [Offset] "ir" (Offset), [Data] "ir" (Data) : "memory");
}

#else /* defined(_M_AMD64) */

/*** FS segment addressing ***/

__INTRIN_INLINE void __writefsbyte(const unsigned long Offset, const unsigned char Data);
__INTRIN_INLINE void __writefsword(const unsigned long Offset, const unsigned short Data);
__INTRIN_INLINE void __writefsdword(const unsigned long Offset, const unsigned long Data);
__INTRIN_INLINE unsigned char __readfsbyte(const unsigned long Offset);
__INTRIN_INLINE unsigned short __readfsword(const unsigned long Offset);
__INTRIN_INLINE unsigned long __readfsdword(const unsigned long Offset);
__INTRIN_INLINE void __incfsbyte(const unsigned long Offset);
__INTRIN_INLINE void __incfsword(const unsigned long Offset);
__INTRIN_INLINE void __incfsdword(const unsigned long Offset);
__INTRIN_INLINE void __addfsbyte(const unsigned long Offset, const unsigned char Data);
__INTRIN_INLINE void __addfsword(const unsigned long Offset, const unsigned short Data);
__INTRIN_INLINE void __addfsdword(const unsigned long Offset, const unsigned int Data);


#pragma aux __writefsbyte = \
        "mov fs:[edx],al" \
        parm [edx] [al];

#pragma aux __writefsword = \
        "mov fs:[edx],ax" \
        parm [edx] [ax];

#pragma aux __writefsdword = \
        "mov fs:[edx],eax" \
        parm [edx] [eax];

#pragma aux __readfsbyte = \
        "mov al,fs:[eax]" \
        parm [eax] value [al];

#pragma aux __readfsword = \
        "mov ax,fs:[eax]" \
        parm [eax] value [ax];

#pragma aux __readfsdword = \
        "mov eax,fs:[eax]" \
        parm [eax] value [eax];

#pragma aux __incfsbyte = \
        "inc byte ptr fs:[eax]" \
        parm [eax];

#pragma aux __incfsword = \
        "inc word ptr fs:[eax]" \
        parm [eax];

#pragma aux __incfsdword = \
        "inc dword ptr fs:[eax]" \
        parm [eax];

#pragma aux __addfsbyte = \
        "add fs:[edx],al" \
        parm [edx] [al];

#pragma aux __addfsword = \
        "mov fs:[edx],ax" \
        parm [edx] [ax];

#pragma aux __addfsdword = \
        "mov fs:[edx],eax" \
        parm [edx] [eax];



#endif /* defined(_M_AMD64) */


/*** Bit manipulation ***/

__INTRIN_INLINE unsigned char _BitScanForward(unsigned long * const Index, const unsigned long Mask);
__INTRIN_INLINE unsigned char _BitScanReverse(unsigned long * const Index, const unsigned long Mask);
__INTRIN_INLINE unsigned char _bittest(const long * const a, const long b);
#ifdef _M_AMD64
__INTRIN_INLINE unsigned char _bittest64(const __int64 * const a, const __int64 b);
#endif
__INTRIN_INLINE unsigned char _bittestandcomplement(long * const a, const long b);
__INTRIN_INLINE unsigned char _bittestandreset(long * const a, const long b);
__INTRIN_INLINE unsigned char _bittestandset(long * const a, const long b);
__INTRIN_INLINE unsigned char _rotl8(unsigned char value, unsigned char shift);
__INTRIN_INLINE unsigned short _rotl16(unsigned short value, unsigned char shift);
__INTRIN_INLINE unsigned int _rotl(unsigned int value, int shift);
__INTRIN_INLINE unsigned int _rotr(unsigned int value, int shift);
__INTRIN_INLINE unsigned char _rotr8(unsigned char value, unsigned char shift);
__INTRIN_INLINE unsigned short _rotr16(unsigned short value, unsigned char shift);
__INTRIN_INLINE unsigned long long __ll_lshift(const unsigned long long Mask, const int Bit);
__INTRIN_INLINE long long __ll_rshift(const long long Mask, const int Bit);
__INTRIN_INLINE unsigned long long __ull_rshift(const unsigned long long Mask, int Bit);
__INTRIN_INLINE unsigned short _byteswap_ushort(unsigned short value);
__INTRIN_INLINE unsigned long _byteswap_ulong(unsigned long value);
#ifdef _M_AMD64
__INTRIN_INLINE unsigned __int64 _byteswap_uint64(unsigned __int64 value);
#else
__INTRIN_INLINE unsigned __int64 _byteswap_uint64(unsigned __int64 value);
#endif


#pragma aux _BitScanForward = \
        "bsf eax,eax" \
        "mov [edx],eax" \
        "setnz al" \
        parm [edx] [eax] \
        value [al];

#pragma aux _BitScanReverse = \
        "bsr eax,eax" \
        "mov [edx],eax" \
        "setnz al" \
        parm [edx] [eax] \
        value [al];

#pragma aux _bittest = \
        "bt [edx],eax" \
        "setb al" \
        parm [edx] [eax] \
        value [al];

#ifdef _M_AMD64
__INTRIN_INLINE unsigned char _bittest64(const __int64 * const a, const __int64 b)
{
        unsigned char retval;

        if(__builtin_constant_p(b))
                __asm__("bt %[b], %[a]; setb %b[retval]" : [retval] "=q" (retval) : [a] "mr" (*(a + (b / 64))), [b] "Ir" (b % 64));
        else
                __asm__("bt %[b], %[a]; setb %b[retval]" : [retval] "=q" (retval) : [a] "mr" (*a), [b] "r" (b));

        return retval;
}
#endif

#pragma aux _bittestandcomplement = \
        "btc [edx],eax" \
        "setb al" \
        parm [edx] [eax] \
        value [al];

#pragma aux _bittestandreset = \
        "btr [edx],eax" \
        "setb al" \
        parm [edx] [eax] \
        value [al];

#pragma aux _bittestandset = \
        "bts [edx],eax" \
        "setb al" \
        parm [edx] [eax] \
        value [al];

#pragma aux _rotl8 = \
        "rol al,cl" \
        parm [al] [cl] \
        value [al];

#pragma aux _rotl16 = \
        "rol ax,cl" \
        parm [ax] [cl] \
        value [ax];

#pragma aux _rotl = \
        "rol eax,cl" \
        parm [eax] [ecx] \
        value [eax];

#pragma aux _rotr = \
        "ror eax,cl" \
        parm [eax] [ecx] \
        value [eax];

#pragma aux _rotr8 = \
        "ror al,cl" \
        parm [al] [cl] \
        value [al];

#pragma aux _rotr16 = \
        "ror ax,cl" \
        parm [ax] [cx] \
        value [ax];

#pragma aux __ll_lshift = \
        "shld edx,eax,cl" \
        "sal eax,cl" \
        parm [edx eax] [ecx] \
        value [edx eax];

#pragma aux __ll_rshift = \
        "shrd eax,edx,cl" \
        "sar edx,cl" \
        parm [edx eax] [ecx] \
        value [edx eax];

#pragma aux __ull_rshift = \
        "shrd eax,edx,cl" \
        "shr edx,cl" \
        parm [edx eax] [ecx] \
        value [edx eax];

#pragma aux _byteswap_ushort = \
        "ror ax,8" \
        parm [ax] \
        value [ax];

#pragma aux _byteswap_ulong = \
        "bswap eax" \
        parm [eax] \
        value [eax];

#ifdef _M_AMD64
__INTRIN_INLINE unsigned __int64 _byteswap_uint64(unsigned __int64 value)
{
        unsigned __int64 retval;
        __asm__("bswapq %[retval]" : [retval] "=r" (retval) : "[retval]" (value));
        return retval;
}
#else

#pragma aux _byteswap_uint64 = \
        "bswap eax" \
        "bswap edx" \
        "xchg eax,edx" \
        parm [edx eax] \
        value [edx eax];

#endif

/*** 64-bit math ***/

__INTRIN_INLINE long long __emul(const int a, const int b);
__INTRIN_INLINE unsigned long long __emulu(const unsigned int a, const unsigned int b);
#ifdef _M_AMD64
__INTRIN_INLINE __int64 __mulh(__int64 a, __int64 b);
__INTRIN_INLINE unsigned __int64 __umulh(unsigned __int64 a, unsigned __int64 b);
#endif


#pragma aux __emul = \
        "imul edx" \
        parm [edx] [eax] \
        value [edx eax];

#pragma aux __emulu = \
        "mul edx" \
        parm [edx] [eax] \
        value [edx eax];

#ifdef _M_AMD64

__INTRIN_INLINE __int64 __mulh(__int64 a, __int64 b)
{
        __int64 retval;
        __asm__("imulq %[b]" : "=d" (retval) : [a] "a" (a), [b] "rm" (b));
        return retval;
}

__INTRIN_INLINE unsigned __int64 __umulh(unsigned __int64 a, unsigned __int64 b)
{
        unsigned __int64 retval;
        __asm__("mulq %[b]" : "=d" (retval) : [a] "a" (a), [b] "rm" (b));
        return retval;
}

#endif

/*** Port I/O ***/

/* inp.. and outp.. are Open Watcom intrinsic */
#define __inbyte        inp
#define __inword        inpw
#define __indword       inpd
#define __outbyte       outp
#define __outword       outpw
#define __outdword      outpd

__INTRIN_INLINE void __inbytestring(unsigned short Port, unsigned char * Buffer, unsigned long Count);
__INTRIN_INLINE void __inwordstring(unsigned short Port, unsigned short * Buffer, unsigned long Count);
__INTRIN_INLINE void __indwordstring(unsigned short Port, unsigned long * Buffer, unsigned long Count);
__INTRIN_INLINE void __outbytestring(unsigned short const Port, const unsigned char * const Buffer, const unsigned long Count);
__INTRIN_INLINE void __outwordstring(unsigned short const Port, const unsigned short * const Buffer, const unsigned long Count);
__INTRIN_INLINE void __outdwordstring(unsigned short const Port, const unsigned long * const Buffer, const unsigned long Count);


#pragma aux __inbytestring = \
        "rep insb" \
        parm [dx] [edi] [ecx];

#pragma aux __inwordstring = \
        "rep insw" \
        parm [dx] [edi] [ecx];

#pragma aux __indwordstring = \
        "rep insd" \
        parm [dx] [edi] [ecx];

#pragma aux __outbytestring = \
        "rep outsb" \
        parm [dx] [esi] [ecx];

#pragma aux __outwordstring = \
        "rep outsw" \
        parm [dx] [esi] [ecx];

#pragma aux __outdwordstring = \
        "rep outsd" \
        parm [dx] [esi] [ecx];

/*** System information ***/

__INTRIN_INLINE void __cpuid(int CPUInfo [], const int InfoType);
__INTRIN_INLINE unsigned long long __rdtsc(void);
__INTRIN_INLINE void __writeeflags(uintptr_t Value);
__INTRIN_INLINE uintptr_t __readeflags(void);


#pragma aux __cpuid = \
        "cpuid" \
        "mov [edi],eax" \
        "mov [edi+4],ebx" \
        "mov [edi+8],ecx" \
        "mov [edi+12],edx" \
        parm [edi] [eax];

#ifdef _M_AMD64
__INTRIN_INLINE unsigned long long __rdtsc(void)
{
        unsigned long long low, high;
        __asm__ __volatile__("rdtsc" : "=a"(low), "=d"(high));
        return low | (high << 32);
}
#else
#pragma aux __rdtsc = \
        "rdtsc" \
        parm [eax] \
        value [edx eax];
#endif

#pragma aux __writeeflags = \
        "push eax" \
        "popf" \
        parm [eax];

#pragma aux __readeflags = \
        "pushf" \
        "pop eax" \
        value [eax];

/*** Interrupts ***/

/* _disable and _enable are Open Watcom intrinsic */

__INTRIN_INLINE void __int2c(void);
__INTRIN_INLINE void __halt(void);


#pragma aux __debugbreak = "int 3";

#pragma aux __int2c = "int 0x2c" ;

#pragma aux __halt = "hlt";

/*** Protected memory management ***/

__INTRIN_INLINE void __invlpg(void * const Address);
#ifdef _M_AMD64
__INTRIN_INLINE void __writecr0(const unsigned __int64 Data);
__INTRIN_INLINE void __writecr3(const unsigned __int64 Data);
__INTRIN_INLINE void __writecr4(const unsigned __int64 Data);
__INTRIN_INLINE void __writecr8(const unsigned __int64 Data);
__INTRIN_INLINE unsigned __int64 __readcr0(void);
__INTRIN_INLINE unsigned __int64 __readcr2(void);
__INTRIN_INLINE unsigned __int64 __readcr3(void);
__INTRIN_INLINE unsigned __int64 __readcr4(void);
__INTRIN_INLINE unsigned __int64 __readcr8(void);
__INTRIN_INLINE unsigned __int64 __readdr(unsigned int reg);
__INTRIN_INLINE void __writedr(unsigned reg, unsigned __int64 value);
#else /* _M_AMD64 */
__INTRIN_INLINE void __writecr0(const unsigned int Data);
__INTRIN_INLINE void __writecr3(const unsigned int Data);
__INTRIN_INLINE void __writecr4(const unsigned int Data);
__INTRIN_INLINE unsigned long __readcr0(void);
__INTRIN_INLINE unsigned long __readcr2(void);
__INTRIN_INLINE unsigned long __readcr3(void);
__INTRIN_INLINE unsigned long __readcr4(void);
__INTRIN_INLINE unsigned int __readdr(unsigned int reg);
__INTRIN_INLINE void __writedr(unsigned reg, unsigned int value);
#endif /* _M_AMD64 */


#ifdef _M_AMD64

__INTRIN_INLINE void __writecr0(const unsigned __int64 Data)
{
        __asm__("mov %[Data], %%cr0" : : [Data] "r" (Data) : "memory");
}

__INTRIN_INLINE void __writecr3(const unsigned __int64 Data)
{
        __asm__("mov %[Data], %%cr3" : : [Data] "r" (Data) : "memory");
}

__INTRIN_INLINE void __writecr4(const unsigned __int64 Data)
{
        __asm__("mov %[Data], %%cr4" : : [Data] "r" (Data) : "memory");
}

__INTRIN_INLINE void __writecr8(const unsigned __int64 Data)
{
        __asm__("mov %[Data], %%cr8" : : [Data] "r" (Data) : "memory");
}

__INTRIN_INLINE unsigned __int64 __readcr0(void)
{
        unsigned __int64 value;
        __asm__ __volatile__("mov %%cr0, %[value]" : [value] "=r" (value));
        return value;
}

__INTRIN_INLINE unsigned __int64 __readcr2(void)
{
        unsigned __int64 value;
        __asm__ __volatile__("mov %%cr2, %[value]" : [value] "=r" (value));
        return value;
}

__INTRIN_INLINE unsigned __int64 __readcr3(void)
{
        unsigned __int64 value;
        __asm__ __volatile__("mov %%cr3, %[value]" : [value] "=r" (value));
        return value;
}

__INTRIN_INLINE unsigned __int64 __readcr4(void)
{
        unsigned __int64 value;
        __asm__ __volatile__("mov %%cr4, %[value]" : [value] "=r" (value));
        return value;
}

__INTRIN_INLINE unsigned __int64 __readcr8(void)
{
        unsigned __int64 value;
        __asm__ __volatile__("movq %%cr8, %q[value]" : [value] "=r" (value));
        return value;
}

#else /* _M_AMD64 */

#pragma aux __writecr0 = \
        "mov cr0,eax" \
        parm [eax];

#pragma aux __writecr3 = \
        "mov cr3,eax" \
        parm [eax];

#pragma aux __writecr4 = \
        "mov cr4,eax" \
        parm [eax];

#pragma aux __readcr0 = \
        "mov eax,cr0" \
        value [eax];

#pragma aux __readcr2 = \
        "mov eax,cr2" \
        value [eax];

#pragma aux __readcr3 = \
        "mov eax,cr3" \
        value [eax];

#pragma aux __readcr4 = \
        "mov eax,cr4" \
        value [eax];

#endif /* _M_AMD64 */

#ifdef _M_AMD64

__INTRIN_INLINE unsigned __int64 __readdr(unsigned int reg)
{
        unsigned __int64 value;
        switch (reg)
        {
                case 0:
                        __asm__ __volatile__("movq %%dr0, %q[value]" : [value] "=r" (value));
                        break;
                case 1:
                        __asm__ __volatile__("movq %%dr1, %q[value]" : [value] "=r" (value));
                        break;
                case 2:
                        __asm__ __volatile__("movq %%dr2, %q[value]" : [value] "=r" (value));
                        break;
                case 3:
                        __asm__ __volatile__("movq %%dr3, %q[value]" : [value] "=r" (value));
                        break;
                case 4:
                        __asm__ __volatile__("movq %%dr4, %q[value]" : [value] "=r" (value));
                        break;
                case 5:
                        __asm__ __volatile__("movq %%dr5, %q[value]" : [value] "=r" (value));
                        break;
                case 6:
                        __asm__ __volatile__("movq %%dr6, %q[value]" : [value] "=r" (value));
                        break;
                case 7:
                        __asm__ __volatile__("movq %%dr7, %q[value]" : [value] "=r" (value));
                        break;
        }
        return value;
}

__INTRIN_INLINE void __writedr(unsigned reg, unsigned __int64 value)
{
        switch (reg)
        {
                case 0:
                        __asm__("movq %q[value], %%dr0" : : [value] "r" (value) : "memory");
                        break;
                case 1:
                        __asm__("movq %q[value], %%dr1" : : [value] "r" (value) : "memory");
                        break;
                case 2:
                        __asm__("movq %q[value], %%dr2" : : [value] "r" (value) : "memory");
                        break;
                case 3:
                        __asm__("movq %q[value], %%dr3" : : [value] "r" (value) : "memory");
                        break;
                case 4:
                        __asm__("movq %q[value], %%dr4" : : [value] "r" (value) : "memory");
                        break;
                case 5:
                        __asm__("movq %q[value], %%dr5" : : [value] "r" (value) : "memory");
                        break;
                case 6:
                        __asm__("movq %q[value], %%dr6" : : [value] "r" (value) : "memory");
                        break;
                case 7:
                        __asm__("movq %q[value], %%dr7" : : [value] "r" (value) : "memory");
                        break;
        }
}

#else /* _M_AMD64 */

extern unsigned int __readdr0( void );
#pragma aux __readdr0 = "mov eax,dr0";
extern unsigned int __readdr1( void );
#pragma aux __readdr1 = "mov eax,dr1";
extern unsigned int __readdr2( void );
#pragma aux __readdr2 = "mov eax,dr2";
extern unsigned int __readdr3( void );
#pragma aux __readdr3 = "mov eax,dr3";
extern unsigned int __readdr4( void );
#pragma aux __readdr4 = ".686p mov eax,dr4";
extern unsigned int __readdr5( void );
#pragma aux __readdr5 = ".686p mov eax,dr5";
extern unsigned int __readdr6( void );
#pragma aux __readdr6 = "mov eax,dr6";
extern unsigned int __readdr7( void );
#pragma aux __readdr7 = "mov eax,dr7";

__INTRIN_INLINE unsigned int __readdr(unsigned int reg)
{
        unsigned int value;
        switch (reg)
        {
                case 0:
                        value = __readdr0();
                        break;
                case 1:
                        value = __readdr1();
                        break;
                case 2:
                        value = __readdr2();
                        break;
                case 3:
                        value = __readdr3();
                        break;
                case 4:
                        value = __readdr4();
                        break;
                case 5:
                        value = __readdr5();
                        break;
                case 6:
                        value = __readdr6();
                        break;
                case 7:
                        value = __readdr7();
                        break;
        }
        return value;
}

extern void __writedr0( unsigned int );
#pragma aux __writedr0 = "mov dr0,eax";
extern void __writedr1( unsigned int );
#pragma aux __writedr1 = "mov dr1,eax";
extern void __writedr2( unsigned int );
#pragma aux __writedr2 = "mov dr2,eax";
extern void __writedr3( unsigned int );
#pragma aux __writedr3 = "mov dr3,eax";
extern void __writedr4( unsigned int );
#pragma aux __writedr4 = ".686p mov dr4,eax";
extern void __writedr5( unsigned int );
#pragma aux __writedr5 = ".686p mov dr5,eax";
extern void __writedr6( unsigned int );
#pragma aux __writedr6 = "mov dr6,eax";
extern void __writedr7( unsigned int );
#pragma aux __writedr7 = "mov dr7,eax";

__INTRIN_INLINE void __writedr(unsigned reg, unsigned int value)
{
        switch (reg)
        {
                case 0:
                        __writedr0( value );
                        break;
                case 1:
                        __writedr1( value );
                        break;
                case 2:
                        __writedr2( value );
                        break;
                case 3:
                        __writedr3( value );
                        break;
                case 4:
                        __writedr4( value );
                        break;
                case 5:
                        __writedr5( value );
                        break;
                case 6:
                        __writedr6( value );
                        break;
                case 7:
                        __writedr7( value );
                        break;
        }
}

#endif /* _M_AMD64 */

#pragma aux __invlpg = \
    "invlpg [eax]" \
    parm [eax];   


/*** System operations ***/

__INTRIN_INLINE unsigned long long __readmsr(const int reg);
__INTRIN_INLINE void __writemsr(const unsigned long Register, const unsigned long long Value);
__INTRIN_INLINE unsigned long long __readpmc(const int counter);
__INTRIN_INLINE unsigned long __segmentlimit(const unsigned long a);
__INTRIN_INLINE void __wbinvd(void);
__INTRIN_INLINE void __lidt(void *Source);
__INTRIN_INLINE void __sidt(void *Destination);


#ifdef _M_AMD64
__INTRIN_INLINE unsigned long long __readmsr(const int reg)
{
        unsigned long low, high;
        __asm__ __volatile__("rdmsr" : "=a" (low), "=d" (high) : "c" (reg));
        return ((unsigned long long)high << 32) | low;
}
#else
#pragma aux __readmsr = \
    "rdmsr" \
    parm [ecx] \
    value [edx eax];
#endif

#ifdef _M_AMD64
__INTRIN_INLINE void __writemsr(const unsigned long Register, const unsigned long long Value)
{
        __asm__ __volatile__("wrmsr" : : "a" (Value), "d" (Value >> 32), "c" (Register));
}
#else
#pragma aux __writemsr = \
    "wrmsr" \
    parm [edx eax] [ecx];
#endif

#pragma aux __readpmc = \
    "rdpmc" \
    parm [ecx] \
    value [edx eax];

#pragma aux __segmentlimit = \
    "lsl eax,eax" \
    parm [eax] \
    value [eax];

#pragma aux __wbinvd = "wbinvd";

#pragma aux __lidt = \
    "lidt fword ptr [eax]" \
    parm [eax];

#pragma aux __sidt = \
    "sidt fword ptr [eax]" \
    parm [eax];


/*** Misc operations ***/

__INTRIN_INLINE void _mm_pause(void);
__INTRIN_INLINE void __nop(void);

#pragma aux _mm_pause = 0xf3 0x90;
#pragma aux __nop = "nop";

#ifdef __cplusplus
}
#endif

#endif /* KJK_INTRIN_X86_H_ */

/* EOF */
