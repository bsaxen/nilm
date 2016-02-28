//===========================================
// nilm_filter.c
// 2016-02-22
//===========================================
#include "nilm_lib.c"
#define MAX_Y  99000

int ndata;
// read raw data

// optional filtering

// guess state levels

//=============================================
int getSpectrum(int center,int delta)
//=============================================
{
  int i,p_low,p_high,res=0;
  p_low = center - delta;
  p_high = center + delta;
  
  for(i=0;i<ndata;i++)
  {
    if(g_yData[i] > p_low && g_yData[i] < p_high)res++;
  }
  return(res);
}

//=============================================
int main(int argc, char **argv)
//=============================================
{
  int i,j,k,kmax;
  int freq[MAX_Y];
  int center[MAX_Y],maximum[MAX_Y],deviation[MAX_Y];
  int x[MAX_DATA],y[MAX_DATA];
  int dev=0; 
  char inpFile[120];
  
  if(argc == 2)
  {
       sscanf(argv[1],"%s",inpFile);
  }
    
  ndata = lib_readNilmFile(inpFile);    
  // First - interpolate y values for all x (seconds)
  
  printf("yMin=%d yMax=%d\n",g_yMin,g_yMax);
  for(j=g_yMin;j<=g_yMax;j++)
  {
    center[j] = getSpectrum(j,10);
    //printf("%d center=%d\n",j,center[j]);
  }

  // find maximums
  k = 0;
  int win = 3;
  for(j=g_yMin+win;j<=g_yMax-win;j++)
  {
    if(center[j-win] < center[j] && center[j+win] < center[j] && center[j] > 100 )
    {
      printf("%d maximum=%d\n",k,j);
      maximum[k] = j;
      k++;
    }
  }
  kmax = k;

  // find deviations for each maximum
  for(k=0;k<kmax;k++)
  {
    deviation[k] = 0;
    i = 0;
    dev = 0;
    while(dev < 20 && i < 100)
    {
      i++;
      dev = dev + getSpectrum(maximum[k],i);
    }
    deviation[k] = i;
  }

  for(i=1;i<ndata;i++)
  {
    y[i] = 0;
    for(k=0;k<kmax;k++)
    {
      int qlow = maximum[k] - 100;
      int qhigh = maximum[k] + 100;
      //printf("check %d low=%d high=%d %d\n",i,qlow,qhigh,g_yData[i]);
      if(g_yData[i] > qlow && g_yData[i] < qhigh)
      {
        y[i] = maximum[k]; // associate system state to y-value
          //printf("*****%d %d\n",i,y[i]);
      }
      else if(y[i] == 0)
        y[i] = y[i-1]; // if no system state association, set previuos association
    }
    printf("smooth %d %d %d\n",i,y[i],g_yData[i]); // pre-processed data, TBD convert i to hh:mm:ss
  }
// based on state levels - generate new data
}
// End of file
