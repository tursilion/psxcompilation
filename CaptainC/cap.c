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
#include <libtap.h>
#include <libcd.h>

/* common */
#include "miscdefs.h"
#include "cap.h"

/* either define CAETLA or CDROM or PCDRIVE */
//#define CAETLA
#define CDROM 

void setup()
{
	/* setup system */
	int i,x,y;
	u_long colorMode;
	unsigned short tp;
	RECT myrect;
	
	PadStatus = 0;
	SetVideoMode( MODE_NTSC );
	ResetGraph(0);
	GsInitGraph(320 ,240, GsOFSGPU|GsNONINTER, 1, 0);                
	GsDefDispBuff(0, 0, 320, 0);
	PadInit(0);
	WorldOrderingTable[0].length = OT_LENGTH;
	WorldOrderingTable[1].length = OT_LENGTH;
	WorldOrderingTable[0].org = zSortTable[0];
	WorldOrderingTable[1].org = zSortTable[1];
	
	buff=GsGetActiveBuff();
	GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
	GsClearOt(0, 0, &WorldOrderingTable[buff]);
	
	CdInit();
	
	/* display loading screen... */
	LoadFile("PATIENCE.TIM");
	initTexture();
	myrect.x=0;
	myrect.y=0;
	myrect.w=SCREEN_WIDTH;
	myrect.h=SCREEN_HEIGHT;
	
	clear_sprites();
	
	sprite_dat[1].tx=0;
	sprite_dat[1].ty=256;
	sprite_dat[1].w=256;
	sprite_dat[1].h=240;
	sprite_dat[1].ox=0;
	sprite_dat[1].oy=0;
	sprite_dat[1].cx=512;
	sprite_dat[1].cy=260;
	initSprite(1,32,0,1);
	Sprites[1].attribute=(2<<24);
	Sprites[1].mx=Sprites[1].my=0;
	drawscreen();
	drawscreen();
	
	/* initialize background sprites... */
	
	/* cell definitions */
	i=0;
	for (y=256; y<512; y+=BGTILESIZE) {
		for (x=0; x<512; x+=BGTILESIZE/2) {
			tp=GetTPage(1,0,x/64*64,256);
			colorMode = 1;  /* 8 bit CLUT */
			BG[i].attribute = (colorMode<<24);
			BG[i].w = BGTILESIZE;
			BG[i].h = BGTILESIZE;
			BG[i].x = x*2;
			BG[i].y = y-264;
			BG[i].mx = BGTILESIZE/2;    /* center f. rotation/scaling */
			BG[i].my = BGTILESIZE/2;
			BG[i].tpage = tp;           
			BG[i].u = x%64*2;           /* offset of the sprite INSIDE */
			BG[i].v = y-256;            /* the 255X255 pixel TIM picture */
			BG[i].cx = 512;             /* color table x,y */
			BG[i].cy = 256;
			BG[i].r = 0x80;             /* colour adjust */
			BG[i].g = 0x80;
			BG[i].b = 0x80;
			BG[i].rotate = 0;
			BG[i].scalex = ONE;
			BG[i].scaley = ONE;
			i++;
		}
	}

	clear_enemies();

	/* done */
	
	/* now the sound system */
	SsInit();
	
	SsSetTickMode(SS_TICK50);
	
	SsStart();
	
	SsSetMVol(0,0);  /* mute sound while loading data */
	SsUtSetReverbType(SS_REV_TYPE_OFF);
	SsUtReverbOn();
	
	SsUtSetReverbDepth(64,64);
	
	SsSetSerialAttr(SS_SERIAL_A,SS_MIX,SS_SON);
	SsSetSerialVol(SS_SERIAL_A,127,127);
	
	PlayCD(NULL);
	
	LoadFile("MISC\\CAPTAINC.VH");
	bcopy((char*)LOADBUFFER,VH,15000);
	
	LoadFile("MISC\\CAPTAINC.VB");
	VAB_ID=SsVabTransfer((unsigned char *)VH,(unsigned char*)LOADBUFFER,-1,SS_WAIT_COMPLETED);
	if (VAB_ID<0) printf("** Sound Transfer Failed. ",VAB_ID);
	printf("VAB_ID - %d\n",VAB_ID);
	
	SsSetMVol(120,120);
	
	SsUtAllKeyOff(0);
	
	strcpy(Text_String,"");
	
	/* define static sprite tables */
	for (i=10; i<38; i++) /* cap */ {
		sprite_dat[i].tx=640;
		sprite_dat[i].ty=0;
		sprite_dat[i].w=32;
		sprite_dat[i].h=32;
		sprite_dat[i].ox=((i-10)%8)*32;
		sprite_dat[i].oy=((i-10)/8)*32;
		sprite_dat[i].cx=512;
		sprite_dat[i].cy=257;
	}
	
	for (i=38; i<62; i++) /* misc */ {
		sprite_dat[i].tx=768;
		sprite_dat[i].ty=256;
		sprite_dat[i].w=32;
		sprite_dat[i].h=32;
		sprite_dat[i].ox=((i-38)%8)*32;
		sprite_dat[i].oy=((i-38)/8)*32;
		sprite_dat[i].cx=512;
		sprite_dat[i].cy=267;
	}
	
	for (i=62; i<99; i++) /* enemies */ {
		sprite_dat[i].tx=768;
		sprite_dat[i].ty=0;
		sprite_dat[i].w=32;
		sprite_dat[i].h=32;
		sprite_dat[i].ox=((i-62)%8)*32;
		sprite_dat[i].oy=((i-62)/8)*32;
		sprite_dat[i].cx=512;
		sprite_dat[i].cy=259;
	}
	
	/* load Cap's color tables */
	LoadFile("SPRITES\\CAP_AIR.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_EA_1.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_FIRE.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_GL_1.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_GL_2.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_HOLY.TIM");
	initTexture();
	
	LoadFile("SPRITES\\CAP_ICE.TIM");
	initTexture();
	
	KEY_FIRE=1;
	KEY_JUMP=2;
	KEY_SLIDE=3;
	MUSIC_VOL=10;
	EFFECT_VOL=10;
	
	FRAME=0;
}

void initSprite(int i, int x, int y, int sprnum)
{
	u_long colorMode;
	u_short tPage;
	
	colorMode = 1;  /* 8 bit CLUT */
	
	tPage = GetTPage(colorMode,0,sprite_dat[sprnum].tx,sprite_dat[sprnum].ty);
	
	Sprites[i].attribute = (colorMode<<24);
	
	Sprites[i].w = sprite_dat[sprnum].w;
	Sprites[i].h = sprite_dat[sprnum].h;
	
	Sprites[i].x = x;
	Sprites[i].y = y;
	
	Sprites[i].mx = sprite_dat[sprnum].w/2;       /* center f. rotation/scaling */
	Sprites[i].my = sprite_dat[sprnum].h/2;
	
	Sprites[i].tpage = tPage;
	
	Sprites[i].u = sprite_dat[sprnum].ox;          /* offset of the sprite INSIDE */
	Sprites[i].v = sprite_dat[sprnum].oy;        /* the 255X255 pixel TIM picture */
	
	Sprites[i].cx = sprite_dat[sprnum].cx;                        /* color table x,y */
	Sprites[i].cy = sprite_dat[sprnum].cy;
	
	Sprites[i].r = 0x80;              /* colour adjust */
	Sprites[i].g = 0x80;
	Sprites[i].b = 0x80;
	
	Sprites[i].rotate = 0;
	Sprites[i].scalex = ONE;
	Sprites[i].scaley = ONE;
	
	Spritepri[i]=1;
}

void set_pattern(int sp, int pat)
{ /* change sprite pattern number */
	int colorMode;
	
	colorMode=1;
	Sprites[sp].w = sprite_dat[pat].w;
	Sprites[sp].h = sprite_dat[pat].h;
	
	Sprites[sp].mx = sprite_dat[pat].w/2;       /* center f. rotation/scaling */
	Sprites[sp].my = sprite_dat[pat].h/2;
	
	Sprites[sp].tpage = GetTPage(colorMode,0,sprite_dat[pat].tx,sprite_dat[pat].ty);
	
	Sprites[sp].u = sprite_dat[pat].ox;          /* offset of the sprite INSIDE */
	Sprites[sp].v = sprite_dat[pat].oy;        /* the 255X255 pixel TIM picture */
	
	Sprites[sp].cx = sprite_dat[pat].cx;                        /* color table x,y */
	Sprites[sp].cy = sprite_dat[pat].cy;
}

void set_flip(int sp,int dir)
{ Sprites[sp].scalex = (dir>0 ? ONE : -ONE+1 );
} 

void PlayCD(int x)
{
	if (x==NULL) CdPlay(0,(int*)&cd_tracks,0);
	else CdPlay(2,(int*)&cd_tracks[x-1],0);
}

void drawscreen()
{ 
	int i,t;
	GsBOXF mybox;
	static int rotate;
	static int rot_speed;
	GsSPRITE tempsprite;
	int time;
	
	FRAME++;
	rot_speed++;
	if (rot_speed>1) {
		rot_speed=0;
		rotate++;
		if (rotate>9) rotate=0;
	}
	
	DrawSync(0);
	
	if (FPSON) {
		time=VSync(1);
		Text_X=0;
		Text_Y=220;
		sprintf(Text_String,"Percentage at 30fps: %d %%",(time*100)/480);
	}
	
	VSync(2);
	GsSwapDispBuff();
	
	for (i=49; i>=0; i--)
	{
		if (ENEMY_NONE != Enemy[i].iType)
		{
			if (Enemy[i].Sprite.r > 0x80)
			{
				Enemy[i].Sprite.r-=10;
				if (Enemy[i].Sprite.r<0x80)
					Enemy[i].Sprite.r=0x80;

				Enemy[i].Sprite.g=Enemy[i].Sprite.r;
				Enemy[i].Sprite.b=Enemy[i].Sprite.r;
			}


			Enemy[i].Sprite.x=Enemy[i].gx-FGScroll;
			Enemy[i].Sprite.y=Enemy[i].gy-FGYScroll;
			
			if (Enemy[i].dead)
			{
				if (FRAME % Enemy[i].dead == 0)
				{
					Enemy[i].dead++;
					if (Enemy[i].dead > 12)
					{
						Enemy[i].iType=ENEMY_NONE;
					}
				}
				else
				{
					goto nodraw;
				}
			}

			if (Enemy[i].fFast)
			{
				GsSortFastSprite(&Enemy[i].Sprite, &WorldOrderingTable[buff], Enemy[i].iPriority );
			}
			else
			{
				GsSortSprite(&Enemy[i].Sprite, &WorldOrderingTable[buff], Enemy[i].iPriority );
			}
nodraw:
		}
	}
	
	for (i=0; i<SPRITEMAX; i++) {
		if (Sprites[i].x!=-1) {
			GsSortSprite(&Sprites[i],&WorldOrderingTable[buff],Spritepri[i]);
		}
	}
	
	if (LINESON) {
		for (i=0; i<LINESON; i++)
			GsSortGLine(&Lines[i],&WorldOrderingTable[buff],0);
	}
	
	if (GALAXYON) {
		Galaxy.rotate-=TWO;
		for (i=0; i<20; i++)
			GsSortLine(&Stars[i],&WorldOrderingTable[buff],3);  
		GsSortSprite(&Galaxy,&WorldOrderingTable[buff],3);
	}
	
	if (SHADOWON) {
		for (i=0; i<5; i++) {
			tempsprite.attribute=(1<<24)|(0<<28)|(1<<30);
			tempsprite.w=32;
			tempsprite.h=32;
			tempsprite.x=ShadowX[i]-FGScroll;
			tempsprite.y=ShadowY[i]-FGYScroll;
			tempsprite.tpage=GetTPage(1,0,640,0);
			tempsprite.u=192;
			tempsprite.v=64;
			tempsprite.cx=512;
			tempsprite.cy=257;
			tempsprite.r=0x80;
			tempsprite.g=0x80;
			tempsprite.b=0x80;
			tempsprite.mx=16;
			tempsprite.my=16;
			tempsprite.scalex=Sprites[1].scalex;
			tempsprite.scaley=ONE;
			tempsprite.rotate=0;
			GsSortSprite(&tempsprite,&WorldOrderingTable[buff],1);
		}
	}
	
	if (BGON) {
		t=BGSCROLL;
		for (i=0; i<BGNUMTILES; i++) {
			if ((BG[i].x>=t-BGTILESIZE)&&(BG[i].x<=t+352)) {
				BG[i].x-=t;
				if (HELLON)
					BG[i].y+=SINE[BG[i].x/32+rotate];
				GsSortFastSprite(&BG[i],&WorldOrderingTable[buff],2);
				if (HELLON)
					BG[i].y-=SINE[BG[i].x/32+rotate];
				BG[i].x+=t;
			}
		}
		t=FGScroll;
		for (i=0; i<FGNUMTILES; i++) {
			if ((FG[i].x!=-1)&&(FG[i].x>=t-BGTILESIZE)&&(FG[i].x<=t+352)) {
				FG[i].x-=t;
				FG[i].y-=FGYScroll;
				GsSortFastSprite(&FG[i],&WorldOrderingTable[buff],1);
				FG[i].x+=t;
				FG[i].y+=FGYScroll;
			}
		}
	}
	
	if (Text_String[0]) { 
		i=0;
		while (Text_String[i]) {
			tempsprite.attribute=(0<<24);
			tempsprite.w=10;
			tempsprite.h=20;
			tempsprite.x=Text_X+i*10;
			tempsprite.y=Text_Y;
			tempsprite.tpage=GetTPage(0,0,960,256);
			tempsprite.u=((Text_String[i]-32)%25)*10;
			tempsprite.v=((Text_String[i]-32)/25)*20;
			tempsprite.cx=512;
			tempsprite.cy=262;
			tempsprite.r=0x80;
			tempsprite.g=0x80;
			tempsprite.b=0x80;
			GsSortFastSprite(&tempsprite,&WorldOrderingTable[buff],0);
			i++;
		}
	}
	
	GsSortClear(0x0, 0x0, 0x0, &WorldOrderingTable[buff]);
	GsDrawOt(&WorldOrderingTable[buff]);
	buff=GsGetActiveBuff();
	GsSetWorkBase((PACKET*)GpuOutputPacket[buff]);
	GsClearOt(0, 0, &WorldOrderingTable[buff]);
}

void LoadFile(char *s)
{ /* this lets me vary between Caetla and CD */
	char buf[80];
	FILE in;
	
#ifdef CAETLA
	/* CAETLA */
	strcpy(buf,"pcdrv:");
	strcat(buf,s);
	printf("Open %s\n",s);
	in=open(buf,O_RDONLY);
	if (in) {
		read(in,(char*)LOADBUFFER,500000);
		close(in);
		printf("good.\n");
	}
#endif
	
#ifdef CDROM
	/* CD */
	strcpy(buf,"\\CAP\\");
	strcat(buf,s);
	strcat(buf,";1");
	printf("Open %s\n",s);
	CdReadFile(buf,(u_long*)LOADBUFFER,0);
	CdReadSync(0,0);
	printf("good.\n");
#endif
}

void initTexture()
{ RECT rect1;
GsIMAGE TexInfo;
u_long *addr;

addr=(u_long*)LOADBUFFER;
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
if (TexInfo.pmode<2) {
	rect1.x=TexInfo.cx;
	rect1.y=TexInfo.cy;
	rect1.w=TexInfo.cw;
	rect1.h=TexInfo.ch;
	printf("Color at %d, %d\n",rect1.x,rect1.y);
	LoadImage(&rect1,TexInfo.clut);
}

DrawSync(0);
/* wait for finish */

}

void main()
{ /* main routine.. start here */
	int i;
	
	printf("Starting Captain Communism...\n");
	setup();
	
	LoadFile("MISC\\COMIC__1.TIM");
	initTexture();
	
	BGON=0;
	HELLON=0;
	GALAXYON=0;
	FPSON=1;
	SHADOWON=0;
	FIREON=0;
	LINESON=0;
	strcpy(Text_String,"");
	Text_X=0;
	Text_Y=210;
	
loop:
	run_intro();
loop2:
	do_title();
	
	if (GAME_TYPE==-1) goto loop;
	
	if (GAME_TYPE==1) {
		do_options();
		clear_sprites();
		drawscreen();
		while ((PadStatus&PADRdown)||(PadStatus&PADstart)) {
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto loop2;
	}
	
	if (GAME_TYPE == 2)
		goto loop2;
	
	if (GAME_TYPE == 3) {
		do_help();
		goto loop2;
	}
	
	FPSON=1;
	
	PadStatus=0;
	while (!(PadStatus&PADRdown)) {
		PadStatus=PadRead(0);
		VSync(0);
	}
	
	scroll_run();
	
	goto loop;
	
}

void clear_sprites()
{ int i;

for (i=0; i<SPRITEMAX; i++) {
	Sprites[i].x=-1;
}
}

void castle_view()
{ int i;

clear_sprites();
BGON=0;

LoadFile("MISC\\CASTLE.TIM");
initTexture();

LoadFile("MISC\\CAP_BACK.TIM");
initTexture();

sprite_dat[1].tx=896;
sprite_dat[1].ty=0;
sprite_dat[1].w=256;
sprite_dat[1].h=152;
sprite_dat[1].ox=0;
sprite_dat[1].oy=0;
sprite_dat[1].cx=512;
sprite_dat[1].cy=260;
initSprite(1,32,239,1);
Sprites[1].mx=Sprites[1].my=0;

sprite_dat[2].tx=768;
sprite_dat[2].ty=0;
sprite_dat[2].w=256;
sprite_dat[2].h=256;
sprite_dat[2].ox=0;
sprite_dat[2].oy=0;
sprite_dat[2].cx=512;
sprite_dat[2].cy=261;
initSprite(2,32,-80,2);
Sprites[2].mx=Sprites[2].my=0;

sprite_dat[3].tx=768;
sprite_dat[3].ty=256;
sprite_dat[3].w=256;
sprite_dat[3].h=80;
sprite_dat[3].ox=0;
sprite_dat[3].oy=0;
sprite_dat[3].cx=512;
sprite_dat[3].cy=261;
initSprite(3,32,176,3);
Sprites[3].mx=Sprites[3].my=0;

for (i=0; i<80; i++) {
	Sprites[1].y=219-i;
	Sprites[2].y=-80+i;
	Sprites[3].y=Sprites[2].y+256;
	drawscreen();
}

for (i=0; i<5*30; i++) {
	/* 5 seconds, 30 fps */
	drawscreen();
}

clear_sprites();
}

void scroll_run()
{ int i,j,t,u;
u_long colorMode;
int cap_anim[8]={12, 34, 14, 36, 16, 36, 14, 34 };
int cap_frame=0,cap_pat=0;
int cap_yd;
int level=1;
int firing,jumping,last_ladder_frame=22;
int global_x, global_y, off_x, off_y;
int new_global_x,new_global_y,new_off_x,new_off_y;
int move_flag,slide,sliding,sgn;

clear_sprites();

LoadFile("SPRITES\\CAP_SP_1.TIM");
initTexture();
LoadFile("SPRITES\\MISC_S_1.TIM");
initTexture();

FPSON=0;

replay:
start_level(level);

initForeground();

initSprite(1,160,-32,10);

cap_yd=0;
firing=0;
jumping=0;
FGScroll=0;
FGYScroll=0;
PadStatus=0;
slide=0;
sliding=0;
FIREON=1;
FireCharge=0;
CapWeapon=1;  /* P-shooter */

cap_palette(CapWeapon);

for (i=0; i<5; i++) {
	FireType[i]=0;
}

start_cap();

while (!(PadStatus&PADstart)) {
	if (FRAME%3==0) cap_frame++;
	if (cap_frame>7) cap_frame=0;
	PadStatus=PadRead(0);
	cap_pat=10;
	move_flag=0;
	
	do_enemies();

	if (PadStatus&PADR1) {
		CapWeapon++;
		if (CapWeapon>8) CapWeapon=1;
		if ((CapWeapon==5)||(CapWeapon==6))
			CapWeapon=7;
		
		play_sample(S_CHANGE);
		
		sgn=(Sprites[1].scalex>0 ? 1 : (-1));
		for (i=ONE; i>128; i=i/2) {
			Sprites[1].scalex=i*sgn;
			drawscreen();
		}
		
		cap_palette(CapWeapon);
		while (PadStatus&PADR1)
			PadStatus=PadRead(0);
		
		for (i=128; i<ONE; i=i*2) {
			Sprites[1].scalex=i*sgn;
			drawscreen();
		}
		
		set_flip(1,sgn);
		
	}
	
	/* Get global x, y and offsets */
	global_x=(Sprites[1].x+FGScroll)/BGTILESIZE;
	global_y=(Sprites[1].y+FGYScroll)/BGTILESIZE;
	off_x=(Sprites[1].x+FGScroll)-(global_x*BGTILESIZE);
	off_y=(Sprites[1].y+FGYScroll)-(global_y*BGTILESIZE);
	
	new_global_x=global_x;
	new_global_y=global_y;
	new_off_x=off_x;
	new_off_y=off_y;
	
	/* check to ensure not walking INSIDE a brick */
	if ((cap_yd==0)&&(tile_type(LEVEL[global_y][global_x])!='5')) {
		new_off_y=2;   /* if not going up or down, and not on a */
		/* ladder, must be atop the brick */
	}
	
	/* sliding */
	if ((PadStatus & Button[KEY_SLIDE])&&(slide==0)&&(jumping==0)&&(sliding==0)) {
		play_sample(S_SLIDE);
		slide=15;
		sliding=1;
		firing=0;
		for (i=0; i<5; i++) {
			ShadowX[i]=(new_global_x*BGTILESIZE)+new_off_x;
			ShadowY[i]=(new_global_y*BGTILESIZE)+new_off_y;
		}
	}
	
	if (!(PadStatus & Button[KEY_SLIDE]))
		sliding=0;
	
	if (slide) {
		slide--;
		
		if (Sprites[1].scalex<0) {
			new_off_x-=(slide<8 ? slide : 8);
			move_flag=1;
			if (new_off_x<0) {
				new_off_x+=BGTILESIZE;
				if (new_global_x>0) {
					new_global_x--;
					if (tile_type(LEVEL[global_y+1][global_x])=='2')
						new_global_y++;
					if (tile_type(LEVEL[global_y][new_global_x])=='3')
						new_global_y--;
				}
				else {
					new_off_x=off_x; 
					slide=0;
				}
			}
			set_flip(1,-1);
			cap_pat=32;
		}
		
		if (Sprites[1].scalex>0) {
			new_off_x+=(slide<8 ? slide : 8);
			move_flag=1;
			if (new_off_x>=BGTILESIZE) {
				new_off_x-=BGTILESIZE;
				if (new_global_x < (MAXSCROLL/BGTILESIZE+(SCREEN_WIDTH/BGTILESIZE))-1) {
					new_global_x++;
					if (tile_type(LEVEL[global_y][new_global_x])=='2')
						new_global_y--;
					if (tile_type(LEVEL[global_y+1][global_x])=='3')
						new_global_y++;
					
				}
				else {
					new_off_x=off_x; 
					slide=0;
				}
			}
			set_flip(1,1);
			cap_pat=32;
		}
	}
	
	SHADOWON=slide;
	
	/* left-right movement */
	if ((PadStatus & PADLleft)&&(slide==0)) {
		new_off_x-=4;
		slide=0;
		move_flag=1;
		if (new_off_x<0) {
			new_off_x+=BGTILESIZE;
			if (new_global_x>0) {
				new_global_x--;
				if (tile_type(LEVEL[global_y+1][global_x])=='2')
					new_global_y++;
				if (tile_type(LEVEL[global_y][new_global_x])=='3')
					new_global_y--;
			}
			else
				new_off_x=off_x; /* don't move */
		}
		set_flip(1,-1);
		cap_pat=cap_anim[cap_frame];
	}
	
	if ((PadStatus & PADLright)&&(slide==0)) {
		new_off_x+=4;
		slide=0;
		move_flag=1;
		if (new_off_x>=BGTILESIZE) {
			new_off_x-=BGTILESIZE;
			if (new_global_x < (MAXSCROLL/BGTILESIZE+(SCREEN_WIDTH/BGTILESIZE))-1) {
				new_global_x++;
				if (tile_type(LEVEL[global_y][new_global_x])=='2')
					new_global_y--;
				if (tile_type(LEVEL[global_y+1][global_x])=='3')
					new_global_y++;
			}
			else
				new_off_x=off_x; /* don't move */
		}
		set_flip(1,1);
		cap_pat=cap_anim[cap_frame];
	}
	
	if (PadStatus & PADLup) {
		if (tile_type(LEVEL[global_y][global_x])=='5') {
			new_off_x=16;
			new_off_y-=4;
			cap_pat=last_ladder_frame=22+2*(cap_frame%2);
			if (new_off_y<0) {
				new_off_y+=BGTILESIZE;
				if (new_global_y>0)
					new_global_y--;
				else
					new_off_y=off_y; /* don't move */
			}
		}
	}
	
	if (PadStatus & PADLdown) {
		if (tile_type(LEVEL[global_y+1][global_x])=='5') {
			new_off_x=16;
			new_off_y+=4;
			cap_pat=last_ladder_frame=22+2*(cap_frame%2);
			if (new_off_y<BGTILESIZE) {
				new_off_y-=BGTILESIZE;
				if (new_global_y<63) {
					new_global_y++;
				}
				else {
					new_global_y=0; /* wrap around */
				}
			}
		}
	}
	
	/* check if standing still on a ladder */
	if ((tile_type(LEVEL[global_y][global_x])=='5')&&
		(tile_type(LEVEL[global_y+1][global_x])=='5')&&(move_flag==0))
		cap_pat=last_ladder_frame;
	
	/* downward/jumps */
	
	if ((cap_yd==0)&&(!(PadStatus&Button[KEY_JUMP])))
		jumping=0;
	
	if ((PadStatus&Button[KEY_JUMP])&&(cap_yd==0)&&(jumping==0)&&(tile_type(LEVEL[global_y][global_x])!='5')) {
		cap_pat=18;
		cap_yd=-15;
		jumping=1;
		slide=0;
		play_sample(S_JUMP);
	}
	
	if (cap_yd) {
		new_off_y+=cap_yd;
		if (new_off_y<0) {
			new_off_y+=BGTILESIZE;
			if (new_global_y>0)
				new_global_y--;
			else
				new_off_y=off_y;  /* don't move */
		}
		if (new_off_y>BGTILESIZE) {
			new_off_y-=BGTILESIZE;
			if (new_global_y<63)
				new_global_y++;
			else
				new_global_y=0;  /* wrap around */
		}
		cap_yd+=2;
		if (cap_yd<0) cap_pat=18;
		if (cap_yd>0) cap_pat=20;
		if (cap_yd>19) cap_yd=19;
	}
	
	/* background interaction */
	
	/* check ground/falling */
	
	if (cap_yd>=0) {
		/* check based on the old 'x' to prevent clinging to walls */
		if (tile_type(LEVEL[global_y+1][global_x])=='0')
		{ 
			cap_yd+=1;
		}
		if ((cap_yd)&&(tile_type(LEVEL[new_global_y+1][global_x])!='0'))
		{ 
			/* landed */
			cap_pat=10;
			play_sample(S_LAND);
			new_off_y=2;
			cap_yd=0;
		}
	}
	
	/* check to ensure not walking INSIDE a brick */
	if ((cap_yd==0)&&(tile_type(LEVEL[global_y][global_x])!='5'))
	{ 
		new_off_y=2;   /* if not going up or down, and not on a */
		/* ladder, must be atop the brick */
	}
	
	/* check legality of new y */
	
	if ((tile_type(LEVEL[new_global_y][new_global_x])=='0')||
		(tile_type(LEVEL[new_global_y][new_global_x])=='5'))
		goto okay;
	/* no good... adjust 'x' and try again */
	t=new_global_x;
	u=new_off_x;
	new_global_x=global_x;
	new_off_x=off_x;
	if (tile_type(LEVEL[new_global_y][new_global_x])=='0') goto okay;
	/* still no good... try just y */
	if (cap_yd<0) cap_yd=1;
	new_global_y=global_y;
	new_off_y=off_y;
	new_global_x=t;
	new_off_x=u;
	if (tile_type(LEVEL[new_global_y][new_global_x])=='0') goto okay;
	/* nope, neither way worked.. reset x again */
	new_global_x=global_x;
	new_off_x=off_x;
	slide=0;
	
okay:
	
	/* adjust offset for ramps */
	if (cap_yd==0)
	{ 
		if (tile_type(LEVEL[new_global_y+1][new_global_x])=='2')
		{ 
			new_off_y=BGTILESIZE-new_off_x-1;
		}
		if (tile_type(LEVEL[new_global_y+1][new_global_x])=='3')
		{ 
			new_off_y=new_off_x-1;
		}
	}
	
	/* Shadow Updates */
	for (i=1; i<5; i++)
	{ 
		ShadowX[i-1]=ShadowX[i];
		ShadowY[i-1]=ShadowY[i];
	}
	ShadowX[4]=(new_global_x*BGTILESIZE)+new_off_x;
	ShadowY[4]=(new_global_y*BGTILESIZE)+new_off_y;
	
	/* firing */
	if ((PadStatus&Button[KEY_FIRE]))
	{ 
		FireCharge++;
		if (FireCharge>119) FireCharge=119;
		
		if ((slide==0)&&(firing<4)&&(FireCharge<3))
		{
			firing=7;
			i=0;
			while ((FireType[i]!=0)&&(i<5))
				i++;
			if (i<5)
			{
				switch (CapWeapon)
				{
				case PSHOOTER: 
					FireType[i]=PSHOOTER;
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 12 : (-12));
					FireYDir[i]=0;
					play_sample(S_FIRESMALL);
					break;
				case AIRGUN:
					for (j=0; j<3; j++)
					{
						i=0;
						while ((FireType[i]!=0)&&(i<5))
						   i++;
						if (i<5)
						{
							FireType[i]=AIRGUN;
							FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
							FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
							FireXDir[i]=(Sprites[1].scalex>0 ? 12-j*3 : (-12)+j*3);
							FireYDir[i]=-8;
						}
					}
					play_sample(S_FIREAIR);
					break;
				case EARTHGUN: 
					FireType[i]=EARTHGUN;
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 8 : (-8));
					FireYDir[i]=0;
					play_sample(S_FIREEARTH);
					break;
				case FIREGUN:  
					FireType[i]=FIREGUN;
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 12 : (-12));
					FireYDir[i]=0;
					play_sample(S_FIREFIRE);
					break;
				case HOLYGUN:  
					FireType[i]=HOLYGUN;
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 12 : (-12));
					FireYDir[i]=0;
					if (PadStatus&PADLup)
					{
						FireYDir[i]=-12;
						if ((!(PadStatus&PADLright))&&(!(PadStatus&PADLleft)))
							FireXDir[i]=0;
					}
					if (PadStatus&PADLdown)
					{
						FireYDir[i]=12;
						if ((!(PadStatus&PADLright))&&(!(PadStatus&PADLleft)))
							FireXDir[i]=0;
					}
					play_sample(S_FIREHOLY);
					break;
				case ICEGUN:   
					FireType[i]=ICEGUN;
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 12 : (-12));
					FireYDir[i]=0;
					play_sample(S_FIREICE);
					break;
				}               
			}
		}
	} else
	{
		if (FireCharge>30)
		{
			if (CapWeapon==PSHOOTER)
			{
				firing=7;
				i=0;
				while ((FireType[i]!=0)&&(i<5))
					i++;
				if (i<5)
				{
					FireType[i]=(FireCharge/30 < 3 ? MEDSHOT : FULLSHOT);
					FireX[i]=(new_global_x*BGTILESIZE)+new_off_x+(Sprites[1].scalex>0 ? 7 : (-7));
					FireY[i]=(new_global_y*BGTILESIZE)+new_off_y-5;
					FireXDir[i]=(Sprites[1].scalex>0 ? 12 : (-12));
					FireYDir[i]=0;
					play_sample(FireType[i]==MEDSHOT ? S_FIRESMALL : S_FIREMED);
				}
			}
		}
		FireCharge=0;
	}
	
	i=0;
	if (CapWeapon==1)
	{
		if ((FireCharge>30)&&(FireCharge<91)&&(FRAME%4))
		{
			cap_palette(MEDSHOT);
			play_sample(S_CHARGEMED);
			i=1;
		}
		if (FireCharge>90)
		{
			if (FRAME%3)
			{
				cap_palette(FULLSHOT);
				i=1;
			}
			if (FRAME%4)
				play_sample(S_CHARGEFULL);
		}
	}
	if (i==0) cap_palette(CapWeapon);
	
	move_shots();
	
	/* adjust screen */
	
	Sprites[1].x=((new_global_x * BGTILESIZE)+new_off_x)-FGScroll;
	Sprites[1].y=((new_global_y * BGTILESIZE)+new_off_y)-FGYScroll;
	
	if ((Sprites[1].x>160)&&(FGScroll<MAXSCROLL))
	{
		FGScroll+=Sprites[1].x-160;
		Sprites[1].x=160;
		if (FGScroll>MAXSCROLL)
		{
			Sprites[1].x+=FGScroll-MAXSCROLL;
			FGScroll=MAXSCROLL;
		}
	}
	
	if ((Sprites[1].x<160)&&(FGScroll>0))
	{
		FGScroll-=160-Sprites[1].x;
		Sprites[1].x=160;
		if (FGScroll<0)
		{
			Sprites[1].x+=FGScroll;
			FGScroll=0;
		}
	}
	
	/* screen Y adjust for going up or down*/
	if ((FGYScroll>0)&&(Sprites[1].y<128))
	{
		FGYScroll-=(128-Sprites[1].y);
		Sprites[1].y=128;
	}
	
	if (Sprites[1].y>194)
	{
		FGYScroll+=(Sprites[1].y-194);
		Sprites[1].y=194;
		if (FGYScroll>1568) FGYScroll=0;
	}
	
	/* set up Cap and draw screen */
	set_pattern(1,cap_pat+(firing>0));
	
	firing--;
	
	drawscreen();
}

if (PadStatus&PADselect)
{
	level++;
	if (level>11) level=1;
	goto replay;
}
}

void run_intro()
{ int EARTH,SHADOW,ASS,i;

/* layers */
SHADOW=1;
EARTH=2;
ASS=3;
BGON=0;
GALAXYON=0;

LoadFile("MISC\\CC_TITLE.TIM");
initTexture();

LoadFile("MISC\\INTRO__1.TIM");
initTexture();

LoadFile("MISC\\INTRO__2.TIM");
initTexture();

LoadFile("MISC\\INTRO__3.TIM");
initTexture();

clear_sprites();
clear_enemies();

sprite_dat[SHADOW].tx=0;
sprite_dat[SHADOW].ty=256;
sprite_dat[SHADOW].w=256;
sprite_dat[SHADOW].h=240;
sprite_dat[SHADOW].ox=0;
sprite_dat[SHADOW].oy=0;
sprite_dat[SHADOW].cx=512;
sprite_dat[SHADOW].cy=264;
initSprite(SHADOW,32,0,SHADOW);
Sprites[SHADOW].r=Sprites[SHADOW].g=Sprites[SHADOW].b=0;
Sprites[SHADOW].mx=Sprites[SHADOW].my=0;
Sprites[SHADOW].x=-1;  /* hide this sprite for now */

sprite_dat[EARTH].tx=768;
sprite_dat[EARTH].ty=256;
sprite_dat[EARTH].w=256;
sprite_dat[EARTH].h=240;
sprite_dat[EARTH].ox=0;
sprite_dat[EARTH].oy=0;
sprite_dat[EARTH].cx=512;
sprite_dat[EARTH].cy=265;
initSprite(EARTH,32,0,EARTH);
Sprites[EARTH].r=Sprites[EARTH].g=Sprites[EARTH].b=0;
Sprites[EARTH].mx=Sprites[EARTH].my=0;

sprite_dat[ASS].tx=320;
sprite_dat[ASS].ty=256;
sprite_dat[ASS].w=256;
sprite_dat[ASS].h=240;
sprite_dat[ASS].ox=0;
sprite_dat[ASS].oy=0;
sprite_dat[ASS].cx=512;
sprite_dat[ASS].cy=266;
initSprite(ASS,32,20,ASS);
Sprites[ASS].r=Sprites[ASS].g=Sprites[ASS].b=0;
Sprites[ASS].mx=Sprites[ASS].my=0;

FPSON=0;

PlayCD(NULL);

drawscreen();

Text_Y=220;

if (SlowText("It is the year XXXX...")) goto earlyout;

for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	Sprites[EARTH].r=Sprites[EARTH].g=Sprites[EARTH].b=i;
	drawscreen();
}

if (SlowText("The entire world...")) goto earlyout;
for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	drawscreen();
}

if (SlowText("is gripped in terror...")) goto earlyout;
for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	drawscreen();
}

if (SlowText("by the Overlord Shadowplay!")) goto earlyout;

Sprites[SHADOW].x=32;  /* show this sprite */

for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	Sprites[SHADOW].r=Sprites[SHADOW].g=Sprites[SHADOW].b=i;
	drawscreen();
}

if (SlowText("Any who defy him...")) goto earlyout;
for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	drawscreen();
}

if (SlowText("face his five associates!")) goto earlyout;
for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	Sprites[ASS].r=Sprites[ASS].g=Sprites[ASS].b=i;
	drawscreen();
}

if (SlowText("Only one can save us now...")) goto earlyout;
for (i=0; i<0x80; i+=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	drawscreen();
}

for (i=0x80; i>0; i-=2)
{
	PadStatus=PadRead(0);
	if (PadStatus&PADRdown)
	{
		while (PadStatus&PADRdown)
		{
			PadStatus=PadRead(0);
			VSync(0);
		}
		goto earlyout;
	}
	Sprites[ASS].r=Sprites[ASS].g=Sprites[ASS].b=i;
	Sprites[SHADOW].r=Sprites[SHADOW].g=Sprites[SHADOW].b=i;
	Sprites[EARTH].r=Sprites[EARTH].g=Sprites[EARTH].b=i;
	drawscreen();
}

PlayCD(CD_Title);

for (i=0; i<0x80; i+=2)
{
	drawscreen();
}

return;

earlyout:
PlayCD(CD_Title);

}

int SlowText(char *s)
{
	int l;
	
	PadStatus=0;
	
	strcpy(Text_String,"");
	
	Text_X=(320-(strlen(s)*10))/2;
	
	while (*s)
	{
		l=strlen(Text_String);
		Text_String[l]=*(s++);
		Text_String[l+1]=0;
		drawscreen();
		drawscreen();
		PadStatus=PadRead(0);
		if (PadStatus&PADRdown)
		{
			while (PadStatus&PADRdown)
			{
				PadStatus=PadRead(0);
				VSync(0);
			}
			return(1);
		}
	}
	return(0);
}

void do_title()
{
	/* draw title - loaded by intro. Return GAME_TYPE */
	int new,j,i;
	int timeout;
	
	timeout=1800;
	
	clear_sprites();
	
	sprite_dat[1].tx=640;
	sprite_dat[1].ty=0;
	sprite_dat[1].w=256;
	sprite_dat[1].h=240;
	sprite_dat[1].ox=0;
	sprite_dat[1].oy=0;
	sprite_dat[1].cx=512;
	sprite_dat[1].cy=260;
	initSprite(1,0,0,1);
	Sprites[1].attribute = (2<<24);
	Sprites[1].mx=Sprites[1].my=0;
	
	sprite_dat[2].tx=896;
	sprite_dat[2].ty=0;
	sprite_dat[2].w=64;
	sprite_dat[2].h=240;
	sprite_dat[2].ox=0;
	sprite_dat[2].oy=0;
	sprite_dat[2].cx=512;
	sprite_dat[2].cy=261;
	initSprite(2,256,0,2);
	Sprites[2].attribute = (2<<24);
	Sprites[2].mx=Sprites[2].my=0;
	
	GAME_TYPE=0;
	
	Text_X=180;
	Text_Y=200;
	strcpy(Text_String,&menu[GAME_TYPE][0]);
	
	PadStatus=0;
	while ((!(PadStatus&PADRdown))&&(!(PadStatus&PADstart))&&(timeout>0))
	{
		rand();
		drawscreen();
		timeout--;
		PadStatus=PadRead(0);
		new=-1;
		if (PadStatus&PADLright)
		{
			timeout=1800;
			new=GAME_TYPE+1;
			if (new>3) new=0;
			play_sample(S_CLICK);
			for (i=0; i<14; i++)
			{
				for (j=1; j<14; j++)
					Text_String[j-1]=Text_String[j];
				Text_String[--j]=menu[new][i];
				drawscreen();
			}
			GAME_TYPE=new;
		}
		if (PadStatus&PADLleft)
		{
			timeout=1800;
			new=GAME_TYPE-1;
			if (new<0) new=3; 
			play_sample(S_CLICK);
			for (i=13; i>=0; i--)
			{
				for (j=14; j>0; j--)
					Text_String[j]=Text_String[j-1];
				Text_String[0]=menu[new][i];
				drawscreen();
			}
			GAME_TYPE=new;
		}
	}
	if (timeout==0) GAME_TYPE=-1;
	play_sample(S_BOOM);
}

void text_row(char s[],int x, int y, int n, int col)
{
	int i;
	/* registers a centered row of text as a line of sprites */
	/* not used by the Text_String - that's coded into drawscreen() */
	i=0;
	x=x-(strlen(s)*5);
	while (s[i])
	{
		Sprites[n+i].attribute=(0<<24); /* 4 bit */
		Sprites[n+i].w=10;
		Sprites[n+i].h=20;
		Sprites[n+i].x=x+i*10;
		Sprites[n+i].y=y;
		Sprites[n+i].tpage=GetTPage(0,0,960,256);
		Sprites[n+i].u=((s[i]-32)%25)*10;
		Sprites[n+i].v=((s[i]-32)/25)*20;
		Sprites[n+i].cx=512;
		Sprites[n+i].cy=262;
		Sprites[n+i].mx=0;
		Sprites[n+i].my=0;
		Sprites[n+i].scalex=ONE;
		Sprites[n+i].scaley=ONE;
		Sprites[n+i].rotate=0;
		if (col&1) Sprites[n+i].r=0x80;
		else Sprites[n+i].r=0;
		if (col&2) Sprites[n+i].g=0x80;
		else Sprites[n+i].g=0;
		if (col&4) Sprites[n+i].b=0x80;
		else Sprites[n+i].b=0;
		i++;
	}
} 

void do_options()
{
	int option;
	char buf[80];
	char key[5]="}{XO";
	int last=0;
	int TEST_NUM=1;
	
	strcpy(Text_String,"");
	
	/* fade title pic */
	Sprites[1].r=Sprites[1].g=Sprites[1].b=0x20; 
	Sprites[2].r=Sprites[2].g=Sprites[2].b=0x20;
	
	text_row("Options",160,10,5,C_CYAN);
	
	option=0;
	
	drawscreen();
	
	while (PadStatus&PADRdown)
	{
		PadStatus=PadRead(0);
		VSync(0);
	}
	
loop:
	sprintf(buf,"Jump...%c",key[KEY_JUMP]);
	text_row(buf,160,50,12,(option==0 ? C_WHITE : C_RED));
	
	sprintf(buf,"Fire...%c",key[KEY_FIRE]);
	text_row(buf,160,70,20,(option==1 ? C_WHITE : C_RED));
	
	sprintf(buf,"Slide..%c",key[KEY_SLIDE]);
	text_row(buf,160,90,28,(option==2 ? C_WHITE : C_RED));
	
	sprintf(buf,"Music Volume...%2d",MUSIC_VOL);
	text_row(buf,160,130,36,(option==3 ? C_WHITE : C_RED));
	
	sprintf(buf,"Effect Volume...%2d",EFFECT_VOL);
	text_row(buf,160,150,53,(option==4 ? C_WHITE : C_RED));
	
	sprintf(buf,"Sound Test...%2d",TEST_NUM);
	text_row(buf,160,170,71,(option==5 ? C_WHITE : C_RED));
	
	text_row("Exit",160,210,86,(option==6 ? C_WHITE : C_RED));
	
	drawscreen();
	
	if (last)
	{
		while (PadRead(0)&last)
			VSync(0);
	}
	
	last=0;
	PadStatus=PadRead(0);
	
	if (PadStatus&PADLup)
	{
		last=PADLup;
		play_sample(S_CLICK);
		option--;
		if (option<0) option=6;
	}
	
	if (PadStatus&PADLdown)
	{
		last=PADLdown;
		play_sample(S_CLICK);
		option++;
		if (option>6) option=0;
	}
	
	if (last) goto loop;
	
	switch (option)
	{
	case 0:
		if (PadStatus&PADRup)
		{
			last=PADRup;
			play_sample(S_CLICK);
			KEY_JUMP=0;
		}
		if (PadStatus&PADRleft)
		{
			last=PADRleft;
			play_sample(S_CLICK);
			KEY_JUMP=1;
		}
		if (PadStatus&PADRdown)
		{
			last=PADRdown;
			play_sample(S_CLICK);
			KEY_JUMP=2;
		}
		if (PadStatus&PADRright)
		{
			last=PADRright;
			play_sample(S_CLICK);
			KEY_JUMP=3;
		}
		break;
	case 1:
		if (PadStatus&PADRup)
		{
			last=PADRup;
			play_sample(S_CLICK);
			KEY_FIRE=0;
		}
		if (PadStatus&PADRleft)
		{
			last=PADRleft;
			play_sample(S_CLICK);
			KEY_FIRE=1;
		}
		if (PadStatus&PADRdown)
		{
			last=PADRdown;
			play_sample(S_CLICK);
			KEY_FIRE=2;
		}
		if (PadStatus&PADRright)
		{
			last=PADRright;
			play_sample(S_CLICK);
			KEY_FIRE=3;
		}
		break;
	case 2:
		if (PadStatus&PADRup)
		{
			last=PADRup;
			play_sample(S_CLICK);
			KEY_SLIDE=0;
		}
		if (PadStatus&PADRleft)
		{
			last=PADRleft;
			play_sample(S_CLICK);
			KEY_SLIDE=1;
		}
		if (PadStatus&PADRdown)
		{
			last=PADRdown;
			play_sample(S_CLICK);
			KEY_SLIDE=2;
		}
		if (PadStatus&PADRright)
		{
			last=PADRright;
			play_sample(S_CLICK);
			KEY_SLIDE=3;
		}
		break;
	case 3:
		if (PadStatus&PADLleft)
		{
			last=PADLleft;
			play_sample(S_CLICK);
			MUSIC_VOL--;
			if (MUSIC_VOL<0) MUSIC_VOL=0;
		}
		if (PadStatus&PADLright)
		{
			last=PADLright;
			play_sample(S_CLICK);
			MUSIC_VOL++;
			if (MUSIC_VOL>10) MUSIC_VOL=10;
		}
		SsSetSerialVol(SS_SERIAL_A,MUSIC_VOL*12,MUSIC_VOL*12);
		break;
	case 4:
		if (PadStatus&PADLleft)
		{
			last=PADLleft;
			EFFECT_VOL--;
			if (EFFECT_VOL<0) EFFECT_VOL=0;
			play_sample(S_CLICK);
		}
		if (PadStatus&PADLright)
		{
			last=PADLright;
			EFFECT_VOL++;
			if (EFFECT_VOL>10) EFFECT_VOL=10;
			play_sample(S_CLICK);
		}
		break;
	case 5:
		if (PadStatus&PADLleft)
		{
			last=PADLleft;
			play_sample(S_CLICK);
			TEST_NUM--;
			if (TEST_NUM<1) TEST_NUM=1;
		}
		if (PadStatus&PADLright)
		{
			last=PADLright;
			play_sample(S_CLICK);
			TEST_NUM++;
			if (TEST_NUM>25) TEST_NUM=25;
		}
		if (PadStatus&PADRdown)
		{
			last=PADRdown;
			play_sample(TEST_NUM);
		}
		break;
	case 6:
		if ((PadStatus&PADRdown)||(PadStatus&PADstart))
		{
			last=99;
			play_sample(S_BOOM);
		}
}
if (last!=99) goto loop;
}

void start_level(int level)
{
	/* load appropriate files and set flags */
	int i,tp,colorMode;
	
	clear_sprites();
	clear_enemies();
	BGON=1;
	FGScroll=0;
	HELLON=0;
	GALAXYON=0;
	MAXSCROLL=3776;  /* default */
	SHADOWON=0;
	PlayCD(NULL);

	/* TODO - load enemies and level data */
	
	switch (level)
	{
	case 1: /* city street */
		LoadFile("LEVEL1\\CITYBACK.TIM");
		initTexture();
		LoadFile("LEVEL1\\CITYTILE.TIM");
		initTexture();
		LoadFile("LEVEL1\\ENEMY209.TIM");
		initTexture();
		LoadFile("LEVEL1\\LEVEL1.DAT");
		bcopy((char*)LOADBUFFER,(char*)LEVEL,LEVEL_SIZE);
		LoadFile("LEVEL1\\LEVEL1.TIL");
		bcopy((char*)LOADBUFFER,(char*)tiletype,64);
		MAXSCROLL=0;
		InitEnemy( 0, ENEMY_209_GUN, 1, 1, 385, 155, 98, 20, 4, 3, 100, FULLSHOT, NOWEAPON );
		Enemy[0].Sprite.mx=65;
		Enemy[0].Sprite.my=10;
		Enemy[0].fFast=0;
		InitEnemy( 1, ENEMY_209_LEG, 1, 1, 365, 156, 70, 54, 16, 64, 1, NOWEAPON, ALLWEAPON);
		Enemy[1].Sprite.mx=35;
		Enemy[1].fFast=0;
		InitEnemy( 2, ENEMY_209_LEG, 1, 1, 357, 150, 70, 54, 16, 64, 1, NOWEAPON, ALLWEAPON);
		Enemy[2].Sprite.mx=35;
		Enemy[2].fFast=0;
		InitEnemy( 3, ENEMY_209_BODY, 1, 1, 335, 135, 75, 34, 8, 25, 100, FULLSHOT, NOWEAPON);
		InitEnemy( 4, ENEMY_209_GUN, 1, 1, 387, 157, 98, 20, 4, 3, 100, FULLSHOT, NOWEAPON);
		Enemy[4].Sprite.mx=65;
		Enemy[4].Sprite.my=10;
		Enemy[4].fFast=0;
		PlayCD(CD_Boss);
		break;
		
	case 2: /* desert */
		LoadFile("LEVEL2\\DESERT_1.TIM");
		initTexture();
		LoadFile("LEVEL2\\LEVEL2.DAT");
		bcopy((char*)LOADBUFFER,(char*)LEVEL,LEVEL_SIZE);
		LoadFile("LEVEL2\\DES_TILE.TIM");
		initTexture();
		LoadFile("LEVEL2\\LEVEL2.TIL");
		bcopy((char*)LOADBUFFER,(char*)tiletype,64);
		PlayCD(CD_Desert);
		break;
		
	case 3: /* Winter */
		LoadFile("LEVEL3\\WINTER_1.TIM");
		initTexture();
		PlayCD(CD_Winter);
		break;
		
	case 4: /* Wooded */
		LoadFile("LEVEL4\\FOREST_1.TIM");
		initTexture();
		PlayCD(CD_Wooded);
		break;
		
	case 5: /* Hell */
		LoadFile("LEVEL5\\HELL_B_1.TIM");
		initTexture();
		PlayCD(CD_Hell);
		HELLON=1;
		break;
		
	case 6: /* Tribulation */
		LoadFile("LEVEL6\\TRIB_B_1.TIM");
		initTexture();
		PlayCD(CD_Trib);
		break;
		
	case 7: /* Castle 1 */
		/* first show the castle intro */
		castle_view();
		
		BGON=1;
		LoadFile("LEVEL7\\CASTLE1.TIM");
		initTexture();
		PlayCD(CD_Castle1);
		break;
		
	case 8: /* Castle 2 */
		LoadFile("LEVEL8\\CASTLE2.TIM");
		initTexture();
		PlayCD(CD_Castle2);
		break;
		
	case 9: /* Teleport Room */
		LoadFile("LEVEL9\\COMPUT_1.TIM");
		initTexture();
		PlayCD(CD_Castle3);
		MAXSCROLL=0;
		break;
		
	case 10: /* War Machine */
		LoadFile("LEVEL10\\WAR_MA_1.TIM");
		initTexture();
		LoadFile("LEVEL10\\GALAXY.TIM");
		initTexture();
		PlayCD(CD_Boss);
		MAXSCROLL=0;
		GALAXYON=1;
		
		tp=GetTPage(1,0,768,256);
		colorMode = 1;  /* 8 bit CLUT */
		Galaxy.attribute = (colorMode<<24|1<<28|1<<30);
		Galaxy.w = 128;
		Galaxy.h = 128;
		Galaxy.x = 160;
		Galaxy.y = 120;
		Galaxy.mx = 64;       /* center f. rotation/scaling */
		Galaxy.my = 64;
		Galaxy.tpage = tp;
		Galaxy.u = 0;          /* offset of the sprite INSIDE */
		Galaxy.v = 0;          /* the 255X255 pixel TIM picture */
		Galaxy.cx = 512;                        /* color table x,y */
		Galaxy.cy = 263;
		Galaxy.r = 0x80;              /* colour adjust */
		Galaxy.g = 0x80;
		Galaxy.b = 0x80;
		Galaxy.rotate = 0;
		Galaxy.scalex = ONE*2;
		Galaxy.scaley = ONE*2;
		for (i=0; i<20; i++)
		{
			Stars[i].attribute=0;
			Stars[i].x0=Stars[i].x1=rand()%320;
			Stars[i].y0=Stars[i].y1=rand()%120;
			Stars[i].r=Stars[i].b=Stars[i].g=0x90;
		}
		break;
		
	case 11: /* Final */
		LoadFile("LEVEL11\\FINAL.TIM");
		initTexture();
		MAXSCROLL=704;
		break;
}
}

void play_sample(int sam)
{
	int a=0,
		b=52,
		c=0,
		temp;
	
	temp=EFFECT_VOL*12;
	SsUtKeyOn(VAB_ID,sam-1,a,b,c,temp,temp);
}

void initForeground()
{
	int x,y,i,colorMode;
	
	i=0;
	for (x=0; x<128; x++)
		for (y=0; y<64; y++)
		{
			colorMode=1;
			FG[i].x=-1;
			if ((LEVEL[y][x]>='A')&&(LEVEL[y][x]<=130)&&(i<FGNUMTILES-1))
			{
				FG[i].attribute = (colorMode<<24);
				FG[i].w = BGTILESIZE;
				FG[i].h = BGTILESIZE;
				FG[i].x = (x*BGTILESIZE)+SCREEN_X_OFFSET;
				FG[i].y = (y*BGTILESIZE)+SCREEN_Y_OFFSET;
				FG[i].mx = BGTILESIZE/2;        /* center f. rotation/scaling */
				FG[i].my = BGTILESIZE/2;
				FG[i].tpage = GetTPage(1,0,896,0);
				FG[i].u=((LEVEL[y][x]-'A')%8)*BGTILESIZE; /* offset of the sprite INSIDE */
				FG[i].v=((LEVEL[y][x]-'A')/8)*BGTILESIZE; /* the 255X255 pixel TIM picture */
				FG[i].cx = 512;                 /* color table x,y */
				FG[i].cy = 258;
				FG[i].r = 0x80;                 /* colour adjust */
				FG[i].g = 0x80;
				FG[i].b = 0x80;
				FG[i].rotate = 0;
				FG[i].scalex = ONE;
				FG[i].scaley = ONE;
				i++;
			}
		}
		printf("%d tiles used in foreground\n",i);
}

int tile_type(int t)
{
	if (t==0) return('0');
	return(tiletype[t-'A']);
}

void start_cap()
{
	int i,j,t;
	
	/* display READY */
	Text_X=135;
	Text_Y=110;
	strcpy(Text_String,"READY");
	
	t=FPSON;
	FPSON=0;
	for (j=0; j<5; j++)
	{
		Text_String[0]='R';
		for (i=0; i<10; i++)
			drawscreen();
		Text_String[0]=0;
		for (i=0; i<5; i++)
			drawscreen();
	}
	FPSON=t;
	
	/* Cap warps in */
	play_sample(S_WARP);
	Sprites[1].scalex=512;
	for (i=-14; i<194; i+=40)
	{
		Sprites[1].y=i;
		drawscreen();
	}
	Sprites[1].y=194;
	
	for (i=512; i<ONE; i+=1536)
	{
		Sprites[1].scalex=i;
		drawscreen();
	}
	Sprites[1].scalex=ONE;
}

void move_shots()
{
	/* move player bullets */
	int i,i2,x,y, fl, tx, ty;
	
	for (i=0; i<5; i++)
	{
		
		FireX[i]+=FireXDir[i];
		FireY[i]+=FireYDir[i];
		x=FireX[i]-FGScroll;
		y=FireY[i]-FGYScroll;
		if ((x>SCREEN_WIDTH)||(x<0)||(y>SCREEN_HEIGHT)||(y<0))
			FireType[i]=0; /* off screen */
		if (tile_type(LEVEL[FireY[i]/BGTILESIZE+1][FireX[i]/BGTILESIZE])=='1')
			FireType[i]=0; /* hit wall */

		// check enemy collisions
		fl=0;
		for (i2=0; i2<50; i2++)
		{
			tx=Enemy[i2].gx-Enemy[i2].Sprite.mx;
			ty=Enemy[i2].gy-Enemy[i2].Sprite.my;

			if ((Enemy[i2].dead==0) && (FireX[i]-tx<Enemy[i2].Sprite.w/2-32)&&(FireX[i]>tx) &&
				(FireY[i]-ty<Enemy[i2].Sprite.h) && (FireY[i]>ty-8))
			{
				if ((FireType[i] == Enemy[i2].immune) || (Enemy[i2].immune==ALLWEAPON))
				{
					play_sample(S_NODAMAGE);
				}
				else
				{
					play_sample(S_DAMAGE);
					Enemy[i2].Sprite.r=0xff;
					Enemy[i2].Sprite.g=0xff;
					Enemy[i2].Sprite.b=0xff;
					Enemy[i2].health-=1;
					if (FireType[i]==MEDSHOT)
					{
						Enemy[i2].health-=1;
					}
					if (FireType[i]==FULLSHOT)
					{
						Enemy[i2].health-=2;
					}
					if (Enemy[i2].weakness == FireType[i])
					{
						Enemy[i2].health-=9;
					}
				}
				fl=1;
				if (Enemy[i2].health < 0)
				{
					Enemy[i2].dead=1;
					if (Enemy[i2].iType<=ENEMY_209_LEG)
					{
						while ((i2>=0)&&(Enemy[i2].iType<=ENEMY_209_LEG)&&(Enemy[i2].iType>0))
						{
							Enemy[i2--].dead=1;
						}
						while ((Enemy[i2].iType<=ENEMY_209_LEG)&&(Enemy[i2].iType>0))
						{
							Enemy[i2++].dead=1;
						}
					}
				}
			}
		}

		if (fl)
		{
			FireType[i]=0;
		}

		switch (FireType[i])
		{
		case PSHOOTER: 
			initSprite(i+2,x,y,29);
			set_flip(i+2,FireXDir[i]);
			break;
		case MEDSHOT: 
			initSprite(i+2,x,y,30);
			set_flip(i+2,FireXDir[i]);
			break;
		case FULLSHOT: 
			initSprite(i+2,x,y,31);
			set_flip(i+2,FireXDir[i]);
			break;
		case AIRGUN: 
			initSprite(i+2,x,y,56); 
			set_flip(i+2,FireXDir[i]);
			break;
		case EARTHGUN: 
			initSprite(i+2,x,y,58);
			set_flip(i+2,FireXDir[i]);
			Sprites[i+2].rotate=(FRAME%60)*6*ONE;
			if (FireXDir[i+2]<0) Sprites[i+2].rotate=360-Sprites[i+2].rotate;
			break;
		case FIREGUN: 
			initSprite(i+2,x,y,59);
			set_flip(i+2,FireXDir[i]);
			break;
		case HOLYGUN: 
			initSprite(i+2,x,y,61);
			set_flip(i+2,FireXDir[i]);
			Sprites[i+2].rotate=(FRAME%30)*12*ONE;
			break;
		case ICEGUN: 
			initSprite(i+2,x,y,57);
			set_flip(i+2,FireXDir[i]);
			break;
			
		default: 
			FireType[i]=0;
			Sprites[i+2].x=-1; /* turn it off */
			break;
		}
	}
}

void cap_palette(int n)
{
	/* change Cap's palette, copy the appropriate one in */
	RECT myrect;
	int i,sgn;
	
	myrect.x=512;
	myrect.y=n+267;
	myrect.w=256;
	myrect.h=1;
	MoveImage(&myrect,512,257);
	
}

void do_help()
{
	
	strcpy(Text_String,"");
	
	/* fade title pic */
	Sprites[1].r=Sprites[1].g=Sprites[1].b=0x20; 
	Sprites[2].r=Sprites[2].g=Sprites[2].b=0x20;
	
	text_row("Captain Communism",160,70,5,C_CYAN);
	
	drawscreen();
	
	while (PadStatus&PADRdown)
	{
		PadStatus=PadRead(0);
		VSync(0);
	}
	
	text_row("Character by Steve Brent",160,90,22,C_WHITE);
	text_row("Code/Some Gfx: Tursi",160,110,46,C_WHITE);
	text_row("Gfx/Animation: Foxxfire",160,130,67,C_WHITE);
	
	while (!(PadStatus & PADRdown))
	{
		
		drawscreen();
		
		PadStatus=PadRead(0);
	}
	
	while ((PadStatus & PADRdown))
	{
		
		drawscreen();
		
		PadStatus=PadRead(0);
	}
	
}

void clear_enemies()
{
	int i;

	/* initialize the enemies */
	for (i=0; i<50; i++)
	{
		Enemy[i].iType=0;
	}
}

void InitEnemy(int num, int type, int priority, int fast, int x, int y, int width, int height, int px, int py, int health, int weakness, int immune)
{
	int tmp;

	px+=768;
	//py+=0;

	Enemy[num].iType=type;
	Enemy[num].iPriority=priority;
	Enemy[num].gx=x;
	Enemy[num].gy=y;
	Enemy[num].iStatus=0;
	Enemy[num].fFast=fast;
	Enemy[num].dx=0;
	Enemy[num].dy=0;
	Enemy[num].health=health;
	Enemy[num].weakness=weakness;
	Enemy[num].immune=immune;
	Enemy[num].dead=0;
	Enemy[num].Sprite.attribute=(1<<24);		// 8 bit
	Enemy[num].Sprite.x=250;
	Enemy[num].Sprite.y=0;
	Enemy[num].Sprite.w=width;
	Enemy[num].Sprite.h=height;
	Enemy[num].Sprite.tpage=GetTPage(1, 0, px, py);
	Enemy[num].Sprite.u=px%64;
	Enemy[num].Sprite.v=py%256;
	Enemy[num].Sprite.cx=512;
	Enemy[num].Sprite.cy=259;
	Enemy[num].Sprite.r=0x80;
	Enemy[num].Sprite.g=0x80;
	Enemy[num].Sprite.b=0x80;
	if (fast)
	{
		Enemy[num].Sprite.mx=0;
		Enemy[num].Sprite.my=0;
	}
	else
	{
		Enemy[num].Sprite.mx=width/2;
		Enemy[num].Sprite.my=height/2;
	}
	Enemy[num].Sprite.scalex=ONE;
	Enemy[num].Sprite.scaley=ONE;
	Enemy[num].Sprite.rotate=0;
}

void Move209(int num)
{
	// update the position/animation for a 209 enemy
	// a 209 uses 5 enemy slots starting at 'num'
	int i, i2;
	int gflag;
	int fflag;

	// as a pretty dumb machine, they just advance to the left while
	// their bits move around
	if (FRAME%3 != 0)
		return;

	gflag=0;
	fflag=0;
	for (i=num; i<num+5; i++)
	{
		Enemy[i].iStatus++;
		if (Enemy[i].iStatus > 39)
			Enemy[i].iStatus=0;

		switch (Enemy[i].iType)
		{
		case ENEMY_209_GUN:
			if (gflag)
			{
				i2=9;
			}
			else
			{
				i2=6;
				gflag=1;
			}
			break;
		case ENEMY_209_LEG:
			if (fflag)
			{
				i2=3;
			}
			else
			{
				i2=0;
				fflag=1;
			}
			break;
		case ENEMY_209_BODY:
			i2=12;
			break;
		}

		Enemy[i].gx+=Walk209[i2][Enemy[i].iStatus];
		Enemy[i].gy+=Walk209[i2+1][Enemy[i].iStatus];
		if (ENEMY_209_BODY != Enemy[i].iType)
		{
			Enemy[i].Sprite.rotate=Walk209[i2+2][Enemy[i].iStatus]*ONE;
		}
	}
}

void do_enemies()
{
	int idx;

	for (idx=0; idx<50; idx++)
	{
		if (Enemy[idx].iType>0)
		{
			switch (Enemy[idx].iType)
			{
			case ENEMY_209_GUN:
			case ENEMY_209_LEG:
			case ENEMY_209_BODY:
				Move209(idx);
				idx+=5;
				break;
			}
		}
	}
}
