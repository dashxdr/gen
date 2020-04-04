#include "M68000.h"
#include "readcpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include "expr.h"
#include "command.h"
#include "em.h"
#include "fm.h"
#include <sys/timeb.h>
#include <sys/types.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <poll.h>

//#define PERFRAME 8896
#define PERFRAME (PERLINE*268)
#define PERLINE 64
#define HBLANK 8
#define NUMLINES 224
#define VINT (VBLANK + PERLINE) // (PERFRAME-400)
#define VBLANK (PERLINE*NUMLINES)
#define ENDMARK   0x01
#define Z80TICK   0x02
#define SCANLINE  0x04
#define HI_F      0x08
#define DISPLAY   0x10
#define VI_F      0x20

#define VB_F 0x800
#define HB_F 0x400
#define SAMPLESPERTICK (SAMPLERATE/60)
#define OFTEN 64
#define Z80CYCLES (4000000/60/(PERFRAME/OFTEN))
#define INTRESOLUTION 64

int soundhandle=-1;

short *soundbuffer;
int soundput, soundtake;

int on1=1,on2=1,on3=1, on4=1, onz=1;
int framecount=0;
int fps=0;
unsigned char fmreg1,fmreg2;
//#define CURRENTLINE ((intat-inttab)/PERLINE)
unsigned short *inttab,*intend,*intat;
int currentline;
int hintcount;
int next_hint;
int hint_limit;
int dcount=0;
int32_t starttime;

int get_hvpos(void)
{
int v;
	v = (currentline-1)<<8;
	if(currentline<NUMLINES)
		v += 40+215*((intat-inttab)%PERLINE)/PERLINE;
	return v & 0xffff;
}

void makeinttab(void)
{
int i,j;
unsigned short *p;
	intat=inttab;
	intend=inttab+PERFRAME;
	for(i=0;i<PERFRAME;i++)
	{
		inttab[i]=(i<VBLANK ? 0 : VB_F) |
			(!(i%OFTEN) ? Z80TICK : 0);
		j=i%PERLINE;
		if(!j)
		{
			j=i/PERLINE;
			inttab[i] |= SCANLINE;
			if(j==NUMLINES)
				inttab[i] |= DISPLAY;
		}
	}
	inttab[VINT]|=VI_F;
	for(i=0;i<NUMLINES;i++)
	{
		p=inttab+i*PERLINE;
		for(j=0;j<HBLANK;j++)
			*p++ |= HB_F;
	}
	inttab[PERFRAME-1]|=ENDMARK;
}

void updatehint(int newfreq)
{
	next_hint = newfreq;
}

void doview(void);
char trapcr;
char blanks;
char writeprotect;
int32_t *cpuhistory,*cpuput,*cpuend,*cputake;
int cpuinhistory;

int movem_index1[256];
int movem_index2[256];
int movem_next[256];
UBYTE *actadr;
int keymode=0;
int32_t lastpc=-1;
#define LINEBUFFLEN 128
char linebuff[LINEBUFFLEN],*linepntr;

regstruct regs, lastint_regs;

union flagu intel_flag_lookup[256];
union flagu regflags;

extern int cpu_interrupt(void);
extern void BuildCPU(void);

#define MC68000_interrupt() (cpu_interrupt())
#define cpu_readop(v) cpu_readmem24_word(v)

#define ReadMEM(A) (cpu_readmem24(A))
#define WriteMEM(A,V) (cpu_writemem24(A,V))

int MC68000_ICount;
uchar pending_interrupts;

static int InitStatus=0;
char exitflag;
unsigned char trace;

extern FILE * errorlog;

ushort *mainmemory;
int32_t mainlen;
ushort *ffpage;
unsigned char *a0page;

ushort gfxmap[256];
unsigned short gfxmask;
unsigned char gfxregs[32];
unsigned short gfxloc;
ushort *gfxmem,*gfxvscroll;
unsigned short gfxpoint;
unsigned char fillprime;


uchar c0mode;
uchar xmode;
ushort *xdest;
ushort xoffset;
ushort xmask;
void dofill(unsigned short v1)
{
uint32_t numwords;
int step;

	if(!xdest) return;
	numwords=(gfxregs[0x14]<<8)|gfxregs[0x13];
	++numwords;
	step=gfxregs[15];
	if(step==1)
	{
		int v2;
		while(numwords--)
		{
			v2=(xoffset>>1)&xmask;
			v1&=255;
			if(xoffset++&1)
				xdest[v2]=(xdest[v2]&0xff00) | v1;
			else
				xdest[v2]=(v1<<8) | (xdest[v2]&0x00ff);
		}
	} else
	{
		while(numwords--)
		{
			xdest[(xoffset>>1)&xmask]=v1;
			xoffset+=step;
		}
	}
}

void dumpregs(void)
{
int i,j;

	printf("register dump\n");
	for(j=0;j<24;j+=12)
	{
		for(i=0;i<12;i++)
			printf("%04x ",gfxregs[j+i]+0x8000+((i+j)<<8));
		printf("\n");
	}
}

void doblit(ushort v1)
{
uint32_t source;
unsigned short numwords,source2;
int step;

	if(~gfxregs[0x01]&0x10) {
		static int wtfc = 0;
		int max = 5;
		if(wtfc<max) printf("doblit with dma (m1) 0, pc=%x, %d/%d\n", regs.pc, ++wtfc, max);
		return;
	}
	source=((gfxregs[0x17]<<16)|(gfxregs[0x16]<<8)|gfxregs[0x15])<<1;
	source&=0xffffff;
	numwords=(gfxregs[0x14]<<8)|gfxregs[0x13];
	if(xmode<8)
	{
		if(!xmode && (v1&0x40) && (gfxregs[0x17]&0x80))
		{
/* this is only used by herzog zwei */
			uchar t1;
			ushort t2;
			step=gfxregs[15];
			source>>=1;
			while(numwords--)
			{
				t1=(source&1) ? gfxmem[((ushort)source)>>1] : (gfxmem[((ushort)source)>>1]>>8);
				t2=((ushort)xoffset)>>1;
				if(xoffset&1)
					gfxmem[t2]=(gfxmem[t2]&0xff00) | t1;
				else
					gfxmem[t2]=(t1<<8) | (gfxmem[t2]&0x00ff);
				xoffset+=step;
				source+=step;
			}
			return;
		}
		printf("blit with xmode<8, xmode=%d\n",xmode);
		dumpregs();
		enterdebug();
		return;
	}
	if(gfxregs[0x17]&0x80) {fillprime=1;return;}
	if(!xdest) return;
	step=gfxregs[15];
	if(source>=0xff0000)
	{
		source>>=1;
		source2=source;
		while(numwords--)
		{
			xdest[(xoffset>>1)&xmask]=ffpage[source2++ & 0x7fff];
			xoffset+=step;
		}
	} else if(source<mainlen)
	{
		if(source+numwords+numwords>mainlen)
			numwords=(mainlen-source)>>1;
		source>>=1;
		while(numwords--)
		{
			xdest[(xoffset>>1)&xmask]=mainmemory[source++];
			xoffset+=step;
		}
	}
}
void newxmode(int v1)
{
	xmode=v1&=15;
	switch(v1)
	{
	case 8:
	case 0:
		xdest=gfxmem;
		xmask=0x7fff;
		break;
	case 9:
	case 1:
		xdest=gfxvscroll;
		xmask=0x7f;
		break;
	case 12:
	case 2:
		xdest=gfxmap;
		xmask=0x3f;
		break;
	default:
		xdest=0;
		xmask=0;
		break;
	}
}
void deb(char *arg1,...)
{
char temp[64],*p;
static int online=0;
int len;
va_list ap;
	va_start(ap, arg1);
	vsprintf(temp,arg1,ap);
	va_end(ap);
	len=strlen(temp);
	online+=len;
	if(online>=80)
	{
		online=len;
		putchar('\n');
	}
	printf("%s", temp);
	p=temp+len;
	len=0;
	while(p-->temp && *p!='\n') ++len;
	if(p>=temp) online=len;
}


void writec0(short v1,unsigned short v2)
{
	int t;
	v1>>=2;
//deb("%d:%04x ",v1,v2);
	switch(c0mode | v1)
	{
	case 2: /* mode 1, write to c00000 or c00002 */
		c0mode=0;
	case 0: /* mode 0, write to c00000 or c00002 */
		if(xmode<8)
			newxmode(15);
		if(fillprime) {dofill(v2);fillprime=0;break;}
		if(xdest)
		{
			xdest[(xoffset>>1)&xmask]=v2;
			xoffset+=gfxregs[15];
		}
		break;
	case 1: /* mode 0, write to c00004 or c00006 */
		switch(v2>>14)
		{
		case 0:
			c0mode=2;
			xoffset=(xoffset&0xc000) | v2;
			newxmode(xmode&3);
			break;
		case 1:
			c0mode=2;
			xoffset=(xoffset&0xc000) | (v2&0x3fff);
			newxmode((xmode&3)|8);
			break;
		case 2:
			t=(v2>>8)&0x1f;
			gfxregs[t]=v2;
//			if(t==10 || !t) updatehint((v2&255)+1);
			if(t==10) updatehint((v2&255)+1);
			break;
		case 3:
			c0mode=2;
			xoffset=(xoffset&0xc0000) | (v2&0x3fff);
			newxmode((xmode&3)|12);
			break;
		}
		break;
	case 3: /* mode 1, write to c00004 or c00006 */
		c0mode=0;
		xoffset=(xoffset&0x3fff) | ((v2&3)<<14);
		newxmode((xmode&12) | ((v2&0x30)>>4));
		if(v2&0x80) doblit(v2);
	}
}
unsigned short readc0(void)
{
ushort val;

	if(c0mode || xmode>=8)
	{
		printf("Lockup!\n");
		c0mode=0;
		return 0;
	}
	c0mode=0;
	if(xdest)
	{
		val=xdest[(xoffset>>1)&xmask];
		xoffset+=gfxregs[15];
		return val;
	}
	printf("Invalid read xmode=%04x\n",xmode);
	return 0;
}


int gfxinit(void)
{
int i;
	gfxmem=malloc(0x10000);
	if(!gfxmem) return 1;
	gfxmask=0;
	gfxloc=0;
	for(i=0;i<32;i++) gfxregs[i]=0;
	return 0;
}

short a11100,a11200;

int32_t z80page;

unsigned Z80_RDMEM(dword a)
{

	if(a<0x2000)
		return a0page[a];
	else if(a>=0x8000)
		return cpu_readmem24(z80page | (a&0x7fff));
	else if(a>=0x4000 && a<=0x4003)
	{
		return YM2612Read(0,a-0x4000);
	}
	return 0;
}
void Z80_WRMEM(dword a,byte V)
{

	if(a<0x2000)
		a0page[a]=V;
	else if(a>=0x8000)
		cpu_writemem24(z80page | (a&0x7fff),V);
	else if(a==0x6000)
		z80page=((z80page>>1) & 0x7f8000) | (V&1 ? 0x800000 : 0);
	else if(a>=0x4000 && a<=0x4003)
	{
		YM2612Write(0,a-0x4000,V);
	}
}
byte Z80_In(byte Port)
{
printf("z80 in %x\n", Port);
	return 0;
}
void Z80_Out(byte Port,byte Value)
{
printf("z80 out %x, %x\n", Port, Value);
}
void Z80_Reti(void)
{
}
void Z80_Retn(void)
{
}
void Z80_Patch(Z80_Regs *Regs)
{
}
int Z80_Interrupt(void)
{
	return 0;
}

int jspad[]={MYUP,MYDOWN,MYLEFT,MYRIGHT,'x','c','z','v'};
int jsor[]={1,2,4,8,1<<12,1<<4,1<<5,1<<13};
int jspad2[]={'1', '2', '3', '4', '5', '6', '7', '8'};

int getporta(void)
{
int i;
int v=~0;

	for(i=0;i<8;++i)
		if(checkpressed(jspad[i]))
			v&= ~jsor[i];
	return v;
}

int getportb(void)
{
int i;
int v=~0;

	for(i=0;i<8;++i)
		if(checkpressed(jspad2[i]))
			v&= ~jsor[i];
	return v;
}



int aoo3_six,aoo3_toggle;
int aoo5_six,aoo5_toggle;
void writea10003(unsigned char d)
{
    if (aoo3_six>=0 && (d&0x40)==0 && aoo3_toggle ) aoo3_six++;

    if (aoo3_six>0xc00000) aoo3_six&=~0x400000;
    // keep it circling around a high value

    if (d&0x40) aoo3_toggle=1; else aoo3_toggle=0;
}
void writea10005(unsigned char d)
{
    if (aoo5_six>=0 && (d&0x40)==0 && aoo5_toggle ) aoo5_six++;

    if (aoo5_six>0xc00000) aoo5_six&=~0x400000;
    // keep it circling around a high value

    if (d&0x40) aoo5_toggle=1; else aoo5_toggle=0;
}
unsigned char reada10003(void)
{
int porta=getporta();
	if (aoo3_six==3)
	{
		// Extended pad info
		if (aoo3_toggle==0) return ((porta>>8)&0x30)+0x00;
		else return ((porta)&0x30)+0x40+((porta>>16)&0xf);
	} else
	{
		if (aoo3_toggle==0)
		{
			if (aoo3_six==4) return ((porta>>8)&0x30)+0x00+0x0f;
			else return ((porta>>8)&0x30)+0x00+(porta&3);
		} else return ((porta)&0x30)+0x40+(porta&15);
	}
}
unsigned char reada10005(void)
{
int portb=getportb();
	if (aoo5_six==3)
	{
		// Extended pad info
		if (aoo5_toggle==0x00) return ((portb>>8)&0x30)+0x00;
		else return ((portb)&0x30)+0x40+((portb>>16)&0xf);
	} else
	{
		if (aoo5_toggle==0x00)
		{
			if (aoo5_six==4) return ((portb>>8)&0x30)+0x00+0x0f;
			else return ((portb>>8)&0x30)+0x00+(portb&3);
		} else return ((portb)&0x30)+0x40+(portb&15);
	}
}

void trysomez80(void)
{
	if((a11200&0x100) && !(a11100&0x100)) if(onz) doz80(20);
}

void cpu_writemem24(uint32_t v1,unsigned char v2)
{
	v1&=0xffffff;
	if(v1>=0xff0000)
	{
		ushort v3;
		v3=((ushort)v1)>>1;
		if(v1&1)
			ffpage[v3]=(ffpage[v3]&0xff00) | v2;
		else
			ffpage[v3]=(v2<<8) | (ffpage[v3]&0x00ff);
	}
	else if(v1==0xa11100)
	{
		a11100=v2<<8;
		trysomez80();
	}
	else if(v1==0xa11200)
	{
		a11200=v2<<8;
		if(!(a11200&0x100)) Z80_Reset();
		else trysomez80();
	}
	else if(v1==0xa10003) writea10003(v2);
	else if(v1==0xa10005) writea10005(v2);
	else if(v1>=0xa00000 && v1<0xa10000)
	{
		v1&=0xffff;
//		if(a11100&a11200&0x100)
		{
			if(v1<0x2000) a0page[v1]=v2;
			else if(v1>=0x4000 && v1<=0x4003) YM2612Write(0,v1-0x4000,v2);
		}
	} else if(v1>=0xc00000 && v1<0xc00008)
	{
		writec0(v1&7,v2);
//		printf("%06x <- %02x\n",v1,v2);
	} else if(v1<mainlen)
	{
		if(!writeprotect)
		{
			int32_t v3;
			v3=v1>>1;
			if(v1&1)
				mainmemory[v3]=(mainmemory[v3]&0xff00) | v2;
			else
				mainmemory[v3]=(v2<<8) | (mainmemory[v3]&0x00ff);
		}
	} else if(v1==0xc00011)
	{
//		printf("Write %02x to PSG!\n", v2);
	}
}
void cpu_writemem24_word(uint32_t v1,unsigned short v2)
{
	v1&=0xffffff;
	if(v1>=0xff0000)
	{
		ffpage[(v1&0xffff)>>1]=v2;
	}
	else if((v1&0xfffff8)==0xc00000)
		writec0(v1&7,v2);
	else if(v1==0xa11100)
	{
		a11100=v2;
		trysomez80();
	}
	else if(v1==0xa11200)
	{
		a11200=v2;
		if(!(v2&0x100)) Z80_Reset();
		else trysomez80();
	}
	else if(v1<mainlen)
	{
		if(!writeprotect)
			mainmemory[v1>>1]=v2;
	} else if(v1>=0xa00000 && v1<0xa10000) printf("16 bit access to A0 page %x\n",regs.pc);
}

void cpu_writemem24_dword(uint32_t v1,uint32_t v2)
{
	cpu_writemem24_word(v1,(unsigned short)(v2>>16));
	cpu_writemem24_word(v1+2,(unsigned short) v2);
}

unsigned char cpu_readmem24(uint32_t v1)
{
	v1&=0xffffff;
	if(v1<mainlen)
	{
		if(v1&1)
			return mainmemory[v1>>1];
		else
			return mainmemory[v1>>1]>>8;
	}
	else if(v1>=0xff0000)
	{
		if(v1&1)
			return ffpage[((ushort)v1)>>1];
		else
			return ffpage[((ushort)v1)>>1]>>8;
	}
	else if(v1==0xa10003) return reada10003();
	else if(v1==0xa10005) return reada10005();
	else if(v1>=0xa10004 && v1<0xa10020) return 0;
	else if(v1==0xa11100)
		return (~a11100|~a11200)>>8;
	else if(v1>=0xa00000 && v1<0xa10000)
	{
		v1&=0xffff;
		if(a11100&a11200&0x100)
		{
			if(v1<0x2000) return a0page[v1];
			if(v1>=0x4000 && v1<=0x4003) return YM2612Read(0,v1-0x4000);
		}
	} else if(v1==0xc00005 || v1==0xc00007)
		return 0x80 | (*intat>>8);
	else if(v1==0xa10001) return 0xa0;
	else if(v1==0xc00008) return get_hvpos()>>8;
	else if(v1==0xc00009) return get_hvpos();
	return (v1&1) ? 0x00 : 0x61;
}

unsigned short cpu_readmem24_word(uint32_t v1)
{
	v1&=0xffffff;
	if(v1<mainlen)
		return mainmemory[v1>>1];
	else if(v1>=0xff0000)
		return ffpage[((ushort)v1)>>1];
	else if(v1==0xa11100)
		return ~a11100|~a11200;
	else if(v1>=0xa10004 && v1<0xa10020) return 0;
	else if(v1==0xc00004 || v1==0xc00006)
		return 0x6280 | (*intat>>8);
	else if(v1==0xc00000 || v1==0xc00002)
		return readc0();
	else if(v1==0xc00008) return get_hvpos();
	else return 0x6100;
}

uint32_t cpu_readmem24_dword(uint32_t v1)
{
	return (cpu_readmem24_word(v1)<<16L) | cpu_readmem24_word(v1+2);
}




void m68k_dumpstate()
{
   int i;
   CPTR nextpc;
   for(i = 0; i < 8; i++){
      printf("D%d: %08x ", i, regs.d[i]);
      if ((i & 3) == 3) printf("\n");
   }
   for(i = 0; i < 8; i++){
      printf("A%d: %08x ", i, regs.a[i]);
      if ((i & 3) == 3) printf("\n");
   }
    if (regs.s == 0) regs.usp = regs.a[7];
   if (regs.s && regs.m) regs.msp = regs.a[7];
   if (regs.s && regs.m == 0) regs.isp = regs.a[7];
   printf("USP=%08x ISP=%08x MSP=%08x VBR=%08x SR=%08x\n",
	  regs.usp,regs.isp,regs.msp,regs.vbr,regs.sr);
   printf ("T=%d%d S=%d M=%d N=%d Z=%d V=%d C=%d IMASK=%d\n",
	   regs.t1, regs.t0, regs.s, regs.m,
	   NFLG, ZFLG, VFLG, CFLG, regs.intmask);
   for(i = 0; i < 8; i++){
	printf("FP%d: %g ", i, regs.fp[i]);
      if ((i & 3) == 3) printf("\n");
   }
   printf("N=%d Z=%d I=%d NAN=%d\n",
	  (regs.fpsr & 0x8000000) != 0,
	  (regs.fpsr & 0x4000000) != 0,
	  (regs.fpsr & 0x2000000) != 0,
	  (regs.fpsr & 0x1000000) != 0);
   MC68000_disasm(m68k_getpc(), &nextpc, 1);
   printf("Next PC = 0x%0x\n", nextpc);
}



int32_t gtime()
{
struct timeb tb;

	ftime(&tb);
	return tb.time*1000 + tb.millitm;
}


static void initCPU(void)
{
    int i,j;

    for (i = 0 ; i < 256 ; i++) {
      for (j = 0 ; j < 8 ; j++) {
       if (i & (1 << j)) break;
      }
     movem_index1[i] = j;
     movem_index2[i] = 7-j;
     movem_next[i] = i & (~(1 << j));
    }
    for (i = 0; i < 256; i++) {
         intel_flag_lookup[i].flags.c = !!(i & 1);
         intel_flag_lookup[i].flags.z = !!(i & 64);
         intel_flag_lookup[i].flags.n = !!(i & 128);
         intel_flag_lookup[i].flags.v = 0;
    }

}

void Exception(int nr, CPTR oldpc)
{
   MakeSR();

   if(!regs.s) {
      regs.a[7]=regs.isp;
      regs.s=1;
   }

   regs.a[7] -= 4;
   put_long (regs.a[7], m68k_getpc ());
   regs.a[7] -= 2;
   put_word (regs.a[7], regs.sr);
   m68k_setpc(get_long(regs.vbr + 4*nr));

   regs.t1 = regs.t0 = regs.m = 0;
}

void Exception2(int nr, int level)
{
   MakeSR();

   if(!regs.s) {
      regs.a[7]=regs.isp;
      regs.s=1;
   }

   regs.a[7] -= 4;
   put_long (regs.a[7], m68k_getpc ());
   regs.a[7] -= 2;
   put_word (regs.a[7], regs.sr);
   m68k_setpc(get_long(regs.vbr + 4*nr));

   regs.intmask=level;
   regs.t1 = regs.t0 = regs.m = 0;
}

void Interrupt68k(int level)
{
   if(level>=regs.intmask)
   {
   	Exception(24+level,0);
   	pending_interrupts &= ~(1 << (level-1));	/* ASG 971105 */
   }
}

void Initialisation() {
   /* Init 68000 emulator */
   BuildCPU();
   initCPU();
}

void MC68000_Reset(void)	/* ASG 971105 */
{
if (!InitStatus)
{
	Initialisation();
	InitStatus=1;
	currentline = 0;
//	hintcount = 0;
}

/* ASG 971105   MC68000_IPeriod = IPeriod;
  icount = IPeriod;*/

   regs.a[7]=get_long(0);
   m68k_setpc(get_long(4));

   regs.s = 1;
   regs.m = 0;
   regs.stopped = 0;
   regs.t1 = 0;
   regs.t0 = 0;
   ZFLG = CFLG = NFLG = VFLG = 0;
   regs.intmask = 7;
   regs.vbr = regs.sfc = regs.dfc = 0;
   regs.fpcr = regs.fpsr = regs.fpiar = 0;

   pending_interrupts = 0;		/* ASG 971105 */
}


void MC68000_SetRegs(MC68000_Regs *src)
{
	regs = src->regs;
	NFLG = (regs.sr >> 3) & 1;
	ZFLG = (regs.sr >> 2) & 1;
	VFLG = (regs.sr >> 1) & 1;
	CFLG = regs.sr & 1;
	pending_interrupts = src->pending_interrupts;
}

void MC68000_GetRegs(MC68000_Regs *dst)
{
	regs.sr = (regs.sr & 0xfff0) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1) |
	CFLG;
	dst->regs = regs;
	dst->pending_interrupts = pending_interrupts;
}

/* ASG 971105 */
void MC68000_Cause_Interrupt(int level)
{
	if (level >= 1 && level <= 7)
		pending_interrupts |= (1 << (level-1));
}

/* ASG 971105 */
void MC68000_Clear_Pending_Interrupts(void)
{
	pending_interrupts = 0;
}

/* ASG 971105 */
int  MC68000_GetPC(void)
{
	return regs.pc;
}


void sound_process(int t)
{
void *tbuf[2];
int s1,s2;
FMSAMPLE buff0[4096], buff1[4096];
int i;
int n;

	s1=SAMPLESPERTICK*t/PERFRAME;
	s2=SAMPLESPERTICK*(t+OFTEN)/PERFRAME;

	tbuf[0] = buff0;
	tbuf[1] = buff1;

	t=s2-s1;
	if(t>0)
	{
		YM2612UpdateOne(0, (void **)tbuf, t);

		for(i=0;i<t;++i)
		{
			soundbuffer[soundput]=buff0[i];
			soundbuffer[soundput+1]=buff1[i];
			n=soundput + 2;
			soundput = (n==SOUNDBUFFERSIZE) ? 0 : n;
		}
	}
}

int soundinit(void)
{
//	int frag,value;

	soundbuffer=malloc(SOUNDBUFFERSIZE*sizeof(soundbuffer[0]));
	if(!soundbuffer)
	{
		printf("Could not allocate soundbuffer\n");
		return 10;
	}
	soundput = soundtake = 0;

	return 0;
}



/* Execute one 68000 instruction */

/* ASG 971105 */
int MC68000_Execute(int cycles)
{

	MC68000_ICount = cycles;
	do
	{
		MC68000_ICount -= 15;

		if(keymode) break;
		stepone(1);

	}
	while (MC68000_ICount > 0);

   return (cycles - MC68000_ICount);
}

unsigned char inttable[128] =
{
0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

int ic=0;

void stepone(int num)
{
UWORD opcode;
unsigned char t;
//printf("%x\n",regs.pc);
int oldmode = keymode;

	while(keymode==oldmode && num--)
	{
		if((t=*intat++))
		{
			if(t&Z80TICK)
			{
				t&=~Z80TICK;
				if((a11200&0x100) && !(a11100&0x100)) if(onz) doz80(Z80CYCLES);
				sound_process(intat-inttab-1);
			}
			if(t&ENDMARK)
			{
 				t&=~ENDMARK;
				intat=inttab;
//				Z80_int();
				hintcount = 0;
				hint_limit = next_hint;
				currentline = 0;
			}
			if(t&VI_F)
				Z80_int();

			if(t&SCANLINE)
			{
				t&=~SCANLINE;
				if(currentline<224)
				{
					makeline(currentline++);
					if(++hintcount >= hint_limit)
					{
						if(*gfxregs&0x10)
						{
							hintcount = 0;
							hint_limit = next_hint;
							t |= HI_F;
						}
					}
				} else
					if(currentline<255)
						++currentline;
			}
			if(t&DISPLAY)
			{
				t&=~DISPLAY;
				display(1);
			}
			if(pending_interrupts|=t)
			{
				int level=inttable[pending_interrupts];
				if (level>regs.intmask )
				{
				   	Exception2(24+level,level);
				   	pending_interrupts &= ~(1 << (level-1));
				}
			}
		}

		lastpc=regs.pc;
#define CHECK 1
		if(breakpoint==lastpc && !keymode) {breakpoint=lastpc=-1;enterdebug();return;}
		*cpuput++=lastpc;
		if(cpuput==cpuend) cpuput=cpuhistory;
		if(cpuinhistory<HISTORYLEN) ++cpuinhistory;

		opcode=nextiword();
		cpufunctbl[opcode](opcode);
	}
}




union flagu regflags;
regstruct regs;


int movem_index1[256];
int movem_next[256];


UWORD nextiword(void)
{
    unsigned int i=regs.pc;
    regs.pc+=2;
    return cpu_readop(i);	/* ASG 971108 */
}

ULONG nextilong(void)
{
    unsigned int i=regs.pc;
    regs.pc+=4;
    return (cpu_readop(i)<<16L) | (cpu_readop(i+2));
}
void m68k_setpc(CPTR newpc)
{
    regs.pc = newpc;
}
CPTR m68k_getpc(void)
{
    return regs.pc;
}
ULONG get_disp_ea (ULONG base)
{
   UWORD dp = nextiword();
        int reg = (dp >> 12) & 7;
        LONG regd = dp & 0x8000 ? regs.a[reg] : regs.d[reg];
        if ((dp & 0x800) == 0)
            regd = (LONG)(WORD)regd;
        return base + (BYTE)(dp) + regd;
}
int cctrue(const int cc)
{
            switch(cc){
              case 0: return 1;                       /* T */
              case 1: return 0;                       /* F */
              case 2: return !CFLG && !ZFLG;          /* HI */
              case 3: return CFLG || ZFLG;            /* LS */
              case 4: return !CFLG;                   /* CC */
              case 5: return CFLG;                    /* CS */
              case 6: return !ZFLG;                   /* NE */
              case 7: return ZFLG;                    /* EQ */
              case 8: return !VFLG;                   /* VC */
              case 9: return VFLG;                    /* VS */
              case 10:return !NFLG;                   /* PL */
              case 11:return NFLG;                    /* MI */
              case 12:return NFLG == VFLG;            /* GE */
              case 13:return NFLG != VFLG;            /* LT */
              case 14:return !ZFLG && (NFLG == VFLG); /* GT */
              case 15:return ZFLG || (NFLG != VFLG);  /* LE */
             }
             abort();
             return 0;
}
void MakeSR(void)
{
    regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
      | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
      | (regs.x << 4) | (NFLG << 3) | (ZFLG << 2) | (VFLG << 1)
      |  CFLG);
}
void MakeFromSR(void)
{
  /*  int oldm = regs.m; */
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.t0 = (regs.sr >> 14) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    regs.x = (regs.sr >> 4) & 1;
    NFLG = (regs.sr >> 3) & 1;
    ZFLG = (regs.sr >> 2) & 1;
    VFLG = (regs.sr >> 1) & 1;
    CFLG = regs.sr & 1;

    if (olds != regs.s) {
       if (olds) {
         regs.isp = regs.a[7];
         regs.a[7] = regs.usp;
        } else {
           regs.usp = regs.a[7];
           regs.a[7] = regs.isp;
        }
     }

}
uchar doit[]={1,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1};

void showfps(void)
{
int32_t diff;

	diff=gtime()-starttime;
	if(diff)
		printf("%3.2f f/sec\n",dcount*1000.0/diff);
}
void writewords(int file,ushort *from,int len)
{
uchar buffer[1024],*p;
	len>>=1;
	p=buffer;
	int res;
	while(len)
	{
		*p++=*from>>8;
		*p++=*from++;
		if(p==buffer+sizeof(buffer))
		{
			res = write(file,buffer,p-buffer);res=res;
			p=buffer;
		}
		--len;
	}
	if(p>buffer)
		res=write(file,buffer,p-buffer);
	res=res;
}
void writegfx()
{
int file;
int res;

	file=creat("gfx",0600);
	if(file>=0)
	{
		writewords(file,gfxmem,0x10000);
		writewords(file,gfxmap,256);
		res=write(file,gfxregs,32);res=res;
		writewords(file,gfxvscroll,0x100);
		close(file);
	}
}

static int rl[2];
static int rPid;
void killRl(void) {
	if(rPid>0) {
		kill(rPid, SIGINT);
		rPid = 0;
	}
}
void setupReadline(void) {
	int res = pipe(rl);res=res;
	rPid=fork();
	if(rPid) {
		close(rl[1]);
		atexit(killRl);
	} else {
		close(rl[0]);
		for(;;) {
			char *p = readline("");
			if(!p) {sleep(60);continue;}
//			printf("Readline:%s\n", p);
			if(strlen(p)) add_history(p);
			int res=write(rl[1], p, strlen(p)+1);res=res;
			free(p);
		}
	}
}
void pollRl(void) {
	static char in[16384], *inp=in;
	struct pollfd pfd = {rl[0], POLLIN, 0};
	int res = poll(&pfd, 1, 0);
	if(!res) return;
	int space = in+sizeof(in)-inp;
	int t;
	if(!space) {
		char dump[1024];
		t = read(rl[0], dump, sizeof(dump));t=t;
	} else {
		t = read(rl[0], inp, space);
		if(t>0) inp+=t;
	}
	char *p = in;
	while(p<inp && *p) ++p;
	if(p<inp) {
		docommand(in);
		++p;
		int l = inp-p;
		memmove(in, p, l);
		inp=in+l;
	}	
}



int enterdebug(void)
{
	if(keymode) return 0;
	keymode=1;
	trapcr=commode=0;
	docommand("");
	linepntr=linebuff;
	return 1;
}

void onoff(char *msg, int onoff)
{
	printf("%s %s\n", msg, onoff ? "on" : "off");
}

void processkey(int val)
{
	if(val&MYALTED)
	{
		val^=MYALTED;
		if(val=='1') {on1=!on1;onoff("Plane A", on1);}
		if(val=='2') {on2=!on2;onoff("Plane B", on2);}
		if(val=='3') {on3=!on3;onoff("Window", on3);}
		if(val=='4') {on4=!on4;onoff("Sprites", on4);}
		if(val=='t') Z80_RegisterDump();
		if(val=='g') {writegfx();printf("Wrote gfx file\n");}
		if(val=='f') showfps();
		if(val=='d') dumpregs();
		if(val=='q' || val=='x') exitflag=1;
		if(val=='z') {onz=!onz;onoff("Z80", onz);}
		return;
	}
	if(val=='\n' || val==0x1b || val=='\r') enterdebug();
}

void writeppm(void)
{
FILE *ppmfile;
unsigned char row[320*3],*p1,*p2,*p3,*p4,ch;
ushort *wp;
int i,j,k;
unsigned char currentmap[768];

	p1=currentmap;
	wp=gfxmap;
	i=64;
	while(i--)
	{
		ch=*wp++;
		*p1++=(ch & 0x00e) << 4;
		*p1++=ch & 0x0e0;
		*p1++=(ch & 0xe00) >> 4;
	}
	rename("test.ppm","test2.ppm");
	ppmfile=fopen("test.ppm","wb");
	if(!ppmfile) return;
	fprintf(ppmfile,"P6\n");
	fprintf(ppmfile,"%d %d\n",320,224);
	fprintf(ppmfile,"255\n");
	p4=video+UPPERLEFT;
	for(j=0;j<224;j++)
	{
		p1=row;
		p2=p4;
		for(i=0;i<320;i++)
		{
			k=*p2++;
			p3=currentmap+k+k+k;
			*p1++=*p3++;
			*p1++=*p3++;
			*p1++=*p3++;
		}
		fwrite(row,1,320*3,ppmfile);
		p4+=VIDEOX;
	}
	fclose(ppmfile);
}

void initGenesis(void) {
	int i;
	fillprime=0;
	a11100=0x100;
	a11200=0x100;
	c0mode=0;
	xmode=0;
	trace=0;
	blanks=0;
	writeprotect=0;
	for(i=0;i<32;i++) gfxregs[i]=0;
	MC68000_Reset();
	Z80_Reset();
	YM2612ResetChip(0);
	lastpc = -1;
}

int main(int argc,char **argv) {
	int rom;
	int i,j,k;
	ushort *p;
	int oldgt=0;
	int count60=0;

	if(argc<2)
	{
		printf("Use\n%s <rom image file>\n",argv[0]);
		return 1;
	}
	for(i=2;i<argc;++i) {
		char *p=argv[i];
		if(!strcmp(p, "-d")) keymode=1;
	}

	rom=open(argv[1],O_RDONLY);
	if(rom<0)
	{
		printf("Could not open rom image %s\n",argv[1]);
		return 2;
	}
	setupReadline();
	mainlen=lseek(rom,0L,2);
	i=(mainlen+1) & ~1;
	mainmemory=malloc(i);
	if(!mainmemory)
	{
		printf("Could not allocate %d bytes of main memory.\n",mainlen);
		return 3;
	}
	lseek(rom,0L,0);
	p=mainmemory;
	while(i>0)
	{
		unsigned char temp[4096];
		j=read(rom,temp,sizeof(temp));
		if(j<1) break;
		k=0;
		while(k<j)
		{
			*p++=(temp[k]<<8) | temp[k+1];
			k+=2;
		}
		i-=j;
	}
	close(rom);
	ffpage=malloc(0x10000L);
	if(!ffpage)
	{
		printf("Could not allocate ff page.\n");
		return 4;
	}
	a0page=malloc(0x2000);
	if(!a0page)
	{
		printf("Could not allocate a0 page.\n");
		return 6;
	}
	gfxvscroll=malloc(0x100);
	if(!gfxvscroll)
	{
		printf("Could not allocate vscroll.\n");
		return 7;
	}
	cpuhistory=malloc(HISTORYLEN*sizeof(int32_t));
	if(!cpuhistory)
	{
		printf("Could not allocate cpu history.\n");
		return 8;
	}
	cpuput=cpuhistory;
	cpuend=&cpuhistory[HISTORYLEN];
	cpuinhistory=0;

	inttab=malloc(PERFRAME*sizeof(short));
	if(!inttab)
	{
		printf("Could not allocate inttab.\n");
		return 9;
	}
	makeinttab();

	soundinit();
	YM2612Init(1,7670442,SAMPLERATE,NULL,NULL);


	if(gfxinit()) return 10;
	openx();
	exitflag=0;

	initvideo();
	initGenesis();
	initGDB();
	starttime=gtime();
	if(keymode) {keymode=0;enterdebug();}
	while(!exitflag)
	{
		doxevents();
		if(!keymode)
			stepone(VBLANK);
//		if(doit[dcount&15])
		{
			int now,diff;
			++framecount;
			now=gtime();
			diff=now-starttime;
			if(diff>=2000)
			{
				starttime=now;
				fps=1000*framecount/diff;
				framecount=0;
//				printf("fps=%d\n",fps);
			}
		}
		dcount++;
		if(!keymode) stepone(PERFRAME - (intat-inttab));

//printf("%d\n",intat-inttab);

		if(keymode)
		{
			briefdelay(10);
			display(0);
			oldgt = globaltime;
		}
		while(!keymode)
		{
			int t;
			t = soundput - soundtake;
			if(t<0) t += SOUNDBUFFERSIZE;
			if(t<1000) break; // low in sound, don't wait...
			while(oldgt == globaltime)
				briefdelay(0);
			oldgt = globaltime;
			count60 -=599;
			if(count60<0)
			{
				count60+=1000;
				break;
			}
		}
		pollRl();
		pollGDB();
	}
	return 0;
}
