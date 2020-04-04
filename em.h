#ifndef _EM_H
#define _EM_H

#include <inttypes.h>

#include "68kint.h"

#define SAMPLERATE 48000
#define FRAGSIZE 512
#define SOUNDBUFFERSIZE (FRAGSIZE*16)



#define KEYMAX 32

#define HISTSIZE 512
#define LINESIZE 128
#define SCROLLHISTORYSIZE 8192

typedef struct {
	void *xstate;
	int pressedcodes[KEYMAX],downcodes[KEYMAX],numpressed,numdown;
	int black, white, cursorcolor, cursorstate;

} ec;

#define MYF1 0x180
#define MYF2 0x181
#define MYF3 0x182
#define MYF4 0x183
#define MYF5 0x184
#define MYF6 0x185
#define MYF7 0x186
#define MYF8 0x187
#define MYF9 0x188
#define MYF10 0x189
#define MYLEFT 0x190
#define MYRIGHT 0x191
#define MYUP 0x192
#define MYDOWN 0x193
#define MYPAGEUP 0x194
#define MYPAGEDOWN 0x195
#define MYHOME 0x196
#define MYEND 0x197
#define MYALTL 0x198
#define MYALTR 0x199
#define MYCTRLL 0x19a
#define MYCTRLR 0x19b
#define MYSHIFTL 0x19c
#define MYSHIFTR 0x19d

#define MYDELETE 0x7f
#define MYSHIFTED 0x40
#define MYALTED 0x400



extern unsigned char trace;
extern int32_t lastpc;
extern int keymode;
extern char trapcr;
extern void stepone(int num);
extern char writeprotect;
#define HISTORYLEN 16384
extern int32_t *cpuhistory,*cpuput,*cpuend,*cputake;
extern int cpuinhistory;
extern int enterdebug(void);
extern unsigned char gfxregs[];
extern unsigned short *gfxmem,gfxmap[],*gfxvscroll;
extern int32_t mainlen;
extern short a11100,a11200;
extern unsigned short readc0(void);
extern unsigned short *ffpage,*mainmemory;
extern unsigned char *a0page;

#include "z80.h"

/* x.c */
void termflush(void);
void termstring(char *str);
void openx(void);
void doxevents(void);
void termchar(int val);
void display(int changed);
extern int colortranslate[];
extern int video32[];
int checkpressed(int code);
extern volatile int globaltime;
void briefdelay(int time);

/* em.c */
void processkey(int val);
extern int on1, on2, on3, on4;
int get_hvpos(void);
extern short *soundbuffer;
extern int soundput, soundtake;
extern char exitflag;
void initGenesis(void);

/* z80.c */
void Z80_int(void);

/* command.c */
void docommand(char *com);

/* video.c */
#define XBORDER 128
#define YBORDER 128
#define VIDEOX (320+XBORDER*2)
#define VIDEOY (224+YBORDER*2)
#define UPPERLEFT (VIDEOX*YBORDER+XBORDER)

typedef unsigned char uchar;
//typedef unsigned short ushort;

void makeline(int line);
extern uchar *video;
extern void initvideo(void);

// gdb.c
void initGDB(void);
void pollGDB(void);

#endif
