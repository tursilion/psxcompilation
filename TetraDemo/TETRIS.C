/* TETRIS by Mike Brent
   V1.0 Dec 1993
   V2.0PSX Aug 1998
   V2.1PSX Dec 1998
   Wow... 5 years later! :)
*/

/* this version doesn't actually load */

#include <libps.h>
#include <sys/file.h>
#include "pad.h"
#include "starslib.h"

void delay(int);
void main(void);
void draw(int,int,int,int,int,char);
void clw(void);
int check(int,int,int,int);
void next(void);
int game(void);
int drop(void);
void checkline(void);
void newlevel(void);
void rndbl(void);
void background(int);
void tick(void);
void boom(void);
void crash(void);

#include "scr_psx.c"

#define getblue(x)   ((x&0x7c00)>>10)
#define getgreen(x)  ((x&0x001f))
#define getred(x)    ((x&0x03e0)>>5)

#define makecol(r,g,b)  ((short int)((b<<10)|(r<<5)|(g)))

int a,b,c,ky,shake;
int score,x,y,ch,sh,rt,nch,nsh,nrt,col,nco;
int lines,speed,level;
int bgxdir,bgydir,bgx,bgy,bgmax,bgxsp,bgysp;
char buf[80];
short int ImgBuf[216][80];
int fade_col;

char x2[10];
char dt[28][4][5]={ "0100", "0100", "0100", "0100", "0000", "0000", "1111",
"0000", "0100", "0100", "0100", "0100", "0000", "0000", "1111", "0000",
"0100", "0100", "0110", "0000", "0000", "0111", "0100", "0000", "0000",
"0110", "0010", "0010", "0000", "0010", "1110", "0000", "0010", "0010",
"0110", "0000", "0000", "0100", "0111", "0000", "0000", "0110", "0100",
"0100", "0000", "1110", "0010", "0000", "0000", "1110", "0100", "0000",
"0000", "0010", "0110", "0010", "0000", "0000", "0100", "1110", "0000",
"1000", "1100", "1000", "0100", "0110", "0010", "0000", "0000", "0011",
"0110", "0000", "0100", "0110", "0010", "0000", "0000", "0011", "0110",
"0000", "0010", "0110", "0100", "0000", "0000", "0110", "0011", "0000",
"0010", "0110", "0100", "0000", "0000", "0110", "0011", "0000", "0000",
"0110", "0110", "0000", "0000", "0110", "0110", "0000", "0000", "0110",
"0110", "0000", "0000", "0110", "0110", "0000" };

void main()
{
scr_setup();
fade_col=255;
scr_rowcol(0,10);
scr_aputs("TETR-a-DEMO by Tursi",14);
scr_attr=127;
for (a=1; a<25; a++)
{ scr_rowcol(a,14);
  scr_co(254);
  scr_rowcol(a,25);
  scr_co(254);
}
for (a=15; a<25; a++)
{ scr_rowcol(24,a);
  scr_co(254);
} 
scr_rowcol(12,1);
scr_aputs("Score:",9);
scr_attr=43;
scr_rowcol(12,8);
sprintf(buf,"%5d",score);
scr_puts(buf);
scr_rowcol(12,29);
scr_aputs("NEXT",112);
scr_rowcol(14,26);
scr_puts("Pad    - Move");  
scr_rowcol(15,26);
scr_puts("Button - Spin");
scr_rowcol(16,26);
scr_puts("START  - Pause");
scr_rowcol(17,26);
scr_puts("L1 - Harder");
scr_rowcol(18,26);
scr_puts("L2 - Hardest");
scr_rowcol(19,26);
scr_puts("(Use to Start)");
clw();
while (ky!=27)
{ scr_rowcol(5,15);
  scr_aputs("Game Over",132);
  delay(1);
  while (scr_csts());
  ky=0;
  while ((ky!=13)&&(ky!='1')&&(ky!='2')&&(ky!='3'))
  {
    delay(1);
    ky=scr_csts();
    rand();
  }
  clw();
  Sprites[PLAYFIELD].rotate=ONE*0;
  if (ky!=27)
    game();
}  
}

void draw(x,y,s,r,a,c) int x,y,s,r,a; char c;
{ /* draw at x,y, shape s, rotation r, character c */
int q,w;

for (q=0; q<4; q++)
 for (w=0; w<4; w++)
  if (dt[s*4+r][q][w]=='1')
  { scr_rowcol(y+q,x+w);
    scr_co(c);
  }
}

void clw()
{ /* clear window */
int q;

for (q=1; q<24; q++)
{ scr_rowcol(q,15);
  scr_aputs("          ",7);
}
}

int check(x,y,s,r) int x,y,s,r;
{ /* check if shape is legal at x,y, shape s, rotation r */
  /* return 1 if illegal */
int q,w,f;

f=0;
for (q=0; q<4; q++)
 for (w=0; w<4; w++)
 { scr_rowcol(y+q,x+w);
   if ((scr_sinp()!=32)&&(dt[s*4+r][q][w]=='1')) f=1;
 }
return(f);
}

void next()
{ /* choose next shape */
int a;

for (a=7; a<11; a++)
{ scr_rowcol(a,29);
  scr_aputs("    ",7);
}
ch=nch; sh=nsh; rt=nrt; col=nco;
nco=0;
while ((nco==0)||(nco==8)||((nco&7)==0))
  nco=rand()%128;
nsh=rand()%7;
nrt=rand()%4;
draw(29,7,nsh,nrt,nco,nsh+1);
}

int game()
{ /* play one game */
int z;

shake=0;
score=0;
lines=5;
level=0;
if (ky=='1') level=10;
if (ky=='2') level=20;
if (ky=='3') level=25;
newlevel(); 
scr_rowcol(15,1);
sprintf(buf,"Level: %2d",level);
scr_puts(buf);
scr_rowcol(16,1);
sprintf(buf,"Left : %2d",lines);
scr_puts(buf);
nsh=6; nrt=0;
next();
while (1)
{ next();
  x=19; y=1;
  z=check(x,y,sh,rt);
  if ((z)||(ky==27)) return(0);
  drop();
  score++;
  scr_rowcol(12,8);
  sprintf(buf,"%5d",score);
  scr_puts(buf);
  if (level>20) rndbl();
  checkline();
  if (lines==0) newlevel();
}
}

int drop()
{ /* drop current piece with key presses */
int cn,fl,df;
int rfl;

cn=0; fl=1; df=0; rfl=0;
while (fl)
{ draw(x,y,sh,rt,7,32);
  ky=scr_csts();
  df=0;

  if (PadStatus&PADstart)
  { /* pause */
    ky=0;
    scr_rowcol(2,0);
    scr_aputs("PAUSE",132);
    delay(1);
    while (PadStatus&PADstart)
    { delay(1);
      PadStatus=PadRead();
    }
    while (!(PadStatus&PADstart))
    { delay(1);
      PadStatus=PadRead();
    }

    scr_rowcol(2,0);
    scr_puts("     ");
    while (PadStatus&PADstart)
    { delay(1);
      PadStatus=PadRead();
    }
  }
  if (PadStatus&PADLleft)
   if (check(x-1,y,sh,rt)==0)
   { x--; }
  if (PadStatus&PADLright)
   if (check(x+1,y,sh,rt)==0)
   { x++; }
  if (PadStatus&PADLdown)
  { cn=4; df=1; }
  if ((ky==13)&&(rfl==0))
  { tick();

    rt++;
    if (rt==4) rt=0;
    if (check(x,y,sh,rt)==0) goto okay;

    /* Lawrence insists it should rotate even at edges.. autoshift */
    if (check(x+1,y,sh,rt)==0)
    { x++; goto okay; }
    if (check(x-1,y,sh,rt)==0)
    { x--; goto okay; }
    if (check(x-2,y,sh,rt)==0)
    { x-=2; goto okay; }

    /* can't rotate it */
    rt--;
    if (rt==-1) rt=3;
    
okay:
    rfl=1;
  }
  if (ky!=13) rfl=0;
  cn++; 
  if (cn==5) 
  { cn=0; 
    fl=1-check(x,y+1,sh,rt);
    if (fl) y++;
  }
  draw(x,y,sh,rt,col,nsh+1);
  if (df==0) delay(5);
  else delay(1);
}

boom();

/* wait for release of 'down' */
while (PadStatus&PADLdown)
{ delay(1);
  PadStatus=PadRead();
}
  
}

void delay(x) int x;
{ /* do screen copy, pause x jiffies, do swap */
int a,i,j,ch;
int activeBuff;
GsBOXF mybox;

for (a=0; a<x; a++)
{
  activeBuff=GsGetActiveBuff();

  GsClearOt(0, 0, &WorldOrderingTable[activeBuff]);

  /* process background */
  background(activeBuff);

  j=0;

  for (i=PLAYFIELD-1; i>=TEXTNUM; i--)
  { GsSortSprite(&Sprites[i],&WorldOrderingTable[activeBuff],j);
  }

  j++;

  GsSortSprite(&Sprites[PLAYFIELD],&WorldOrderingTable[activeBuff],j++);

  for(i = TEXTNUM-1; i>1; i--)
  { GsSortFastSprite(&Sprites[i], &WorldOrderingTable[activeBuff], j);
  }

  GsSortSprite(&Sprites[1],&WorldOrderingTable[activeBuff],j++);

  GsSetWorkBase((PACKET*)GpuOutputPacket[activeBuff]);

  DrawSync(0);

  VSync(0);     /* waits for vertical retrace */

  GsSwapDispBuff();                               

  /*GsSortClear(0, 0, 0, &WorldOrderingTable[activeBuff]);*/
  mybox.attribute=(2<<28)|(1<<30);
  mybox.x=0;
  mybox.y=0;
  mybox.w=320;
  mybox.h=240;
  mybox.r=fade_col;
  mybox.g=fade_col;
  mybox.b=fade_col;
  if (fade_col<255) fade_col++;

  GsSortBoxFill(&mybox,&WorldOrderingTable[activeBuff],3);

  GsDrawOt(&WorldOrderingTable[activeBuff]);

  /* rotate playfield <evil laughter> */
  if (level>25) Sprites[PLAYFIELD].rotate+=ONE;

  /* process shaking */
  if (shake)
  { shake--;
    Sprites[PLAYFIELD].scaley=ONE+((shake*shake)*3);
    Sprites[PLAYFIELD].x=160+(shake*(shake%2==0?1:-1));
  }

  /* text update */
  if (frame%2==1)
  { for (i=TEXTNUM; i<TEXTNUM+14; i++)
    { Sprites[i].x-=4;
      if (Sprites[i].x<=-36)
      { ch=*(msg++);
        if (*msg=='*') msg=(char*)MSGADD;
        if (ch=='!')
        { ch=26;
          goto gotchar;
        }
        if (ch=='-')
        { ch=27;
          goto gotchar;
        }
        if (ch=='?')
        { ch=28;
          goto gotchar;
        }
        if (ch=='.')
        { ch=29;
          goto gotchar;
        }
        if ((ch>='A')&&(ch<='Z'))
        { ch-='A';
          goto gotchar;
        }
        /* else */
        ch=30;
  
gotchar:
        Sprites[i].x+=392;
        Sprites[i].u=(ch%8)*28;
        Sprites[i].v=(ch/8)*29;
      }
    }
  }
}

}

void checkline()
{ /* check and erase lines */
int l,c,fl,x,y,as=10;
char qw[2];

l=23;

while (l>2)
{ fl=1;
  for (c=15; c<25; c++)
  { scr_rowcol(l,c);
    if (scr_sinp()==32) fl=0;
  }
  if (fl)
  { crash();
    for (c=15; c<25; c++)
    { scr_rowcol(l,c);
      scr_co(32);
      delay(1);
    }
    for (x=l-1; x>1; x--)
      for (c=15; c<25; c++)
      { scr_rowcol(x,c);
        y=scr_sinp();
        scr_rowcol(x+1,c);
        qw[0]=y; qw[1]=0;
        if (y==32) scr_aputs(qw,7);
        else scr_co(y);
      }
    score=score+as; as=as<<1;
    shake=25;
    switch (as)
    { case 20: fade_col=192; break;
      case 40: fade_col=128; break;
      case 80: fade_col=64;  break;
      case 160: fade_col=0;  break;
    }
    scr_rowcol(12,8);
    sprintf(buf,"%5d",score);
    scr_puts(buf);
    if (lines)
    { lines--;
      scr_rowcol(16,1);
      sprintf(buf,"Left : %2d",lines);
      scr_puts(buf);
    }
  }
    else l--;
}
}

void newlevel()
{ /* set up new level */
int a,c,fl,t;
u_long *addr;
RECT myrect;

level++;
scr_rowcol(15,1);
sprintf(buf,"Level: %2d",level);
scr_puts(buf);
lines=level*2+5;
scr_rowcol(16,1);
sprintf(buf,"Left : %2d",lines);
scr_puts(buf);

a=1; fl=1;
if ((ky=='1')||(ky=='2')||(ky=='3')||(level==1))
  a=25;
while (a<24)
{ for (c=15; c<25; c++)
  { scr_rowcol(a,c);
    if (scr_sinp()!=32) fl=0;
    scr_co(253);
    delay(1);
  }
  if (fl) score=score+5;
  scr_rowcol(12,8);
  sprintf(buf,"%5d",score);
  scr_puts(buf);
  delay(2);
  a++;
}

/* wait for key release */
while (PadStatus&(PADRleft|PADRright|PADRup|PADRdown))
{ delay(1);
  PadStatus=PadRead();
}

/* wait for new keypress */
if (!((ky=='1')||(ky=='2')||(ky=='3')||(level==1))) /* not if starting here */
  while (!(PadStatus&(PADRleft|PADRright|PADRup|PADRdown)))
  { delay(1);
    PadStatus=PadRead();
  }

clw();
/* pause, then wait for key release */
delay(30);
while (PadStatus&(PADRleft|PADRright|PADRup|PADRdown))
{ delay(1);
  PadStatus=PadRead();
}

addr=BGBUF[(level-1)%8];
addr++;
GsGetTimInfo(addr, &TexInfo);
myrect.x=440;
myrect.y=249;
myrect.w=80;
myrect.h=207;
LoadImage(&myrect,TexInfo.pixel);  

if (level>10)
{ /* handicap */
  a=(level-10);
  if (a>10) a=10;
  for (a=24-a; a<24; a++)
  { c=rand()%10+15;
    scr_rowcol(a,c);
    scr_co(255);
  }
}
scr_rowcol(2,0);
switch (level)
{ case 11: scr_puts("Handicap!"); break;
  case 21: scr_puts("Beware!"); break;
  case 26: scr_puts("Smartass!"); break;
}
if ((level==11)||(level==21)||(level==26)) delay(200);
scr_rowcol(2,0);
scr_puts("         ");
}

void rndbl()
{ /* maybe add a random block */
int y,c,qw;

if (rand()%100<=level)
{ c=rand()%10+15;
  y=0;
  qw=32;
  while (qw==32)
  { y++; scr_rowcol(y,c);
    qw=scr_sinp();
  }
  y--;
  scr_rowcol(y,c);
  scr_co(255);
}
}

void background(int abuff)
{ int xpixel,ypixel,temp,pix1,pix2,pix3,pix4,pix5,pix6,pix7,pix8;
  int col,pos;
  RECT myrect;

  frame++;
  col=255-frame%64;

  /* process plasma */
  Sprites[1].rotate-=ONE+ONE;

  pos=frame%5;

  switch (pos)
  { case 0: ballx+=ballxd;
            if ((ballx<3)||(ballx>56)) ballxd*=-1;
            bally+=ballyd;
            if ((bally<3)||(bally>55)) ballyd*=-1;
            PLASMABUF[bally][ballx]=col;
            PLASMABUF[bally-1][ballx]=col;
            PLASMABUF[bally+1][ballx]=col;
            PLASMABUF[bally][ballx-1]=col;
            PLASMABUF[bally][ballx+1]=col;
            PLASMABUF[59-bally][59-ballx]=col;
            PLASMABUF[58-bally][59-ballx]=col;
            PLASMABUF[60-bally][59-ballx]=col;
            PLASMABUF[59-bally][58-ballx]=col;
            PLASMABUF[59-bally][60-ballx]=col;
            for(ypixel = 1; ypixel < 12; ypixel++)
            {
              for(xpixel = 1 ;xpixel < 59; xpixel++)
              {
                pix1=PLASMABUF[ypixel+1][xpixel+1];
                pix2=PLASMABUF[ypixel+1][xpixel-1];
                pix3=PLASMABUF[ypixel+1][xpixel];
                pix4=PLASMABUF[ypixel-1][xpixel+1];
                pix5=PLASMABUF[ypixel-1][xpixel-1];
                pix6=PLASMABUF[ypixel-1][xpixel];
                pix7=PLASMABUF[ypixel][xpixel+1];
                pix8=PLASMABUF[ypixel][xpixel-1];
               
                temp=(pix1+pix2+pix3+pix4+pix5+pix6+pix7+pix8)>>3;
            
                PLASMABUF[ypixel][xpixel]=temp;
        
                *(PICBUF+(ypixel<<6)+xpixel)=PALETTE[temp];
              }
            }
            break;
    case 1: for(ypixel = 12; ypixel < 24; ypixel++)
            {
              for(xpixel = 1 ;xpixel < 59; xpixel++)
              {
                pix1=PLASMABUF[ypixel+1][xpixel+1];
                pix2=PLASMABUF[ypixel+1][xpixel-1];
                pix3=PLASMABUF[ypixel+1][xpixel];
                pix4=PLASMABUF[ypixel-1][xpixel+1];
                pix5=PLASMABUF[ypixel-1][xpixel-1];
                pix6=PLASMABUF[ypixel-1][xpixel];
                pix7=PLASMABUF[ypixel][xpixel+1];
                pix8=PLASMABUF[ypixel][xpixel-1];
               
                temp=(pix1+pix2+pix3+pix4+pix5+pix6+pix7+pix8)>>3;
            
                PLASMABUF[ypixel][xpixel]=temp;
        
                *(PICBUF+(ypixel<<6)+xpixel)=PALETTE[temp];
              }
            }
            break;
    case 2: for(ypixel = 24; ypixel < 36; ypixel++)
            {
              for(xpixel = 1 ;xpixel < 59; xpixel++)
              {
                pix1=PLASMABUF[ypixel+1][xpixel+1];
                pix2=PLASMABUF[ypixel+1][xpixel-1];
                pix3=PLASMABUF[ypixel+1][xpixel];
                pix4=PLASMABUF[ypixel-1][xpixel+1];
                pix5=PLASMABUF[ypixel-1][xpixel-1];
                pix6=PLASMABUF[ypixel-1][xpixel];
                pix7=PLASMABUF[ypixel][xpixel+1];
                pix8=PLASMABUF[ypixel][xpixel-1];
               
                temp=(pix1+pix2+pix3+pix4+pix5+pix6+pix7+pix8)>>3;
            
                PLASMABUF[ypixel][xpixel]=temp;
        
                *(PICBUF+(ypixel<<6)+xpixel)=PALETTE[temp];
              }
            }
            break;
    case 3: for(ypixel = 36; ypixel < 48; ypixel++)
            {
              for(xpixel = 1 ;xpixel < 59; xpixel++)
              {
                pix1=PLASMABUF[ypixel+1][xpixel+1];
                pix2=PLASMABUF[ypixel+1][xpixel-1];
                pix3=PLASMABUF[ypixel+1][xpixel];
                pix4=PLASMABUF[ypixel-1][xpixel+1];
                pix5=PLASMABUF[ypixel-1][xpixel-1];
                pix6=PLASMABUF[ypixel-1][xpixel];
                pix7=PLASMABUF[ypixel][xpixel+1];
                pix8=PLASMABUF[ypixel][xpixel-1];
               
                temp=(pix1+pix2+pix3+pix4+pix5+pix6+pix7+pix8)>>3;
            
                PLASMABUF[ypixel][xpixel]=temp;
        
                *(PICBUF+(ypixel<<6)+xpixel)=PALETTE[temp];
              }
            }
            break;
    case 4: for(ypixel = 48; ypixel < 59; ypixel++)
            {
              for(xpixel = 1 ;xpixel < 59; xpixel++)
              {
                pix1=PLASMABUF[ypixel+1][xpixel+1];
                pix2=PLASMABUF[ypixel+1][xpixel-1];
                pix3=PLASMABUF[ypixel+1][xpixel];
                pix4=PLASMABUF[ypixel-1][xpixel+1];
                pix5=PLASMABUF[ypixel-1][xpixel-1];
                pix6=PLASMABUF[ypixel-1][xpixel];
                pix7=PLASMABUF[ypixel][xpixel+1];
                pix8=PLASMABUF[ypixel][xpixel-1];
               
                temp=(pix1+pix2+pix3+pix4+pix5+pix6+pix7+pix8)>>3;
            
                PLASMABUF[ypixel][xpixel]=temp;
        
                *(PICBUF+(ypixel<<6)+xpixel)=PALETTE[temp];
              }
            }
            myrect.x=640;
            myrect.y=100;
            myrect.w=64;
            myrect.h=60;
            LoadImage(&myrect,(u_long*)PICBUF);
            break;
  }

  /* process starfield */

  if (frame%2==0)
  { TheOT = &WorldOrderingTable[abuff];
    ani_stars();
  }
  
}

#define S_TICK 127

void tick()
{ /* play a tick sound for rotation */

  if (OLDVOICE!=-1)
    SsUtKeyOff(OLDVOICE,VAB_ID,S_TICK,1,20);

  OLDVOICE=SsUtKeyOn(VAB_ID,S_TICK,1,20,0,100,100);
}

#define S_BOOM 127

void boom()
{ /* play a boom sound for landing */

  if (OLDVOICE!=-1)
    SsUtKeyOff(OLDVOICE,VAB_ID,S_BOOM,1,8);

  OLDVOICE=SsUtKeyOn(VAB_ID,S_BOOM,1,8,0,115,115);
}

#define S_CRASH 127

void crash()
{ /* play a crash sound? for clearing a line */

  if (OLDVOICE!=-1)
    SsUtKeyOff(OLDVOICE,VAB_ID,S_CRASH,1,1);

  OLDVOICE=SsUtKeyOn(VAB_ID,S_CRASH,1,1,0,127,127);
}

void PadInit (void)
{
	GetPadBuf(&bb0, &bb1);
}

u_long PadRead(void)
{ u_long PadStatus;
  int i;

  PadStatus=(~(*(bb0+3) | *(bb0+2) << 8 | *(bb1+3) << 16 | *(bb1+2) << 24));

  if (PadStatus&PADR1)
  { current_tune=CdPlay(3,(int*)&cd_tracks,0);
    current_tune++;
    if (current_tune>13)
      current_tune=0;
    PlaySeq(current_tune);
    delay(15);
  }

  if (PadStatus&PADselect) 
  { TEXTSTAT++;
    if (TEXTSTAT==2) TEXTSTAT=3;
    if (TEXTSTAT==4) TEXTSTAT=0;
    for (i=TEXTNUM; i<TEXTNUM+14; i++)
    { Sprites[i].attribute = (1<<6)|(TEXTSTAT<<28)|(1<<30)|(2<<24);
    }
    delay(15);
  }

  return(PadStatus);
}

