#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dis68k.h"
#include "em.h"
#include "command.h"

#define getword(x) cpu_readmem24_word(x)

struct diskey
{
	unsigned short mask,key;
	int32_t (*handler)(int32_t,ushort);
};
extern struct diskey diskeys[];

char dbuff[80],*dpoint;

void ddprintf(char *str,...)
{
va_list ap;
	va_start(ap, str);
	vsprintf(dpoint,str,ap);
	va_end(ap);
	dpoint+=strlen(dpoint);
}

int32_t unasm(int32_t val)
{
int32_t ret;
	ret=unasm2(val);
	printf("%s\n",dbuff);
	return ret;
}
int32_t unasm2(int32_t v1)
{
struct diskey *dk;
ushort v2;

	dpoint=dbuff;
	if(v1==-1)
		{ddprintf("------");return -1;}
	ddprintf("%s\t",getaddr(v1));
	v2=getword(v1);v1+=2;
	dk=diskeys;
	for(;;)
	{
		if((v2&dk->mask) == dk->key)
			return dk->handler(v1,v2);
		++dk;
	}
}
void d68kpraddr(int32_t v1)
{
	ddprintf("$%x",v1);
}
int32_t d68kdlow(int32_t v1,ushort v2)
{
	ddprintf("d%d",v2&7);
	return v1;
}
int32_t d68kalow(int32_t v1,ushort v2)
{
	ddprintf("a%d",v2&7);
	return v1;
}
int32_t d68kindirect(int32_t v1,ushort v2)
{
	ddprintf("(a%d)",v2&7);
	return v1;
}
int32_t d68kpostinc(int32_t v1,ushort v2)
{
	ddprintf("(a%d)+",v2&7);
	return v1;
}
int32_t d68kpredec(int32_t v1,ushort v2)
{
	ddprintf("-(a%d)",v2&7);
	return v1;
}
int32_t d68kindwoffs(int32_t v1,ushort v2)
{
short v3;
	v3=getword(v1);v1+=2;
	ddprintf("%d(a%d)",v3,v2&7);
	return v1;
}
int32_t d68kindex(int32_t v1,ushort v2)
{
short v3;
	v3=getword(v1);v1+=2;
	if(v2&8)
		ddprintf("%d(pc,",(char)v3);
	else
		ddprintf("%d(a%d,",(char)v3,v2&7);
	if(v3&0x8000)
		ddprintf("a%d.",(v3>>12)&7);
	else
		ddprintf("d%d.",(v3>>12)&7);
	if(v3&0x800)
		ddprintf("l)");
	else
		ddprintf("w)");
	return v1;
}
int32_t d68kabsshort(int32_t v1,ushort v2)
{
uint32_t v3;
	v3=getword(v1);v1+=2;
	if(v3>=0x8000) v3+=0xff0000;
	ddprintf("$%x.w",v3);
	return v1;
}
int32_t d68kabsint32_t(int32_t v1,ushort v2)
{
uint32_t v3;
	v3=(getword(v1)<<16L) | getword(v1+2);v1+=4;
	ddprintf("$%x.l",v3);
	return v1;
}
int32_t d68kpcindirect(int32_t v1,ushort v2)
{
short v3;
	v3=getword(v1);v1+=2;
	d68kpraddr(v1+v3-2);
	ddprintf("(pc)");
	return v1;
}
int32_t d68kstatus(int32_t v1,ushort v2)
{
	ddprintf("sr");
	return v1;
}
int32_t d68kmoderr(int32_t v1,ushort v2)
{
	ddprintf("<illegal mode>");
	return v1;
}
int32_t (*d68kmode7s[])(int32_t,ushort)=
{
d68kabsshort,d68kabsint32_t,d68kpcindirect,d68kindex,
d68kstatus,d68kmoderr,d68kmoderr,d68kmoderr
};
int32_t d68kmode7(int32_t v1,ushort v2)
{
	return d68kmode7s[v2&7](v1,v2);
}

int32_t (*d68keamodes[])(int32_t,ushort)=
{
d68kdlow,d68kalow,d68kindirect,d68kpostinc,
d68kpredec,d68kindwoffs,d68kindex,d68kmode7
};

int32_t d68keffectaddr(int32_t v1,ushort v2)
{
	return d68keamodes[(v2>>3)&7](v1,v2);
}

char *d68kdsizes[]={
".b","",".l",".-"
};
void d68ksize(ushort v1)
{
	ddprintf(d68kdsizes[(v1>>6)&3]);
}
int32_t d68kbreakpoint(int32_t v1,ushort v2)
{
	ddprintf("bkpt\t#%d",v2&7);
	return v1;
}
int32_t d68killegal(int32_t v1,ushort v2)
{
	ddprintf("illegal");
	return v1;
}
int32_t d68kmoveq(int32_t v1,ushort v2)
{
	ddprintf("moveq\t#%d,d%d",(char)v2,(v2&0xe00)>>9);
	return v1;
}
int32_t d68kcatchall(int32_t v1,ushort v2)
{
	ddprintf("dc\t$%04x",v2);
	return v1;
}
char *drots[]={"asr","asl","lsr","lsl","roxr","roxl","ror","rol"};
int32_t d68kmemrots(int32_t v1,ushort v2)
{
	ddprintf("%s\t",drots[(v2>>8)&7]);
	return d68keffectaddr(v1,v2);
}
char d68k18map[]={8,1,2,3,4,5,6,7};
int32_t d68kregrots(int32_t v1,ushort v2)
{
	ddprintf(drots[((v2>>2)&6) | ((v2&0x100) ? 1 : 0)]);
	d68ksize(v2);
	if(v2 & 0x20)
		ddprintf("\td%d,d%d",d68k18map[(v2>>9)&7],v2&7);
	else
		ddprintf("\t#%d,d%d",d68k18map[(v2>>9)&7],v2&7);
	return v1;
}
char *d68kconds2[]=
{
"ra","sr","hi","ls","cc","cs","ne","eq",
"vc","vs","pl","mi","ge","lt","gt","le"
};
char *d68kconds[]=
{
"tr","fa","hi","ls","cc","cs","ne","eq",
"vc","vs","pl","mi","ge","lt","gt","le"
};
int32_t d68kdbcond(int32_t v1,ushort v2)
{
short v3;
	v3=getword(v1);v1+=2;
	ddprintf("db%s\td%d,",d68kconds[(v2>>8)&15],v2&7);
	d68kpraddr(v1+v3-2);
	return v1;
}
int32_t d68kbranch(int32_t v1,ushort v2)
{
	ddprintf("b%s",d68kconds2[(v2>>8)&15]);
	if(v2&255)
	{
		char v3;
		v3=v2;
		ddprintf(".s\t");
		d68kpraddr(v1+v3);
	} else
	{
		short v3;
		v3=getword(v1);v1+=2;
		ddprintf("\t");
		d68kpraddr(v1+v3-2);
	}
	return v1;
}
int32_t d68kexgdd(int32_t v1,ushort v2)
{
	ddprintf("exg\td%d,d%d",v2&7,(v2>>9)&7);
	return v1;
}
int32_t d68kexgaa(int32_t v1,ushort v2)
{
	ddprintf("exg\ta%d,a%d",v2&7,(v2>>9)&7);
	return v1;
}
int32_t d68kexgda(int32_t v1,ushort v2)
{
	ddprintf("exg\td%d,a%d",(v2>>9)&7,v2&7);
	return v1;
}
int32_t d68kasbcd(int32_t v1,ushort v2)
{
	ddprintf("%cbcd\t",(v2&0x4000) ? 'a' : 's');
	if(v2&8)
		ddprintf("-(a%d),-(a%d)",v2&7,(v2>>9)&7);
	else
		ddprintf("d%d,d%d",v2&7,(v2>>9)&7);
	return v1;
}
int32_t d68kmusp(int32_t v1,ushort v2)
{
	if(v2&8)
		ddprintf("move.l\tusp,a%d",v2&7);
	else
		ddprintf("move.l\ta%d,usp",v2&7);
	return v1;
}
int32_t d68kext(int32_t v1,ushort v2)
{
	if(v2&0x40)
		ddprintf("ext.l\td%d",v2&7);
	else
		ddprintf("ext\td%d",v2&7);
	return v1;
}
int32_t d68ktrap(int32_t v1,ushort v2)
{
	ddprintf("trap\t#%d",v2&15);
	return v1;
}
int32_t d68kswap(int32_t v1,ushort v2)
{
	ddprintf("swap\td%d",v2&7);
	return v1;
}
int32_t d68klink(int32_t v1,ushort v2)
{
short v3;
	v3=getword(v1);v1+=2;
	ddprintf("link\ta%d,%d",v2&7,v3);
	return v1;
}
int32_t d68kunlk(int32_t v1,ushort v2)
{
	ddprintf("unlk\ta%d",v2&7);
	return v1;
}
char *d68konewordlist[]=
{
"reset","nop","stop","rte",
"rtd","rts","trapv","rtr"
};
int32_t d68koneword(int32_t v1,ushort v2)
{
	ddprintf(d68konewordlist[v2&7]);
	if(v2==0x4e72 || v2==0x4e74)
	{
		ushort v3;
		v3=getword(v1);v1+=2;
		ddprintf("\t#%x",v3);
	}
	return v1;
}
int32_t d68kmovep(int32_t v1,ushort v2)
{
	if(v2&0x40)
		ddprintf("movep.l\t");
	else
		ddprintf("movep\t");
	if(v2&0x80)
	{
		ddprintf("d%d,",(v2>>9)&7);
		v1=d68kindwoffs(v1,v2);
	} else
	{
		v1=d68kindwoffs(v1,v2);
		ddprintf(",d%d",(v2>>9)&7);
	}
	return v1;
}
char *d68kdbits[]=
{
"btst","bchg","bclr","bset"
};
int32_t d68kbitimmed(int32_t v1,ushort v2)
{
unsigned char v3;

	v3=getword(v1);v1+=2;
	ddprintf("%s\t#%d,",d68kdbits[(v2>>6)&3],v3&255);
	return d68keffectaddr(v1,v2);
}
int32_t d68kbitdatar(int32_t v1,ushort v2)
{
unsigned short v3;
	ddprintf("%s\td%d,",d68kdbits[(v2>>6)&3],(v2>>9)&7);
	if((v2&0x3f)!=0x3c)
		return d68keffectaddr(v1,v2);
	v3=getword(v1);v1+=2;
	ddprintf("#$%x",v3);
	return v1;
}
char *d68kdopsi[]=
{
"ori","andi","subi","addi",
"","eori","cmpi",""
};
int32_t d68kopsi(int32_t v1,ushort v2)
{
	ddprintf("%s",d68kdopsi[(v2>>9)&7]);
	d68ksize(v2);
	ddprintf("\t#");
	if(v2&0x80)
	{
		int32_t v3;
		v3=getword(v1);v1+=2;
		v3=(v3<<16L)+getword(v1);v1+=2;
		d68kpraddr(v3);
	} else
	{
		unsigned short v3;
		v3=getword(v1);v1+=2;
		if(!(v2&0x40)) v3&=255;
		ddprintf("$%x",v3);
	}
	ddprintf(",");
	v1=d68keffectaddr(v1,v2);
	return v1;
}
char *d68kdmove[]={"move.-","move.b","move.l","move"};
int32_t d68kmove(int32_t v1,ushort v2)
{
	ddprintf("%s\t",d68kdmove[(v2>>12)&3]);
	if((v2&0x3f)==0x3c)
	{
		ddprintf("#");
		if(v2&0x1000)
		{
			unsigned short v3;
			v3=getword(v1);v1+=2;
			if(v2&0x2000)
				ddprintf("$%x",v3);
			else
				ddprintf("$%x",v3&255);
		} else
		{
			int32_t v3;
			v3=getword(v1);v1+=2;
			v3=(v3<<16L)+getword(v1);v1+=2;
			d68kpraddr(v3);
		}
	} else v1=d68keffectaddr(v1,v2);
	ddprintf(",");
	v1=d68keffectaddr(v1,(v2 & 0xffc0) | ((v2>>3)&0x38) | ((v2>>9)&7));
	return v1;
}
char *d68kdopsa[]={"suba","cmpa","adda",""};
int32_t d68kopsa(int32_t v1,ushort v2)
{
	ddprintf(d68kdopsa[(v2>>13)&3]);
	if(v2&0x100) ddprintf(".l");
	ddprintf("\t");
	if((v2&0x3f)==0x3c)
	{
		ddprintf("#");
		if(v2&0x100)
		{
			int32_t v3;
			v3=getword(v1);v1+=2;
			v3=(v3<<16L) | getword(v1);v1+=2;
			d68kpraddr(v3);
		} else
		{
			short v3;
			v3=getword(v1);v1+=2;
			ddprintf("$%x",v3);
		}
	} else
		v1=d68keffectaddr(v1,v2);
	ddprintf(",a%d",(v2>>9)&7);
	return v1;
}
int32_t d68kcmpm(int32_t v1,ushort v2)
{
	ddprintf("cmpm");
	d68ksize(v2);
	ddprintf("\t(a%d)+,(a%d)+",v2&7,(v2>>9)&7);
	return v1;
}
int32_t d68kadsbx(int32_t v1,ushort v2)
{
	if(v2&0x4000)
		ddprintf("addx");
	else
		ddprintf("subx");
	d68ksize(v2);
	if(v2&8)
		ddprintf("\t-(a%d),-(a%d)",v2&7,(v2>>9)&7);
	else
		ddprintf("\td%d,d%d",v2&7,(v2>>9)&7);
	return v1;
}
char *d68kddivmul[]={"divu","divs","mulu","muls"};
int32_t d68kdivmul(int32_t v1,ushort v2)
{
	ddprintf("%s\t",d68kddivmul[((v2&0x4000) ? 2 : 0) | ((v2&0x100) ? 1 : 0)]);
	if((v2&0x3f)==0x3c)
	{
		short v3;
		v3=getword(v1);v1+=2;
		ddprintf("#$%x",v3);
	} else
		v1=d68keffectaddr(v1,v2);
	ddprintf(",d%d",(v2>>9)&7);
	return v1;
}
int32_t d68keor(int32_t v1,ushort v2)
{
	ddprintf("eor");
	d68ksize(v2);
	ddprintf("\td%d,",(v2>>9)&7);
	return d68keffectaddr(v1,v2);
}
char *d68kdops[]={"or","sub","","cmp","and","add","",""};
int32_t d68kops(int32_t v1,ushort v2)
{
	ddprintf(d68kdops[(v2>>12)&7]);
	d68ksize(v2);
	ddprintf("\t");
	if(v2&0x100)
	{
		ddprintf("d%d,",(v2>>9)&7);
		v1=d68keffectaddr(v1,v2);
	} else
	{
		if((v2&0x3f)==0x3c)
		{
			ddprintf("#");
			if(v2&0x80)
			{
				int32_t v3;
				v3=getword(v1);v1+=2;
				v3=(v3<<16L) | getword(v1);v1+=2;
				d68kpraddr(v3);
			} else
			{
				unsigned short v3;
				v3=getword(v1);v1+=2;
				if(!(v2&0x40))
					v3&=255;
				ddprintf("$%x",v3);
			}
		} else
			v1=d68keffectaddr(v1,v2);
	}
	ddprintf(",d%d",(v2>>9)&7);
	return v1;
}
int32_t d68kscond(int32_t v1,ushort v2)
{
	ddprintf("s%s\t",d68kconds[(v2>>8)&15]);
	return d68keffectaddr(v1,v2);
}
int32_t d68kadsbq(int32_t v1,ushort v2)
{
	if(v2&0x100)
		ddprintf("subq");
	else
		ddprintf("addq");
	d68ksize(v2);
	ddprintf("\t#%d,",d68k18map[(v2>>9)&7]);
	return d68keffectaddr(v1,v2);
}
int32_t d68klea(int32_t v1,ushort v2)
{
	ddprintf("lea\t");
	v1=d68keffectaddr(v1,v2);
	ddprintf(",a%d",(v2>>9)&7);
	return v1;
}
int32_t d68kpea(int32_t v1,ushort v2)
{
	ddprintf("pea\t");
	return d68keffectaddr(v1,v2);
}
int32_t d68kjmpjsr(int32_t v1,ushort v2)
{
	if(v2&0x40)
		ddprintf("jmp\t");
	else
		ddprintf("jsr\t");
	return d68keffectaddr(v1,v2);
}
char *d68kdmops[]={"negx","clr","neg","not","","tst","",""};
int32_t d68kmoreops(int32_t v1,ushort v2)
{
	ddprintf(d68kdmops[(v2>>9)&7]);
	d68ksize(v2);
	ddprintf("\t");
	return d68keffectaddr(v1,v2);
}
int32_t d68kmovetosr(int32_t v1,ushort v2)
{
	if(v2&0x200)
		ddprintf("move\t");
	else
		ddprintf("move.b\t");
	if((v2&0x3f)==0x3c)
	{
		short v3;
		v3=getword(v1);v1+=2;
		if(!(v2&0x40))
			v3&=255;
		ddprintf("#%x",v3);
	} else
		v1=d68keffectaddr(v1,v2);
	ddprintf(",sr");
	return v1;
}
int32_t d68kmovefromsr(int32_t v1,ushort v2)
{
	if(v2&0x200)
		ddprintf("move\tsr,");
	else
		ddprintf("move.b\tsr,");
	return d68keffectaddr(v1,v2);
}
int32_t d68kmovem(int32_t v1,ushort v2)
{
ushort v3;
	v3=getword(v1);v1+=2;
	if(v2&0x40)
		ddprintf("movem.l\t");
	else
		ddprintf("movem\t");
	if(v2&0x400)
	{
		v1=d68keffectaddr(v1,v2);
		ddprintf(",#$%x",v3);
	} else
	{
		ddprintf("#$%x,",v3);
		v1=d68keffectaddr(v1,v2);
	}
	return v1;
}
int32_t d68ktas(int32_t v1,ushort v2)
{
	ddprintf("tas\t");
	return d68keffectaddr(v1,v2);
}
int32_t d68kchk(int32_t v1,ushort v2)
{
	ddprintf("chk\t");
	if((v2&0x3f)==0x3c)
	{
		int32_t v3;
		v3=getword(v1);v1+=2;
		v3=(v3<<16L) | getword(v1);v1+=2;
		ddprintf("#");
		d68kpraddr(v3);
	} else
		v1=d68keffectaddr(v1,v2);
	ddprintf(",d%d",(v1>>9)&7);
	return v1;
}
int32_t d68knbcd(int32_t v1,ushort v2)
{
	ddprintf("nbcd\t");
	return d68keffectaddr(v1,v2);
}

struct diskey diskeys[]=
{
{0xfff8,0x4848,d68kbreakpoint},
{0xffff,0x4afc,d68killegal},
{0xf100,0x7000,d68kmoveq},
{0xf8c0,0xe0c0,d68kmemrots},
{0xf000,0xe000,d68kregrots},
{0xf0f8,0x50c8,d68kdbcond},
{0xf000,0x6000,d68kbranch},
{0xf1f8,0xc140,d68kexgdd},
{0xf1f8,0xc148,d68kexgaa},
{0xf1f8,0xc188,d68kexgda},
{0xb1f0,0x8100,d68kasbcd},
{0xfff0,0x4e60,d68kmusp},
{0xffb8,0x4880,d68kext},
{0xfff0,0x4e40,d68ktrap},
{0xfff8,0x4840,d68kswap},
{0xfff8,0x4e50,d68klink},
{0xfff8,0x4e58,d68kunlk},
{0xfff8,0x4e70,d68koneword},
{0xf138,0x0108,d68kmovep},
{0xff00,0x0800,d68kbitimmed},
{0xf100,0x0100,d68kbitdatar},
{0xf100,0x0000,d68kopsi},
{0xc000,0x0000,d68kmove},

{0x90c0,0x90c0,d68kopsa},
{0xf138,0xb108,d68kcmpm},
{0xb130,0x9100,d68kadsbx},
{0xb0c0,0x80c0,d68kdivmul},
{0xf100,0xb100,d68keor},
{0x8000,0x8000,d68kops},

{0xf0c0,0x50c0,d68kscond},
{0xf000,0x5000,d68kadsbq},

{0xfb80,0x4880,d68kmovem},
{0xfdc0,0x44c0,d68kmovetosr},
{0xfdc0,0x40c0,d68kmovefromsr},
{0xf1c0,0x41c0,d68klea},
{0xffc0,0x4ac0,d68ktas},
{0xf1c0,0x4180,d68kchk},
{0xff80,0x4e80,d68kjmpjsr},
{0xffc0,0x4800,d68knbcd},
{0xffc0,0x4840,d68kpea},
{0xf100,0x4000,d68kmoreops},

{0x0000,0x0000,d68kcatchall} /* make me last */
};
