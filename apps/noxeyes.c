#include <stdio.h>
#include <stdlib.h>
#include <video.h>
#include <process.h>
#include <mouse.h>

#define OFFSET_EYE_1_X 8
#define OFFSET_EYE_1_Y 6
#define OFFSET_EYE_2_X 23
#define OFFSET_EYE_2_Y 6

#define NB_LINES 12
#define NB_COLUMNS 31 

typedef struct {
  int x, y;
} coord;

typedef struct {
  coord center;
  coord iris;
} eye;

void displayEyes() {
	printf("\033[2J"); // cls
	fflush(stdout);

	printf("\033[0;0H");
  printf("  ###########    ###########\n");
  printf(" #           #  #           #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf("#             ##             #\n");
  printf(" #           #  #           #\n");
  printf("  ###########    ###########\n");
}

static int min(int a, int b) {
  return a < b ? a : b;
}

static int max(int a, int b) {
  return a > b ? a : b;
}

coord getVect(coord mouse, coord eye) {
  coord result;
  if(mouse.x > eye.x)
    result.x = min(mouse.x, eye.x + OFFSET_EYE_1_X - 3);
  else
    result.x = max(mouse.x, eye.x - OFFSET_EYE_1_X + 3);
  
  if(mouse.y > eye.y)
    result.y = min(mouse.y, eye.y + OFFSET_EYE_1_Y - 3);
  else
    result.y = max(mouse.y, eye.y - OFFSET_EYE_1_Y + 3);
 
  return result;
}

static void print_locate(coord c, char car) {
	printf("\033[%d;%df%c\033[%d;%df", c.y, c.x, car, NB_LINES, NB_COLUMNS);
	fflush(stdout);
}

void updateMouseCoord(coord *mouse)
{
  coord tmp;

  getMouseCoord(&(tmp.x),&(tmp.y));
	tmp.x = tmp.x*80/640;
  tmp.y = 25 - (tmp.y*25/480);

  if(mouse->x != tmp.x || mouse->y != tmp.y) {
    if(mouse->x != -1)
  		set_attribute_position(BLACK, LIGHT_BLUE, mouse->x, mouse->y);
    *mouse = tmp;
	  set_attribute_position(LIGHT_BLUE, BLACK, mouse->x, mouse->y);
  }
}

void updateIris(eye *e1, eye *e2, coord ir1, coord ir2) {
  if(e1->iris.x == ir1.x && e1->iris.y == ir1.y &&
     e2->iris.x == ir2.x && e2->iris.y == ir2.y )
    return;

  print_locate(e1->iris, ' ');
  print_locate(e2->iris, ' ');
  e1->iris = ir1;
  e2->iris = ir2;
  print_locate(e1->iris, 'o');
 	print_locate(e2->iris, 'o');
}

int noxeyes_main(int zboing) {
  int wait = 0;
  int isz_l = 1;
  int isz_c = 1;
  coord mouse = { -1, -1 };
  eye eye1, eye2;
  text_window *tw = get_current_process()->fd[1].ofd->extra_data;

  resize_text_window(tw, NB_COLUMNS, NB_LINES);
  disable_cursor(1);

  displayEyes();

  // init
  eye1.center.x = tw->x + OFFSET_EYE_1_X; 
  eye1.center.y = tw->y + OFFSET_EYE_1_Y; 
  eye2.center.x = tw->x + OFFSET_EYE_2_X; 
  eye2.center.y = tw->y + OFFSET_EYE_2_Y; 
  eye1.iris = eye1.center;
  eye2.iris = eye2.center;
	print_locate(eye1.iris, 'o');
	print_locate(eye2.iris, 'o');

  while(1) {
    coord newIris1, newIris2;

    eye1.center.x = tw->x + OFFSET_EYE_1_X; 
    eye1.center.y = tw->y + OFFSET_EYE_1_Y; 
    eye2.center.x = tw->x + OFFSET_EYE_2_X; 
    eye2.center.y = tw->y + OFFSET_EYE_2_Y; 
  
    updateMouseCoord(&mouse);

    newIris1 = getVect(mouse, eye1.center);
    newIris2 = getVect(mouse, eye2.center);

    updateIris(&eye1, &eye2, newIris1, newIris2);

    // XXX : marche bof
    if(zboing) {
      wait++;

      if(wait >= 1000000) {
        if(tw->x == 0)
          isz_c = 1;
        if(tw->y == 0)
          isz_l = 1;
        if(tw->x + tw->cols > 80)
          isz_c = -1;
        if(tw->y + tw->lines > 24)
          isz_l = -1;
      
        move_text_window(tw, tw->x + isz_c, tw->y + isz_l);
        wait = 0;
      }
    }
  }

  return 0;
}

int noxeyes_proc() {
  return noxeyes_main(0);
}

int noxeyes() {
	exec(noxeyes_proc, "noxeyes");
	return 0;
}

