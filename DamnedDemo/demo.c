/* psyq */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h>
#include <libgs.h>
#include <libmath.h>
#include <kernel.h>
#include "hitmod.h"

#define ONE 4096

#define FIRE_HOTSPOTS 48

#define OT_LENGTH (3)
#define PACKETMAX (10000)
#define PACKETMAX2 (PACKETMAX * 24)

GsOT            WorldOrderingTable[2];
GsOT_TAG        zSortTable[2][1<<OT_LENGTH];
PACKET          GpuOutputPacket[2][PACKETMAX2];
volatile u_char *bb0, *bb1;
u_long          PadStatus;
int             buff, FRAME;
RECT            myRect;
int             filt, dir, filnum;
int             layer, c_flag;
int             blue=1;
char            *pText;
int             bright[2], flip;
int lightning, Fire, Stars, ballson;
int yaw, roll, pitch;
int SPEED, TextSine;
int high=0, low=999;
int firebottom=-1;
int lightX, lightY, lightDIRX, lightDIRY;
int ballXS, ballYS, ballFRAME;
int MODReady, StarRange;

unsigned short firebuf[256];

struct PAL {
  unsigned int r,g,b;
} palette[256];

unsigned short mypal[256];
int hotspot[FIRE_HOTSPOTS];     // this fire code from the Allegro sample

int pattern[65]={
16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,
 0, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
16, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
 0, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47,
16
};

float SINE[120], COSINE[120], TAN[90];
GsLINE Star[100];
short StarZ[100];
GsSPRITE Sprites[80];           // overlay sprites
GsSPRITE Misc[32];
GsSPRITE Text[15];
GsSPRITE Lightning[2];
GsSPRITE Balls[35];

void main();
void dofilter();
void setup();
void drawscreen();
void palOrNtsc();
void initTexture(u_long *addr);
void initsprites();
void setsprite(int i, int x, int y, int w, int h, int tx, int ty, int cx, int cy, int tm);
void Cycle();
void setup_text();
int pos(char);
void movetext();
void dodemo();
void dolightning();
void dostars();
void initstars();
void dorotate(short*,short*,short*);
void initfire();
void dofire();
int MIN(int,int);
int MAX(int,int);
void StartFire();
void initballs();
void stars_in();
void stars_out();
void balls_in();
void balls_out();
void fire_out();

// TIM files
#include "filters.inc"
#include "wpw.inc"
#include "balls.inc"
#include "tursi.inc"
#include "font.inc"
#include "light.inc"
#include "anarchy.inc"

char Story[]=
#include "story.txt"
;

int MIN(int a, int b)
{
  if (a<b) return a;
  else return b;
}

int MAX(int a, int b)
{
  if (a>b) return a;
  else return b;
}

void main()
{
  int i, flag;

  c_flag=0;

  setup();

  SPEED=0;      // passed to VSYNC

  printf("\n\n\n*** Here we go, ladies and gents.. ***\n");
  printf("Fasten your seatbelts, and keep your hands,\n");
  printf("paws, and other appendages inside the compartment\n");
  printf("at all times...\n\n");

  filnum=0;
  dir=1;
  flag=1;
  layer=0;
  TextSine=0;

  while (flag)
  {
    flag=0;
    filt=0;
    for (i=0; i<80; i++)
    {
      if (Sprites[i].y < (i/10)*32)
      {
        flag=1;
        Sprites[i].y+=8;
      }
    }
    drawscreen();
  }

  for (i=0; i<60; i++)
    drawscreen();

  setsprite(0, 163, 128, 252, 209, 448, 0, 0, 481, 1);

  printf("W-P-W makes their debut...\n");

  for (filnum=16; filnum<33; filnum++)
  {
    filt=pattern[filnum];
    for (i=0; i<5; i++)
      drawscreen();
  }

  for (i=0; i<60; i++)
    drawscreen();

  for (filnum=64; filnum>47; filnum--)
  {
    filt=pattern[filnum];
    for (i=0; i<5; i++)
      drawscreen();
  }

  for (i=0; i<60; i++)
    drawscreen();

  printf("with this demo by Tursi...\n");

  setsprite(0, 160, 120, 256, 240, 384, 256, 0, 483, 1);

  for (filnum=16; filnum>=0; filnum--)
  {
    filt=pattern[filnum];
    for (i=0; i<5; i++)
      drawscreen();
  }

  for (i=0; i<60; i++)
    drawscreen();

  if (MODReady)
  {
    MOD_Start();
    MODReady=0;
  }

  for (filnum=0; filnum<17; filnum++)
  {
    filt=pattern[filnum];
    for (i=0; i<5; i++)
      drawscreen();
  }

  layer=7;              // move gfx to back
  Misc[0].x=-1;         // turn off logo

  // cycle in a blue palette
  c_flag=1;

  for (i=0; i<60; i++)
    drawscreen();

  // set up lightning
  setsprite(0, 160, 0, 256, 190, 768, 0, 0, 485, 1);    // cheat
  memcpy(&Lightning[0], &Misc[0], sizeof(GsSPRITE));
  lightning=1;
  Misc[0].x=-1;
  Lightning[0].scalex=5120; //125%
  Lightning[0].scaley=5120;
  Lightning[0].mx=128;
  Lightning[0].my=0;
  Lightning[0].r=0;
  Lightning[0].g=0;
  Lightning[0].b=0;
  // H-flipped, 8-bit, 100+100, semi-trans on
  Lightning[0].attribute= (1<<23) | (1<<24) | (1<<28) | (1<<30);
  memcpy(&Lightning[1], &Lightning[0], sizeof(GsSPRITE));
  Lightning[1].attribute= (1<<24) | (1<<28) | (1<<30);
  bright[0]=0;
  bright[1]=0;
  flip=1;
  
  SPEED=2;              // 30hz
  dodemo();
}

void setup()
{ /* setup system */
int i,x,y;
u_long colorMode;
unsigned short tp;
RECT myrect;
double dbl;

i=0;
printf("...building tables...\n");
for (dbl=0; dbl<6.283; dbl=dbl+(6.2831/120))
{
  SINE[i]=(float)sin(dbl);
  COSINE[i]=(float)cos(dbl);
  i++;
  if (i>119)
    break;
}

i=0;
for (dbl=0; dbl<1.571; dbl=dbl+(1.571/90))
{
  TAN[i++]=tan(dbl);
}

PadStatus = 0;
palOrNtsc();
ResetGraph(0);
GsInitGraph(320, 240, GsOFSGPU|GsNONINTER, 0, 0);                
GsDefDispBuff(0, 0, 0, 240);
WorldOrderingTable[0].length = OT_LENGTH;
WorldOrderingTable[1].length = OT_LENGTH;
WorldOrderingTable[0].org = zSortTable[0];
WorldOrderingTable[1].org = zSortTable[1];

initTexture((u_long*)filters);
initTexture((u_long*)wpw);
initTexture((u_long*)balls);
initTexture((u_long*)tursi);
initTexture((u_long*)font);
initTexture((u_long*)light);

Lightning[0].x=-1;
Lightning[1].x=-1;

initsprites();
setup_text();

pText=Story;

buff=GsGetActiveBuff();
GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
GsClearOt(0, 0, &WorldOrderingTable[buff]);
FRAME=0;

lightning=Fire=Stars=ballson=0;
yaw=roll=pitch=0;

initfire();

// now init the mod... (started by the text)
MOD_Init();
MODReady=MOD_Load((unsigned char*)mod);
}

void drawscreen()
{ int i, s, e, t, i2;
  int u,v, x, y, z;
  GsBOXF mybox;
  SVECTOR v0;
  long Results[4];
  int tmp;
  float tmp2;

  DrawSync(0);

#if 0           // this only works when HITMOD is not running
  t=VSync(1);
  if (0==SPEED)
  {
    if (t>240)
    {
      printf("High VSync: %d/240\n", t);
    }
  }
  else
  {
    if (t>240*SPEED)
    {
      printf("High Vsync: %d/%d\n", t, 240*SPEED);
    }
  }
#endif

  VSync(SPEED);     // 30 fps

  GsSwapDispBuff();

  FRAME++;

  Cycle();

  // copy in the correct filter pattern

  u=((filt%8)<<5);
  v=((filt>>3)<<5);

  for (i=0; i<80; i++)
  {
    Sprites[i].u=u;
    Sprites[i].v=v;

    GsSortFastSpriteB(&Sprites[i], &WorldOrderingTable[buff],layer,0);
  }

  if (lightning)
  {
    if (Lightning[0].x!=-1)
      GsSortSprite(&Lightning[0], &WorldOrderingTable[buff], 5);
    if (Lightning[1].x!=-1)
      GsSortSprite(&Lightning[1], &WorldOrderingTable[buff], 5);
  }

  if (ballson)
  {
    lightX+=lightDIRX;
    if (lightX>315) lightDIRX=-4;
    if (lightX<5) lightDIRX=4;

    lightY+=lightDIRY;
    if (lightY>235) lightDIRY=-4;
    if (lightY<5) lightDIRY=4;

    // light (3)
    Balls[2].x=Balls[1].x;
    Balls[2].y=Balls[1].y;
    Balls[1].x=Balls[0].x;
    Balls[1].y=Balls[0].y;
    Balls[0].x=lightX;
    Balls[0].y=lightY;

    Balls[0].scalex=4096;
    Balls[0].scaley=4096;
    Balls[0].attribute=(1<<30);

    for (i=0; i<5; i++)
    {
      Balls[0].x=lightX+((rand()%15)-7);
      Balls[0].y=lightY+((rand()%15)-7);
      GsSortSprite(&Balls[0], &WorldOrderingTable[buff], 3);
    }

    // bright spot
    Balls[0].x=lightX;
    Balls[0].y=lightY;
    Balls[0].attribute=(3<<28)|(1<<30);
    Balls[0].u=0;
    Balls[0].v=32;
    Balls[0].w=96;
    Balls[0].h=96;
    Balls[0].mx=48;
    Balls[0].my=48;
    GsSortSprite(&Balls[0], &WorldOrderingTable[buff], 4);

    Balls[0].u=32;
    Balls[0].v=0;
    Balls[0].w=32;
    Balls[0].h=32;
    Balls[0].mx=16;
    Balls[0].my=16;
    Balls[0].scalex=8192;
    Balls[0].scaley=8192;
    Balls[0].attribute=(1<<28)|(1<<30);
    GsSortSprite(&Balls[0], &WorldOrderingTable[buff], 2);

    GsSortSprite(&Balls[1], &WorldOrderingTable[buff], 2);
    GsSortSprite(&Balls[2], &WorldOrderingTable[buff], 2);

    z=(FRAME%40)*3;

    ballFRAME++;
    if (ballFRAME>559)
      ballFRAME=0;

    if (ballFRAME<160)
    {
      ballXS=160-ballFRAME;
      goto carryon;
    }
    if (ballFRAME<320)
    {
      ballXS=ballFRAME-160;
      goto carryon;
    }
    if (ballFRAME<440)
    {
      ballYS=120-(ballFRAME-320);
      goto carryon;
    }
    if (ballFRAME<560)
    { 
      ballYS=ballFRAME-440;
      goto carryon;
    }

carryon:
    for (i=3; i<35; i++)
    {
      Balls[i].x=SINE[z]*ballXS + 160;
      Balls[i].y=COSINE[z]*ballYS + 120;
      z+=2;
      if (z>119) z-=120;

      // ball - get rotation
      if (lightX == Balls[i].x)
      {
        if (lightY<Balls[i].y)
          tmp=0;
        else
          tmp=180;
        goto gotit;
      }
      if (lightY == Balls[i].y)
      {
        if (lightX<Balls[i].x)
          tmp=270;
        else
          tmp=90;
        goto gotit;
      }

      // get quadrant, then find angle
      if ((lightX>=Balls[i].x)&&(lightY<=Balls[i].y))
      {
        tmp2=(float)(Balls[i].y-lightY)/(float)(lightX-Balls[i].x);
        for (i2=0; i2<90; i2++)
          if (TAN[i2]>tmp2) break;
        tmp=(90-i2)*ONE;
      }

      if ((lightX>=Balls[i].x)&&(lightY>Balls[i].y))
      {
        tmp2=(float)(lightY-Balls[i].y)/(float)(lightX-Balls[i].x);
        for (i2=0; i2<90; i2++)
          if (TAN[i2]>tmp2) break;
        tmp=(90+i2)*ONE;
      }

      if ((lightX<Balls[i].x)&&(lightY>=Balls[i].y))
      {
        tmp2=(float)(lightY-Balls[i].y)/(float)(Balls[i].x-lightX);
        for (i2=0; i2<90; i2++)
          if (TAN[i2]>tmp2) break;
        tmp=(270-i2)*ONE;
      }

      if ((lightX<Balls[i].x)&&(lightY<Balls[i].y))
      {
        tmp2=(float)(Balls[i].y-lightY)/(float)(Balls[i].x-lightX);
        for (i2=0; i2<90; i2++)
          if (TAN[i2]>tmp2) break;
        tmp=(270+i2)*ONE;
      }
gotit:
      Balls[i].rotate=tmp;
      GsSortSprite(&Balls[i], &WorldOrderingTable[buff], 3);
    }
  }

  if (Stars)
  {
    for (i=0; i<100; i++)
    { 
      x=Star[i].x0;
      y=Star[i].y0;
      z=StarZ[i];

      dorotate(&Star[i].x0, &Star[i].y0, &(StarZ[i]));

      Star[i].r=256-StarZ[i];
      Star[i].b=Star[i].r;
      Star[i].g=Star[i].r;

      Star[i].x1=Star[i].x0+1;
      Star[i].y1=Star[i].y0+1;

      if (StarZ[i]>high) high=StarZ[i];
      if (StarZ[i]<low) low=StarZ[i];

      GsSortLine(&Star[i], &WorldOrderingTable[buff], 2);
      Star[i].y1=Star[i].y0;
      GsSortLine(&Star[i], &WorldOrderingTable[buff], 2);

      Star[i].x0=x;
      Star[i].y0=y;
      StarZ[i]=z;
    }

    if (FRAME%2 == 0)
    {
      yaw++;              // z axis
      if (yaw>119) yaw=0;
    }

    roll++;               // y axis
    if (roll>119) roll=0;

    if (FRAME%3 == 0)
    {
      pitch++;          // x axis
      if (pitch>119) pitch=0;
    }

  }

  for (i=0; i<32; i++)
  {
    if (Misc[i].x != -1)
    {
      GsSortSprite(&Misc[i], &WorldOrderingTable[buff], 1);
    }
  }

  for (i=0; i<15; i++)
  {
    Text[i].y=(COSINE[TextSine]*90.0)+120;
    GsSortFastSpriteB(&Text[i], &WorldOrderingTable[buff],0,0);
  }
  TextSine++;
  if (TextSine>119) TextSine-=120;

  GsDrawOt(&WorldOrderingTable[buff]);

  buff=GsGetActiveBuff();

  GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
  GsClearOt(0,0,&WorldOrderingTable[buff]);

}

void palOrNtsc(void) {
	if (*(char *)0xbfc7ff52=='E')
                SetVideoMode(1);                // PAL MODE
	else
		SetVideoMode(0);		// NTSC MODE
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

  printf("Data at %d, %d, mode %d (%d x %d)\n",rect1.x,rect1.y,TexInfo.pmode, rect1.w, rect1.h);

  LoadImage(&rect1,TexInfo.pixel);
  /* dma xfer from memory to videoram */

  /* do color table */
  if (TexInfo.pmode<2)
  { rect1.x=TexInfo.cx;
    rect1.y=TexInfo.cy;
    rect1.w=TexInfo.cw;
    rect1.h=TexInfo.ch;
    printf("Color at %d, %d (%d x %d)\n",rect1.x,rect1.y, rect1.w, rect1.h);
    LoadImage(&rect1,TexInfo.clut);
  }

  DrawSync(0);
  /* wait for finish */

}

void Cycle()
{
// color cycling routine
// shifts the palette at 0, 480
// every vertical blank. There are 33 colors, and the first is ignored

RECT myRect;
unsigned short buf[32], t;
int i;

myRect.x=1;
myRect.y=480;
myRect.w=32;
myRect.h=1;
StoreImage(&myRect, (unsigned long*)&buf);
DrawSync(0);

t=buf[0];

for (i=1; i<32; i++)
{  
   buf[i-1]=buf[i];
}

if (c_flag == 0)
{
  buf[31]=t;
}
else
{
  buf[31]=blue;
  if (FRAME%2)
  {
    blue+=0x400;
    if ((blue>>10)>17)
      c_flag=0;
  }
}

LoadImage(&myRect, (unsigned long*)&buf);
}

void initsprites()
{ int j, tPage;

  /* Sprites fade FX */

  for (j=0; j<80; j++)
  {  tPage = GetTPage(1,1,320,0);
                     // no RGB | no Rot/Scale | 8 bit CLUT
     Sprites[j].attribute = (1<<27)|(1<<24)|(1<<30);
     Sprites[j].w = 32;
     Sprites[j].h = 32;
                
     Sprites[j].x = (j%10)*32;
     Sprites[j].y = -32;
                
     Sprites[j].mx = 0;         /* center f. rotation/scaling */
     Sprites[j].my = 0;
                
     Sprites[j].tpage = tPage;
                
     Sprites[j].u = 224;         /* offset of the sprite INSIDE */
     Sprites[j].v = 224;         /* the 255X255 pixel TIM picture */
               
     Sprites[j].cx = 0;
     Sprites[j].cy = 480;
                
     Sprites[j].r = 0x80;
     Sprites[j].g = 0x80;
     Sprites[j].b = 0x80;
                
     Sprites[j].rotate = ONE*0;
     Sprites[j].scalex = ONE;
     Sprites[j].scaley = ONE;
  }

  for (j=0; j<32; j++)
  {
    Misc[j].x=-1;
  }
}

void setsprite(int j, int x, int y, int w, int h, int tx, int ty, int cx, int cy, int tm)
{
  int tPage;

  tPage = GetTPage(tm,1,tx,ty);

  Misc[j].attribute = (tm<<24);
  Misc[j].w = w;
  Misc[j].h = h;
               
  Misc[j].x = x;
  Misc[j].y = y;
                
  Misc[j].mx = w/2;         /* center f. rotation/scaling */
  Misc[j].my = h/2;
                
  Misc[j].tpage = tPage;
                
  Misc[j].u = 0;         /* offset of the sprite INSIDE */
  Misc[j].v = 0;         /* the 255X255 pixel TIM picture */
              
  Misc[j].cx = cx;
  Misc[j].cy = cy;
                
  Misc[j].r = 0x80;
  Misc[j].g = 0x80;
  Misc[j].b = 0x80;
                
  Misc[j].rotate = ONE*0;
  Misc[j].scalex = ONE;
  Misc[j].scaley = ONE;
}

void setup_text()
{
  int j;
  int tPage;

  tPage = GetTPage(0,1,512,256);

  for (j=0; j<15; j++)
  {
    Text[j].attribute = (1<<28)|(1<<30); // 1+1 semi-transparent
    Text[j].w = 25;
    Text[j].h = 25;
               
    Text[j].x = -27+(25*j);
    Text[j].y = 150;
                
    Text[j].mx = 12;         /* center f. rotation/scaling */
    Text[j].my = 12;
                
    Text[j].tpage = tPage;
                
    Text[j].u = 150;       /* offset of the sprite INSIDE */
    Text[j].v = 100;       /* the 255X255 pixel TIM picture */
              
    Text[j].cx = 0;
    Text[j].cy = 484;
                
    Text[j].r = 0x80;
    Text[j].g = 0x80;
    Text[j].b = 0x80;
                
    Text[j].rotate = ONE*0;
    Text[j].scalex = ONE;
    Text[j].scaley = ONE;
  }
}

int pos(char c)
{
  char STRING[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!.,?)(@:/- ";
  int idx;

  c=toupper(c);

  for (idx=0; idx<strlen(STRING); idx++)
  {
    if (c==STRING[idx])
      break;
  }

  if (STRING[idx]==0)
    return -1;
  else
    return idx;
}

void movetext()
{
int j;
int c;
int proc;

proc=0;

for (j=0; j<15; j++)
{
  Text[j].x-=4;
  if (Text[j].x<=-27)
  {
    Text[j].x+=375;

    if (*pText == '*')
      pText=Story;

    while (*pText < ' ')
      pText++;

    if (*pText == '[')
    {
      pText++;

      if (*pText == '0')
      {
        if (MODReady)
        {
          MOD_Start();
          MODReady=0;
        }
        pText++;
      }

      if (*pText == '1')
      {
        initstars();
        StarRange=300;
        pText++;
      }
      if (*pText == '2')
      {
        initballs();
        pText++;
      }
      if (*pText == '3')
      {
        initfire();
        StartFire();
        pText++;
      }
      if (*pText == '4')
      {
        pText++;
        proc=4;
      }
      if (*pText == '5')
      {
        pText++;
        proc=5;
      }
      if (*pText == '6')
      {
        pText++;
        proc=6;
      }
      if (*pText == '7')
      {
        pText++;
        proc=7;
      }
      if (*pText == '8')
      {
        pText++;
        proc=8;
      }
    }
    c=pos(*(pText++));

    Text[j].u=(c%10)*25;
    Text[j].v=(c/10)*25;
  }
}

if (proc)
{
  drawscreen();

  switch (proc)
  {
    case 4: stars_out(); break;
    case 5: balls_out(); break;
    case 6: fire_out(); break;
    case 7: stars_in(); break;
    case 8: balls_in(); break;
  }
}

}

void dodemo()
{
  int c;

  StarRange=300;
  initstars();

  while (1)
  {
    if (lightning)
      dolightning();
    if (Stars)
      dostars();
    if (Fire)
      dofire();

    movetext();
    drawscreen();
  }
}

void dolightning()
{
  Lightning[0].r=bright[0];
  Lightning[0].g=bright[0];
  Lightning[0].b=bright[0];
  Lightning[1].r=bright[1];
  Lightning[1].g=bright[1];
  Lightning[1].b=bright[1];

  bright[0]-=16;
  if (bright[0]<0) bright[0]=0;
  bright[1]-=16;
  if (bright[1]<0) bright[1]=0;

  if (MOD_LastSample[2]==5)
  {
    if (flip==0)
      flip=1;
    else
      flip=0;
    bright[flip]=128;
    MOD_LastSample[2]=0;
  }

}

void initstars()
{
  int i;

  for (i=0; i<100; i++)
  {
    Star[i].attribute=0;
    Star[i].x0=rand()%100;
    Star[i].y0=rand()%100;
    StarZ[i]=rand()%100;
    Star[i].r=0x80;
    Star[i].g=0x80;
    Star[i].b=0x80;
  }

  dostars();

  Stars=1;
  Fire=0;
  ballson=0;
  for (i=0; i<32; i++)
    Misc[i].x=-1;       // turn off fire

}

void dostars()
{
  int i;

  for (i=0; i<100; i++)
  {
    StarZ[i]-=5;
    if (StarZ[i]<1) StarZ[i]+=100;
  }
}

void dorotate(short *x, short *y, short *z)
{
  float xa, ya, za, cr1, sr1, cr2, sr2, cr3, sr3;
  float t1,t2, t3;

  // co-ordinate adjustment
  *x-=50;
  *y-=50;

  // rotation around yaw, pitch and roll
  cr1=COSINE[yaw];
  sr1=SINE[yaw];
  cr2=COSINE[roll];
  sr2=SINE[roll];
  cr3=COSINE[pitch];
  sr3=SINE[pitch];

  xa=cr1*(*x)-sr1*(*z);
  za=sr1*(*x)+cr1*(*z);
  t1/* (*x)*/=cr2*xa+sr2*(*y);
  ya=cr2*(*y)-sr2*xa;
  t2/*(*z)*/=cr3*za-sr3*ya;
  t3/*(*y)*/=(sr3*za)+(cr3*ya);

  *x=(short)t1;
  *z=(short)t2;
  *y=(short)t3;

  // projection
  *z+=128;
  if (*z == 0) *z=1;

  *x=(*x*StarRange)/(*z);
  *y=(*y*StarRange)/(*z);

  // screen co-ordinate adjust
  *x+=160;
  *y+=120;
}

void initfire()
{
  // initialize the fire
  int c;
  short p;
  GsBOXF mybox;
  RECT myrect;

  /* fill in hotspots */
  for (c=0; c<FIRE_HOTSPOTS; c++)
    hotspot[c]=rand()%256;

  /* fill our palette with a gradually altering sequence of colors */
  for (c=0; c<64; c++) {
    palette[c].r = c;
    palette[c].g = 0;
    palette[c].b = 0;
  }
  for (c=64; c<128; c++) {
    palette[c].r = 63;
    palette[c].g = c-64;
    palette[c].b = 0;
  }
  for (c=128; c<192; c++) {
    palette[c].r = 63;
    palette[c].g = 63;
    palette[c].b = c-192;
  }
  for (c=192; c<256; c++) {
    palette[c].r = 63;
    palette[c].g = 63;
    palette[c].b = 63;
  }

  for (c=0; c<256; c++)
  {
    palette[c].r/=2;
    palette[c].g/=2;
    palette[c].b/=2;
  }

  memset(&firebuf, 0, 256*2);

  myrect.x=768;
  myrect.y=256;
  myrect.w=256;
  myrect.h=240;
  ClearImage(&myrect, 0, 0, 0);

  // fill with transparency
  for (c=0; c<256; c++)
  {
    firebuf[c]=0x8000;
  }

  myrect.x=768;
  myrect.y=495;
  myrect.w=256;
  myrect.h=1;

  for (myrect.y=256; myrect.y<512; myrect.y++)
    LoadImage(&myrect, (unsigned long*)&firebuf[0]);

  Fire=1;
  Stars=0;
  ballson=0;
  firebottom=-1;

}

void dofire()
{
  char temp[256];
  int c,c2;
  RECT myrect;

  memset(&temp[0],0,256);
  for (c=0; c<FIRE_HOTSPOTS; c++)
  {
    for (c2=hotspot[c]-20; c2<hotspot[c]+20; c2++)
      if ((c2>=0) && (c2<256))
        temp[c2]=MIN(temp[c2]+20-ABS(hotspot[c]-c2),192);

     hotspot[c]+=(rand() & 7) - 3;
     if (hotspot[c] < 0)
       hotspot[c]+=256;
     else
       if (hotspot[c]>255)
         hotspot[c]-=256;
   }

   for (c=0; c<256; c++)
   {
     firebuf[c]=0x8000 | (palette[temp[c]].b << 10) | (palette[temp[c]].g << 5)
                        | (palette[temp[c]].r);
   }

   myrect.x=768;
   myrect.y=495;
   myrect.w=256;
   myrect.h=1;

   LoadImage(&myrect, (unsigned long*)&firebuf[0]);

   myrect.y=257;
   myrect.h=240;
   MoveImage(&myrect, 768, 256);

   if ((firebottom<32)&&(FRAME%5==0))
   {
     firebottom++;
     for (c=0; c<=firebottom; c++)
     {
       Misc[c].r=0x80-((c+(32-firebottom))*4);
       Misc[c].g=MAX(0x80-((c+(32-firebottom))*6),0);
       Misc[c].b=Misc[c].g;
     }
   }  
}

void StartFire()
{
  int c;

  Fire=1;
  Stars=0;
  ballson=0;

  for (c=0; c<32; c++)        // 32 sprites 4 pixels tall for fading
  {
    setsprite(c, 128+(c%3-1), 236-(c*8), 256, 4, 768, 256, 0, 0, 2);
    Misc[c].attribute=(2<<24)|(1<<28)|(1<<30);
    Misc[c].scalex=6200;
    Misc[c].scaley=8192;
    Misc[c].v=236-(c*4);
    Misc[c].r=0;              // fire starts invisible
    Misc[c].g=Misc[c].r;
    Misc[c].b=Misc[c].r;
    Misc[c].my=0;
  }
}

void initballs()
{
  int j;
  int tPage;
 
  ballson=1;
  Fire=0;
  Stars=0;
  for (j=0; j<32; j++)
    Misc[j].x=-1;       // turn off fire

  Stars=0;

  lightX=162;
  lightY=50;
  lightDIRX=-4;
  lightDIRY=4;
  ballXS=160;
  ballYS=120;
  ballFRAME=0;

  for (j=0; j<35; j++)
  {
    tPage = GetTPage(0,1,320, 256);

    Balls[j].attribute = 0;
    Balls[j].w = 32;
    Balls[j].h = 32;
               
    Balls[j].x = j*6;
    Balls[j].y = j*6;
                
    Balls[j].mx = 16;         /* center f. rotation/scaling */
    Balls[j].my = 16;
                
    Balls[j].tpage = tPage;
                
    Balls[j].u = 0;         /* offset of the sprite INSIDE */
    Balls[j].v = 0;         /* the 255X255 pixel TIM picture */
              
    Balls[j].cx = 0;
    Balls[j].cy = 482;
                
    Balls[j].r = 0x80;
    Balls[j].g = 0x80;
    Balls[j].b = 0x80;
                
    Balls[j].rotate = ONE*0;
    Balls[j].scalex = ONE;
    Balls[j].scaley = ONE;
  }

  Balls[0].u=32;
  Balls[1].u=64;
  Balls[2].u=64;
  Balls[0].attribute=(1<<28)|(1<<30);
  Balls[1].attribute=(1<<30);
  Balls[2].attribute=(1<<30);
  Balls[2].r=0x40;
  Balls[2].g=0x40;
  Balls[2].b=0x40;

}

void stars_in()
{
   initstars();

   for (StarRange=4000; StarRange>300; StarRange-=100)
   {
     dostars();
     movetext();
     if (lightning)
       dolightning();
     drawscreen();
   }

}

void stars_out()
{
   for (StarRange=300; StarRange<4000; StarRange+=100)
   {
     dostars();
     movetext();
     if (lightning)
       dolightning();
     drawscreen();
   }
   Stars=0;
}

void balls_in()
{
  int i, j;

  initballs();
  for (i=0; i<0x100; i++)
  {
    for (j=0; j<35; j++)
    {
      if ((j>2)||(i<0x81))
      {
        if ((j!=2)||(i<0x41))
        {
          Balls[j].r=i;
          Balls[j].g=i;
          Balls[j].b=i;
        }
        if (j>2)
          Balls[j].attribute=(1<<30)|(3<<28);
      }
    }
    movetext();
     if (lightning)
       dolightning();
    drawscreen();
  }
  for (j=3; j<35; j++)
  {
      Balls[j].attribute=0;
      Balls[j].r=0x80;
      Balls[j].g=0x80;
      Balls[j].b=0x80;
  }
}

void balls_out()
{
  int i,j;

  for (i=0x100; i>=1; i--)
  {
    for (j=0; j<35; j++)
    {
      if ((j>2)||(i<0x80))
      {
        if ((j!=2)||(i<0x41))
        {
          Balls[j].r=i;
          Balls[j].g=i;
          Balls[j].b=i;
        }
        if (j>2)
          Balls[j].attribute=(1<<30)|(3<<28);
      }
    }
    movetext();
     if (lightning)
       dolightning();
    drawscreen();
  }

  ballson=0;

  for (j=3; j<35; j++)
    Balls[j].attribute=0;
}

void fire_out()
{
  int j;

  while (firebottom>0)
  {
    if (FRAME%5==0)
      firebottom-=2;

    dofire();
    movetext();
     if (lightning)
       dolightning();
    for (j=firebottom; j<32; j++)
    {
      Misc[j].r=0;
      Misc[j].g=0;
      Misc[j].b=0;
    }
    drawscreen();
  }
  Fire=0;
}

