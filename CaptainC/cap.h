/* Captain C header file */

/* preprocessor defines */

#define FILE                    int
#define PACKETMAX               (10000)
#define PACKETMAX2              (PACKETMAX*24)
#define OT_LENGTH               (3)      
#define LOADBUFFER              0x80080000
#define LEVEL_SIZE              8192
#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240      
#define SCREEN_X_OFFSET         (0)
#define SCREEN_Y_OFFSET         (-14)
#define blue(x)                 (x<<10)
#define green(x)                (x)
#define red(x)                  (x<<5)
#define TRANS                   (0x8000)
#define C_RED                   0x1
#define C_BLUE                  0x2
#define C_GREEN                 0x4
#define C_MAGENTA               0x3
#define C_CYAN                  0x6
#define C_YELLOW                0x5
#define C_WHITE                 0x7
#define BGTILESIZE              32
#define BGNUMTILES              512
#define FGNUMTILES              3210
#define SPRITEMAX               100
#define BGSCROLL                ((FGScroll*1000)/5364)

/* function prototypes */

void setup(void);
void initSprite(int,int,int,int);
void PlayCD(int);
void main(void);
void drawscreen(void);
void LoadFile(char *);
void initTexture(void);
void castle_view(void);
void clear_sprites(void);
void scroll_run(void);
void run_intro(void);
int  SlowText(char*);
void do_title(void);
void do_options(void);
void text_row(char *,int,int,int,int);
void set_pattern(int,int);
void set_flip(int,int);
void start_level(int);
void play_sample(int);
void initForeground(void);
int tile_type(int);
void start_cap(void);
void move_shots(void);
void cap_palette(int);
void do_help(void);
void clear_enemies(void);
void InitEnemy(int num, int type, int priority, int fast, int x, int y, int width, int height, int px, int py, int health, int weakness, int immune);
void Move209(int num);
void do_enemies();

/* global variables */

GsOT            WorldOrderingTable[2];

GsOT_TAG        zSortTable[2][1<<OT_LENGTH];

GsSPRITE        BG[BGNUMTILES];
GsSPRITE        FG[FGNUMTILES];
GsSPRITE        Galaxy;
GsSPRITE        Sprites[SPRITEMAX];

GsLINE          Stars[20];
GsGLINE         Lines[25];
int             Line_Angle[25];

PACKET          GpuOutputPacket[2][PACKETMAX2];

struct ENEMY {
	GsSPRITE Sprite;
	int iType;
	int iStatus;
	int iPriority;
	int fFast;
	int gx, gy;
	int dx, dy;
	int health;
	int weakness;
	int immune;
	int dead;
} Enemy[50];

volatile u_char *bb0, *bb1;

u_long          PadStatus;

int             buff, frame, CD_ID, current_tune;
int             FGScroll,BGON,HELLON,FPSON,GAME_TYPE;
int             FGYScroll;
int             KEY_JUMP,KEY_FIRE,KEY_SLIDE,FRAME,MAXSCROLL;
int             MUSIC_VOL,EFFECT_VOL;
int             GALAXYON;
int             Text_X, Text_Y;
int             Spritepri[SPRITEMAX];
int             SHADOWON;
int             ShadowX[5],ShadowY[5];
int             FIREON;
int             LINESON;
int             FireX[5],FireY[5],FireType[5],FireXDir[5],FireYDir[5];
int             FireCharge;
int             CapWeapon;

RECT            myrect;

short           VAB_ID;

unsigned char   LEVEL[64][128];
unsigned char   tiletype[64];
char            VH[15000];
char            Text_String[80];

short           VAB_ID;

/* structures */

struct
{ int w,h,tx,ty,ox,oy,cx,cy; }         sprite_dat[SPRITEMAX];

/* tables */

int cd_tracks[12][2]={5,0,6,0,7,0,8,0,9,0,10,0,11,0,12,0,13,0,14,0,15,0,16,0};

int SINE[20]={0,1,2,2,1,0,-1,-2,-2,-1,0,1,2,2,1,0,-1,-2,-2,-1};

char menu[5][15]={"  Start Game  ",
"   Options    ",
"  Load Game   ",
"   Credits    ",
""
};

int Button[4]={ PADRup,PADRleft,PADRdown,PADRright };

int Walk209[14][40]= {
	{ 0,0,0,0,0,			0,0,0,0,0,			-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,	0,0,0,0,0,		0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0 },			// leg1 X
	{ 0,0,0,0,0,			-2,-2,-2,-2,-2,		0,0,0,0,0,0,0,0,0,0,			2,2,2,2,2,		0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0 },			// leg1 Y
	{ 0,0,0,0,0,			0,0,0,0,0,			3,6,9,12,15,18,21,24,27,30,		24,18,12,6,0,	0,0,0,0,0,		0,0,0,0,0,0,0,0,0,0 },			// leg1 Rot
	{ 0,0,0,0,0,			0,0,0,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,0,0,0,		0,0,0,0,0,		-2,-2,-2,-2,-2,-2,-2,-2,-2,-2 },// leg2 X
	{ 2,2,2,2,2,			0,0,0,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,0,0,0,		-2,-2,-2,-2,-2,	0,0,0,0,0,0,0,0,0,0 },			// leg2 Y
	{ 24,18,12,6,0,			0,0,0,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,0,0,0,		0,0,0,0,0,		3,6,9,12,15,18,21,24,27,30 },	// leg2 Rot
	{ 0,0,0,0,0,			0,0,0,0,0,			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,	0,0,0,0,0,		0,0,0,0,0,		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },// Cannon1 X
	{ 0,0,0,0,-1,			1,1,1,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,-1,-1,-1,	1,0,0,0,0,		0,0,0,0,0,0,0,0,0,0	},			// Cannon1 Y
	{ 0,0,0,0,0,			0,0,0,0,0,			-1,-2,-3,-4,-5,-6,-7,-8,-9,		-10-10,-10,-10,-10,-10,	-10,-10,-10,-10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0},// Cannon1 Rot
	{ 0,0,0,0,0,			0,0,0,0,0,			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,	0,0,0,0,0,		0,0,0,0,0,		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },// Cannon2 X
	{ 0,0,-1,-1,-1,			1,0,0,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,0,0,-1,		1,1,1,0,0,		0,0,0,0,0,0,0,0,0,0 },			// Cannon2 Y
	{ -10,-10,-10,-10,-10,	-10,-10,-10,-10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,	0,0,0,0,0,		0,0,0,0,0,		-1,-2,-3,-4,-5,-6,-7,-8,-9,-10},// Cannon2 Rot
	{ 0,0,0,0,0,			0,0,0,0,0,			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,	0,0,0,0,0,		0,0,0,0,0,		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },// Body
	{ 0,0,0,-1,-1,			1,1,0,0,0,			0,0,0,0,0,0,0,0,0,0,			0,0,0,-1,-1,	1,1,0,0,0,		0,0,0,0,0,0,0,0,0,0 }			// Body Y
};


