/* I used to use PCC, these routines provide Playstation     */
/* compatible equivalents for what I used of PCIO for Tetris */

#include "pad.h"

#define PACKETMAX               (10000)
#define PACKETMAX2      (PACKETMAX*24)
#define OT_LENGTH               (3)      
GsOT            WorldOrderingTable[2];
GsOT_TAG        zSortTable[2][1<<OT_LENGTH];   
PACKET  GpuOutputPacket[2][PACKETMAX2];

#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     240      
#define SCREEN_X_OFFSET     0
#define SCREEN_Y_OFFSET     0

#define blue(x)   (x<<10)
#define green(x)  (x)
#define red(x)    (x<<5)
#define TRANS     (0x8000)

#define FONT1 0x80071280
#define FONT2 0x801F1A80
#define SNDVB 0x8007e970
#define SNDVH 0x800e0940
#define MSGADD 0x800ef560

GsIMAGE TexInfo;

void scr_setup(void);
void scr_rowcol(int,int);
void scr_aputs(char*,int);
void scr_puts(char*);
void scr_co(int);
int scr_csts(void);
int scr_sinp(void);
void initSprite(void);
void PlaySeq(int);

int scr_attr;
int scr_text[25][40];
int scr_row,scr_col;
u_long PadStatus;
int outputBufferIndex;
#define SPRITEMAX 30             /* This should be more than enough */     
GsSPRITE Sprites[SPRITEMAX];           
int PLAYFIELD,frame;
int PLASMABUF[60][60],ballx,bally,ballxd,ballyd;
unsigned short PALETTE[255];
unsigned short *PICBUF=(unsigned short*)0x8007CD50;
int current_tune;
short VAB_ID,SEQ_ID;
int TEXTNUM;
int OLDVOICE,TEXTSTAT;
char *msg;
int cd_tracks[14]={2,3,4,5,6,7,8,9,10,11,12,13,0};

	// low-level pad buffers: never need to touch
volatile u_char *bb0, *bb1;


u_long *BGBUF[8]={ (u_long*)0x80190000, (u_long*)0x8019c350, (u_long*)0x801a86a0,
                    (u_long*)0x801b49f0, (u_long*)0x801c0d40, (u_long*)0x801cd090,
                    (u_long*)0x801d93e0, (u_long*)0x801e5730 };

u_long *SNDBUF[6]={ (u_long*)0x800f161e, (u_long*)0x800f3442,
                    (u_long*)0x800f5124, (u_long*)0x800f7234, (u_long*)0x800fa92e,
                    (u_long*)0x800fcd62 };

void scr_setup()
{ /* Initialize SCR Routines */
int r,c,i,buff;
RECT myrect;
u_long *addr;

PadStatus = 0;
SetVideoMode( MODE_NTSC ); 
ResetGraph(0);
GsInitGraph(320 ,240, GsOFSGPU|GsNONINTER, 1, 0);                
GsDefDispBuff(0, 0, 0, 240);
PadInit();
WorldOrderingTable[0].length = OT_LENGTH;
WorldOrderingTable[1].length = OT_LENGTH;
WorldOrderingTable[0].org = zSortTable[0];
WorldOrderingTable[1].org = zSortTable[1];

buff=GsGetActiveBuff();
GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);

scr_attr=127;
scr_row=0;
scr_col=0;

for (r=0; r<25; r++)
  for (c=0; c<40; c++)
    scr_text[r][c]=32;

myrect.x=0;
myrect.y=0;
myrect.w=320;
myrect.h=480;
ClearImage(&myrect,0,0,0);

myrect.x=320;
ClearImage(&myrect,0,0,0);

myrect.y=240;
myrect.h=240;
ClearImage(&myrect,255,255,255);

myrect.x=640;
myrect.y=100;
myrect.w=61;
myrect.h=61;
ClearImage(&myrect,0,0,0);

/* load font */
addr=(u_long*)FONT1;
addr++;
GsGetTimInfo(addr, &TexInfo);
myrect.x=640;
myrect.y=0;
myrect.w=257;
myrect.h=93;
LoadImage(&myrect,TexInfo.pixel);

addr=(u_long*)FONT2;
addr++;
GsGetTimInfo(addr, &TexInfo);
myrect.x=768;
myrect.y=256;
myrect.w=224;
myrect.h=116;
LoadImage(&myrect,TexInfo.pixel);  

/* load tetris background picture */
addr=BGBUF[0];
addr++;
GsGetTimInfo(addr, &TexInfo);
myrect.x=440;
myrect.y=249;
myrect.w=80;
myrect.h=207;
LoadImage(&myrect,TexInfo.pixel);  

initSprite();

star_field(50, STAR_IN, 0, 0, 240, 320);

for (r=0; r<60; r++)
  for (c=0; c<60; c++)
  { PLASMABUF[r][c]=0;
    *(PICBUF+(r<<6)+c)=0;
  }

/* fill our pallete with a gradually altering sequence of colors */
/* black to blue to red to white */
for (c=0; c<64; c++)
{
    PALETTE[c] = blue(c>>1)|TRANS;
}
for (c=64; c<192; c++)
{
    PALETTE[c] = red((c-64)>>2)|blue((191-c)>>2)|TRANS;
}
for (c=192; c<256; c++)
{
    PALETTE[c] = red(63>>1)|green((c-192)>>1)|blue((c-192)>>1);
}

/* draw palette */
/*
for (i=0; i<256; i++)
{ r=i/20*3;
  c=i%20*3;
  PLASMABUF[r][c]=i;
  PLASMABUF[r+1][c]=i;
  PLASMABUF[r+2][c]=i;
  PLASMABUF[r][c+1]=i;
  PLASMABUF[r+1][c+1]=i;
  PLASMABUF[r+2][c+1]=i;
  PLASMABUF[r][c+2]=i;
  PLASMABUF[r+1][c+2]=i;
  PLASMABUF[r+2][c+2]=i;
}
*/

frame=0;
ballx=5;
bally=45;
ballxd=1;
ballyd=1;

msg=(char*)MSGADD;

VSync(0);
DrawSync(0);
GsSwapDispBuff();

/* now the sound system */
SsSetMVol(127,127);
SsUtSetReverbType(SS_REV_TYPE_ROOM);
SsUtReverbOn();
VAB_ID=SsVabTransfer((unsigned char*)SNDVH,(unsigned char*)SNDVB,-1,1);
if (VAB_ID<0) printf("\nSound Transfer failed!\n");
SsUtSetReverbDepth(64,64);
OLDVOICE=-1;

SsSetSerialAttr(SS_CD,SS_MIX,SS_SON);
SsSetSerialVol(SS_CD,127,127);

SEQ_ID=-2;
PlaySeq(0);

current_tune=0;
}

void scr_rowcol(int r, int c)
{ scr_row=r;
  scr_col=c;
}

void scr_aputs(char *s, int attr)
{ /* print a string using scr_attr, at scr_row, scr_col */
int t;

/* call scr_co() for each character */

t=scr_attr;  /* save old attr */
scr_attr=attr;

while (*s)
{ scr_co(*(s++));
  scr_col++;
}

scr_attr=t;  /* restore old attr */

}

void scr_puts(char *s)
{ /* call aputs with scr_attr */
  scr_aputs(s,scr_attr);
}

void scr_co(int ch)
{ /* output a single character at scr_row, scr_col, using scr_attr */

RECT myrect,mysrc;

myrect.x=scr_col*8+320;
myrect.y=scr_row*9;
myrect.w=8;
myrect.h=9;
mysrc.x=myrect.x+1;
mysrc.y=myrect.y+241;
mysrc.w=7;
mysrc.h=8;

if ((scr_col>=14)&&(scr_col<=25)&&(scr_row>=1))
{ /* second buffer */
  myrect.x+=208;
  myrect.y+=247;
}

switch (ch)
{ case 1  :
  case 2  :
  case 3  :
  case 4  :
  case 5  :
  case 6  :
  case 7  : MoveImage(&mysrc,myrect.x,myrect.y); break;

  case 253: mysrc.x--;
            mysrc.y--;
            mysrc.w++;
            mysrc.h++;
            MoveImage(&mysrc,myrect.x,myrect.y);
            break;

  case 32 : ClearImage(&myrect,0,0,0); break;
  case 254: ClearImage(&myrect,scr_attr,scr_attr,scr_attr); break;
  case 255: ClearImage(&myrect,0xff,0xff,0xff); break;
  default : /* this should be a text character, then */
            mysrc.w=8;
            mysrc.h=9;
            mysrc.x=((ch-' ')%16)*16+640;
            mysrc.y=((ch-' ')/16)*16;
            MoveImage(&mysrc,myrect.x,myrect.y);
            break;
}


scr_text[scr_row][scr_col]=ch;
}

int scr_csts()
{ /* this is the "keyboard" input */
  /* map only A=left, S=Right, Space=Down, Enter=Spin, ESC=Quit */
  int r,i;

  r=0;
  PadStatus=PadRead();

  if (PadStatus&PADLleft) r='A';
  if (PadStatus&PADLright) r='S';
  if (PadStatus&PADL1) r='1';
  if (PadStatus&PADL2) r='2';
  if (PadStatus&PADR2) r='3';
  if (PadStatus&(PADRleft|PADRright|PADRup|PADRdown)) r=13;

  return(r);
}


int scr_sinp()
{ /* read the character on the display */
return(scr_text[scr_row][scr_col]);
}

void initSprite()
{
                int i,j;
                u_long colorMode;
                u_short tPage;

                colorMode = 2;  /* 16 bit direct */

                i=1; /* first sprite */

/* background plasma */

                tPage = GetTPage(2,0,640,0);

                Sprites[i].attribute = (1<<6)|(1<<28)|(1<<30)|(colorMode<<24);
                /* brightness off, semi-transparency (1+1) and 16-bit direct mode */
                Sprites[i].w = 60;
                Sprites[i].h = 60;
                
                Sprites[i].x = 160;
                Sprites[i].y = 120;
                
                Sprites[i].mx = 30;       /* center f. rotation/scaling */
                Sprites[i].my = 30;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 0;          /* offset of the sprite INSIDE */
                Sprites[i].v = 100;        /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE*2;
                Sprites[i].scaley = ONE*2;

                i++;

/* foreground text (several sprites) */

                tPage = GetTPage(2,0,320,0);

                Sprites[i].attribute = (colorMode<<24);
                Sprites[i].w = 111;
                Sprites[i].h = 43;
                
                Sprites[i].x = 0;
                Sprites[i].y = 109;
                
                Sprites[i].mx = 0;       /* center f. rotation/scaling */
                Sprites[i].my = 0;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 0;        /* offset of the sprite INSIDE */
                Sprites[i].v = 109;      /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE;
                Sprites[i].scaley = ONE;

                i++;

                tPage = GetTPage(2,0,320,0);

                Sprites[i].attribute = (colorMode<<24);
                Sprites[i].w = 208;
                Sprites[i].h = 8;
                
                Sprites[i].x = 48;
                Sprites[i].y = 1;
                
                Sprites[i].mx = 0;       /* center f. rotation/scaling */
                Sprites[i].my = 0;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 48;     /* offset of the sprite INSIDE */
                Sprites[i].v = 1;      /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE;
                Sprites[i].scaley = ONE;

                i++;

                tPage = GetTPage(2,0,512,0);

                Sprites[i].attribute = (colorMode<<24);
                Sprites[i].w = 111;
                Sprites[i].h = 116;
                
                Sprites[i].x = 208;
                Sprites[i].y = 63;
                
                Sprites[i].mx = 0;       /* center f. rotation/scaling */
                Sprites[i].my = 0;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 16;        /* offset of the sprite INSIDE */
                Sprites[i].v = 63;        /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE;
                Sprites[i].scaley = ONE;

                i++;

                tPage = GetTPage(2,0,320,0);

                Sprites[i].attribute = (colorMode<<24);
                Sprites[i].w = 80;
                Sprites[i].h = 10;
                
                Sprites[i].x = 0;
                Sprites[i].y = 17;
                
                Sprites[i].mx = 0;       /* center f. rotation/scaling */
                Sprites[i].my = 0;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 0;        /* offset of the sprite INSIDE */
                Sprites[i].v = 17;        /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE;
                Sprites[i].scaley = ONE;

                i++;

/* Sprites for babbling in */

             TEXTNUM=i;
             TEXTSTAT=1;
             for (j=0; j<14; j++)
             {  tPage = GetTPage(2,0,768,256);

                Sprites[i].attribute = (1<<6)|(TEXTSTAT<<28)|(1<<30)|(colorMode<<24);
                /* brightness off, semi-transparency (1+1) and 16-bit direct mode */
                Sprites[i].w = 28;
                Sprites[i].h = 29;
                
                Sprites[i].x = (j*28)-36;
                Sprites[i].y = 210;
                
                Sprites[i].mx = 0;       /* center f. rotation/scaling */
                Sprites[i].my = 0;
                
                Sprites[i].tpage = tPage;
                
                Sprites[i].u = 196;        /* offset of the sprite INSIDE */
                Sprites[i].v = 87;      /* the 255X255 pixel TIM picture */
                
                Sprites[i].cx = 0;
                Sprites[i].cy = 0;
                
                Sprites[i].r = 0x80;
                Sprites[i].g = 0x80;
                Sprites[i].b = 0x80;
                
                Sprites[i].rotate = ONE*0;
                Sprites[i].scalex = ONE;
                Sprites[i].scaley = ONE;

                i++;
              }

/* game play buffer */

                tPage = GetTPage(2,0,640,256);

                PLAYFIELD=i;

                Sprites[PLAYFIELD].attribute = (colorMode<<24);
                Sprites[PLAYFIELD].w = 96;
                Sprites[PLAYFIELD].h = 216;
                
                Sprites[PLAYFIELD].x = 160;
                Sprites[PLAYFIELD].y = 120;
                
                Sprites[PLAYFIELD].mx = 48;       /* center f. rotation/scaling */
                Sprites[PLAYFIELD].my = 108;
                
                Sprites[PLAYFIELD].tpage = tPage;
                
                Sprites[PLAYFIELD].u = 0;        /* offset of the sprite INSIDE */
                Sprites[PLAYFIELD].v = 0;        /* the 255X255 pixel TIM picture */
                
                Sprites[PLAYFIELD].cx = 0;
                Sprites[PLAYFIELD].cy = 0;
                
                Sprites[PLAYFIELD].r = 0x80;
                Sprites[PLAYFIELD].g = 0x80;
                Sprites[PLAYFIELD].b = 0x80;
                
                Sprites[PLAYFIELD].rotate = ONE*0;
                Sprites[PLAYFIELD].scalex = ONE;
                Sprites[PLAYFIELD].scaley = ONE;                            

}

void PlaySeq(int x)
{ if (SEQ_ID!=-2) CdPlay(0,(int*)&cd_tracks,0);
  SEQ_ID=1;
  CdPlay(2,(int*)&cd_tracks,current_tune);
}
