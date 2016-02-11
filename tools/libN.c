//======================================
// libN.c
// apt-get install ncurses-dev
// gcc -o libN libN.c -lncurses -lm
// Version: 2016-01-30
//======================================
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define DATA_MAX 86401
//#define X_MAX 500
//#define Y_MAX 500

float g_pi=3.1415926;
int g_hour,g_minute,g_second;
int g_dataSource = 1;
int g_mode = 1;
int g_display,g_nData,g_selectedFile,g_zn;


float g_xmin=9999.,g_xmax=0.,g_ymin=9999.,g_ymax=0.;
float g_wxmax=0., g_wxmin=999999.,g_wymax=0., g_wymin=999999.;
//char str[40],sstr[20];
//char command[40][40];
char g_data_file[120];
int g_selected_file = 1,g_chan = 1;
//char g_area[X_MAX][Y_MAX];
float g_xdata[DATA_MAX],g_ydata[DATA_MAX],g_der[DATA_MAX];
float g_xvalue[DATA_MAX];
int g_NX,g_NY,g_X,g_Y,g_freq[DATA_MAX];

char g_errMsg[250];
//========================================
void lib_writeErrMsg(char msg[])
//========================================
{
    strcpy(g_errMsg,"  \0");
    strcpy(g_errMsg,msg);
    return;
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
int lib_readNilmFile(int mode,char *selectedFile)
//========================================
{
    FILE *in;
    int i,n,skip = 0,hour,minute,sec;
    float t,t_prev,old_t;
    float x,x1,x2,old_x,v;    
    char line[80],ttime[80];
    
       for(i=0;i<DATA_MAX;i++)
       {
          g_xdata[i] = 0.0;
          g_ydata[i] = 0.0;
          g_xvalue[i] = 0.0;
       }
       g_xmin=9999.;
       g_xmax=0.;
       g_ymin=9999.;
       g_ymax=0.;
    

       in = fopen(selectedFile,"r");
       if(in == NULL)
       {
         n = 0;
    //printf("Error: No data file\n");  
         sprintf(g_errMsg,"Error: Unable to open file=%s",selectedFile);
         g_display = 0; 
       }
       else
       {
          n = 0;
          while (fgets(line,sizeof line,in) != NULL)
          {
             n++;
             if(n > DATA_MAX)
             {
              //printf("**** Warning: Max data range reached ****");
              sprintf(g_errMsg,"Warning: Max data range reached %d - Data truncated",n);
              return(n);
             }
             sscanf(line,"%s %f %f",ttime,&x1,&x2);
             sscanf(ttime,"%d:%d:%d",&hour,&minute,&sec);

             //if(g_chan == 1) 
             //{
                t = hour*3600+minute*60+sec;
                v = x1;
             //}
             g_xvalue[(int)t] = v;
             g_xdata[n] = t*1.0;
             g_ydata[n] = v;
               
            if(g_ymax < v)g_ymax = v;
            if(g_ymin > v)g_ymin = v;
            if(g_xmax < t)g_xmax = t;
            if(g_xmin > t)g_xmin = t;
         }
    
        fclose(in);
        for(i=1;i<DATA_MAX;i++)
        {
            if(g_xvalue[i] == 0.0) g_xvalue[i] = g_xvalue[i-1];
        }
      } 
    g_xdata[0] = n;
    sprintf(g_errMsg,"Info: %d data read from %s",n,selectedFile);
return(n);
}

//========================================
float lib_stepFunction(float s,float h,float x)
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
    //printf("c1 %f\n",x);
    float y;
    if(x >= s) y = h;
    if(x < s)  y = 0.0;
    //printf("c2\n");
    return(y);    
}
//========================================
float lib_squareFunction(float s,float h,float d,float x)
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
    //printf("b1 %f\n",x);
    float y,temp1,temp2;
    temp1 = s+d;
    temp2 = -h;
    y = lib_stepFunction(s,h,x) + lib_stepFunction(temp1,temp2,x);
    //printf("b2\n");
    return(y);    
}
//========================================
float lib_squareWave(float s,float h,float T,float p, float x)
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
    
    float y,temp;
 
    float z = floor(x/T); x = x-z*T;
    temp = T-p;
    y = lib_squareFunction(s,h,temp,x);
    //printf("z=%f y=%f s=%f h=%f T=%f p=%f x=%f temp=%f\n",z,y,s,h,T,p,x,temp);
    //printf("a2\n");   
    return(y);    
}
    //========================================
int lib_generatedData(char fileName [],int left, int right)
//========================================
{
        FILE *out;
int i,n,skip = 0,hour,minute,sec;
float x,x1,x2,old_x,v,t;    
char line[80],ttime[80];
char fName[120];

for(i=0;i<DATA_MAX;i++)
{
        g_xdata[i] = 0.0;
        g_ydata[i] = 0.0;
}
g_xmin=9999.;
g_xmax=0.;
g_ymin=9999.;
g_ymax=0.;
    
strcpy(fName,fileName);
strcat(fName,".nilm");

out = fopen(fName,"w");
n = 0;
 
for(i=left;i<right;i++)
{
  n++;
  t = i;
  //t = g_pi*i/200;
  //v = sin(g_pi*i/200)*t*t;
  v = 
      lib_squareWave(10.0,1100.0, 110.0,50.0, t)+
      lib_squareWave(20.0,200.0, 1200.0,50.0, t)+
      lib_squareWave(30.0,2300.0, 300.0,50.0, t)+
      lib_squareWave(40.0,400.0, 1200.0,50.0, t)+
      lib_squareWave(50.0,5100.0, 500.0,50.0, t);
      
  g_xdata[n] = t;
  g_ydata[n] = v;
  if(g_ymax < v)g_ymax = v;
  if(g_ymin > v)g_ymin = v;
  if(g_xmax < t)g_xmax = t;
  if(g_xmin > t)g_xmin = t;
  lib_valToHourMinSec(t);
  //fprintf(out,"%d %f %f\n",n,t,v);
  fprintf(out,"%d:%d:%d %f \n",g_hour,g_minute,g_second,v);
}
fclose(out);

return(n);
}