//======================================
// graphN.c
// apt-get install ncurses-dev
// gcc -o graphN graphN.c -lncurses -lm
// Version: 2016-02-19
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

#include "libN.c"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


WINDOW *graph,*data,*misc,*feedback;
static struct termios orig, nnew;

#define GRAPH_H  20
#define GRAPH_W  61
#define GRAPH_COLOR 6
#define DATA_COLOR 7
#define MISC_COLOR 5
#define FEEDBACK_COLOR 8
#define WORLD 1000

#define RF 1 
#define ER 1
#define SR 20

int graph_h=0, graph_w=0, graph_x=0, graph_y=0;
int data_h=0, data_w=0, data_x=0, data_y=0;
int misc_h=0, misc_w=0, misc_x=0, misc_y=0;
int feedback_w=0,feedback_h=0,feedback_x=0,feedback_y=0;
int ap,dp,s_row,s_col,pos=0;



float r_ymax=0.0,r_ymin=0.0,r_xmin=0.0,r_xmax=0.0;



int generatedData();

//========================================
int countlines(char filename[])
//========================================
{                                  
  FILE *fp;
  int ch=0;
  int lines=0;

  fp = fopen(filename,"r");
  if (fp == NULL)return 9;

  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
      lines++;
    }
  }
  fclose(fp);
  return lines;
}
//========================================
void list_all_nilm_files()
//========================================
{
  FILE *in;
  int i,j,lines;
  char line[80];

  system("ls *.nilm > nilm_data_file_list.wrk");
  wclear(data);    
    
  lines = data_h-2;
  //wmove(misc,1,2);wprintw(misc,"%d",start);wrefresh(misc); 

  i = 0;
  j = 0;
  in = fopen("nilm_data_file_list.wrk","r");
  if(in == NULL)
  {
    //printf("Error: Noh file: %s\n",file);   
  }
  else
  {
    while (fgets(line,sizeof line,in) != NULL)
    {
        i++;
        if(i >= g_selected_file)
        {
          j++;
          wmove(data,j,1);
          if(i == g_selected_file)
          {
              wprintw(data,">%3d %s",i,line);
              sscanf(line,"%s",g_data_file); 
          }
          else
              wprintw(data," %3d %s",i,line);
            
          if(j == lines)return;
        }
    }
    return;
  }
}

//========================================
void prep_area(int ndata,float xdata[],float ydata[], float left, float right, float down, float top)
//========================================
{
    
      wclear(graph); 

      int i,j,k,ix,iy,zeroline,plot[WORLD][WORLD];
      float dx,dy,tx,ty,ave_y;
      int old_ix = 0;
 
      dx = (right - left)/graph_w;
      dy = (top - down)/graph_h;
    
      float midX = (right + left)/2;
      float midY = (top + down)/2;
    
      int origo = WORLD/2;

      for(i=0;i<WORLD;i++)
      {
        for(j=0;j<WORLD;j++)
        {
           plot[i][j] = 0;
        }
      }
 
      g_wxmin = 999999.; g_wxmax = 0.;
      g_wymin = 999999.; g_wymax = 0.;
    
      int ix0 = origo -midX/dx + 0.5;
      if(ix0 < 0)ix0=0;
      int iy0 = origo -midY/dy + 0.5;
      if(iy0 < 0)iy0=0;
      int ix1 = origo +(left-midX)/dx + 0.5;
      if(ix1 < 0)ix1=0;
      int ix2 = origo +(right-midX)/dx + 0.5;
      if(ix2 < 0)ix2=0;
      int iy1 = origo +(down-midY)/dy + 0.5;
      if(iy1 < 0)iy1=0;
      int iy2 = origo +(top-midY)/dy + 0.5;
      if(iy2 < 0)iy2=0;
      //wmove(misc,1,2);wprintw(misc,"ndata=%d ix0=%d iy0=%d ix1=%d ix2=%d iy1=%d iy2=%d\n",ndata,ix0,iy0,ix1,ix2,iy1,iy2);
      
      lib_valToHourMinSec(left);
      wmove(misc,1,2);wprintw(misc,"%2d:%2d:%2d",g_hour,g_minute,g_second); 
      int mid = (right + left)/2;
      lib_valToHourMinSec(mid);
      wmove(misc,1,11);wprintw(misc,"<%2d:%2d:%2d>",g_hour,g_minute,g_second);    
      lib_valToHourMinSec(right);
      wmove(misc,1,22);wprintw(misc,"%2d:%2d:%2d",g_hour,g_minute,g_second);    
      lib_valToHourMinSec(dx);
      wmove(misc,1,32);wprintw(misc,"(%2d:%2d:%2d)",g_hour,g_minute,g_second);    

      wmove(misc,2,2);wprintw(misc,"[%d]->%d ->%.2f",ndata,mid,g_xvalue[mid]); 
    
      wmove(misc,3,2);wprintw(misc,"X-axis [%.0f,%.0f]=%.2f",left,right,dx);
    
      wmove(misc,4,2);wprintw(misc,"Y-axis [%.0f,%.0f]=%.2f",down,top,dy);    
    
      for(i=iy1;i<=iy2;i++)plot[ix0][i] = 4;
      for(i=ix1;i<=ix2;i++)plot[i][iy0] = 3;
      plot[ix0][iy0] = 2;
      
    
      k = 0;
      for(i=1;i<=ndata;i++)
      {       
        tx = xdata[i]; ty = ydata[i];
        if((tx >= left && tx <= right))
        {
              k++;
              ix = origo + (tx-midX)/dx + 0.5;
              ave_y = ave_y + ty;
              if(old_ix != ix)
              {   
                ave_y = ave_y/k;
                iy = origo + (ave_y-midY)/dy + 0.5;
                if(iy > iy0)for(j=iy;j>iy0;j--)plot[ix][j] = 1;
                if(iy < iy0)for(j=iy;j<iy0;j++)plot[ix][j] = 1;
                if(i == mid)
                  plot[ix][iy] = 8; 
                else
                  plot[ix][iy] = 5; 

                old_ix = ix;
                k = 0;
                ave_y = 0.0;
              }
        }
        if(g_wxmax < tx)g_wxmax = tx;
        if(g_wxmin > tx)g_wxmin = tx;
         
        if(g_wymax < ty)g_wymax = ty;
        if(g_wymin > ty)g_wymin = ty;
      }

          
      for(i=1;i<graph_w;i++)
      {
        for(j=1;j<graph_h;j++)
        {
            ix = origo+i-graph_w/2;
            iy = origo+j-graph_h/2;
            wmove(graph,graph_h-j,i);
     
            if(plot[ix][iy] == 1)wprintw(graph,"."); // fill 
            if(plot[ix][iy] == 2)wprintw(graph,"o"); // origo
            if(plot[ix][iy] == 3)wprintw(graph,"-"); // x-axis
            if(plot[ix][iy] == 4)wprintw(graph,"|"); // y-axis
            if(plot[ix][iy] == 5)wprintw(graph,"*"); // top value
            if(plot[ix][iy] == 6)wprintw(graph,"="); // left corner
            if(plot[ix][iy] == 7)wprintw(graph,"+"); // right corner
            if(plot[ix][iy] == 8)wprintw(graph,"B"); // mid marker
        }
      }

}
//====================================
void show(WINDOW *win)
//====================================
{
  int next;
  box(win,0,0);
  if(win == graph)
  {
     wmove(win,0,2);
     wprintw(win,g_loadedFile);
  }
  if(win == data)
  {
     wmove(win,0,2);
     wprintw(win,"nilm Files");
  }
  if(win == misc)
  {
     wmove(win,0,2);
     wprintw(win," 2016-02-19 Status ");
  }
  if(win == feedback)
  {
     wmove(win,0,2);
     wprintw(win," Feedback ");
  }
  wrefresh(win);
}
//====================================
int tokCommand(char res[40][40],char *inp)
//====================================
{
  char *pch;
  int count = 0;

  pch = strtok(inp," ");
  while (pch != NULL)
    {
      strcpy(res[count],pch);
      count++;
      pch = strtok(NULL, " ");
    }
  return(count);
}

//====================================
void displayErrMsg()
//====================================
{
    int i;
    char stemp[250];
    wmove(feedback,1,1);
    clrtoeol();
    wprintw(feedback,g_errMsg);
    int slen = strlen(g_errMsg);
    if(slen < feedback_w)
    {
        slen++;
        wmove(feedback,1,slen);
        for(i=0;i<feedback_w-slen;i++) stemp[i] = ' ';
        stemp[i] = '\0';
        wprintw(feedback,stemp);
    }
}

//====================================
void runMode()
//====================================
{
  int ch,x,step,tmp,res=0,a=0,b=0,ir,ok=0,n=0;
  char tempName[80],syscom[120],stemp[80];
  char command[40][40];
  float xdelta = 0.0,ydelta=0.0;

  while(1)  
    {
      displayErrMsg();
      xdelta = (r_xmax-r_xmin)/graph_w;
      ydelta = (r_ymax-r_ymin)/graph_h;
      show(graph); show(data); show(misc);show(feedback);         
      wmove(misc,misc_h-2,1);
      wprintw(misc,"                                    ");
      mvwprintw(misc,misc_h-2,1,"%d-%d>",g_n_nilm_files,g_selected_file);
      sprintf(g_errMsg,"-");       
      wrefresh(misc);
      ch = getchar();
      //strcpy(temp," ");
      if (ch=='q')
	  {
	    return;
	  }
      else if (ch=='h')
      {
        lib_writeErrMsg("h - help");
      }
      else if (ch=='1')
      {
            g_chan = 1;
            g_nData = lib_readNilmFile(g_mode,g_data_file);
      }
      else if (ch=='2')
      {
            g_chan = 2;
            g_nData = lib_readNilmFile(g_mode,g_data_file);
      }
      else if (ch=='3')
      {
            g_chan = 3;
            g_nData = lib_readNilmFile(g_mode,g_data_file);
      }
      else if (ch=='4')
      {
            g_chan = 4;
            g_nData = lib_readNilmFile(g_mode,g_data_file);
      }
      else if (ch=='g')
      {
          lib_writeErrMsg("g - read nilm data file");
          g_nData = lib_readNilmFile(g_mode,g_data_file);
          r_xmin = g_xmin;r_xmax = g_xmax;
          r_ymin = g_ymin;r_ymax = g_ymax;
 
      }
      else if (ch=='n')
      {
          lib_writeErrMsg("n - next nilm data file");
          if(g_selected_file < g_n_nilm_files)g_selected_file++;
          list_all_nilm_files();
          //g_nData = lib_readNilmFile(g_mode,g_data_file);
          //r_xmin = g_xmin;r_xmax = g_xmax;
          //r_ymin = g_ymin;r_ymax = g_ymax;
 
      }
      else if (ch=='p')
      {
          lib_writeErrMsg("p - previous nilm data file");
          if(g_selected_file > 1)g_selected_file--;
          list_all_nilm_files();
          //g_nData = lib_readNilmFile(g_mode,g_data_file);
          //r_xmin = g_xmin;r_xmax = g_xmax;
          //r_ymin = g_ymin;r_ymax = g_ymax;
      }
      else if (ch=='b')
	  {
          lib_writeErrMsg("b - restore");
          r_xmin = g_wxmin;
          r_xmax = g_wxmax;
          r_ymin = g_wymin;
          r_ymax = g_wymax;
	  }
      else if (ch=='x')
	  {
          lib_writeErrMsg("x - ?");
          r_xmin = r_xmin+xdelta;
          r_xmax = r_xmax-xdelta;
          r_ymin = r_ymin+ydelta;
          r_ymax = r_ymax-ydelta;
	  }
      else if (ch=='X')
	  {
          lib_writeErrMsg("X - ?");
          r_xmin = r_xmin-xdelta;
          r_xmax = r_xmax+xdelta;
          r_ymin = r_ymin-ydelta;
          r_ymax = r_ymax+ydelta;
	  }
      else if (ch=='z')
	  {
          if(xdelta >= 1.0)
          {
            sprintf(stemp,"z - X zoom in %.2f",xdelta);
   
            r_xmin = r_xmin+xdelta;
            r_xmax = r_xmax-xdelta;
          }
          else
             sprintf(stemp,"z - MAX X zoom "); 
          lib_writeErrMsg(stemp);
	  }
      else if (ch=='w')
	  {
          xdelta = 1.0;
          sprintf(stemp,"1 - MAX X zoom in %.2f",xdelta);
          r_xmin = (g_wxmax + g_wxmin)/2 - graph_w/2;
          r_xmax = (g_wxmax + g_wxmin)/2 + graph_w/2;
          lib_writeErrMsg(stemp);
	  }
      else if (ch=='Z')
	  {
          sprintf(stemp,"z - X zoom out %.2f",xdelta);
          lib_writeErrMsg(stemp);
          r_xmin = r_xmin-xdelta;
          r_xmax = r_xmax+xdelta;
	  }
      else if (ch=='l')
	  {
          lib_writeErrMsg("l - move left");
          r_xmin = r_xmin-xdelta;
          r_xmax = r_xmax-xdelta;
	  }
      else if (ch=='r')
	  {
          lib_writeErrMsg("r - move right");
          r_xmin = r_xmin+xdelta;
          r_xmax = r_xmax+xdelta;
	  }
      
      else if (ch=='s')  // zoom out Y
	  {
          lib_writeErrMsg("s - Y zoom out");
          r_ymin = r_ymin+ydelta;
          r_ymax = r_ymax-ydelta;
	  }
      else if (ch=='S')  //zoom in Y
	  {
          lib_writeErrMsg("S - Y zoom in");
          r_ymin = r_ymin-ydelta;
          r_ymax = r_ymax+ydelta;
	  }
      else if (ch=='u')
	  {
          lib_writeErrMsg("u - move up");
          r_ymin = r_ymin-ydelta;
          r_ymax = r_ymax-ydelta;
	  }
      else if (ch=='d')
	  {
          lib_writeErrMsg("d - move down");
          r_ymin = r_ymin+ydelta;
          r_ymax = r_ymax+ydelta;
	  }
      else if (ch=='y')
	  {
          lib_writeErrMsg("m - mid Y");
          g_mode++;
          if (g_mode>2)g_mode=1;
	  }     
      else
	  {
	    sprintf(stemp,"Unknown command: %c",ch);
        lib_writeErrMsg(stemp);
	  }
      
      prep_area(g_nData,g_xdata,g_ydata, r_xmin, r_xmax, r_ymin, r_ymax);           
    }
    return;
}
//====================================
void openCommand()
//====================================
{
  struct stat st;
  int ch,nsteps=1000,x,i,n,stop=0,loop,projNo = 0,ok=0,tmp;
  char *p,str[120],sstr[20],fileName[120],stemp[120],syscom[120];
  char command[40][40];

  list_all_nilm_files();
  g_n_nilm_files = countlines("nilm_data_file_list.wrk");
  g_nData = lib_readNilmFile(g_mode,g_data_file);
  r_xmin = g_xmin;r_xmax = g_xmax;
  r_ymin = g_ymin;r_ymax = g_ymax;
  prep_area(g_nData,g_xdata,g_ydata, r_xmin, r_xmax, r_ymin, r_ymax);     
  
    
  runMode();    
    
//  while(strstr(str,"ex") == NULL)
//  {
//      displayErrMsg();
//      show(graph); show(data);show(misc);show(feedback);
//      wmove(misc,misc_h-2,1);
//      wprintw(misc,"                                                  ");
//      mvwprintw(misc,misc_h-2,1,"nilm>");
//      wrefresh(misc);
//      strcpy(command[0],"");
//      wgetstr(misc,str);
//      n = tokCommand(command,str);
//      strcpy(sstr,command[0]);
//      sprintf(g_errMsg,"-");
//      if(strstr(sstr,"he"))
//        {
//          // TBD
//        }
//      if(strstr(sstr,"run"))
//        {
//          runMode();
//        }
//      else if(strlen(sstr) > 0) 
//        {
//          sprintf(g_errMsg,"Unknown command: %s",sstr); 
//          //lib_writeErrMsg(stemp);
//        }
//  }
    
    return;
}
//====================================
int main(int argc, char *argv[])
//====================================
{
  char syscom[120];
  int ch,i,x;

  initscr();
  clear();
  cbreak();

//strcpy(g_errMsg,"No errors detected");    
    
    
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
  
  /*     COLOR_BLACK   0 */
  /*     COLOR_RED     1 */
  /*     COLOR_GREEN   2 */
  /*     COLOR_YELLOW  3 */
  /*     COLOR_BLUE    4 */
  /*     COLOR_MAGENTA 5 */
  /*     COLOR_CYAN    6 */
  /*     COLOR_WHITE   7 */
    
  // Graph Window   
  graph_w = s_col;
  graph_h = s_row-13;
  graph_x = 0;
  graph_y = 0;
  graph=newwin(graph_h,graph_w,graph_x,graph_y);
  wbkgd(graph,COLOR_PAIR(GRAPH_COLOR));
    
  // Data Window   
  data_w = s_col/2;
  data_h = 10;
  data_x = s_row-10;
  data_y = 0;
  data=newwin(data_h,data_w,data_x,data_y);
  wbkgd(data,COLOR_PAIR(DATA_COLOR));
    
  // Misc Window   
  misc_w = s_col/2;
  misc_h = 10;
  misc_x = s_row-10;
  misc_y = s_col/2;
  misc=newwin(misc_h,misc_w,misc_x,misc_y);
  wbkgd(misc,COLOR_PAIR(MISC_COLOR));
    
 // Feedback Window   
  feedback_w = s_col;
  feedback_h = 3;
  feedback_x = s_row-13;
  feedback_y = 0;
  feedback=newwin(feedback_h,feedback_w,feedback_x,feedback_y);
  wbkgd(feedback,COLOR_PAIR(FEEDBACK_COLOR));
 
  openCommand();
 
  delwin(graph);
  endwin();

}
//====================================
// End of file
//====================================
