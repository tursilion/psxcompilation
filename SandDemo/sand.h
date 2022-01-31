/* global variables */

#define ONE 4096

#define PARTICLES (935)

#define OT_LENGTH (3)
#define PACKETMAX (10000)
#define PACKETMAX2 (PACKETMAX * 24)

GsOT            WorldOrderingTable[2];
GsOT_TAG        zSortTable[2][1<<OT_LENGTH];
GsLINE          Sand[PARTICLES];
PACKET          GpuOutputPacket[2][PACKETMAX2];
volatile u_char *bb0, *bb1;
u_long          PadStatus;
int             CHANGETIME;
int             cur_par;
int             COSCOUNT;
int             effect;
int             timer;

struct Sdest
{ int x, y, r, g, b; }
dest[PARTICLES];

#define SPRITEMAX 30             /* This should be more than enough */     
GsSPRITE Sprites[SPRITEMAX];           

char *msg;

void setup();
void drawscreen();
void main();
inline int move(int x);
void initTexture(u_long *addr);
void initsprites();
void textmove();
void dna();
void sandstorm();
void cloud();
void all();
void circle();
void palOrNtsc();
