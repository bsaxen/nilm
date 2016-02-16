//===========================================
// nilm_lib.c
// 2016-02-16
//===========================================
#include <stdio.h>
#include <stdlib.h>

#define MAX_DEVICES        10

#define N_DEVICE_PAR        6
#define IX_DEVICE_ID        0
#define IX_DEVICE_HEIGHT    1
#define IX_DEVICE_DURATION  2
#define IX_DEVICE_PERIOD    3
#define IX_DEVICE_START     4
#define IX_DEVICE_SUBDEV    5

// Global variables
int g_device[MAX_DEVICES][N_DEVICE_PAR];

//===========================================
void lib_nilmError(char msg[], int errno)
//===========================================
{
  printf("NILM ERROR: %s errno=%d",msg,errno);
  return;
}
//===========================================
int lib_checkValue(char par[], int value)
//===========================================
{
  // Add check here ...
  return(value);
}
//===========================================
int lib_readDeviceModel(char filename[])
//===========================================
{
  int i,j,n,pc[N_DEVICE_PAR];
  FILE* fp;
  char buf[bufSize],stemp[20][20];
  
  if ((fp = fopen(filename, "r")) == NULL)
  { 
    lib_nilmError("Unable to open Device Model file",0);
    return;
  }

  strcpy(stemp[IX_DEVICE_ID],"DEVICE_ID");
  strcpy(stemp[IX_DEVICE_HEIGHT],"DEVICE_HEIGHT");
  strcpy(stemp[IX_DEVICE_DURATION],"DEVICE_DURATION");
  strcpy(stemp[IX_DEVICE_PERIOD],"DEVICE_PERIOD");
  strcpy(stemp[IX_DEVICE_START],"DEVICE_START");
  strcpy(stemp[IX_DEVICE_SUBDEV],"DEVICE_SUBDEV");

  n = 0;
  for(j=0;j<N_DEVICE_PAR;j++)pc[j] = 0;
  
  while (fgets(buf, sizeof(buf), fp) != NULL)
  {
    buf[strlen(buf) - 1] = '\0';
    for(i=0;i<N_DEVICE_PAR;i++)
    {
      sscanf(buf,"%s %d",key,&itemp);
      if(strstr(stemp[i],key) != NULL)
      {
        res = lib_checkValue(stemp[i],itemp);
        g_device[n][i] = res;
        pc[i]++;
        if(i == IX_DEVICE_ID)
        {
          n++;
          for(j=0;j<N_DEVICE_PAR;j++)pc[j] = 0;
        }
      }
    }
    printf("Number of devices read: %d\n", n);
    for(j=0;j<N_DEVICE_PAR;j++)
    {
      if(pc[j] > 1) lib_nilmError("Device Parameter multiple reading",j);
    }
  }
  fclose(fp);
  return(n);
}
