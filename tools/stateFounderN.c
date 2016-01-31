//======================================
// stateFounderN.c
// apt-get install ncurses-dev
// gcc -o stateFounderN stateFounderN.c -lncurses -lm
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
#define DATA_MAX 90000
float g_pmin,g_pmax;
//================================
float fSub = 10.;
int thresholdA = 500;
//================================
int nSub;
int g_nfreq[DATA_MAX], g_time[DATA_MAX];
float g_data[DATA_MAX];
float g_state[DATA_MAX];
float g_state2[DATA_MAX];


//===========================================
void init()
//===========================================
{
	int i;
	for(i=0;i<1000;i++) g_nfreq[i] = 0;
}
//===========================================
int readNilmDataFile(char nilmFile[],int mode)
//===========================================
{
    int n=0;
    g_pmin = 99999.;
    g_pmax = -99999.;
    FILE *in;
    int i,j,skip = 0,hour,minute,sec;
    float t,t_prev,old_t;
    float x,x1,x2,old_x,v,v_prev;    
    char line[80],ttime[80];
    

       in = fopen(nilmFile,"r");
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
             if(n > DATA_MAX)
             {
              printf("**** Warning: Max data range reached %d****\n",n);
              return(n);
             }
             v_prev = v;
             sscanf(line,"%s %f %f",ttime,&v,&x2);
             sscanf(ttime,"%d:%d:%d",&hour,&minute,&sec);
             t = hour*3600+minute*60+sec;
            if( mode == 1) // absolute values
            {
            	g_data[n] = v;
            	g_time[n] = t; 
            }
            if( mode == 2 && n > 1) // derivative values
            {
            	g_data[n] = v - v_prev;
            	g_time[n] = t; 
            }
            if(g_pmax < v)g_pmax = v;
            if(g_pmin > v)g_pmin = v;
         }

        // remove bias
        for(j=1;j<=n;j++)
        {
              g_data[j] = g_data[j] - g_pmin;
        }
        g_pmax = g_pmax -g_pmin;	
        g_pmin = 0.;
    
        fclose(in);
      } 
      return(n);
}

//===========================================
//===========================================
main()
//===========================================
//===========================================
{
   int i,j,n;
   float left,right;
   init();
   n = readNilmDataFile("autoGraph.nilm",1);


   nSub = (int)((g_pmax - g_pmin)/fSub);
   printf("Number of data = %d fSub=%f nSub=%d\n",n,fSub,nSub);

   float subInt = fSub;//(g_pmax- g_pmin)/nSub;
   printf("subInt=%f\n",subInt);

   // ===== Frequency =======

   // ---- Fixed sub intervals
   for(i=0;i<=nSub;i++)
   {
        left = g_pmin +i*subInt;
        right = g_pmin + (i+1)*subInt;
        for(j=1;j<=n;j++)
        {
	     if(g_data[j]>left && g_data[j]<=right) g_nfreq[i]++;
	  /*if(g_data[j]>left && g_data[j]<right)
             { 
                g_nfreq[i] = g_nfreq[i]+(int)(3600/g_data[j]+1);
             }*/
        }
   }

   // ---- Gliding sub interval
//   int loop_start = 0;
//   int loop_end = (int)(g_pmax-fSub);
//   for(i=loop_start;i<=loop_end;i++)
//   {
//        left = g_pmin +i;
//        right = left + (int)fSub;
//        for(j=1;j<=n;j++)
//        {
//	  //if(g_data[j]>left && g_data[j]<=right) g_nfreq[i]++;
//	  if(g_data[j]>left && g_data[j]<right)
//             { 
//                g_nfreq[i] = g_nfreq[i]+(int)(3600/g_data[j]+1);
//             }
//        }	
//   }


   //====== States ========
   int k=0,m=0;
   float sum = 0.;
   for(i=0;i<=nSub;i++)
   {
     float ftemp = i*subInt+subInt/2;
     printf("%3d %5.1f %d\n",i,ftemp,g_nfreq[i]);
     if(g_nfreq[i] > thresholdA)
     {
         k++;
         g_state[k] = ftemp;
         float diff = ftemp - g_state[k-1];
         //printf("%3d %3d %3d %5.1f %5.1f\n",k,i,g_nfreq[i],ftemp,diff);
         if(diff > subInt+1)
         {
           printf("***%3d %3d %5.1f %5.1f\n",k,i,ftemp,diff);
           sum = sum +diff;
         }
         
     }
   }
   int ndev = (int)(log(k*1.0)/log(2.));	

   printf("Number of devices(2log) = %d\n",ndev);
   printf("Max Power Level = %f (min=%f max=%f)\n",sum,g_pmin,g_pmax);

}
// End of file
