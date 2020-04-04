#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em.h"

uchar *linestarts[VIDEOY];
unsigned char *video;

void initvideo(void)
{
int i;
uchar *p;

	video=malloc(VIDEOX*VIDEOY);
	if(!video)
	{
		printf("Could not allocate video page.\n");
		exit(5);
	}
	p=video;
	for(i=0;i<VIDEOY;i++)
	{
		linestarts[i]=p;
		p+=VIDEOX;
	}
}




#define MAXSTRIPS (40*224*6)
struct strip
{
	ushort *p1;
	uchar *p2;
	short tilepointer;
} strips[MAXSTRIPS],*strippointer;
void qstrip(ushort *p1,uchar *p2,short tilepointer)
{
	strippointer->p1=p1;
	strippointer->p2=p2;
	strippointer++->tilepointer=tilepointer;
}
void putstrip(ushort *p1,uchar *p2,short tilepointer)
{
int ch,ctab;
register int c1;
	ctab=(tilepointer&0x6000)>>9;
	if(tilepointer&0x800)
	{
		ch=*p1;
		if((c1=ch&0xf000)) p2[7]=ctab + (c1>>12);
		if((c1=ch&0x0f00)) p2[6]=ctab + (c1>>8);
		if((c1=ch&0x00f0)) p2[5]=ctab + (c1>>4);
		if((c1=ch&0x000f)) p2[4]=ctab + c1;
		ch=p1[1];
		if((c1=ch&0xf000)) p2[3]=ctab + (c1>>12);
		if((c1=ch&0x0f00)) p2[2]=ctab + (c1>>8);
		if((c1=ch&0x00f0)) p2[1]=ctab + (c1>>4);
		if((c1=ch&0x000f)) *p2=ctab + c1;
	} else
	{
		ch=*p1;
		if((c1=ch&0xf000)) *p2=ctab + (c1>>12);
		if((c1=ch&0x0f00)) p2[1]=ctab + (c1>>8);
		if((c1=ch&0x00f0)) p2[2]=ctab + (c1>>4);
		if((c1=ch&0x000f)) p2[3]=ctab + c1;
		ch=p1[1];
		if((c1=ch&0xf000)) p2[4]=ctab + (c1>>12);
		if((c1=ch&0x0f00)) p2[5]=ctab + (c1>>8);
		if((c1=ch&0x00f0)) p2[6]=ctab + (c1>>4);
		if((c1=ch&0x000f)) p2[7]=ctab + c1;
	}
}
void putstrips(void)
{
struct strip *stp;
	stp=strips;
	while(stp<strippointer)
	{
		putstrip(stp->p1,stp->p2,stp->tilepointer);
		stp++;
	}
}
uchar activex[32];
uchar activey[32];
void makeactives(void)
{
int cutx,cuty,i;
	cutx=gfxregs[0x11];
	cuty=gfxregs[0x12];
	for(i=0;i<32;i++)
	{
		activex[i]=(cutx&0x80) ? (i>=(cutx&0x1f)) : (i<(cutx&0x1f));
		activey[i]=(cuty&0x80) ? (i>=(cuty&0x1f)) : (i<(cuty&0x1f));
	}
}
unsigned int hmasks[]={0,7,0xf8,0xff};

void putplane(int y, unsigned short planepointer,int whichplane)
{
int x;
ushort *hscroll;
unsigned int hmask,vmask;
unsigned int t1,t2,t3;
int dx,dxh,dxl,dy,dyh,dyl;
ushort *base,*ap,*p1;
uchar *p2;
unsigned int arrayx,arrayy,arrayxmask,arrayymask;
uchar extra;

	if(whichplane==2) return;
	hscroll=gfxmem+(gfxregs[13]<<9);
	hmask=hmasks[gfxregs[11]&3];
	base=gfxmem+(planepointer>>1);
	arrayx=(((gfxregs[0x10]&3)+1)<<5);
	arrayxmask=arrayx-2;
	arrayy=(((gfxregs[0x10]&0x30)+0x10)<<1);
	arrayymask=arrayy-1;
	vmask=(gfxregs[11]&4) ? arrayxmask : 0;
	extra=!(gfxregs[0]&0x20);

	if(!whichplane && activey[y>>3])
		return;
	t1=((y&hmask)<<1)|whichplane;
	dx=hscroll[t1] & 0x3ff;
	dxh=-((dx&0x3f0)>>3);
	dxl=dx&15;
	p2=video + UPPERLEFT + y*VIDEOX + dxl;
	if(extra && dxl)
	{
		x=-1;
		dxh-=2;
		p2-=16;
	} else x=0;
	for(;x<20;x++)
	{
		if(!whichplane && x>=0 && activex[x]) continue;
//		t2=(dxh&vmask)|whichplane;
		t2=((x*2)&vmask)|whichplane;
		dy=gfxvscroll[t2] + y;
		dyh=dy>>3;
		dyl=(dy&7)<<1;
		ap=base+arrayx*(dyh&arrayymask)+(dxh&arrayxmask);
		dxh+=2;
		t3=*ap++;
		p1=gfxmem+((t3&0x7ff)<<4)+(dyl^((t3&0x1000) ? 0x0e : 0));
		if(t3&0x8000)
			qstrip(p1,p2,t3);
		else
			putstrip(p1,p2,t3);
		p2+=8;
		t3=*ap;
		p1=gfxmem+((t3&0x7ff)<<4)+(dyl^((t3&0x1000) ? 0x0e : 0));
		if(t3&0x8000)
			qstrip(p1,p2,t3);
		else
			putstrip(p1,p2,t3);
		p2+=8;
	}
}


void putplane2(int y, unsigned short planepointer)
{
int x,ay;
ushort *base;
int arrayx;
unsigned short *p1;
unsigned char *p2;
unsigned short tile;
int yt, yi;

	arrayx=(gfxregs[12]&1) ? 64 : 32;
	base=gfxmem+(planepointer>>1) + (y>>3)*arrayx;
	ay=activey[y>>3];
	p2 = linestarts[y+YBORDER] + XBORDER;
	yt = (y&7)<<1;
	yi = (~y&7)<<1;

	for(x=0;x<20;++x)
	{
		if(ay || activex[x])
		{
			tile = *base++;
			p1 = gfxmem + ((tile&0x7ff)<<4) +
				((tile&0x1000) ? yi : yt);
			if(tile&0x8000)
				qstrip(p1, p2, tile);
			else
				putstrip(p1, p2, tile);

			tile = *base++;
			p1 = gfxmem + ((tile&0x7ff)<<4) +
				((tile&0x1000) ? yi : yt);
			if(tile&0x8000)
				qstrip(p1, p2+8, tile);
			else
				putstrip(p1, p2+8, tile);
		} else
			base += 2;
		p2 += 16;
	}
}


struct sprite
{
	int xpos,ypos;
	int xsize,ysize;
	unsigned short spritetile;
} sprites[128],*sp;
void buildsprites(unsigned short spritepointer)
{
int xpos,ypos;
unsigned short spritetile;
int xsize,ysize;
int max,next;
ushort *p1;

	sp=sprites;
	max=80;
	spritepointer>>=1;
	p1=gfxmem+spritepointer;
	do
	{
		ypos=*p1 & 0x1ff;
		xpos=p1[3] & 0x1ff;
		xsize=(p1[1]&0x0c00)>>7;
		ysize=(p1[1]&0x0300)>>5;
		spritetile=p1[2];
		next=p1[1]&0x7f;
		p1=gfxmem+spritepointer+(next<<2);
		if(ypos+ysize<=YBORDER-8 || xpos+xsize<=XBORDER-8 ||
			ypos>=YBORDER+224 || xpos>=XBORDER+320) continue;
		sp->xpos=xpos;
		sp->ypos=ypos;
		sp->xsize=xsize;
		sp->ysize=ysize;
		sp->spritetile=spritetile;
		++sp;
	} while(next && max--);
}
void putsprites(int line)
{
struct sprite *sp2;
int xsize,ysize,xpos,ypos;
unsigned short spritetile;
int tx,ty;
unsigned short *p1;
unsigned char *p2;
int invertbits;
int ds;
int dx;

	line += YBORDER;
	sp2=sp;
	while(sp2-->sprites)
	{
		invertbits = spritetile=sp2->spritetile;
		ypos=sp2->ypos;
		ysize=sp2->ysize+8;
		if(line<ypos || line>=ypos+ysize) continue;

		xpos=sp2->xpos;
		xsize=sp2->xsize+8;
		ty = line-ypos;
		if(invertbits & 0x1000)
			ty = ysize-1 - ty;
		ds = ysize<<1;
		spritetile += ty>>3;
		ty &= 7;
		p1=gfxmem+((spritetile&0x7ff)<<4) + ty*2;
		p2 = linestarts[line] + xpos;
		if(spritetile&0x800)
		{
			p2 += xsize - 8;
			dx = -8;
		} else
			dx = 8;
		for(tx=0;tx<xsize;tx+=8)
		{
			if(invertbits&0x8000)
				qstrip(p1, p2, invertbits);
			else
				putstrip(p1, p2, invertbits);
			p1+=ds;
			p2+=dx;
		}
	}
}
void makeline(int line)
{
unsigned char *p, *e;
int *ip;

	if(line<0 || line>=224) return;

	p = video + UPPERLEFT + line*VIDEOX;
	e = p+320;
	memset(p, gfxregs[7], 320);

	if(gfxregs[1]&0x40)
	{
		strippointer=strips;
		makeactives();
// gfxregs[12] bit 0 if 1 means 40 tiles per row, otherwise only 32.
		buildsprites((gfxregs[5]&((gfxregs[12]&1) ? 0x7e : 0x7f))<<9);
		if(on1)
			putplane(line, (gfxregs[4]&7)<<13,1);
		if(on2)
			putplane(line, (gfxregs[2]&0x38)<<10,0);
		if(on3)
			putplane2(line, (gfxregs[3]&((gfxregs[12]&1)?0x3c : 0x3e))<<10);
		if(on4)
			putsprites(line);
		putstrips();
	}
	ip = video32+line*320;
	while(p<e)
		*ip++ = colortranslate[(gfxmap[*p++]>>1)&0x777];

}

