#include <stdio.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>
//#include <dos/dos.h>
//#include <exec/exec.h>
#include <clib/exec_protos.h>
#include <graphics/gfxbase.h>
#include <clib/graphics_protos.h>
//#include <hardware/custom.h>
#include <clib/debug_protos.h>

//#include "debug.h"

#define MemNeeded 500

#define WIDTH 640
#define HEIGHT 256
#define BUFSIZE 1000

#define RP s->RastPort

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
extern struct ExecBase *SysBase;

struct Custom *Hardware;

//extern PLANEPTR rasters[9]; /*Scrolling bitplane pointers. */

struct BitMap BitMap;
PLANEPTR PlanePtr=NULL;

struct NewScreen NewScreen =
		{
			0,0,WIDTH,HEIGHT,2,
			1,0,
			HIRES,
			CUSTOMSCREEN,
			NULL,
			"",
			NULL,NULL
		};

struct Screen *OpenScreen();
struct Screen *s;

struct GfxBase *GraphicsBase;
extern struct Custom *Hardware;

UWORD SystemInts;
UWORD SystemDMA;

UBYTE *ICRA=(UBYTE *)0xbfed01;
UBYTE *PortA=(UBYTE *)0xbfe001;

ULONG Level2Vector;
ULONG Level3Vector;

void TakeSystem()
{
   /*Save current interrupts and DMA settings so we can restore them upon exit. */
   SystemInts=Hardware->intenar;
   SystemDMA=Hardware->dmaconr;

   /*Kill all interrupts and DMA. */
   Hardware->intena=0x7fff;
   Hardware->dmacon=0x7fff;

   /*Also kill interrupts at the CIA controller itself, which handles keyboard interrupts among others, just to be on the safe side. */
   *ICRA=0x7f;

   /*Save current vectors for level 2 and 3 interrupts (those dealing with keyboard and other I/O and with the video hardware), so that we can replace them with new routines. */
   Level2Vector=*(ULONG *)0x68;
   Level3Vector=*(ULONG *)0x6c;
}

void FreeSystem()
{
   /*Restore level 2 and 3 interrupt vectors. */
   *(ULONG *)0x68=Level2Vector;
   *(ULONG *)0x6c=Level3Vector;

   /*Restore system copper list(s). */
   Hardware->cop1lc=(ULONG)GraphicsBase->copinit;
   Hardware->cop2lc=(ULONG)GraphicsBase->LOFlist;

   /*Restore all interrupts and DMA settings. */
   Hardware->intena=(SystemInts|0xc000);
   Hardware->dmacon=(SystemDMA|0x8100);
   *ICRA=0x9b;
}

/*Clear all of screen memory. */
void clear_screen()
{
  KPrintF("clear_screen()\n");
  //ULONG *i;

  //for (i=(ULONG *)rasters[0];i<(ULONG *)(rasters[0]+MemNeeded);i++)
  //   *i=0;
  //InitBitMap(&BitMap,5,320,256);
  //PlanePtr=(PLANEPTR)AllocMem(36288,MEMF_CHIP);
  Hardware->color[0]=0x0000;

  //BltClear(PlanePtr,36288,1);
  
}

void init_screen()
{
  KPrintF("init_screen()\n");

  GraphicsBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  //IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

  
  //printf("here");

  Hardware=(struct Custom *)0xdff000;
  
  TakeSystem();

  Hardware->bltafwm=0xffff;
  Hardware->bltalwm=0xffff;

  clear_screen();
  
  //Delay(20);
  
  
  
  //if ((IntuitionBase = (struct IntuitionBase *)

  //s = OpenScreen (&NewScreen);

  //font
  //tf = (struct TextFont *)OpenFont(&myAttr);
  
  //black screen
  //SetRGB4(&s->ViewPort,0,0,0,0);
  
  //clear screen
  //SetRast(&RP,0);
  //SetFont(&RP,tf);
}

void cleanup()
{
  KPrintF("cleanup()\n");
  
  FreeSystem();
}

