//======================================
// ncLib.c
// apt-get install ncurses-dev
// gcc -o app app.c -lncurses -lm
// Version: 2016-03-20
//======================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <math.h> 
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <form.h>
#include <sys/ioctl.h>

WINDOW *win3,*win4,*win5,*win6,*win7,*win8;
static struct termios orig, nnew;
int s_row,s_col;

//====================================
void ncInit()
//====================================
{
  initscr();
  clear();
  cbreak();

  getmaxyx(stdscr,s_row,s_col);
    
  start_color();
  init_pair(1,COLOR_BLACK,COLOR_BLUE);
  init_pair(2,COLOR_BLACK,COLOR_GREEN);
  init_pair(3,COLOR_BLUE,COLOR_WHITE); 
  init_pair(4,COLOR_RED,COLOR_WHITE); 
  init_pair(5,COLOR_MAGENTA,COLOR_WHITE); 
  init_pair(6,COLOR_WHITE,COLOR_BLACK); 
  init_pair(7,COLOR_WHITE,COLOR_BLUE);
  init_pair(8,COLOR_RED,COLOR_BLACK);   
}
//====================================
WINDOW *ncWinSetup(int h, int w, int x, int y, int c)
//====================================
{
  h = (int)s_row*h/100;
  w = (int)s_col*w/100;
  x = (int)s_row*x/100;
  y = (int)s_col*y/100;
  WINDOW *win=newwin(h,w,x,y);
  wbkgd(win,COLOR_PAIR(c));
  return(win);
}
//====================================
void ncShow(WINDOW *win, char title[])
//====================================
{
  box(win,0,0);
  wmove(win,0,2);
  wprintw(win,title);
  wrefresh(win);
}
//====================================
void ncWinClose(WINDOW *win)
//====================================
{
      delwin(win);
}
//====================================
void ncClose()
//====================================
{
  endwin();
}