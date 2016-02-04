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
float fSub = 50.;
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

int state_transition_from[DATA_MAX];
int state_transition_to[DATA_MAX];
int M[2000][2000];
//===========================================
void init()
//===========================================
{
	int i;
	for(i=0;i<DATA_MAX;i++) 
    {
        g_nfreq[i] = 0;
        g_time[i] = 0;
        g_state[i] = 0;
        state_transition_from[i] = 0;
        state_transition_to[i] = 0;
    }
}

//===========================================
void stateFinderFunc()
//===========================================
{
   int i,j;
   float left,right;
   n_data = lib_readNilmFile(1,"../../nytomta/data-302-2016-01-13.nbc");
   //n_data = lib_readNilmFile(1,"autoGraph.nilm");
   g_pmax = g_ymax;
   g_pmin = g_ymin; 

   nSub = (int)((g_pmax - g_pmin)/fSub);
   //printf("min=%f max=%f\n",g_pmin,g_pmax);


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
   for(i=0;i<=nSub;i++)
   {
     float ftemp = i*subInt+subInt/2;
     if(g_nfreq[i] > thresholdA)
     {
         k++;
         g_state[k] = ftemp;
         float diff = ftemp - g_state[k-1];
         //printf("state=%3d sub=%3d freq=%3d value=%5.1f diff=%5.1f\n",k,i,g_nfreq[i],ftemp,diff);
     }
   }
   n_state = k;
   printf("Data = %d fSub=%f threshold=%d filter=%d => states=%d\n",n_data,fSub,thresholdA,g_filter,n_state);
}
//===========================================
int getState(float x)
//===========================================
{
    int i,j=0,res;
    for(i=1;i<=n_state;i++)
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
    //printf("State transitions = %d\n",k);
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
            if(M[i][j] > g_filter)printf("%3.0f ",ftemp);
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
