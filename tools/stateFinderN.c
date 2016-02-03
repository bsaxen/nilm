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
//================================
float fSub = 1.;
int thresholdA = 1;
//================================
int nSub;
int g_nfreq[DATA_MAX], g_time[DATA_MAX];
//float g_data[DATA_MAX];
float g_state[DATA_MAX];
//float g_state2[DATA_MAX];

int state_transition_from[DATA_MAX];
int state_transition_to[DATA_MAX];
int M[2000][2000];
//===========================================
void init()
//===========================================
{
	int i;
	for(i=0;i<1000;i++) g_nfreq[i] = 0;
}

//===========================================
void stateFinderFunc()
//===========================================
{
   int i,j;
   float left,right;
   n_data = lib_readNilmFile(1,"autoGraph.nilm");
   g_pmax = g_ymax;
   g_pmin = g_ymin; 

   nSub = (int)((g_pmax - g_pmin)/fSub);
   printf("min=%f max=%f\n",g_pmin,g_pmax);
   printf("Number of data = %d fSub=%f nSub=%d\n",n_data,fSub,nSub);

   float subInt = fSub;//(g_pmax- g_pmin)/nSub;
   printf("subInt=%f\n",subInt);

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
   for(i=0;i<=nSub;i++)
   {
     float ftemp = i*subInt+subInt/2;
     if(g_nfreq[i] > thresholdA)
     {
         k++;
         g_state[k] = ftemp;
         float diff = ftemp - g_state[k-1];
         printf("state=%3d sub=%3d freq=%3d value=%5.1f diff=%5.1f\n",k,i,g_nfreq[i],ftemp,diff);
     }
   }
   n_state = k;
}
//===========================================
int getState(float x)
//===========================================
{
    int i,j=0,res;
    for(i=1;i<=n_state;i++)
    {
        //printf("%d %f %f\n",i,x,g_state[i]);
        if(abs(g_state[i] - x) <= 0.5 )
        {
            j++;
            res = i;
            //printf("**** %d %f %f\n",i,x,g_state[i]);
        }
    }
    if(j==0)printf("No state found for %f\n",x);
    if(j>1)printf("Multiple states found for %f\n",x);
    //printf("Number of states %d\n",g_k);
    return(res);
}

//===========================================
void stateSeqFinderFunc()
//===========================================
{
    int i,k,st=0,prev_st=0;
    
    for(i=1;i<=n_data;i++)
    {
        prev_st = st;
        st = getState(g_ydata[i]);
        if(st != prev_st)
        {
            k++;
            state_transition_from[k] = prev_st;
            state_transition_to[k] = st;
        }
    }
    printf("State transitions = %d\n",k);
    n_trans = k;
}
//===========================================
void stateFreqFinderFunc()
//===========================================
{
    int i,j;
    float ftemp;
    for(i=1;i<=n_state;i++)
    {
        for(j=1;j<=n_state;j++)M[i][j] = 0;
    }
    for(i=1;i<=n_trans;i++)
    {
        M[state_transition_from[i]][state_transition_to[i]]++;
    }
    
    printf("   ");
    for(j=1;j<=n_state;j++)printf("%3d ",j);
    printf("\n");
    for(i=1;i<=n_state;i++)
    {
        printf("%2d ",i);
        for(j=1;j<=n_state;j++)printf("%3d ",M[i][j]);
        printf("\n");
    }
    
    printf("\n");
    printf("   ");
    for(j=1;j<=n_state;j++)printf("%3d ",j);
    printf("\n");
    for(i=1;i<=n_state;i++)
    {
        printf("%2d ",i);
        for(j=1;j<=n_state;j++)
        {
            ftemp = (g_state[i]-g_state[j])/10;
            ftemp = abs(ftemp);
            if(M[i][j] > 30)printf("%3.0f ",ftemp);
            else 
                printf("    ");
        }
        printf("\n");
    }
}
//===========================================
//===========================================
main()
//===========================================
//===========================================
{
   init();
   stateFinderFunc();
   stateSeqFinderFunc();
   stateFreqFinderFunc();
}
// End of file
