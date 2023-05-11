/** ColEm: portable Coleco emulator **************************/
/**                                                         **/
/**                         Unix.c                          **/
/**                                                         **/
/** This file contains Unix-dependent subroutines and       **/
/** drivers. It includes screen drivers via Display.h.      **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2021                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "Coleco.h"
#include "Console.h"
#include "EMULib.h"
#include "NetPlay.h"
#include "Record.h"
#include "Sound.h"

#include <string.h>
#include <stdio.h>

#define WIDTH       272                   /* Buffer width    */
#define HEIGHT      200                   /* Buffer height   */

/* Combination of EFF_* bits */
int UseEffects  = EFF_SCALE|EFF_SAVECPU|EFF_MITSHM|EFF_VARBPP|EFF_SYNC;

int InMenu;                /* 1: In MenuColeco(),ignore keys */
int UseZoom     = 2;       /* Zoom factor (1=no zoom)        */
int UseSound    = 22050;   /* Audio sampling frequency (Hz)  */
int SyncFreq    = 60;      /* Sync frequency (0=sync off)    */
int FastForward;           /* Fast-forwarded UPeriod backup  */
int SndSwitch;             /* Mask of enabled sound channels */
int SndVolume;             /* Master volume for audio        */
int Keypad;                /* Keypad key being pressed       */
Image ScrBuf;              /* Display buffer                 */

char *Title = "ColEm 5.6";

void HandleKeys(unsigned int Key);

/** InitMachine() ********************************************/
/** Allocate resources needed by machine-dependent code.    **/
/*************************************************************/
int InitMachine(void)
{
  /* Initialize variables */
  UseZoom        = UseZoom<1? 1:UseZoom>5? 5:UseZoom;
  VDP.DrawFrames = UPeriod;
  InMenu         = 0;
  FastForward    = 0;
  ScrBuf.Data    = 0;
  Keypad         = 0;

  /* Initialize system resources */
  InitUnix(Title,UseZoom*WIDTH,UseZoom*HEIGHT);

  /* Set visual effects */
  SetEffects(UseEffects);

  /* Initialize video */
  if(!NewImage(&ScrBuf,WIDTH,HEIGHT)) return(0);
  ScrWidth  = WIDTH;
  ScrHeight = HEIGHT;
  ScrBuffer = ScrBuf.Data;

  /* Set correct screen drivers */
  if(!SetScreenDepth(ScrBuf.D)) { TrashUnix();return(0); }

  /* Set video buffer */
  SetVideo(&ScrBuf,0,0,WIDTH,HEIGHT);

  /* Attach keyboard handler */
  SetKeyHandler(HandleKeys);

  /* Initialize sound */
  InitSound(UseSound,150);
  SndSwitch=(1<<(SN76489_CHANNELS+AY8910_CHANNELS))-1;
  SndVolume=255/SN76489_CHANNELS;
  SetChannels(SndVolume,SndSwitch);

  /* Initialize sync timer if needed */
  if((SyncFreq>0)&&!SetSyncTimer(SyncFreq*UPeriod/100)) SyncFreq=0;

  /* Initialize record/replay */
  RPLInit(SaveState,LoadState,MAX_STASIZE);
  RPLRecord(RPL_RESET);

  /* Done */
  return(1);
}

/** TrashMachine() *******************************************/
/** Deallocate all resources taken by InitMachine().        **/
/*************************************************************/
void TrashMachine(void)
{
  /* Flush and free recording buffers */
  RPLTrash();

  SetSyncTimer(0);
  FreeImage(&ScrBuf);
  TrashAudio();
  TrashUnix();
}

/** Mouse() **************************************************/
/** This function is called to poll mouse. It should return **/
/** the X coordinate (-512..+512) in the lower 16 bits, the **/
/** Y coordinate (-512..+512) in the next 14 bits, and the  **/
/** mouse buttons in the upper 2 bits. All values should be **/
/** counted from the screen center!                         **/
/*************************************************************/
unsigned int Mouse(void)
{
  unsigned int X,Y;

  X = GetMouse();
  Y = ((((X>>16)&0x0FFF)<<10)/200-512)&0x3FFF;
  Y = (Y<<16)|((((X&0x0FFF)<<10)/320-512)&0xFFFF);
  return(Y|(X&MSE_BUTTONS));
}

/** Joystick() ***********************************************/
/** This function is called to poll joysticks. It should    **/
/** return 2x16bit values in a following way:               **/
/**                                                         **/
/**      x.FIRE-B.x.x.L.D.R.U.x.FIRE-A.x.x.N3.N2.N1.N0      **/
/**                                                         **/
/** Least significant bit on the right. Active value is 1.  **/
/*************************************************************/
unsigned int Joystick(void)
{
  unsigned int J,I;

  /* Render audio (two frames to avoid skipping) */
  RenderAndPlayAudio(UseSound/30);

  /* Get joystick state and keypad buttons */
  J = GetJoystick();
  J = RPLControls(J);
  I = J&BTN_ALT? (Keypad<<16):Keypad;

  /* Parse joystick state */
  if(J&BTN_SHIFT)       J|=J&BTN_ALT? (BTN_FIREA<<16):BTN_FIREA;
  if(J&BTN_CONTROL)     J|=J&BTN_ALT? (BTN_FIREB<<16):BTN_FIREB;
  if(J&BTN_LEFT)        I|=JST_LEFT;
  if(J&BTN_RIGHT)       I|=JST_RIGHT;
  if(J&BTN_UP)          I|=JST_UP;
  if(J&BTN_DOWN)        I|=JST_DOWN;
  if(J&BTN_FIREA)       I|=JST_FIRER;
  if(J&BTN_FIREB)       I|=JST_FIREL;
  if(J&BTN_FIREL)       I|=JST_PURPLE;
  if(J&BTN_FIRER)       I|=JST_BLUE;
  if(J&(BTN_LEFT<<16))  I|=JST_LEFT<<16;
  if(J&(BTN_RIGHT<<16)) I|=JST_RIGHT<<16;
  if(J&(BTN_UP<<16))    I|=JST_UP<<16;
  if(J&(BTN_DOWN<<16))  I|=JST_DOWN<<16;
  if(J&(BTN_FIREA<<16)) I|=JST_FIRER<<16;
  if(J&(BTN_FIREB<<16)) I|=JST_FIREL<<16;
  if(J&(BTN_FIREL<<16)) I|=JST_PURPLE<<16;
  if(J&(BTN_FIRER<<16)) I|=JST_BLUE<<16;

  /* Replay joystick state */
  J = RPLPlay(RPL_NEXT);
  I = J!=RPL_ENDED? J:I;

  /* Make sure VDP has correct frameskip after state restored */
  VDP.DrawFrames=UPeriod;

  /* Record joystick state */
  RPLRecord(I);

  /* If exchanged keymaps with remote... */
  if(NETExchange((char *)&J,(const char *)&I,sizeof(I)))
    /* Merge joysticks, server is player #1, client is player #2 */
    I = NETConnected()==NET_SERVER?
        ((I&0xFFFF)|((J&0xFFFF)<<16))
      : ((J&0xFFFF)|((I&0xFFFF)<<16));

  /* Done */
  return(I);
}

/** RefreshScreen() ******************************************/
/** Refresh screen. This function is called in the end of   **/
/** refresh cycle to show the entire screen.                **/
/*************************************************************/
void RefreshScreen(void *Buffer,int Width,int Height)
{
  /* Show replay icon */
  if(RPLPlay(RPL_QUERY)) RPLShow(&ScrBuf,(WIDTH-256)/2+10,(HEIGHT-192)/2+10);

  /* Display the screen buffer */
  ShowVideo();
}

/** SetColor() ***********************************************/
/** Set color N to (R,G,B).                                 **/
/*************************************************************/
int SetColor(byte N,byte R,byte G,byte B)
{
  return(X11GetColor(R,G,B));
}

/** HandleKeys() *********************************************/
/** Key handler.                                            **/
/*************************************************************/
void HandleKeys(unsigned int Key)
{
  unsigned int Mod,IsAdam;

  /* Do not enter this function twice */
  if(InMenu||CPU.Trace) return;

  /* Separate key codes and modifiers */
  Mod = Key & ~CON_KEYCODE;
  Key = Key & CON_KEYCODE;

  /* When in Adam mode, use CTRL key for special functions */
  IsAdam = (Mode&CV_ADAM) && !(Mod&CON_CONTROL);

  if(Mod&CON_RELEASE)
  {
    switch(Key)
    {
      case XK_F9:
      case XK_Page_Up:
        if(FastForward)
        {
          SetEffects(UseEffects);
          VDP.DrawFrames=UPeriod=FastForward;
          FastForward=0;
        }
        break;
      case '0':     Keypad&=~JST_0;break;
      case '1':     Keypad&=~JST_1;break;
      case '2':     Keypad&=~JST_2;break;
      case '3':     Keypad&=~JST_3;break;
      case '4':     Keypad&=~JST_4;break;
      case '5':     Keypad&=~JST_5;break;
      case '6':     Keypad&=~JST_6;break;
      case '7':     Keypad&=~JST_7;break;
      case '8':     Keypad&=~JST_8;break;
      case '9':     Keypad&=~JST_9;break;
      case '-':     Keypad&=~JST_STAR;break;
      case '=':     Keypad&=~JST_POUND;break;
    }
  }
  else
  {
    /* Cancel replay when a key is pressed */
    if((Key!=XK_F9)&&(Key!=XK_Page_Up)&&(Key!=XK_Left)&&(Key!=XK_Right)&&(Key!=XK_Up)) RPLPlay(RPL_OFF);

    switch(Key)
    {
      case XK_F1:
#ifdef DEBUG
        if(IsAdam) PutKBD(Mod|KEY_F1); else CPU.Trace=1;
#else
        PutKBD(Mod|KEY_F1);
#endif
        break;

      case XK_F2:
        if(IsAdam) PutKBD(Mod|KEY_F2); else MIDILogging(MIDI_TOGGLE);
        break;

      case XK_F3:
        if(IsAdam) PutKBD(Mod|KEY_F3); else Mode^=CV_AUTOFIRER;
        break;

      case XK_F4:
        if(IsAdam) PutKBD(Mod|KEY_F4); else Mode^=CV_AUTOFIREL;
        break;

      case XK_F5:
        if(IsAdam) PutKBD(Mod|KEY_F5);
        else
        {
          InMenu=1;
          SetChannels(0,0);
          MenuColeco();
          SetChannels(SndVolume,SndSwitch);
          InMenu=0;
        }
        break;

      case XK_F6:
        if(IsAdam) PutKBD(Mod|KEY_F6);
        else
        {
          LoadSTA(StaName? StaName:"DEFAULT.STA");
          RPLPlay(RPL_OFF);
        }
        break;

      case XK_F7:
        if(IsAdam) PutKBD(Mod|KEY_UNDO); else SaveSTA(StaName? StaName:"DEFAULT.STA");
        break;

      case XK_F8:
        if(IsAdam) PutKBD(Mod|KEY_WILDCARD); else RPLPlay(RPL_RESET);
        break;

      case XK_F9:
      case XK_Page_Up:
        if(IsAdam && (Key!=XK_F9)) PutKBD(Mod|KEY_MOVE);
        else if(!FastForward)
        {
          SetEffects(UseEffects&~EFF_SYNC);
          FastForward=UPeriod;
          VDP.DrawFrames=UPeriod=10;
        }
        break;

      case XK_Escape:
        if(IsAdam) PutKBD(Mod|KEY_ESC); else ExitNow=1;
        break;

      case XK_F11:  ResetColeco(Mode);RPLPlay(RPL_OFF);break;
      case XK_F12:  ExitNow=1;break;
      case '0':     PutKBD(Mod|'0');Keypad|=JST_0;break;
      case '1':     PutKBD(Mod|'1');Keypad|=JST_1;break;
      case '2':     PutKBD(Mod|'2');Keypad|=JST_2;break;
      case '3':     PutKBD(Mod|'3');Keypad|=JST_3;break;
      case '4':     PutKBD(Mod|'4');Keypad|=JST_4;break;
      case '5':     PutKBD(Mod|'5');Keypad|=JST_5;break;
      case '6':     PutKBD(Mod|'6');Keypad|=JST_6;break;
      case '7':     PutKBD(Mod|'7');Keypad|=JST_7;break;
      case '8':     PutKBD(Mod|'8');Keypad|=JST_8;break;
      case '9':     PutKBD(Mod|'9');Keypad|=JST_9;break;
      case '-':     PutKBD(Mod|'-');Keypad|=JST_STAR;break;
      case '=':     PutKBD(Mod|'=');Keypad|=JST_POUND;break;

      case XK_BackSpace: PutKBD(Mod|KEY_BS);break;
      case XK_Return:    PutKBD(Mod|KEY_ENTER);break;
      case XK_Tab:       PutKBD(Mod|KEY_TAB);break;
      case XK_Up:        PutKBD(Mod|KEY_UP);break;
      case XK_Down:      PutKBD(Mod|KEY_DOWN);break;
      case XK_Left:      PutKBD(Mod|KEY_LEFT);break;
      case XK_Right:     PutKBD(Mod|KEY_RIGHT);break;
      case XK_Home:      PutKBD(Mod|KEY_HOME);break;
      case XK_End:       PutKBD(Mod|KEY_CLEAR);break;
      case XK_Insert:    PutKBD(Mod|KEY_INS);break;
      case XK_Delete:    PutKBD(Mod|KEY_DEL);break;
      case XK_Page_Down: PutKBD(Mod|KEY_STORE);break;
      case XK_Print:     PutKBD(Mod|KEY_PRINT);break;

      default:
        /* Assume any key in 0x00..0x7E range an ASCII code */
        if(Key<='~') PutKBD(Key|Mod);
        break;
    }
  }
}

/** Display.h ************************************************/
/** Display drivers for all possible display depths.        **/
/*************************************************************/
#include "Display.h"
