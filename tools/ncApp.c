//======================================
// ncApp.c
// apt-get install ncurses-dev
// gcc -o ncApp ncApp.c -lncurses -lm
// Version: 2016-03-20
//======================================
#include "ncLib.c"
//====================================
int main(int argc, char *argv[])
//====================================
{
    char ch;
ncInit();
    // height width row column  %
WINDOW *win1 = ncWinSetup(50, 100, 0, 0, 3);
WINDOW *win2 = ncWinSetup(10, 100, 50, 0, 4);
WINDOW *win3 = ncWinSetup(40, 100, 60, 0, 5);
//WINDOW *win4 = ncWinSetup(20, 50, 80, 50, 6);
while(ch != 'q')  
{
    ncShow(win1,"w1");
    ncShow(win2,"w2");
    ncShow(win3,"w3");
    //ncShow(win4,"w4");
    //wmove(win1,2,1);
    // wprintw(win1,"");
    mvwprintw(win3,1,1,">");  
    wrefresh(win3);
    ch = getchar();
}
ncWinClose(win1);
ncWinClose(win2);
ncWinClose(win3);
//ncWinClose(win4);
ncClose();
}