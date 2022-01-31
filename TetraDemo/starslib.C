/*
Starfield Lib

Coded by Jeff Lawton (aka Zark Wizard)
Group Nemesis   - May 29 - 1998

Visit our home site at : http://yaroze-world.org
*/

#include "starslib.h"

static star_points star_pnts[MAX_STARS];
static GsLINE starfield;
static int star_dir, star_cnt, star_x, star_y, star_w, star_h, star_on;  // Area of current star field.
static int star_mult=1;

GsOT *TheOT;

/////////////////////////////////////////////////////////////
// Starfield Stuff
/////////////////////////////////////////////////////////////

void star_field(int cnt, int dir, int x, int y, int h, int w) {
  int i;

  if (cnt > MAX_STARS) 
    cnt = MAX_STARS;

  star_cnt = cnt;
  star_x = x;  
  star_y = y;  
  star_w = w;  
  star_h = h;
  star_dir = dir;  
  for (i = 0; i < cnt; i++) {
    star_pnts[i].x  = x + (rand()%(w - x));
    star_pnts[i].y  = y + (rand()%(h - y));

    starfield.attribute = 0;
    switch(rand()%MAX_FIELDS) {
      // slowest -> fastest
      case      0:      star_pnts[i].vel = 1 * star_mult;
          			star_pnts[i].depth = 62;
                break;
      case      1:      star_pnts[i].vel = 2 * star_mult;
           			star_pnts[i].depth = 128;
                break;
      case      2:      star_pnts[i].vel = 3 * star_mult;
           			star_pnts[i].depth = 196;
                break;
      case      3:      star_pnts[i].vel = 4 * star_mult;
          			star_pnts[i].depth = 250;
                break;
    }
    starfield.r = starfield.g = starfield.b = star_pnts[i].depth;
    
  }
}

void ani_stars(void)
{
  int i;

  for (i = 0; i < star_cnt; i++)
  {
retry:
    switch(star_dir)
    {
      case STAR_U :  // up
                star_pnts[i].y -= star_pnts[i].depth/50;
              	if (star_pnts[i].y <= star_y) 
                  star_pnts[i].y = star_pnts[i].y = (star_y + star_h);
                break;
      case STAR_D :  // down
                star_pnts[i].y += star_pnts[i].depth/50;
              	if (star_pnts[i].y >= (star_h + star_y)) 
                  star_pnts[i].y = star_y;
                break;
      case STAR_R :  // right
                star_pnts[i].x += star_pnts[i].depth/50;
              	if (star_pnts[i].x >= (star_w + star_x)) 
                  star_pnts[i].x = star_x;
                break;
      case STAR_L :  // left
                star_pnts[i].x -= star_pnts[i].depth/50;
              	if (star_pnts[i].x <= star_x) 
                  star_pnts[i].x = (star_w + star_x);
                break;
      case STAR_OUT: // out
                star_pnts[i].depth -= star_pnts[i].vel;
                if (star_pnts[i].depth<5)
                { star_pnts[i].x  = star_x + (rand()%(star_w - star_x));
                  star_pnts[i].y  = star_y + (rand()%(star_h - star_y));

                  switch(rand()%MAX_FIELDS)
                  {
                    case      0:      star_pnts[i].vel = 1 * star_mult;
                                      star_pnts[i].depth = 62;
                                      break;
                    case      1:      star_pnts[i].vel = 2 * star_mult;
                                      star_pnts[i].depth = 128;
                                      break;
                    case      2:      star_pnts[i].vel = 3 * star_mult;
                                      star_pnts[i].depth = 196;
                                      break;
                    case      3:      star_pnts[i].vel = 4 * star_mult;
                                      star_pnts[i].depth = 250;
                                      break;
                  }
                }
                break;
      case STAR_IN: // out
                star_pnts[i].depth += star_pnts[i].vel;
                if (star_pnts[i].depth>251)
                { star_pnts[i].x  = star_x + (rand()%(star_w - star_x));
                  star_pnts[i].y  = star_y + (rand()%(star_h - star_y));

                  switch(rand()%MAX_FIELDS)
                  {
                    case      0:      star_pnts[i].vel = 1 * star_mult;
                                      star_pnts[i].depth = 62;
                                      break;
                    case      1:      star_pnts[i].vel = 2 * star_mult;
                                      star_pnts[i].depth = 128;
                                      break;
                    case      2:      star_pnts[i].vel = 3 * star_mult;
                                      star_pnts[i].depth = 196;
                                      break;
                    case      3:      star_pnts[i].vel = 4 * star_mult;
                                      star_pnts[i].depth = 250;
                                      break;
                  }
                }
                break;
    }

/*    if ((star_dir!=STAR_OUT)&&(star_dir!=STAR_IN))
    { starfield.x0 = starfield.x1 = star_pnts[i].x;
      starfield.y0 = starfield.y1 = star_pnts[i].y;
      starfield.r = starfield.g = starfield.b = star_pnts[i].depth;
    } else
*/
    { starfield.x0 = starfield.x1 = (((star_pnts[i].x-(star_w/2)-star_x)*500)/star_pnts[i].depth)+(star_w/2)+star_x;
      starfield.x1++;
      starfield.y0 = starfield.y1 = (((star_pnts[i].y-(star_h/2)-star_y)*500)/star_pnts[i].depth)+(star_h/2)+star_y;
      starfield.r = starfield.g = starfield.b = 255-star_pnts[i].depth;
      if ((star_dir!=STAR_OUT)&&(star_dir!=STAR_IN))
        starfield.r = starfield.g = starfield.b = star_pnts[i].depth;
        
      if ((starfield.x0>star_x+star_w)||
          (starfield.x0<star_x)||
          (starfield.y0>star_y+star_h)||
          (starfield.y0<star_y))
            goto retry;
    }

    GsSortLine(&starfield,TheOT,3);
  }
}

void new_stardir(int dir)
{
star_dir=dir;
}

void new_starspeed(int sp)
{
  star_mult=sp;
}
