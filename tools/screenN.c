//======================================
// graphN.c
// apt-get install ncurses-dev
// gcc -o screenN screenN.c -lncurses -lm
// Version: 2016-01-30
//======================================
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define C_NORMAL  0
#define C_RED     1
#define C_GREEN   2
#define C_YELLOW  3
#define C_BLUE    4
#define C_MAGENTA 5
#define C_CYAN    6
#define C_WHITE   7

struct winsize w;
int g_rows;
int g_cols;
int g_iter = 0;
char dmx[200][200];
int  cmx[200][200];
float g_fbuff[200];
int g_ibuff[200];
int g_imax = 0;
int g_imin = 999999;
int g_fmax = 0.;
int g_fmin = 999999.;
char g_server[80];
float g_modMax;
int g_ndiv=1;
int g_yscale;
int g_mid = 2;

void writeInt(int number,int x, int y, int color);
void writeFloat(float value,int x, int y, int color);
void writeString(char text[],int x, int y, int color);
void setElement(char c, int i, int j, int color);
//==========================================
void initTool() 
//==========================================
{
    int i,j;
    strcpy(g_server,"78.67.160.17");
    //strcpy(g_server,"127.0.0.1");
    for(j=1;j<=g_rows;j++)
    {
       for(i=1;i<=g_cols;i++)
       {
          setElement(' ',i,j,C_NORMAL);
          g_ibuff[i] = 0;
          g_fbuff[i] = 0.0;
       }
    }
}
//==========================================
char *trim(char *s) 
//==========================================
{
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}
//==========================================
void setElement(char c, int i, int j, int color)
//==========================================
{
          dmx[i][j] = c;
          cmx[i][j] = color;
}
//==========================================
void readFile(int x, int y, char filename[],int color)
//==========================================
{
       FILE * fp;
       char * line = NULL;
       size_t len = 0;
       ssize_t read;
       int k=g_rows-x;

       fp = fopen(filename, "r");
       if (fp == NULL)
           return;
     
       while ((read = getline(&line, &len, fp)) != -1) {
           line = trim(line);
           k--;
           writeString(line,k,y,color);
       }

       fclose(fp);
       if (line)
           free(line);
       return;
}
//==========================================
float readLatestSidValue(int sid,int mid)
//==========================================
{
       FILE * fp;
       char sys[240];
       char * line = NULL;
       size_t len = 0;
       ssize_t read;
       int tsid;
       float value;

       sprintf(sys,"wget -q -O server_response.txt \"http://%s/sxndata/index.php?mid=%d&nsid=1&sid1=%d\"",g_server,mid,sid); 
       //printf("%s",sys);
       system(sys);

       fp = fopen("server_response.txt", "r");
       if (fp == NULL)
           return;
     
       while ((read = getline(&line, &len, fp)) != -1) 
       {
           line = trim(line);
           sscanf(line,"%d %f",&tsid,&value);
           //writeFloat(value,70,g_rows, C_BLUE);
           //writeString(line,5,g_rows, C_RED);
           if(mid == 4) 
           {
               value = value*100.;
               if(value < 0)value =value*(-1.0);
               //writeFloat(value,40,g_rows, C_RED);
           }
          // writeFloat(value,40,g_rows, C_RED);
       }

       fclose(fp);
       system("rm -f server_response.txt");
    
       if (line)
           free(line);
       return(value);
}
//==========================================
void display(int mode)
//==========================================
{
  int i,j,color;
  system("clear");
  for(j=g_rows;j>0;j--)
  {
    if(mode == 1)printf("%s%2d",KYEL,j);
    for(i=1;i<=g_cols;i++)
    {
          color = cmx[i][j];
          if(color == C_NORMAL) printf("%s%c",KNRM,dmx[i][j]);
          if(color == C_RED)    printf("%s%c",KRED,dmx[i][j]);
          if(color == C_GREEN)  printf("%s%c",KGRN,dmx[i][j]);
          if(color == C_YELLOW) printf("%s%c",KYEL,dmx[i][j]);
          if(color == C_BLUE)   printf("%s%c",KBLU,dmx[i][j]);
          if(color == C_MAGENTA)printf("%s%c",KMAG,dmx[i][j]);
          if(color == C_CYAN)   printf("%s%c",KCYN,dmx[i][j]);
          if(color == C_WHITE)  printf("%s%c",KWHT,dmx[i][j]);
          printf("%s",KNRM);
          cmx[i][j] = 0;
          dmx[i][j] = ' ';
    }
    printf("\n");
  }
}
//==========================================
void writeInt(int number,int x, int y, int color)
//==========================================
{
    int i,k=0;
    char text[20];
   
    sprintf(text,"%d",number);
    writeString(text,x,y,color);
}
//==========================================
void writeFloat(float value,int x, int y, int color)
//==========================================
{
    int i,k=0;
    char text[20];
   
    sprintf(text,"%f",value);
    writeString(text,x,y,color);
}
//==========================================
void writeString(char text[],int x, int y, int color)
//==========================================
{
    int i,k=0;
    int len = strlen(text);
    if(len > g_cols - x) len = g_cols - x;
    for(i=x;i<x+len;i++)
    {
        dmx[i][y] = text[k];
        cmx[i][y] = color;
        k++;
    }
}
//==========================================
int getRowNumber(float fmax,float value)
// Returns row for specific value
//==========================================
{
    int res = (int)(value/fmax*g_rows);
    return(res);
}
//==========================================
float getRowValue(float fmax,int row)
// Returns value for specific row
//==========================================
{
    int res = (int)(row/g_rows*fmax);
    return(res);
}
//==========================================
float getDivision(float fmax)
// Returns row for specific value
//==========================================
{
    int ii = g_rows/5;
    float div = fmax/ii;
    
    if(div >=1  && div   <15  ) div =  10;
    if(div >=15  && div  <25  ) div =  20;
    if(div >=25  && div  <75  ) div =  50;
    if(div >=75  && div  <125  ) div =  100;
    if(div >=125  && div <250  ) div =  200;
    if(div >=250  && div  <750  ) div =  500;
    if(div >=750  && div  <1500  ) div =  1000;

    return(div);
}
//==========================================
void histogram(int sid)
//==========================================
{
    int i,j,k=0,temp,height = g_rows - 1;
    int color=0,div,rscale = 0,rn1,rn2;
    char sstr[40];
    
    float fx = readLatestSidValue(sid,g_mid);
    // Get max and min value
    g_fmax = 0.;
    g_fmin = 999999.;
    for(i=1;i<=g_cols-5;i++)
    {
         g_fbuff[i] = g_fbuff[i+1];
         if(g_fmin > g_fbuff[i])g_fmin = g_fbuff[i];
         if(g_fmax < g_fbuff[i])g_fmax = g_fbuff[i];
    }
    g_fmax = g_fmax*1.1;
    
    g_fbuff[g_cols-5] = fx;
    
    div = getDivision(g_fmax);
    
    k=1;
    int kindex = div;
    for(j=1;j<=g_rows;j++)
    {
        if(j == getRowNumber(g_fmax,kindex))
        {
            writeInt(kindex,1, j, C_BLUE);
            writeInt(kindex,g_cols-4, j, C_BLUE);
            k++;
            kindex = k*div;
        }
    }
    

    rn1 = getRowNumber(g_fmax,3000.);
    rn2 = getRowNumber(g_fmax,1000.);
    //writeFloat(g_fmax,7,g_rows-1, 0);
    sprintf(sstr,"%d Watt",(int)fx);
    //writeFloat(fx,g_cols/2,g_rows-1, C_RED);
    writeString(sstr,g_cols/2,g_rows-1, C_RED);
 
    g_fmin = 0.0;
    for(i=1;i<=g_cols-5;i++)
    {
         if(g_fmax > 0)temp = (int)(height*(g_fbuff[i]-g_fmin)/(g_fmax - g_fmin));  
         for(j=1;j<=temp;j++) 
         {
            if (j >= rn1)color = C_YELLOW;
            else if (j >= rn2)color = C_RED;
            if (j < rn2)color = C_GREEN;
            setElement('*',i,j,color);
         }
    }
}
//==========================================
int main(int argc, char **argv)
//==========================================
{
    int i,j,x=0,y=0,sid=1,data[200],delay;
    char ch,sys[240];
    
    if(argc == 1 || argc > 4)
    {
      printf("Syntax: ./screen <sid> <delay> <mid>\n");
      exit(0);
    }
    if(argc == 2)
    {
        sscanf(argv[1],"%d",&sid);
        delay = 3;
        g_mid = 2;
    }
    if(argc == 3)
    {
       sscanf(argv[1],"%d",&sid);
       sscanf(argv[2],"%d",&delay);
       g_mid = 2;
    }
    if(argc == 4)
    {
       sscanf(argv[1],"%d",&sid);
       sscanf(argv[2],"%d",&delay);
       sscanf(argv[3],"%d",&g_mid);
    }
    ioctl(0, TIOCGWINSZ, &w);
    g_rows = w.ws_row-1;
    g_cols = w.ws_col-2;

    initTool();
  
    g_iter = 0;
    while (1)
    {
  
      writeInt(sid,7,g_rows, C_YELLOW); 
      writeInt(delay,14,g_rows, C_YELLOW); 
   
      histogram(sid);
  
      display(0);
    
      //printf("%s>>",KNRM);   

      sleep(delay);
         printf("a8,");
      //ch = getchar();
      //if (ch=='q')
	  //{
	   // exit(0);
	//  }
    }
    //return 0;  
}
