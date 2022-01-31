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
#include <kernel.h>

#include "hitmod.h"
#include "sand.h"

#include "dssdemo.inc"
#include "font.inc"

char MSGADD[] = { 
#include "story.txt"
};

int COS[] = {
#include "cosin.h"
};

#define NUMPICS 21
#define MAXSFX 5

unsigned char pic1[5000]={
#include "sand.txt"
0,0 };

unsigned char pic2[5000]={
#include "beyes.txt"
0,0 };

unsigned char pic3[5000]={
#include "ccsymbol.txt"
0,0 };

unsigned char pic4[5000]={
#include "dolline2.txt"
0,0 };

unsigned char pic5[5000]={
#include "dolphin1.txt"
0,0 };

unsigned char pic6[5000]={
#include "dump3.txt"
0,0 };

unsigned char pic7[5000]={
#include "foxxfi.txt"
0,0 };

unsigned char pic8[5000]={
#include "gradius2.txt"
0,0 };

unsigned char pic9[5000]={
#include "moving.txt"
0,0 };

unsigned char pic10[5000]={
#include "psx.txt"
0,0 };

unsigned char pic11[5000]={
#include "psyg.txt"
0,0 };

unsigned char pic12[5000]={
#include "s095234.txt"
0,0 };

unsigned char pic13[5000]={
#include "saturn.txt"
0,0 };

unsigned char pic14[5000]={
#include "solare.txt"
0,0 };

unsigned char pic15[5000]={
#include "spotted.txt"
0,0 };

unsigned char pic16[5000]={
#include "em.txt"
0,0 };

unsigned char pic17[5000]={
#include "demoship.txt"
0,0 };

unsigned char pic18[5000]={
#include "bombshel.txt"
0,0 };

unsigned char pic19[5000]={
#include "sn1987.txt"
0,0 };

unsigned char pic20[5000]={
#include "gaunt4.txt"
0,0 };

unsigned char pic21[5000]={
#include "flipette.txt"
0,0 };

unsigned char *ppic;

int FRAME, buff, pic;
int sand_count=0;
int i,i2,j;

void setup()
{ /* setup system */
int i,x,y;
u_long colorMode;
unsigned short tp;
RECT myrect;

PadStatus = 0;
palOrNtsc();
ResetGraph(0);
GsInitGraph(320 ,240, GsOFSGPU|GsNONINTER, 1, 0);                
GsDefDispBuff(0, 0, 320, 0);
WorldOrderingTable[0].length = OT_LENGTH;
WorldOrderingTable[1].length = OT_LENGTH;
WorldOrderingTable[0].org = zSortTable[0];
WorldOrderingTable[1].org = zSortTable[1];

buff=GsGetActiveBuff();
GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
GsClearOt(0, 0, &WorldOrderingTable[buff]);

FRAME=-5;
timer=0;
pic=0;
CHANGETIME=500;
cur_par=0;
COSCOUNT=0;
effect=0;

msg=MSGADD;

initTexture((u_long*)FONT);

initsprites();

MOD_Init();
MOD_Load((unsigned char*)MOD);
printf("\n...Now Playing DSSDEMO.MOD - Digital Sound Studio Demo from Amiga\n");
MOD_Start();

}

void drawscreen()
{ int i, s, e;
  GsBOXF mybox;

  mybox.attribute=(2<<28)|(1<<30);
  mybox.x=0;
  mybox.y=0;
  mybox.w=320;
  mybox.h=240;
  mybox.r=mybox.g=mybox.b= 0x20;

  FRAME++;
  timer++;

  DrawSync(0);
  
  VSync(0);

  GsSwapDispBuff();

  if (FRAME%2==0)
  { s=0; e=sand_count/2;}
  else
  { s=sand_count/2; e=sand_count; }

  for (i=s; i<e; i++)
  { Sand[i].x0+=move((dest[i].x+32)-Sand[i].x0);
    Sand[i].x1=Sand[i].x0+1;
    Sand[i].y0+=move(dest[i].y-Sand[i].y0);
    Sand[i].y1=Sand[i].y0;
    Sand[i].r+=move(dest[i].r-Sand[i].r);
    Sand[i].g+=move(dest[i].g-Sand[i].g);
    Sand[i].b+=move(dest[i].b-Sand[i].b);
  }

  textmove();

  for (i=0; i<14; i++)
    GsSortFastSpriteB(&Sprites[i], &WorldOrderingTable[buff],1,0);

  for (i=0; i<sand_count; i++)
  {
    GsSortLine(&Sand[i],&WorldOrderingTable[buff],2);
  }

  if (FRAME<0)
    GsSortClear(0,0,0,&WorldOrderingTable[buff]);
  else
    GsSortBoxFill(&mybox, &WorldOrderingTable[buff], 0);

  GsDrawOt(&WorldOrderingTable[buff]);

  buff=GsGetActiveBuff();

  GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
  GsClearOt(0,0,&WorldOrderingTable[buff]);

}

void main()
{ /* main routine.. start here */

int cnt,cnt2,s,e;

printf("\n\nTursi the lion presents\n\n");
printf("SAND! It gets everywhere, even into your PSX\n\n");
printf("Actually it's kinda a disjointed slide show.\n");
printf("Silpheed's MOD player (as you can see), and Loser/Ogre's\n");
printf("PAL/NTSC test routine included, the rest of the code is\n");
printf("mine. None of the graphics or sound are, just the way I\n");
printf("display them. Millions of phosphors died to bring you this\n");
printf("demo. Tursi signing off. http://neteng.bc.ca/~tursi\n\n");

setup();

sand_count=0;
 
drawscreen();

for (cnt=0; cnt<PARTICLES; cnt++)
{ Sand[cnt].attribute=0;
  Sand[cnt].x0=cnt%320;
  Sand[cnt].y0=0;
  Sand[cnt].x1=0;
  Sand[cnt].y1=0;
  Sand[cnt].r=0;
  Sand[cnt].g=0;
  Sand[cnt].b=0;
  dest[cnt].x=0;
  dest[cnt].y=0;
  dest[cnt].r=0;
  dest[cnt].g=0;
  dest[cnt].b=0;
}

sand_count=PARTICLES;

cnt=cnt2=0;

while (cnt<PARTICLES)
{
  Sand[cnt].attribute=0;
  Sand[cnt].x0=(cnt/3)+(rand()%20);
  Sand[cnt].y0=FRAME%7;
  Sand[cnt].x1=Sand[cnt].x0+1;
  Sand[cnt].y1=Sand[cnt].y0;
  Sand[cnt].r=rand()%256;
  Sand[cnt].g=rand()%256;
  Sand[cnt].b=rand()%256;

  if (pic1[cnt2*5]==0)     // end of pic
  { break;
  }

  dest[cnt].x=pic1[cnt2*5];
  dest[cnt].y=pic1[cnt2*5+1];
  dest[cnt].r=pic1[cnt2*5+2];
  dest[cnt].g=pic1[cnt2*5+3];
  dest[cnt].b=pic1[cnt2*5+4];

  cnt++;
  cnt2++;

  drawscreen();
}

for (i=0; i<60; i++)
  drawscreen();

timer=0;

while (1)
{
  switch (effect)
  { case 0:  sandstorm(); break;
    case 1:  dna(); break;
    case 2:  cloud(); break;
    case 3:  circle(); break;
    case 4:  all(); break;

  }

  if (timer>CHANGETIME)
  {
    effect++;
    if (effect>=MAXSFX) effect=0;

    pic++;
    if (pic==NUMPICS)
        pic=0;

    switch (pic)
    {
      case 0: ppic=pic1; printf("Title pic...\n"); break;
      case 1: ppic=pic2; printf("Beast Eyes - an old old GIF\n"); break;
      case 2: ppic=pic3; printf("Captain Communism logo - by Vidi\n"); break;
      case 3: ppic=pic4; printf("Two dolphins, line art\n"); break;
      case 4: ppic=pic5; printf("A dolphin (orca?) drawing\n"); break;
      case 5: ppic=pic6; printf("Screenshot from my old Super Space Acer for the TI\n"); break;
      case 6: ppic=pic7; printf("Foxx and Tursi :)\n"); break;
      case 7: ppic=pic8; printf("Screenshot from Gradius 2 - NES\n"); break;
      case 8: ppic=pic9; printf("Moving Target logo by Vombatus\n"); break;
      case 9: ppic=pic10; printf("Playstation logo\n"); break;
      case 10:ppic=pic11; printf("Psygnosis logo\n"); break;
      case 11:ppic=pic12; printf("Stingray silhouette from a Microsoft collection\n"); break;
      case 12:ppic=pic13; printf("Saturn\n"); break;
      case 13:ppic=pic14; printf("A solar eclipse\n"); break;
      case 14:ppic=pic15; printf("A spotted dolphin\n"); break;
      case 15:ppic=pic16; printf("The Earth and moon\n"); break;
      case 16:ppic=pic17; printf("Demo ship done in Lightwave by ArchAngle\n"); break;
      case 17:ppic=pic18; printf("Marilyn Monroe - blonde bomshell\n"); break;
      case 18:ppic=pic19; printf("SN-1987 - supernove remnants\n"); break;
      case 19:ppic=pic20; printf("BMP from the Thunder Force 4 CD (PSX)\n"); break;
      case 20:ppic=pic21; printf("Flipette - dolphin enchantress by me ;)\n"); break;

      default: ppic=pic1; break;
    }

    if (CHANGETIME<11)
      CHANGETIME-=1;
    else
      CHANGETIME-=10;

    if (CHANGETIME < 1) CHANGETIME=500;

    cnt2=0;
    for (cnt=0; cnt<PARTICLES; cnt++)
    {
      if (ppic[cnt2*5]==0)     // end of pic
      { cnt2=0;
      }

      dest[cnt].x=ppic[cnt2*5];
      dest[cnt].y=ppic[cnt2*5+1];
      dest[cnt].r=ppic[cnt2*5+2];
      dest[cnt].g=ppic[cnt2*5+3];
      dest[cnt].b=ppic[cnt2*5+4];
      cnt2++;
    }

    for (i=0; i<180; i++)
      drawscreen();
    timer=0;
  }    
}

}

inline int move(int x)
{
if ((x<7)&&(x>-7))
  return x;

if (x>0) return 7;
if (x<0) return -7;
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
{ int j, tPage;

  /* Sprites for babbling in */

  for (j=0; j<14; j++)
  {  tPage = GetTPage(2,1,640,0);

     Sprites[j].attribute = (0<<28)|(1<<30)|(2<<24);
     Sprites[j].w = 28;
     Sprites[j].h = 29;
                
     Sprites[j].x = (j*28)-36;
     Sprites[j].y = 210;
                
     Sprites[j].mx = 0;         /* center f. rotation/scaling */
     Sprites[j].my = 0;
                
     Sprites[j].tpage = tPage;
                
     Sprites[j].u = 196;        /* offset of the sprite INSIDE */
     Sprites[j].v = 87;         /* the 255X255 pixel TIM picture */
               
     Sprites[j].cx = 0;
     Sprites[j].cy = 0;
                
     Sprites[j].r = 0x80;
     Sprites[j].g = 0x80;
     Sprites[j].b = 0x80;
                
     Sprites[j].rotate = ONE*0;
     Sprites[j].scalex = ONE;
     Sprites[j].scaley = ONE;
  }
}

void textmove()
{ char ch;
  int i;

  /* text update */
  for (i=0; i<14; i++)
  { Sprites[i].x-=2;
    Sprites[i].y=COS[Sprites[i].x+36+COSCOUNT]+50;

    COSCOUNT++;
    if (COSCOUNT>640) COSCOUNT=0;

    if (Sprites[i].x<=-36)
    { ch=toupper(*(msg++));
      if (*msg=='*') msg=(char*)MSGADD;

      if ((ch>='A')&&(ch<='Z'))
      { ch-='A';
        goto gotchar;
      }

      switch (ch)
      {
        case '!' : ch=26; break;
        case '-' : ch=27; break;
        case '?' : ch=28; break;
        case '.' : ch=29; break;
        case ',' : ch=32; break;
        case '/' : ch=33; break;
        case '~' : ch=34; break;

        default: ch=30; break;
      }

gotchar:
      Sprites[i].x+=392;
      Sprites[i].u=(ch%8)*28;
      Sprites[i].v=(ch/8)*29;  
    }
  }
}


void sandstorm()
{
  if (cur_par >= PARTICLES)
    cur_par=0;

  for (i2=cur_par; i2<cur_par+35; i2++)
  { if (i2<PARTICLES)
    { dest[i2].r=rand()%255;
      dest[i2].g=rand()%255;
      dest[i2].b=rand()%255;
      dest[i2].x=rand()%255;
      dest[i2].y=rand()%240;
    }
  }
  cur_par+=35;

  for (i2=0; i2<5; i2++)
  {
    drawscreen();
  }
}

void dna()
{
  if (cur_par >= PARTICLES)
    cur_par=0;

  for (i2=cur_par; i2<cur_par+30; i2++)
  { if (i2<PARTICLES)
    { dest[i2].r=dest[i2].x;
      dest[i2].g=dest[i2].y;
      dest[i2].b=FRAME%250;
      dest[i2].x=COS[(i2-cur_par)+(FRAME%640) + ((FRAME+i2-cur_par)%2)*160];
      dest[i2].y=(i2-cur_par)+(FRAME%240);
    }
  }
  cur_par+=30;

  for (i2=0; i2<3; i2++)
  {
    drawscreen();
  }
}

void cloud()
{ int x,y;

  x=rand()%256+32;
  y=rand()%240;

  if (cur_par >= PARTICLES)
    cur_par=0;

  for (i2=cur_par; i2<cur_par+29; i2++)
  { if (i2<PARTICLES)
    { dest[i2].r=timer%250;
      dest[i2].g=COS[timer%640];
      dest[i2].b=200;
      dest[i2].x=x+(rand()%50 -25);
      dest[i2].y=y+(rand()%36 -18);
    }
  }
  cur_par+=29;

    drawscreen();
}

void circle()
{ int x,y;

  x=rand()%256-148;
  y=rand()%240-90;

  if (cur_par >= PARTICLES)
    cur_par=0;

  for (i2=cur_par; i2<cur_par+30; i2++)
  { if (i2<PARTICLES)
    { dest[i2].r=x;
      dest[i2].g=y;
      dest[i2].b=x+y/2;
      dest[i2].x=COS[(i2-cur_par)*16]+x;
      dest[i2].y=COS[(i2-cur_par)*16+120]+y;
    }
  }
  cur_par+=30;

  for (i2=0; i2<3; i2++)
  {
    drawscreen();
  }
}

void all()
{
  sandstorm();
  dna();
  cloud();
  circle();
}

void palOrNtsc(void) {
	if (*(char *)0xbfc7ff52=='E')
		SetVideoMode(1);		//PAL MODE
	else
		SetVideoMode(0);		// NTSC MODE
}


