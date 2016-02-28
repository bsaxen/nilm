//===========================================
// nilm_lib.c
// 2016-02-16
//===========================================
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

#define MAX_DEVICES        10
#define MAX_DATA        86401

#define N_DEVICE_PAR        6
#define IX_DEVICE_ID        0
#define IX_DEVICE_HEIGHT    1
#define IX_DEVICE_DURATION  2
#define IX_DEVICE_PERIOD    3
#define IX_DEVICE_START     4
#define IX_DEVICE_SUBDEV    5

// Global variables
int g_nDevices;
int g_device[MAX_DEVICES][N_DEVICE_PAR];
char g_device_par_name[N_DEVICE_PAR][40];
int g_xData[MAX_DATA];  // index i
int g_yData[MAX_DATA];  // index i
int g_yValue[MAX_DATA]; // index g_xData

int g_xMin=99999;
int g_xMax=0;
int g_yMin=99999;
int g_yMax=0;

int g_hour,g_minute,g_second;
//===========================================
void lib_nilmError(char msg[], int errno)
//===========================================
{
  printf("NILM ERROR: %s errno=%d\n",msg,errno);
  return;
}
//===========================================
void lib_log(char msg[])
//===========================================
{
  FILE* fp;
  if ((fp = fopen("nilm_log.txt", "a+")) == NULL)
  { 
    lib_nilmError("Unable to open-write nilm log file",0);
    return;
  }
  fprintf(fp,"%s\n",msg);
  fclose(fp);
  return;
}
//===========================================
void lib_nilmInit()
//===========================================
{
  strcpy(g_device_par_name[IX_DEVICE_ID],      "DEVICE_ID");
  strcpy(g_device_par_name[IX_DEVICE_HEIGHT],  "DEVICE_HEIGHT");
  strcpy(g_device_par_name[IX_DEVICE_DURATION],"DEVICE_DURATION");
  strcpy(g_device_par_name[IX_DEVICE_PERIOD],  "DEVICE_PERIOD");
  strcpy(g_device_par_name[IX_DEVICE_START],   "DEVICE_START");
  strcpy(g_device_par_name[IX_DEVICE_SUBDEV],  "DEVICE_SUBDEV");
    
  g_nDevices = 1;    
  g_device[0][IX_DEVICE_ID]         = 1;
  g_device[0][IX_DEVICE_HEIGHT]     = 1000;
  g_device[0][IX_DEVICE_DURATION]   = 100;
  g_device[0][IX_DEVICE_PERIOD]     = 500;
  g_device[0][IX_DEVICE_START]      = 10;
  g_device[0][IX_DEVICE_SUBDEV]     = 0;
  return;
}

//===========================================
int lib_checkValue(char par[], int value)
//===========================================
{
  // Add check here ...
  return(value);
}
//========================================
void lib_valToHourMinSec(float val)
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
int lib_readNilmFile(char *selectedFile)
//========================================
{
    FILE *in;
    int i,n,skip = 0,hour,minute,sec;
    int t,t_prev,old_t;
    int x,x1,x2,old_x,v;    
    char line[80],ttime[80];
    
       for(i=0;i<MAX_DATA;i++)
       {
          g_xData[i]  = 0;
          g_yData[i]  = 0;
          g_yValue[i] = -1;
       }
       g_xMin=99999;
       g_xMax=0;
       g_yMin=99999;
       g_yMax=0;
    

       in = fopen(selectedFile,"r");
       if(in == NULL)
       {
         n = 0;
       }
       else
       {
          n = 0;
          while (fgets(line,sizeof line,in) != NULL)
          {
             n++;
             if(n > MAX_DATA)
             {
              //printf("**** Warning: Max data range reached ****");
              return(n);
             }
             sscanf(line,"%s %d %d",ttime,&x1,&x2);
             sscanf(ttime,"%d:%d:%d",&hour,&minute,&sec);
             t = hour*3600+minute*60+sec;
             v = x1;
             g_yValue[t] = v;
             g_xData[n] = t;
             g_yData[n] = v;
               
             if(g_yMax < v)g_yMax = v;
             if(g_yMin > v)g_yMin = v;
             if(g_xMax < t)g_xMax = t;
             if(g_xMin > t)g_xMin = t;
         }
    
        fclose(in);
        // interpolation
        for(i=1;i<MAX_DATA;i++)
        {
            if(g_yValue[i] == -1) g_yValue[i] = g_yValue[i-1];
        }
      } 
    g_xData[0] = n;
    return(n);
}

//===========================================
int lib_readDeviceModel(char filename[])
//===========================================
{
  int i,j,n,pc[N_DEVICE_PAR],itemp;
  int res;
  FILE* fp;
  char buf[120],stemp[20][20],key[80];
  
  if ((fp = fopen(filename, "r")) == NULL)
  { 
    lib_nilmError("Unable to open-read Device Model file",0);
    return(0);
  }

  

  n = -1;
  for(j=0;j<N_DEVICE_PAR;j++)pc[j] = 0;
  
  while (fgets(buf, sizeof(buf), fp) != NULL)
  {
    buf[strlen(buf) - 1] = '\0';
      //printf("a:%s\n",buf);
    for(i=0;i<N_DEVICE_PAR;i++)
    {
      sscanf(buf,"%s %d",key,&itemp);
      if(strstr(g_device_par_name[i],key) != NULL)
      {
        res = lib_checkValue(g_device_par_name[i],itemp);
        //printf("n=%d res=%d %s itemp=%d\n",n,res,g_device_par_name[i],itemp);
        g_device[n][i] = res;
        pc[i]++;
        if(i == IX_DEVICE_ID)
        {
            n++;
            //printf("b:%s\n",buf);
          for(j=0;j<N_DEVICE_PAR;j++)pc[j] = 0;
        }
      }
    }
 
    for(j=0;j<N_DEVICE_PAR;j++)
    {
      if(pc[j] > 1) lib_nilmError("Device Parameter multiple reading",j);
    }
  }
  fclose(fp);
  n++;
  printf("Number of devices read: %d\n", n);
  return(n);
}
//===========================================
void lib_writeDeviceModel(char filename[])
//===========================================
{
  int i,j;
  FILE* fp;
  
  if ((fp = fopen(filename, "w")) == NULL)
  { 
    lib_nilmError("Unable to open-write Device Model file",0);
    return;
  }

  for(i=0;i<g_nDevices;i++)
  {
    g_device[i][IX_DEVICE_ID] = i+1;
    fprintf(fp,"#==== %d ======\n",i);
    for(j=0;j<N_DEVICE_PAR;j++)
    {
      fprintf(fp,"%s %d\n",g_device_par_name[j],g_device[i][j]);
    }
  }
  fprintf(fp,"#END OF FILE\n");
  printf("Number of devices written: %d\n", g_nDevices);
  fclose(fp);
  return;
}
//========================================
int lib_stepFunction(int s,int h,int x)
//========================================
{
    //              
    //        -------------------------
    //        |           
    //       h|            
    //        |           
    //---------            
    //        s
    //
    int y;
    if(x >= s) y = h;
    if(x < s)  y = 0;
    return(y);    
}
//========================================
int lib_squareFunction(int s,int h,int d,int x)
//========================================
{
    //            d  
    //        ----------
    //        |        |  
    //       h|        |    
    //        |        |   
    //---------        ----------    
    //        s
    //
    int y,temp1,temp2;
    temp1 = s+d;
    temp2 = -h;
    y = lib_stepFunction(s,h,x) + lib_stepFunction(temp1,temp2,x);
    return(y);    
}
//========================================
int lib_squareWave(int s,int h,int T,int p, int x)
//========================================
{
    //              d
    //        --------------         -----------
    //        |            |         |
    //       h|            |         |
    //        |            |   p     |
    //---------            -----------
    //        s
    //
    // Period T = d+p
    //
    
    int y,temp;
 
    if(x < s)return(0);
    x = x-s; s = 0;
    float z = floor(x/T); 
    x = x-z*T;
    temp = T-p;
    y = lib_squareFunction(s,h,temp,x);
    return(y);    
}

