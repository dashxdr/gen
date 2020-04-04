/*
     Definitions for the CPU-Modules
*/

#ifndef __m68000defs__
#define __m68000defs__


#include <stdlib.h>
#include <inttypes.h>

#include "memory.h"

extern unsigned char pending_interrupts;
extern short cpu_readop();

#define BYTE signed char
#define UBYTE unsigned char
#define UWORD unsigned short
#define WORD short
#define ULONG unsigned int
#define LONG int
#define CPTR unsigned int

extern void Exception(int nr, CPTR oldpc);


typedef void cpuop_func(ULONG);
extern cpuop_func *cpufunctbl[65536];


typedef char flagtype;
#define READ_MEML(a,b) asm ("mov (%%esi),%%eax \n\t bswap %%eax \n\t" :"=a" (b) :"S" (a))
#define READ_MEMW(a,b) asm ("mov (%%esi),%%ax\n\t  xchg %%al,%%ah" :"=a" (b) : "S" (a))

#define get_byte(a) cpu_readmem24((a)&0xffffff)
#define get_word(a) cpu_readmem24_word((a)&0xffffff)
#define get_long(a) cpu_readmem24_dword((a)&0xffffff)
#define put_byte(a,b) cpu_writemem24((a)&0xffffff,b)
#define put_word(a,b) cpu_writemem24_word((a)&0xffffff,b)
#define put_long(a,b) cpu_writemem24_dword((a)&0xffffff,b)

union flagu {
    struct {
        char v;
        char c;
        char n;
        char z;
    } flags;
    ULONG longflags;
};

extern int areg_byteinc[];
extern int movem_index1[256];
extern int movem_index2[256];
extern int movem_next[256];
extern int imm8_table[];
extern UBYTE *actadr;

typedef struct
{
            ULONG d[8];
            CPTR  a[8],usp,isp,msp;
            UWORD sr;
            flagtype t1;
            flagtype t0;
            flagtype s;
            flagtype m;
            flagtype x;
            flagtype stopped;
            int intmask;
            ULONG pc;

            ULONG vbr,sfc,dfc;
            double fp[8];
            ULONG fpcr,fpsr,fpiar;
} regstruct;

extern regstruct regs, lastint_regs;

extern union flagu intel_flag_lookup[256];
extern union flagu regflags;

#define ZFLG (regflags.flags.z)
#define NFLG (regflags.flags.n)
#define CFLG (regflags.flags.c)
#define VFLG (regflags.flags.v)


extern UWORD nextiword(void);
extern ULONG nextilong(void);
extern void m68k_setpc(CPTR newpc);
extern ULONG get_disp_ea(ULONG base);
extern CPTR m68k_getpc(void);
extern int cctrue(const int cc);
extern void MakeSR(void);
extern void MakeFromSR(void);

#include "68kint.h"

extern int raiseTrap(int n);

#endif
