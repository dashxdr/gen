#include <stdio.h>
#include "command.h"
#include "expr.h"
#include "em.h"
#include "cpudefs.h"
#include "dis68k.h"
#include "z80dis.h"

int vwindow=0;
int breakpoint=-1;
int vsize=0x10;
char addrstr[32];
char *getaddr(int val)
{
	sprintf(addrstr,"$%-16x",val);
	return addrstr;
}

int iswhite(int ch)
{
	return ch==' ' || ch==9;
}
void doview(void)
{
int i,j;
unsigned char tbuff[16],ch;
int where;
int size;

	where=vwindow;
	size=vsize;

	while(size)
	{
		printf("%s ",getaddr(where));
		j=size>16 ? 16 : size;
		i=0;
		while(i<j)
		{
			ch=tbuff[i]=cpu_readmem24(where++);
			if(i++&1)
				printf("%02x ",ch);
			else
				printf("%02x",ch);
		}

		while(i<16)
			if(i++&1)
				printf("   ");
			else
				printf("  ");
		i=0;
		while(i<j)
		{
			tbuff[i]&=0x7f;
			if(tbuff[i]<0x20) tbuff[i]+=0x20;
			printf("%c",tbuff[i++]);
		}
		printf("\n");
		size-=j;
	}
}
void prompt(void)
{
}
char *helptext="\
<CR>                      Show registers, halt if not halted\n\
!                         Initialize Genesis\n\
<SPACE> [expr]            Single step\n\
= expr                    Evaluate expression\n\
,                         Toggle write protect\n\
d [expr]                  Disassemble\n\
e[w] expr[,expr] expr ... Modify memory\n\
h [expr]                  History\n\
i                         Clear breakpoints\n\
j [expr]                  Jump to address\n\
g                         Go\n\
k                         Wait until here again\n\
Q                         Quit\n\
r                         Return from function call, not too useful\n\
s                         Break after current instruction\n\
v [expr][,expr]           View memory, set window size\n\
[ and ]                   Move memory view window\n\
";
char *exprmsg(char **p)
{
char *err;
	err=expr(p);
	if(err) printf("%s\n",err);
	return err;
}
int skipwhite(char **com)
{
	while(iswhite(**com)) (*com)++;
	return **com;
}
#define LISTMAX 100
int numlist[LISTMAX];
int fetchlist(char **com)
{
int count=0;
char *p;

	for(;;)
	{
		skipwhite(com);
		if(!*com) break;
		p=*com;
		if(exprmsg(com)) {count=0;break;}
		if(p==*com) break;
		numlist[count++]=exprval;
		if(count==LISTMAX) break;
	}
	return count;
}

void shregs(void)
{
int i;

	printf("SR ");
	printf("%c%c%c%c%c%c%c%c Write protect %s  Interrupts pending",
		CFLG ? 'C' : ' ',
		VFLG ? 'V' : ' ',
		ZFLG ? 'Z' : ' ',
		NFLG ? 'N' : ' ',
		regs.x ? 'X' : ' ',
		regs.intmask + '0',
		regs.s ? 'S' : ' ',
		regs.t1 ? 'T' : ' ',
		writeprotect ? "on " : "off");
	for(i=0;i<7;i++)
		if(pending_interrupts & (1<<i)) printf("%d",i);
	i=get_hvpos();
	printf("  LINE %3d POS %3d", i>>8, i&255);
	printf("\nDR");


	for(i=0;i<8;i++)
		printf(" %08x",regs.d[i]);
	printf("\nAR");
	for(i=0;i<8;i++)
		printf(" %08x",regs.a[i]);
	printf("\n");
	doview();
	unasm(lastpc);
	unasm(regs.pc);
}
#define COMNORMAL 0
#define COMDISASSEMBLING 1
#define COMZDISASSEMBLING 2
int commode=COMNORMAL;
int dloc;
int dzbase;
void docommand(char *com)
{
int which1,which2;
char doprompt=1;
char *p;
unsigned int t1,t2,t3,t4;
char tbuffer[128];

	if(commode==COMDISASSEMBLING ||
			commode==COMZDISASSEMBLING) /* disassembling line by line */
	{
		if(*com)
		{
			printf("\n");
			if(*com=='q' && !com[1]) {commode=COMNORMAL;trapcr=0;goto done;}
			p=com;
			if(exprmsg(&com) || p==com) {commode=COMNORMAL;trapcr=0;goto done;}
			dloc=exprval;
		}
		if(commode == COMDISASSEMBLING)
			dloc=unasm(dloc);
		else
		{
			t1 = Z80_Dasm(a0page+(dloc&0xffff),tbuffer,dloc);
			printf("%04x %s\n",dloc,tbuffer);
			dloc += t1;
		}


		doprompt=0;
		goto done;
	}
	which1=*com;
	if(which1) com++;
	if(which1!=' ' && *com && !iswhite(*com)) which2=*com++;
	else which2=0;
	skipwhite(&com);
	switch(which1)
	{
	case 0:
		if(!enterdebug())
			shregs();
		break;
	case '?':
		printf("%s", helptext);
		break;
	case 'v':
		p=com;
		if(exprmsg(&com)) break;
		if(p!=com)
			vwindow=exprval;
		if(*com==',')
		{
			p=++com;
			if(exprmsg(&com)) break;
			if(p!=com)
				vsize=exprval&0x1ff;
		}
		doview();
		break;
	case ']':
		vwindow+=vsize;
		doview();
		break;
	case '[':
		vwindow-=vsize;
		doview();
		break;
	case 'k':
		breakpoint=-1;
		if(lastpc==-1)
			stepone(1);
		breakpoint=lastpc;
	case 'g':
		keymode=0;
		doprompt=0;
		break;
	case '=':
		if(exprmsg(&com)) break;
		printf("$%x   %d\n",exprval,exprval);
		break;		
	case 'e':
		if(exprmsg(&com)) break;
		t1=exprval;
		if(*com==',')
		{
			++com;
			if(exprmsg(&com)) break;
			t2=exprval;
		} else t2=0;
		t3=fetchlist(&com);
		if(!t3) break;
		t4=0;
		for(;;)
		{
			if(which2=='w')
				cpu_writemem24_word(t1++,numlist[t4++]);
			else
				cpu_writemem24(t1,numlist[t4++]);
			++t1;
			if(t4<t3) continue;
			if((t2 && t1>=t2) || !t2) break;
			t4=0;
		}		
		break;
	case 'd':
		if(which2=='z')
		{
			p=com;
			if(exprmsg(&com)) break;
			if(p!=com)
				t1=exprval;
			else t1=0;
			if(*com==',')
			{
				++com;
				if(exprmsg(&com)) break;
				t2=exprval;
				if(t2<0) break;
				if(t2>10000) t2=10000;
				while(t2--)
				{
					t3=Z80_Dasm(a0page+(t1&0xffff),tbuffer,t1);
					printf("%04x %s\n",t1,tbuffer);
					t1+=t3;
				}
			} else
			{
				t3=Z80_Dasm(a0page+(t1&0xffff),tbuffer,t1);
				printf("%04x %s\n",t1,tbuffer);
				dloc=t1=t1+t3;
				commode=COMZDISASSEMBLING;
				trapcr=1;
				doprompt=0;
			}
		} else
		{
			p=com;
			if(exprmsg(&com)) break;
			if(p!=com)
				t1=exprval;
			else {t1=lastpc;if(t1==-1) t1=regs.pc;}
			if(*com==',')
			{
				++com;
				if(exprmsg(&com)) break;
				t2=exprval;
				if(t2<0) break;
				if(t2>10000) t2=10000;
				while(t2--)
					t1=unasm(t1);
			} else
			{
				dloc=t1=unasm(t1);
				commode=COMDISASSEMBLING;
				trapcr=1;
				doprompt=0;
			}
		}
		break;
	case ' ':
		p=com;
		if(exprmsg(&com)) break;
		if(p==com)
			stepone(1);
		else
		{
			t1=exprval;
			t2=regs.pc;
			while(t1--)
				stepone(1);
		}
		shregs();
		break;
	case ',':
		writeprotect=!writeprotect;
		printf("Write protect %s\n",writeprotect ? "on" : "off");
		break;
	case 'h':
		p=com;
		if(exprmsg(&com)) break;
		if(p!=com)
			t1=exprval;
		else
			t1=16;
		if(t1>cpuinhistory) t1=cpuinhistory;
		if(t1>100) t1=100;
		cputake=cpuput;
		t2=t1;
		while(t2-->0)
			if(cputake==cpuhistory) cputake=cpuend-1;
			else --cputake;
		while(t1-->0)
		{
			unasm(*cputake++);
			if(cputake==cpuend) cputake=cpuhistory;
		}
		break;
	case 'p':
		p=com;
		if(exprmsg(&com)) break;
		if(p!=com)
			breakpoint=exprval;
		else
			if(breakpoint>=0) printf("%x\n",breakpoint);
			else printf("No breakpoint\n");
		break;
	case 'i':
		breakpoint=-1;
		break;
	case 'r':
		breakpoint=cpu_readmem24_dword(regs.a[7]);
		printf("Putting breakpoint at %x\n", breakpoint);
		keymode=doprompt=0;
		break;
	case 's':
		if(lastpc==-1)
			stepone(1);
		printf("Putting breakpoint after\n");
		breakpoint=unasm(lastpc);
		keymode=doprompt=0;
		break;
	case 'j':
		p=com;
		if(exprmsg(&com)) break;
		if(p!=com)
		{
			regs.pc=exprval;
			shregs();
		}
		break;
	case 'Q':
		exitflag = 1;
		break;
	case '!':
		initGenesis();
		shregs();
		break;
	}
done:
	if(doprompt) prompt();
}
