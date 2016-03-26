#include <stdio.h>
//#include <hardware/custom.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <clib/debug_protos.h>
#include <inline/exec_protos.h>

#define MemNeeded 500


PLANEPTR rasters[9]; /*Scrolling bitplane pointers. */

extern void init_screen();
extern void cleanup();


int main(int argc, char *argv[])
{
  printf("Welcome to Gravity!\n");
  KPrintF("\n---- Session start -----\n");

  ULONG chip = AvailMem(MEMF_CHIP);
  KPrintF("Chip free: %ld\n", chip);
  
  init_screen();

  for (int i=0;i<=100000;i++) {}

  KPrintF("Are we alive ? %s\n","maybe");


  cleanup();


  
  
  return 0;
}
