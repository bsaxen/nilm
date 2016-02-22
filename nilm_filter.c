//===========================================
// nilm_filter.c
// 2016-02-22
//===========================================
#include "nilm_lib.c"
#define MAX_Y  20000
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
    if(ydata[i] > p_low && ydata[i] < p_high)res++;
  }
  return(res);
}

//=============================================
main ()
//=============================================
{
  int i,j,k,kmax;
  int freq[MAX_Y];
  int center[MAX_Y],maximum[MAX_Y],deviation[MAX_Y];
  int x[MAX_DATA];
  int dev=0; 
  
  for(j=ymin;j<=ymax;j++)
  {
    center[j] = getSpectrum(j,1);
  }

  // find maximums
  k = 0;
  for(j=ymin+1;j<ymax;j++)
  {
    if(center[j-1] < center[j] && center[j+1] < center[j]  )
    {
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

  for(i=0;i<ndata;i++)
  {
    y[i] = 0;
    for(k=0;k<kmax;k++)
    {
      if(ydata[i] > maximum[k] - deviation[k] && ydata[i] < > maximum[k] + deviation[k])
        y[i] = maximum[k];
      else
        y[i] = y[i-1];
    }
    printf("%d %d\n",i,y[i]); // pre-processed data
  }
// based on state levels - generate new data
}
// End of file
