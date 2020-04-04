#include <unistd.h>
#include <fcntl.h>
#include <SDL.h>

#define XSIZE 1024
#define YSIZE 1024

int mousex,mousey;

SDL_Surface *thescreen;
void scrlock(void)
{
	if ( SDL_LockSurface(thescreen) < 0 )
	{
		fprintf(stderr, "Couldn't lock display surface: %s\n",
							SDL_GetError());
		return;
	}
}
void scrunlock(void)
{
	SDL_UnlockSurface(thescreen);
}

void update(void)
{
	SDL_UpdateRect(thescreen, 0, 0, 0, 0);
}

unsigned long maprgb(int r,int g,int b)
{
	return SDL_MapRGB(thescreen->format,r,g,b);
}

void colordot(Uint32 x,Uint32 y,int c)
{
	if(x<XSIZE && y<YSIZE)
		*(Uint32 *)(thescreen->pixels+y*thescreen->pitch+(x<<2))=c;
}
void clear(int c)
{
int i;
	for(i=0;i<XSIZE;++i)
		colordot(i,0,c);

	for(i=1;i<YSIZE;++i)
		memcpy(thescreen->pixels + i*thescreen->pitch,
				thescreen->pixels, XSIZE*4);
}
void colorrect(int x, int y, int w, int h, int c)
{
int i,j;
	for(j=0;j<h;++j)
		for(i=0;i<w;++i)
			colordot(x+i, y+j, c);
}


// 0x10000 of gfx memoey
// 0x100 of color registers
// 0x20 of gfx registers
// 0x100 of vscroll registers
unsigned char gfx[0x10220], *gfxreg = gfx+0x10100;
int plane=0;
int gx = 128;
int gy = 128;

int cmap[64];

int anyword(int off)
{
	off&=~1;
	return gfx[off+1] | (gfx[off]<<8);
}
int gfxword(int off)
{
	return anyword(off&0xfffe);
}

int hscroll(int n)
{
int hs;

	n&=0x3ff;
	hs = gfxreg[13]*0x400;
	return gfxword(hs + n*2);
}

void puttile(int px, int py, int tile)
{
int pal;
int flipy = (tile&0x1000) ? 7 : 0;
int flipx = (tile&0x0800) ? 7 : 0;
int x,y;
int p;

	pal = (tile&0x6000) >> 9;
	tile &= 0x7ff;
	tile<<=5;
	for(y=0;y<8;++y)
	{
		for(x=0;x<8;++x)
		{
			p = gfx[tile];
			if(x&1)
				++tile;
			else
				p>>=4;
			p=pal | (p&15);
			if(p&15)
				colordot(px + (x^flipx), py + (y^flipy), cmap[p]);
		}

	}

	
}


void iterate(void)
{
int x, y, t;

	scrlock();
	clear(cmap[gfxreg[7]&0x3f]);

	t=0;
	for(y=0;y<gy;++y)
	{
		for(x=0;x<gx;++x)
		{
			puttile(x*8, y*8, gfxword((plane<<11) + t));
			t+=2;
		}
	}


if(0)
	for(y=0;y<0x400;++y)
	{
		x = hscroll(y);
		if(x>=0x8000)
			x-=0x10000;
		colordot(XSIZE/2 + x, y/2, ~0);
		colordot(XSIZE/2 + x+1, y/2, 0);
		colordot(XSIZE/2 + x-1, y/2, 0);
	}




	scrunlock();
	update();
}

void handlekey(int code)
{
	switch(code)
	{
	case SDLK_LEFT:
		plane = (plane-1) & 0x1f;
		break;
	case SDLK_RIGHT:
		plane = (plane+1) & 0x1f;
		break;
	case 'x':
		gx <<=1;
		if(gx==256) gx=32;
		break;
	case 'y':
		gy <<=1;
		if(gy==256) gy=32;
		break;
	}
	iterate();
}


int main(int argc,char **argv)
{
int code;
SDL_Event event;
unsigned long videoflags;
int done=0;
int fd;
int i;
	if(argc<2)
	{
		printf("specify the gfx file\n");
		return -1;
	}
	fd=open(argv[1], O_RDONLY);
	if(fd<0)
	{
		printf("Couldn't open %s\n", argv[1]);
		return -2;
	}
	int res = read(fd, gfx, sizeof(gfx));res=res;
	close(fd);

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(1);
	}
	videoflags = 0;
	thescreen = SDL_SetVideoMode(XSIZE, YSIZE, 32, videoflags);
	if ( thescreen == NULL )
	{
		fprintf(stderr, "Couldn't set display mode: %s\n",
							SDL_GetError());
		exit(5);
	}

	for(i=0;i<64;++i)
	{
		int c = anyword(0x10000+i*2);
		cmap[i] = maprgb((c&0x00e)<<4, (c&0x0e0), (c&0xe00)>>4);

	}

	iterate();
	while(!done)
	{
//		iterate();
		SDL_Delay(50);
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_MOUSEMOTION:
				mousex=event.motion.x;
				mousey=event.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mousex=event.button.x;
				mousey=event.button.y;
				break;
			case SDL_KEYDOWN:
				code=event.key.keysym.sym;
				if(code==SDLK_ESCAPE) done=1;
				handlekey(code);
				break;
			case SDL_QUIT:
				done=1;
				break;
			}
		}
	}
	SDL_Quit();
	return 0;
}
