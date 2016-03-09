#include <stdio.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>
//#include <dos/dos.h>
//#include <exec/exec.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>

#define WIDTH 640
#define HEIGHT 256
#define BUFSIZE 1000

#define RP s->RastPort

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
extern struct ExecBase *SysBase;

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

void clear_screen()
{
  printf("In clear screen\n");

  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

  //chip = AvailMem(MEMF_CHIP);
  //printf("Chip: %d\n", chip);
  //printf("here");

  //if ((IntuitionBase = (struct IntuitionBase *)

  s = OpenScreen (&NewScreen);

  //font
  //tf = (struct TextFont *)OpenFont(&myAttr);
  
  //black screen
  SetRGB4(&s->ViewPort,0,0,0,0);
  
  //clear screen
  SetRast(&RP,0);
  //SetFont(&RP,tf);
}
