//======================================
// stateFinderN.c
// apt-get install ncurses-dev
// gcc -o stateFinderN stateFinderN.c -lncurses -lm
// Version: 2016-02-03
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

float g_pmin,g_pmax;
int n_trans = 0;
int n_state = 0;
int n_data = 0;
int n_dev = 0;

//================================
float fSub = 1.;
int thresholdA = 50;
int g_filter = 0;
//float fSub = 1.;
//int thresholdA = 1;
//================================
int nSub;
int g_nfreq[DATA_MAX], g_time[DATA_MAX];
//float g_data[DATA_MAX];
float g_state[DATA_MAX];
//float g_state2[DATA_MAX];
float g_device[DATA_MAX];
int g_device_duration[DATA_MAX];

int state_transition_from[DATA_MAX];
int state_transition_to[DATA_MAX];
int state_transition_time[DATA_MAX];
float state_transition_delta[DATA_MAX];
int M[2000][2000];
//===========================================
void init()
//===========================================
{
	int i;
    
    n_dev = 0;
    n_trans = 0;
    n_state = 0;

	for(i=0;i<DATA_MAX;i++) 
    {
        g_nfreq[i] = 0;
        g_time[i] = 0;
        g_state[i] = 0;
        g_device[i] = 0.0;
        g_device_duration[i] = 0;
        state_transition_from[i] = 0;
        state_transition_to[i] = 0;
        state_transition_time[i] = 0;
        state_transition_delta[i] = 0.0;
    }
}

//===========================================
void stateFinderFunc()
//===========================================
{
   int i,j;
   float left,right;
   //n_data = lib_readNilmFile(1,"../../nytomta/data-302-2016-01-13.nbc");
   n_data = lib_readNilmFile(1,"autoGraph.nilm");
   g_pmax = g_ymax;
   g_pmin = g_ymin; 
   nSub = (int)((g_pmax - g_pmin)/fSub);
   printf("min=%f max=%f\n",g_pmin,g_pmax);


   float subInt = fSub;//(g_pmax- g_pmin)/nSub;
   //printf("subInt=%f\n",subInt);

   for(i=0;i<=nSub;i++)
   {
        left = g_pmin +i*subInt;
        right = g_pmin + (i+1)*subInt;
        for(j=1;j<=n_data;j++)
        {
	     if(g_ydata[j]>left && g_ydata[j]<=right) g_nfreq[i]++;
        }
   }
   int k=0,m=0,check;
   float sum = 0.;
   g_state[0] = g_ymin;
   for(i=0;i<=nSub;i++)
   {
     float ftemp = i*subInt+subInt/2;
     if(g_nfreq[i] > thresholdA)
     {
         k++;
         g_state[k] = ftemp + (int)(fSub/2);
         //printf("state=%3d sub=%3d freq=%3d value=%5.1f\n",k,i,g_nfreq[i],ftemp);
     }
   }
   printf("Bias = %f\n",g_ymin);
   n_state = k;
   printf("Data = %d fSub=%f threshold=%d filter=%d => states=%d\n",n_data,fSub,thresholdA,g_filter,n_state);
}
//===========================================
int getState(float x)
//===========================================
{
    int i,j=0,res;
    for(i=0;i<=n_state;i++)
    {
        //printf("%d %f %f\n",i,x,g_state[i]);
        if(abs(g_state[i] - x) <= fSub )
        {
            j++;
            res = i;
            //printf("**** %d %f %f\n",i,x,g_state[i]);
        }
    }
    //if(j==0)printf("No state found for %f\n",x);
    //if(j>1)printf("Multiple(%d) states found for %f\n",j,x);
    //printf("Number of states %d\n",g_k);
    return(res);
}

//===========================================
void stateSeqFinderFunc()
//===========================================
{
    int i,k=0,st=0,prev_st=0;
    float delta,sum=0;
    
    st = getState(g_ymin); //bias
    for(i=1;i<=n_data;i++)
    {
        prev_st = st;
        st = getState(g_ydata[i]);
        if(st != prev_st)
        {
            k++;
            delta = g_state[st] - g_state[prev_st];
            sum = sum + delta;
            state_transition_from[k] = prev_st;
            state_transition_to[k]   = st;
            state_transition_time[k] = i;
            state_transition_delta[k] = delta;
        }
    }
    prev_st = st;
    st = getState(g_ymin); //bias
    if(st != prev_st)
    {
            k++;
            delta = g_state[st] - g_state[prev_st];
            sum = sum + delta;
            state_transition_from[k]  = prev_st;
            state_transition_to[k]    = st;
            state_transition_time[k]  = i;
            state_transition_delta[k] = delta;
    }
    
    printf("%s State transitions = %d sum=%f\n",KBLU,k,sum);
    n_trans = k;
}
//===========================================
int addDevice(float x)
//===========================================
{
    int i;
    for(i=1;i<=n_dev;i++)
    {
        if(g_device[i] == x) return(n_dev);
    }
    n_dev++;
    g_device[n_dev] = x;
    //printf("%s Add device %d %f\n",KYEL,n_dev,x);
    return(n_dev);
}
//===========================================
void listDevices()
//===========================================
{
    int i;
    for(i=1;i<=n_dev;i++)
    {
        printf("%s List device %d %5.0f duration=%d\n",KRED,i,g_device[i],g_device_duration[i]);
    }
}
//===========================================
void getDeviceEnergy()
//===========================================
{
    int i,j,itemp,t1,t2,n;
    float delta; 
    
    for(i=1;i<=n_dev;i++)
    {
        //printf("Check %d %f\n",i,g_device[i]);
        n = 0;
        for(j=1;j<=n_trans;j++)
        {
           delta = state_transition_delta[j];
        
           if(g_device[i] == delta)
           {
              //printf("left %d %f\n",j,g_device[i]);
              t1 = state_transition_time[j];
           }
           if(g_device[i] == -delta)
           {
              t2 = state_transition_time[j];
              itemp = t2 - t1;
              //printf("**** %d %f  delta = %d\n",j,g_device[i],itemp);
              g_device_duration[i] += itemp;
              n++;
           }
            
        }
        g_device_duration[i] = g_device_duration[i]/n;
        //printf("%sEnergy device %d %5.0f duration = %d\n",KRED,i,g_device[i],g_device_duration[i]);
    }
}
//===========================================
void stateFreqFinderFunc()
//===========================================
{
    int i,j,t1=0,t2=0,itemp;
    float ftemp;
    for(i=0;i<=n_state;i++)
    {
        for(j=0;j<=n_state;j++)M[i][j] = 0;
    }
    for(i=1;i<=n_trans;i++)
    {
        M[state_transition_from[i]][state_transition_to[i]]++;
    }
    
    printf("   ");
    for(j=0;j<=n_state;j++)printf("%3d ",j);
    printf("\n");
    for(i=0;i<=n_state;i++)
    {
        printf("%2d ",i);
        for(j=0;j<=n_state;j++)printf("%3d ",M[i][j]);
        printf("%5.0f\n",g_state[i]);
    }
    
    printf("\n");
    printf("   ");
    for(j=0;j<=n_state;j++)printf("%s%3d ",KNRM,j);
    printf("\n");
    for(i=0;i<=n_state;i++)
    {
        printf("%2d ",i);
        for(j=0;j<=n_state;j++)
        {
            ftemp = (g_state[j]-g_state[i]);
            //ftemp = abs(ftemp);
            if(M[i][j] > g_filter)
            {
                if(ftemp < 0.0)
                {
                    ftemp = abs(ftemp);
                    printf("%s%3.0f ",KYEL,ftemp);
                }
                else
                {
                    printf("%s%3.0f ",KGRN,ftemp);
                    n_dev = addDevice(ftemp);
                }
            }
            else 
                printf("    ");
        }
        printf("%s%5.0f\n",KNRM,g_state[i]);
    }
    printf("benny %d\n",n_dev);
    //getDeviceEnergy();
    listDevices();
    
}
//===========================================
//===========================================
main()
//===========================================
//===========================================
{
    int ch;

    while(1)  
    {
        init();
        stateFinderFunc();
        stateSeqFinderFunc();
        stateFreqFinderFunc();
        ch = getchar();
        if (ch=='a')
	    {
            fSub++;
	    }
        else if (ch=='z')
        {
            fSub--;
        }
        else if (ch=='s')
        {
            thresholdA++;
        }
        else if (ch=='x')
        {
            thresholdA--;
        }
        else if (ch=='d')
        {
            g_filter++;
        }
        else if (ch=='c')
        {
            g_filter--;
        }

      }
}
// End of file
