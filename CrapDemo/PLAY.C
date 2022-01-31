// quick example of how to use the player in your own stuff

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <kernel.h>
#include <libtap.h>

#include "hitmod.h"

#include "1.h"
#include "2.h"
#include "3.h"
#include "4.h"

#define PACKETMAX 10000
#define PACKETMAX2 (PACKETMAX*24)

#define OT_LENGTH 6

int main(void);
void initTexture(u_long*);
void initsprites(void);
void do_scroll(void);

GsOT WorldOrderingTable[2];
GsOT_TAG zSortTable[2][1<<OT_LENGTH];   
PACKET GpuOutputPacket[2][PACKETMAX2];

GsSPRITE sprite[40];

char bufA[64],bufB[64];
long len_buf=64;
long PadStatus;
int light_on=0;
int i;
int text_pos=0;
int drum_vol=0;
int cos_pos=0;
int cos_on=0;

#include "text.h"
#include "cosin.h"

int main(void)
{
unsigned int outputBufferIndex;

printf("\n\nC r a p D e m o !\n");
printf("\nBy Tursi of Moving Target\n\n");
printf("- Epilepsy Warning - ;)\n");
printf("Flashing lights ahead! Be warned!\n\n");

PadInit(0);
InitTAP((char*)bufA,len_buf,(char*)bufB,len_buf);
StartTAP();

MOD_Init();
MOD_Load((unsigned char*)dat1);

SetVideoMode(MODE_NTSC);
ResetGraph(0);
GsInitGraph(320, 240, GsOFSGPU|GsNONINTER, 0, 0);
GsDefDispBuff(0, 0, 0, 240);

WorldOrderingTable[0].length = OT_LENGTH;
WorldOrderingTable[1].length = OT_LENGTH;
WorldOrderingTable[0].org = zSortTable[0];
WorldOrderingTable[1].org = zSortTable[1];

initTexture((u_long*)dat2);
initTexture((u_long*)dat3);
initTexture((u_long*)dat4);

MOD_Start();

initsprites();

while(1)
  {
  outputBufferIndex = GsGetActiveBuff();
  GsSetWorkBase((PACKET*)GpuOutputPacket[outputBufferIndex]);
  GsClearOt(0, 0, &WorldOrderingTable[outputBufferIndex]);

  PadStatus=PadRead(0);
  if (PadStatus&0x8)
  { printf("PAL mode\n");
    SetVideoMode(MODE_PAL);
  }
    
  if (PadStatus&0x4)
  { printf("NTSC mode\n");
    SetVideoMode(MODE_NTSC);
  }

  if (drum_vol)
  { drum_vol-=8;
    if (drum_vol<0) drum_vol=0;
  }

  for (i=0; i<4; i++)
  { if (MOD_LastSample[i]==5)
    { if (MOD_Volume[i]>drum_vol)
        drum_vol=MOD_Volume[i];
      MOD_LastSample[i]=0;
    }
  }

  do_scroll();

  /* misc effects */
  if (sprite[2].rotate)
  { sprite[2].rotate+=ONE*6;
    if (sprite[2].rotate>=360*ONE)
      sprite[2].rotate=0;
  }

  if (light_on)
  { sprite[0].r=drum_vol*4;
    sprite[0].g=drum_vol*4;
    sprite[0].b=drum_vol*4;
    sprite[1].r=drum_vol*4;
    sprite[1].g=drum_vol*4;
    sprite[1].b=drum_vol*4;
  }

  DrawSync(0);
  VSync(0);
  GsSwapDispBuff();

  for (i=0; i<37; i++)
  { GsSortSprite(&sprite[i],&WorldOrderingTable[outputBufferIndex],38-i);
  }

  GsSortClear(0, 0, 0, &WorldOrderingTable[outputBufferIndex]); 

  GsDrawOt(&WorldOrderingTable[outputBufferIndex]);
  }
	
return 0;
}

void initTexture(u_long *addr)
{ RECT rect1;
  GsIMAGE TexInfo;

  addr++;

  GsGetTimInfo(addr, &TexInfo);

  TexInfo.pmode&=0x03;
  rect1.x=TexInfo.px;
  rect1.y=TexInfo.py;
  rect1.w=TexInfo.pw;
  rect1.h=TexInfo.ph;
  printf("Data at %d, %d, mode %d\n",rect1.x,rect1.y,TexInfo.pmode);

  LoadImage(&rect1,TexInfo.pixel);
  /* dma xfer from memory to videoram */

  /* do color table */
  if (TexInfo.pmode<2)
  { rect1.x=TexInfo.cx;
    rect1.y=TexInfo.cy;
    rect1.w=TexInfo.cw;
    rect1.h=TexInfo.ch;
    printf("Color at %d, %d\n",rect1.x,rect1.y);
    LoadImage(&rect1,TexInfo.clut);
  }

  DrawSync(0);
  /* wait for finish */


}

void initsprites()
{ int i;

  i=0;
  sprite[i].attribute=(2<<24);
  sprite[i].w=256;
  sprite[i].h=240;
  sprite[i].x=0;
  sprite[i].y=0;
  sprite[i].tpage=GetTPage(2,0,320,0);
  sprite[i].u=0;
  sprite[i].v=0;
  sprite[i].cx=0;
  sprite[i].cy=0;
  sprite[i].r=0x0;
  sprite[i].g=0x0;
  sprite[i].b=0x0;
  sprite[i].mx=0;
  sprite[i].my=0;
  sprite[i].rotate = 0;
  sprite[i].scalex = ONE;
  sprite[i].scaley = ONE;

  i++;
  sprite[i].attribute=(2<<24);
  sprite[i].w=64;
  sprite[i].h=240;
  sprite[i].x=256;
  sprite[i].y=0;
  sprite[i].tpage=GetTPage(2,0,576,0);
  sprite[i].u=0;
  sprite[i].v=0;
  sprite[i].cx=0;
  sprite[i].cy=0;
  sprite[i].r=0x0;
  sprite[i].g=0x0;
  sprite[i].b=0x0;
  sprite[i].mx=0;
  sprite[i].my=0;
  sprite[i].rotate = 0;
  sprite[i].scalex = ONE;
  sprite[i].scaley = ONE;

  i++;
  sprite[i].attribute=(0<<24);
  sprite[i].w=187;
  sprite[i].h=183;
  sprite[i].x=160;
  sprite[i].y=120;
  sprite[i].tpage=GetTPage(0,0,320,256);
  sprite[i].u=0;
  sprite[i].v=0;
  sprite[i].cx=640;
  sprite[i].cy=256;
  sprite[i].r=0x80;
  sprite[i].g=0x80;
  sprite[i].b=0x80;
  sprite[i].mx=93;
  sprite[i].my=91;
  sprite[i].rotate = 0;
  sprite[i].scalex = ONE;
  sprite[i].scaley = ONE;

  while (i<37)
  {
    i++;
    sprite[i].attribute=(0<<24);
    sprite[i].w=10;
    sprite[i].h=20;
    sprite[i].x=(i-4)*10;
    sprite[i].y=200;
    sprite[i].tpage=GetTPage(0,0,640,0);
    sprite[i].u=0;
    sprite[i].v=0;
    sprite[i].cx=640;
    sprite[i].cy=257;
    sprite[i].r=0x80;
    sprite[i].g=0x80;
    sprite[i].b=0x80;
    sprite[i].mx=0;
    sprite[i].my=0;
    sprite[i].rotate = 0;
    sprite[i].scalex = ONE;
    sprite[i].scaley = ONE;
  }
}

void do_scroll()
{ /* do the scrolltext */
int i,ty;

if (cos_on)
{ cos_pos++;
  if (cos_pos>320) cos_pos=0;
}

for (i=3; i<37; i++)
{ sprite[i].x-=2;
  if (cos_on)
  { ty=COS[(cos_pos)+sprite[i].x];
    if (sprite[i].y>ty)
      sprite[i].y-=(sprite[i].y-ty<10 ? sprite[i].y-ty : 10);
    if (sprite[i].y<ty)
      sprite[i].y+=(ty-sprite[i].y<10 ? ty-sprite[i].y : 10);
  }
  if (sprite[i].x<(-10))
  { sprite[i].x=330;
    text_pos++;
    if (text_dat[text_pos]==0)
      text_pos=0;
    if (text_dat[text_pos]=='*')
    { text_pos++;
      sprite[2].rotate+=ONE*6;
    }
    if (text_dat[text_pos]=='&')
    { text_pos++;
      light_on=1;
    }
    if (text_dat[text_pos]=='%')
    { text_pos++;
      cos_on=1;
    }
    sprite[i].u=((text_dat[text_pos]-' ')%25)*10;
    sprite[i].v=((text_dat[text_pos]-' ')/25)*20;
  }
}
}
