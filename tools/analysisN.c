//======================================
// analysisN.c
// apt-get install ncurses-dev
// gcc -o analysisN analysisN.c -lncurses -lm
// Version: 2016-01-30
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

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


WINDOW *graph,*data,*misc;
static struct termios orig, nnew;

#define GRAPH_H  20
#define GRAPH_W  61
#define GRAPH_COLOR 6
#define DATA_COLOR 7
#define MISC_COLOR 5
#define WORLD 1000

#define RF 1 
#define ER 1
#define SR 20

int graph_h=0, graph_w=0, graph_x=0, graph_y=0;
int data_h=0, data_w=0, data_x=0, data_y=0;
int misc_h=0, misc_w=0, misc_x=0, misc_y=0;
//int board_w=0,board_h=0,board_x=0,board_y=0;
int ap,dp,s_row,s_col,pos=0;

#define DATA_MAX 86400
//#define X_MAX 500
//#define Y_MAX 500

int g_dataSource = 1;
int g_mode = 1;
int g_display,g_nData,g_selectedFile,g_zn;
int g_hour,g_minute,g_second;
float g_xmin=9999.,g_xmax=0.,g_ymin=9999.,g_ymax=0.;
float g_wxmax=0., g_wxmin=999999.,g_wymax=0., g_wymin=999999.;
//char str[40],sstr[20];
//char command[40][40];
char g_data_file[120];
int g_selected_file = 1,g_chan = 1;
//char g_area[X_MAX][Y_MAX];
float g_xdata[DATA_MAX],g_ydata[DATA_MAX],g_der[DATA_MAX];
float g_xvalue[DATA_MAX],g_x[DATA_MAX],g_y[DATA_MAX];
int g_NX,g_NY,g_X,g_Y,g_f[DATA_MAX];
float r_ymax=0.0,r_ymin=0.0,r_xmin=0.0,r_xmax=0.0;

float g_pi=3.1415926;

int generatedData(int mdl);

//========================================
void valToHourMinSec(float val)
//========================================
{
    g_hour = floor(val/3600);
    val = val-g_hour*3600;
    g_minute = floor(val/60);
    val = val-g_minute*60;
    g_second = val;
    return;
}
//========================================
int readFile(char *selectedFile)
//========================================
{
    FILE *in;
    int i,n,skip = 0,hour,minute,sec;
    float t,t_prev,old_t;
    float x,x1,x2,old_x,v;    
    char line[80],ttime[80];
    
     printf("Read data file\n");  

       for(i=0;i<DATA_MAX;i++)
       {
          g_xdata[i] = 0.0;
          g_ydata[i] = 0.0;
          g_xvalue[i] = 0.0;
       }
       g_xmin=9999.;
       g_xmax=0.;
       g_ymin=9999.;
       g_ymax=0.;
    

       in = fopen(selectedFile,"r");
       if(in == NULL)
       {
         n = 0;
         printf("Error: No data file\n");  
         g_display = 0; 
       }
       else
       {
          n = 0;
          while (fgets(line,sizeof line,in) != NULL)
          {
             n++;
             if(n > DATA_MAX)
             {
              //printf("**** Warning: Max data range reached ****");
              return(n);
             }
             sscanf(line,"%s %f %f",ttime,&x1,&x2);
             sscanf(ttime,"%d:%d:%d",&hour,&minute,&sec);
             t = hour*3600+minute*60+sec;
             v = x1;
             g_xvalue[(int)t] = v;
             g_xdata[n] = t*1.0;
             g_ydata[n] = v;
               
            if(g_ymax < v)g_ymax = v;
            if(g_ymin > v)g_ymin = v;
            if(g_xmax < t)g_xmax = t;
            if(g_xmin > t)g_xmin = t;
         }
    
        fclose(in);
        for(i=1;i<DATA_MAX;i++)
        {
            if(g_xvalue[i] == 0.0) g_xvalue[i] = g_xvalue[i-1];
        }
      } 

    
    g_xdata[0] = n;
    
    printf("Number of data %d\n",n);      
    return(n);
}
//====================================
int bubblesort(int d,int k)
//====================================
{
    int i,change = 1;
    float t,work[10];

    for(i=0;i<d;i++)
    {
        work[i] = g_x[k+i];  
    }
    while(change == 1)
    {
      change = 0;
      for(i=0;i<d;i++)
      {
        if(work[i]>work[i+1])
        {
            change = 1;
            t = work[i];
            work[i] = work[i+1];
            work[i+1] = t;
        }
      }
    }
  d = d/2;
  return(work[d]);
}
//====================================
void filter_median(int m)
//====================================
{
    int i,j,k,n;
    float dx,dy;
    float dtemp[DATA_MAX];

    n = g_x[0];
 
    for(i=1;i<=n;i++)
    {
        dtemp[i] = bubblesort(m,i); 
        printf("bubble %d %f\n",i,dtemp[i]);
    }
    for(i=1;i<=n;i++)
    {
        g_x[i] = dtemp[i];  
    }
}
//====================================
void normera()
//====================================
{
    int i,n;
    float dx,dy;

    n = g_xdata[0];
    dx = g_xmax - g_xmin;
    dy = g_ymax - g_ymin;
    
    for(i=1;i<=n;i++)
    {
        g_x[i] = 1000*g_xdata[i]/dx;
        g_y[i] = 1000*g_ydata[i]/dy;
        //printf("%d %f %f\n",i,g_x[i],g_y[i]);
    }
    g_x[0] = n;
}
//====================================
void step()
//====================================
{
    int i,n;
    float dx,dy;

    n = g_x[0];

    for(i=1;i<n;i++)
    {
        g_y[i] = g_y[i+1] - g_y[i];
        //printf("step %d %f %f\n",i,g_x[i],g_y[i]);
    }
    
}
//====================================
void freq()
//====================================
{
    int i,j,n;
    float dx,dy;

    n = g_x[0];
    
    for(i=0;i<4000;i++)
    {
        g_f[i] = 0;
    }
    
    for(i=0;i<n;i++)
    {
        int k = round(g_y[i]);
        g_f[k+2000]++;
        //printf("%d %f > %f %f < %f \n",i,g_y[i],k,g_y[i],k+1);
        //if(g_f[k+2000] > 0)printf("+freq %d %d %d\n",i,k,g_f[k+2000]);
        //printf("freq %d %d\n",i,g_f[i]);
    }
    
    for(i=0;i<4000;i++)
    {
        if(g_f[i] > 0)printf("freq %d %d\n",i-2000,g_f[i]);
    }
}
//====================================
int main(int argc, char *argv[])
//====================================
{
  char syscom[120];
  int ch,i,x;

  readFile("data.nbc");
  normera();
  filter_median(6);
  step();
  freq();
}
//====================================
// End of file
//====================================

