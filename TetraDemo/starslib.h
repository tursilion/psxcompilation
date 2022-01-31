// Starfield lib header file.

#include <libps.h>

#define STAR_U 0
#define STAR_D 2
#define STAR_R 1
#define STAR_L 3
#define STAR_OUT 4
#define STAR_IN 5

#define MAX_STARS 200
#define MAX_FIELDS 4

typedef struct {
  int x, y;
  int vel;
  int depth;
} star_points;

extern GsOT *TheOT;


void star_field(int cnt, int dir, int x, int y, int h, int w);
void ani_stars(void);
void new_stardir(int dir);
void new_starspeed(int sp);
