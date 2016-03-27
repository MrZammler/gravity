//#include <stdio.h>
//#include <graphics/rastport.h>
//#include <intuition/intuition.h>
//#include <dos/dos.h>
//#include <exec/exec.h>
//#include <clib/exec_protos.h>
//#include <graphics/gfxbase.h>
//#include <clib/graphics_protos.h>
//#include <hardware/custom.h>
#include <hardware/cia.h>
#include <clib/debug_protos.h>
//#include <clib/debug_protos.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <proto/exec.h>

#define INTERRUPT(NAME, PRI, HANDLER)       \
static struct Interrupt NAME = {            \
  { NULL, NULL, NT_INTERRUPT, PRI, #NAME }, \
  NULL, (APTR)&HANDLER                      \
};

#define E_CLOCK 0.709379
#define TIMER_US(ms) ((ms) * E_CLOCK)

extern BOOL end_game;

//#include <hardware/cia.h>
//#define CIAA 0xBFE001

//struct CIA *cia = (struct CIA *) CIAA;
struct CIA* const ciaa = (APTR)0xbfe001;
struct CIA* const ciab = (APTR)0xbfd000;

void WaitTimerB(volatile struct CIA *cia, UWORD delay) {
  //KPrintF("Delay\n");
  cia->ciacra |= CIACRBF_RUNMODE;
  cia->ciaicr = CIAICRF_TB;
  cia->ciatalo = delay;
  cia->ciatahi = delay >> 8;
  while (cia->ciaicr & CIAICRF_TB);
}

//#include "debug.h"

//extern struct Custom *Hardware;

//UWORD SystemInts;
//UWORD SystemDMA;

//UBYTE *ICRA=(UBYTE *)0xbfed01;
//UBYTE *PortA=(UBYTE *)0xbfe001;

//ULONG Level2Vector;
//ULONG Level3Vector;

#define QUEUELEN 32

static UBYTE rawkey[QUEUELEN];
static UBYTE head, tail, used;
static UBYTE modifier;

void CheckKey();

static inline void PushRawKey(UBYTE raw) {

  if (used < QUEUELEN) {
    rawkey[tail] = raw;
    tail = (tail + 1) & (QUEUELEN - 1);
    used++;
    //CheckKey();
  }
}

static WORD PopRawKey() {
  WORD raw = -1;

  Disable();

  if (used > 0) {
    raw = rawkey[head];
    head = (head + 1) & (QUEUELEN - 1);
    used--;
  }

  Enable();

  //KPrintF ("%i\n", raw);
  return raw;
}

static __interrupt LONG KeyboardIntHandler() {

  //UBYTE code;
  KPrintF("a");
  //KPrintF("%d %d %d \n", tail, used, head);
  
  if (ciaa->ciaicr & CIAICRF_SP) {
    
    /* Read keyboard data register. */
    UBYTE sdr = ciaa->ciasdr;
    /* Set serial port to output mode. */
    ciaa->ciacra |= CIACRAF_SPMODE;
    /* Send handshake. */
    ciaa->ciasdr = 0;
    /* Save raw key in the queue. Filter out exceptional conditions. */
    {
      UBYTE raw = ((sdr >> 1) | (sdr << 7)) ^ 0xff;
      //KPrintF("%x\n", sdr);
      switch (raw) {
        case 0x78: /* Reset warning. */
        case 0xf9: /* Last key code bad. */
        case 0xfa: /* Keyboard key buffer overflow. */
        case 0xfc: /* Keyboard self-test fail. */
        case 0xfd: /* Initiate power-up key stream. */
        case 0xfe: /* Terminate power-up key stream. */
          break;
        default:
          PushRawKey(raw);
          break;
      }
    }
    /* Wait for at least 85us for handshake to be registered. */
    WaitTimerB(ciab, TIMER_US(85));
    /* Set back to input mode. */
    ciaa->ciacra &= (UBYTE)~CIACRAF_SPMODE;
  }

  return 0;
}

INTERRUPT(KeyboardInterrupt, -5, KeyboardIntHandler)

void KeyboardInit() {
  /* Disable all CIA-A interrupts. */
  ciaa->ciaicr = (UBYTE)(~CIAICRF_SETCLR);
  /* Enable keyboard interrupt.
   * The keyboard is attached to CIA-A serial port. */
  ciaa->ciaicr = CIAICRF_SETCLR | CIAICRF_SP;
  ciaa->ciacra = (UBYTE)(~CIACRAF_SPMODE);

  KPrintF("KeyboardInit()\n");
  
  AddIntServer(INTB_PORTS, &KeyboardInterrupt);
}

void KeyboardKill() {
  RemIntServer(INTB_PORTS, &KeyboardInterrupt);
}

void CheckKey()
{
  WORD raw = PopRawKey();
  UBYTE code;

  code = raw & 0x7f;

  if (raw == 117)
    KPrintF(" -----------     ESC --------------------------\n", code);

  //KPrintF(" -----------         %x --------------------------\n", code);

}

void throttle()
{
  KPrintF("throttle()\n");

}

void CheckKeyb()
{

  //keycodes in: http://wiki.amigaos.net/wiki/Keymap_Library#Raw_Key_Table
  
  if (ciaa->ciaicr & CIAICRF_SP) {
    
    /* Read keyboard data register. */
    UBYTE sdr = ciaa->ciasdr;
    /* Set serial port to output mode. */
    ciaa->ciacra |= CIACRAF_SPMODE;
    /* Send handshake. */
    ciaa->ciasdr = 0;
    /* Save raw key in the queue. Filter out exceptional conditions. */
    
    {
      UBYTE raw = ((sdr >> 1) | (sdr << 7)) ^ 0xff;
      //KPrintF(" *** %x  ****", raw);
      switch (raw) {
      case 0x78: /* Reset warning. */
      case 0xf9: /* Last key code bad. */
      case 0xfa: /* Keyboard key buffer overflow. */
      case 0xfc: /* Keyboard self-test fail. */
      case 0xfd: /* Initiate power-up key stream. */
      case 0xfe: /* Terminate power-up key stream. */
	break;
      case 0x45: /* ESC */
	end_game = TRUE;
	break;
      case 0x4C: /* Curs up */
	throttle();
	break;
      default:
	//PushRawKey(raw);
	break;
      }
    }
    /* Wait for at least 85us for handshake to be registered. */
    WaitTimerB(ciab, TIMER_US(85));
    /* Set back to input mode. */
    ciaa->ciacra &= (UBYTE)~CIACRAF_SPMODE;
  }

}

