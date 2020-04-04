/*
  File: fm.h -- header file for software emuration for FM sound genelator

*/
#ifndef _H_FM_FM_
#define _H_FM_FM_

#define BUILD_YM2612 1

/* stereo mixing / separate */
/*#define FM_STEREO_MIX */
/* output bit size 8 or 16 */
#define FM_OUTPUT_BIT 16

#define YM2203_NUMBUF 1

#ifdef FM_STEREO_MIX
  #define YM2151_NUMBUF 1
  #define YM2608_NUMBUF 1
  #define YM2612_NUMBUF 1
  #define YM2610_NUMBUF  1
#else
  #define YM2151_NUMBUF 2    /* FM L+R */
  #define YM2608_NUMBUF 2    /* FM L+R+ADPCM+RYTHM */
  #define YM2612_NUMBUF 2    /* FM L+R */
  #define YM2610_NUMBUF  2
#endif

#if (FM_OUTPUT_BIT==16)
typedef unsigned short FMSAMPLE;
typedef unsigned long FMSAMPLE_MIX;
#endif
#if (FM_OUTPUT_BIT==8)
typedef unsigned char  FMSAMPLE;
typedef unsigned short FMSAMPLE_MIX;
#endif

/* For YM2151/YM2608/YM2612 option */

typedef void (*FM_TIMERHANDLER)(int n,int c,int cnt,double stepTime);
typedef void (*FM_IRQHANDLER)(int n,int irq);
/* FM_TIMERHANDLER : Stop or Start timer         */
/* int n          = chip number                  */
/* int c          = Channel 0=TimerA,1=TimerB    */
/* int count      = timer count (0=stop)         */
/* doube stepTime = step time of one count (sec.)*/

/* FM_IRQHHANDLER : IRQ level changing sense     */
/* int n       = chip number                     */
/* int irq     = IRQ level 0=OFF,1=ON            */

#ifdef BUILD_OPN
/* -------------------- YM2203/YM2608 Interface -------------------- */
/*
** 'n' : YM2203 chip number 'n'
** 'r' : register
** 'v' : value
*/
/*void OPNWriteReg(int n, int r, int v); */

/*
** read status  YM2203 chip number 'n'
*/
unsigned char OPNReadStatus(int n);

#endif /* BUILD_OPN */

#if BUILD_YM2203
/* -------------------- YM2203(OPN) Interface -------------------- */

/*
** Initialize YM2203 emulator(s).
**
** 'num'     is the number of virtual YM2203's to allocate
** 'rate'    is sampling rate
** return    0 = success
*/
int YM2203Init(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);

/*
** shutdown the YM2203 emulators .. make sure that no sound system stuff
** is touching our audio buffers ...
*/
void YM2203Shutdown(void);

/*
** reset all chip registers for YM2203 number 'num'
*/
void YM2203ResetChip(int num);

void YM2203UpdateOne(int num, void *buffer, int length);

/*
** return : InterruptLevel
*/
int YM2203Write(int n,int a,int v);
unsigned char YM2203Read(int n,int a);

/*
**	Timer OverFlow
*/
int YM2203TimerOver(int n, int c);

/*int YM2203SetBuffer(int n, FMSAMPLE *buf );*/

#endif /* BUILD_YM2203 */

#if BUILD_YM2608
/* -------------------- YM2608(OPNA) Interface -------------------- */

int YM2608Init(int num, int baseclock, int rate,
               void **pcmroma,int *pcmsizea,short *rhythmrom,int *rhythmpos,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2608Shutdown(void);
void YM2608ResetChip(int num);
void YM2608UpdateOne(int num, void **buffer, int length);

int YM2608Write(int n, int a,int v);
unsigned char YM2608Read(int n,int a);
int YM2608TimerOver(int n, int c );
/*int YM2608SetBuffer(int n, FMSAMPLE **buf );*/
#endif /* BUILD_YM2608 */

#if BUILD_YM2610
/* -------------------- YM2610(OPNB) Interface -------------------- */

#define   MAX_2610    (2)

int YM2610Init(int num, int baseclock, int rate,
               void **pcmroma,int *pcmasize,void **pcmromb,int *pcmbsize,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2610Shutdown(void);
void YM2610ResetChip(int num);
void YM2610UpdateOne(int num, void **buffer, int length);
#if BUILD_YM2610B
void YM2610BUpdateOne(int num, void **buffer, int length);
#endif

int YM2610Write(int n, int a,int v);
unsigned char YM2610Read(int n,int a);
int YM2610TimerOver(int n, int c );
/*int YM2610SetBuffer(int n, FMSAMPLE **buf );*/

#ifdef __RAINE__
void Set_YM2610_ADPCM_Buffers(int num, UBYTE *bufa, UBYTE *bufb, ULONG sizea, ULONG sizeb);
#endif
#endif /* BUILD_YM2610 */

#if BUILD_YM2612
int YM2612Init(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void YM2612Shutdown(void);
void YM2612ResetChip(int num);
void YM2612UpdateOne(int num, void **buffer, int length);
int YM2612Write(int n, int a,int v);
unsigned char YM2612Read(int n,int a);
int YM2612TimerOver(int n, int c );
FMSAMPLE *YM2612Buffer(int n);
/*int YM2612SetBuffer(int n, FMSAMPLE **buf );*/

#endif /* BUILD_YM2612 */

#if BUILD_YM2151
/* -------------------- YM2151(OPM) Interface -------------------- */
/*
** Initialize YM2151 emulator(s).
**
** 'num'     is the number of virtual YM2151's to allocate
** 'rate'    is sampling rate
** 'bitsize' is sampling bits (8 or 16)
** 'bufsiz' is the size of the buffer
*/
int OPMInit(int num, int baseclock, int rate,
               FM_TIMERHANDLER TimerHandler,FM_IRQHANDLER IRQHandler);
void OPMShutdown(void);
void OPMResetChip(int num);

void OPMUpdateOne(int num, void **buffer, int length );
void OPMWriteReg(int n, int r, int v);
unsigned char OPMReadStatus(int n );
/* ----- get pointer sound buffer ----- */
FMSAMPLE *OPMBuffer(int n,int c );
/* ----- set sound buffer ----- */
/*int OPMSetBuffer(int n, FMSAMPLE **buf );*/
/* ---- set user interrupt handler ----- */
void OPMSetIrqHandler(int n, void (*handler)(void) );
/* ---- set callback hander when port CT0/1 write ----- */
/* CT.bit0 = CT0 , CT.bit1 = CT1 */
void OPMSetPortHander(int n,void (*PortWrite)(int offset,int CT) );
/* JB 981119  - so it will match MAME's memory write functions scheme*/

int YM2151Write(int n,int a,int v);
unsigned char YM2151Read(int n,int a);
int YM2151TimerOver(int n,int c);
#endif /* BUILD_YM2151 */

#endif /* _H_FM_FM_ */
