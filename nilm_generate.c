//===========================================
// nilm_generate.c
// gcc -o nilm_generate nilm_generate.c -lm
// 2016-02-16
//===========================================

#include "nilm_lib.c"

//==========================================
int main(int argc, char **argv)
//==========================================
{
    char devModFile[120];
    int i,j,t,v,left,right,s,d,p,h,low;
    
    lib_nilmInit();
    
    if(argc == 1)
    {
       left = 0;
       right = 3600*24;
       strcpy(devModFile,"default_device_model.dm");
    }
    if(argc == 2)
    {
       sscanf(argv[1],"%s",devModFile);
       left = 0;
       right = 3600*24;
    }
    if(argc == 3)
    {
       sscanf(argv[1],"%d",&left);
       sscanf(argv[2],"%d",&right);
    }
    if(argc == 4)
    {
       sscanf(argv[1],"%s",devModFile);
       sscanf(argv[2],"%d",&left);
       sscanf(argv[3],"%d",&right);
    }
    
    printf("Generate data left=%d right=%d model=%s\n",left,right,devModFile);
 // read device model
    
    int n = lib_readDeviceModel(devModFile);
    
    if(n == 0)
        lib_writeDeviceModel(devModFile);
    else
        g_nDevices = n;
    
    //exit(0);
    
    n = 0;
    for(i=left;i<=right;i++)
    {
      n++;
      t = i;
      v = 0;
        //float lib_squareWave(float s,float h,float T,float p, float x)
//========================================

    //              d
    //        --------------         -----------
    //        |            |         |
    //       h|            |         |
    //        |            |   p     |
    //---------            -----------
    //        s
    //
    // Period T = d+p

    int sum = 0;
    for(j=0;j<g_nDevices;j++)
      {
        s = g_device[j][IX_DEVICE_START];
        h = g_device[j][IX_DEVICE_HEIGHT];
        d = g_device[j][IX_DEVICE_DURATION];
        p = g_device[j][IX_DEVICE_PERIOD];
        low = p - d;
        v = lib_squareWave(s,h,p,low,t);
        sum = sum + v;
        printf("sum=%d v=%d %d %d %d %d %d %d\n",sum,v,j,s,h,p,low,t);
      }
      
      g_xData[n] = t;
      g_yData[n] = v;
      if(g_yMax < v)g_yMax = v;
      if(g_yMin > v)g_yMin = v;
      if(g_xMax < t)g_xMax = t;
      if(g_xMin > t)g_xMin = t;
      lib_valToHourMinSec(t);
      printf("%d %d:%d:%d %d \n",i,g_hour,g_minute,g_second,v);
   }
}
