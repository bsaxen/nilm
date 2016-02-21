//===========================================
// nilm_identify.c
// apt-get install ncurses-dev
// gcc -o nilm_identify nilm_identify.c -lncurses -lm
// 2016-02-21
//===========================================
#include "nilm_lib.c"

WINDOW *graph1,*graph2,*data,*misc,*feedback;
static struct termios orig, nnew;

#define GRAPH1_COLOR 6
#define GRAPH2_COLOR 8
#define DATA_COLOR 7
#define MISC_COLOR 5
#define FEEDBACK_COLOR 4
#define WORLD 1000

#define RF 1 
#define ER 1
#define SR 20

int graph1_h=0, graph1_w=0, graph1_x=0, graph1_y=0;
int graph2_h=0, graph2_w=0, graph2_x=0, graph2_y=0;
int data_h=0, data_w=0, data_x=0, data_y=0;
int misc_h=0, misc_w=0, misc_x=0, misc_y=0;
int feedback_w=0,feedback_h=0,feedback_x=0,feedback_y=0;
int ap,dp,s_row,s_col,pos=0;

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
int g_nfreq[MAX_DATA], g_time[MAX_DATA];
float g_state[MAX_DATA];
int g_device_duration[MAX_DATA];
int state_transition_from[MAX_DATA];
int state_transition_to[MAX_DATA];
int state_transition_time[MAX_DATA];
float state_transition_delta[MAX_DATA];
int M[2000][2000];

char g_loadedFile[120];
char g_errMsg[240];
int g_nd = 3; // number of digits in matrix display
//====================================
void displayErrMsg()
//====================================
{
    int i;
    char stemp[250];
    wmove(feedback,1,1);
    clrtoeol();
    wprintw(feedback,g_errMsg);
    int slen = strlen(g_errMsg);
    if(slen < feedback_w)
    {
        slen++;
        wmove(feedback,1,slen);
        for(i=0;i<feedback_w-slen;i++) stemp[i] = ' ';
        stemp[i] = '\0';
        wprintw(feedback,stemp);
    }
}
//===========================================
void init()
//===========================================
{
	int i;
    
    n_dev = 0;
    n_trans = 0;
    n_state = 0;

	for(i=0;i<MAX_DEVICES;i++) 
    {
        g_device[i][IX_DEVICE_HEIGHT] = 0.0;
        g_device_duration[i] = 0;
    }
    for(i=0;i<MAX_DATA;i++) 
    {
        g_nfreq[i] = 0;
        g_time[i] = 0;
        g_state[i] = 0;
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

   n_data = lib_readNilmFile("generatedData.nilm");
   g_pmax = g_yMax;
   g_pmin = g_yMin; 
   nSub = (int)((g_pmax - g_pmin)/fSub);
   wmove(misc,4,2);wprintw(misc,"min=%0.f max=%.0f\n",g_pmin,g_pmax);
    
    if(g_pmax < 10000) g_nd = 4;
    if(g_pmax < 1000) g_nd = 3;
    if(g_pmax < 100) g_nd = 2;
    if(g_pmax < 10) g_nd = 1;

   float subInt = fSub;//(g_pmax- g_pmin)/nSub;
   //printf("subInt=%f\n",subInt);

   for(i=0;i<=nSub;i++)
   {
        left = g_pmin +i*subInt;
        right = g_pmin + (i+1)*subInt;
        for(j=1;j<=n_data;j++)
        {
	     if(g_yData[j]>left && g_yData[j]<=right) g_nfreq[i]++;
        }
   }
   int k=0,m=0,check;
   float sum = 0.;
   g_state[0] = g_yMin;
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
   wmove(misc,1,2);wprintw(misc,"Bias = %d\n",g_yMin);
   n_state = k;
   wmove(misc,2,2);wprintw(misc,"Data = %d fSub=%.0f threshold=%d filter=%d => states=%d\n",n_data,fSub,thresholdA,g_filter,n_state);
}
//===========================================
int getState(float x)
//===========================================
{
    int i,j=0,res;
    for(i=0;i<=n_state;i++)
    {
        if(abs(g_state[i] - x) <= fSub )
        {
            j++;
            res = i;
        }
    }
    return(res);
}

//===========================================
void stateSeqFinderFunc()
//===========================================
{
    int i,k=0,st=0,prev_st=0;
    float delta,sum=0;
    
    st = getState(g_yMin); //bias
    for(i=1;i<=n_data;i++)
    {
        prev_st = st;
        st = getState(g_yData[i]);
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
    st = getState(g_yMin); //bias
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
    
    wmove(misc,3,2);wprintw(misc,"State transitions = %d sum=%.0f\n",k,sum);
    n_trans = k;
}
//===========================================
int addDevice(float x)
//===========================================
{
    int i;
    for(i=1;i<=n_dev;i++)
    {
        if(g_device[i][IX_DEVICE_HEIGHT] == x) return(n_dev);
    }
    n_dev++;
    g_device[n_dev][IX_DEVICE_HEIGHT] = x;
    return(n_dev);
}
//===========================================
void listDevices()
//===========================================
{
    int i;
    for(i=1;i<=n_dev;i++)
    {
        wmove(data,i,2);wprintw(data,"List device %d %d duration=%d\n",i,g_device[i][IX_DEVICE_HEIGHT],g_device_duration[i]);
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
        n = 0;
        for(j=1;j<=n_trans;j++)
        {
           delta = state_transition_delta[j];
        
           if(g_device[i][IX_DEVICE_HEIGHT] == delta)
           {
              t1 = state_transition_time[j];
           }
           if(g_device[i][IX_DEVICE_HEIGHT] == -delta)
           {
              t2 = state_transition_time[j];
              itemp = t2 - t1;
              g_device_duration[i] += itemp;
              n++;
           }
            
        }
        g_device_duration[i] = g_device_duration[i]/n;
    }
}

//===========================================
void nilm_wmove(WINDOW *win,int i, int j, int step, int nd)
//===========================================
{
    wmove(win,i,j+step*(g_nd+1));
}
//===========================================
void nilm_printw_int(WINDOW *win, int x, int nd)
//===========================================
{
    if(nd == 1) wprintw(win,"%1d ",x);
    if(nd == 2) wprintw(win,"%2d ",x);
    if(nd == 3) wprintw(win,"%3d ",x);
    if(nd == 4) wprintw(win,"%4d ",x);
}
//===========================================
void nilm_printw_float(WINDOW *win, float x, int nd)
//===========================================
{
    if(nd == 1) wprintw(win,"%1.0f ",x);
    if(nd == 2) wprintw(win,"%2.0f ",x);
    if(nd == 3) wprintw(win,"%3.0f ",x);
    if(nd == 4) wprintw(win,"%4.0f ",x);
}


//===========================================
void nilm_printw_space(WINDOW *win, int nd)
//===========================================
{
    if(nd == 1) wprintw(win,"  ");
    if(nd == 2) wprintw(win,"   ");
    if(nd == 3) wprintw(win,"    ");
    if(nd == 4) wprintw(win,"     ");
}
//===========================================
void stateFreqFinderFunc()
//===========================================
{
    int i,j,t1=0,t2=0,itemp;
    int ftemp;
    for(i=0;i<=n_state;i++)
    {
        for(j=0;j<=n_state;j++)M[i][j] = 0;
    }
    for(i=1;i<=n_trans;i++)
    {
        M[state_transition_from[i]][state_transition_to[i]]++;
    }
    
    //---------------------------------------
    for(j=0;j<=n_state;j++)
    {
        //wmove(graph1,1,5+j*5);
        nilm_wmove(graph1,1,g_nd+1,j,g_nd);
        nilm_printw_int(graph1,j,g_nd);
    }
    //printf("\n");
    for(i=0;i<=n_state;i++)
    {
        wmove(graph1,2*i+2,1);
        nilm_printw_int(graph1,i,g_nd);
        for(j=0;j<=n_state;j++)
        {
            //wmove(graph1,2*i+2,5+j*5);
            nilm_wmove(graph1,2*i+2,g_nd+1,j,g_nd);
            
            
            //**********************************
            if(M[i][j] > 0)
                nilm_printw_int(graph1,M[i][j],g_nd);
            else
                nilm_printw_space(graph1, g_nd);
            
            //**********************************
            
            
        }
        wmove(graph1,2*i+2,5+j*5);
        nilm_printw_float(graph1, g_state[i], g_nd);
    }
    
    //---------------------------------------

    for(j=0;j<=n_state;j++)
    {
        wmove(graph2,1,5+j*5);
        wprintw(graph2,"%4d ",j);
    }
 
    for(i=0;i<=n_state;i++)
    {
        wmove(graph2,2*i+2,1);wprintw(graph2,"%3d ",i);
        for(j=0;j<=n_state;j++)
        {
            wmove(graph2,2*i+2,5+j*5);
            
            
            
            //**********************************
            ftemp = g_state[j]-g_state[i];
            if(M[i][j] > g_filter)
            {
                if(ftemp < 0)
                {
                    ftemp = abs(ftemp);
                    wprintw(graph2,"%4d*",ftemp);
                }
                else
                {
                    wprintw(graph2,"%4d ",ftemp);
                    n_dev = addDevice(ftemp);
                }
            }
            else 
                wprintw(graph2,"     ");
            //**********************************
            
            
        }
        wmove(graph2,2*i+2,5+j*5);wprintw(graph2,"%5.0f\n",g_state[i]);
    }
    //getDeviceEnergy();
     
}
//====================================
void show(WINDOW *win)
//====================================
{
  int next;
  box(win,0,0);
  if(win == graph1)
  {
     wmove(win,0,2);
     wprintw(win,"Number of State Transitions");
  }
  if(win == graph2)
  {
     wmove(win,0,2);
     wprintw(win,"State Transition Jump");
  }
  if(win == data)
  {
     wmove(win,0,2);
     wprintw(win,"Devices");
  }
  if(win == misc)
  {
     wmove(win,0,2);
     wprintw(win," 2016-02-21 NILM IDENTIFY ");
  }
  if(win == feedback)
  {
     wmove(win,0,2);
     wprintw(win," Feedback ");
  }
  wrefresh(win);
}
//===========================================
//===========================================
main()
//===========================================
//===========================================
{
    int ch = '1';
    int h_level = 20;
    
  initscr();
  clear();
  cbreak();

//strcpy(g_errMsg,"No errors detected");    
    
    
getmaxyx(stdscr,s_row,s_col);
    
start_color();
init_pair(1,COLOR_BLACK,COLOR_BLUE);
init_pair(2,COLOR_BLACK,COLOR_GREEN);
init_pair(3,COLOR_BLUE,COLOR_WHITE); 
init_pair(4,COLOR_RED,COLOR_WHITE); 
init_pair(5,COLOR_MAGENTA,COLOR_WHITE); 
init_pair(6,COLOR_WHITE,COLOR_BLACK); 
init_pair(7,COLOR_WHITE,COLOR_BLUE);
init_pair(8,COLOR_RED,COLOR_BLACK); 
  
  /*     COLOR_BLACK   0 */
  /*     COLOR_RED     1 */
  /*     COLOR_GREEN   2 */
  /*     COLOR_YELLOW  3 */
  /*     COLOR_BLUE    4 */
  /*     COLOR_MAGENTA 5 */
  /*     COLOR_CYAN    6 */
  /*     COLOR_WHITE   7 */
    
  // Graph1 Window   
  graph1_w = s_col/2;
  graph1_h = s_row-h_level-3;
  graph1_x = 0;
  graph1_y = 0;
  graph1=newwin(graph1_h,graph1_w,graph1_x,graph1_y);
  wbkgd(graph1,COLOR_PAIR(GRAPH1_COLOR));
    
  // Graph2 Window   
  graph2_w = s_col/2;
  graph2_h = s_row-h_level-3;
  graph2_x = 0;
  graph2_y = s_col/2;
  graph2=newwin(graph2_h,graph2_w,graph2_x,graph2_y);
  wbkgd(graph2,COLOR_PAIR(GRAPH2_COLOR));
    
  // Device Window   
  data_w = s_col/2;
  data_h = h_level;
  data_x = s_row-h_level;
  data_y = 0;
  data=newwin(data_h,data_w,data_x,data_y);
  wbkgd(data,COLOR_PAIR(DATA_COLOR));
    
  // Misc Window   
  misc_w = s_col/2;
  misc_h = h_level;
  misc_x = s_row-h_level;
  misc_y = s_col/2;
  misc=newwin(misc_h,misc_w,misc_x,misc_y);
  wbkgd(misc,COLOR_PAIR(MISC_COLOR));
    
 // Feedback Window   
  feedback_w = s_col;
  feedback_h = 3;
  feedback_x = s_row-h_level-3;
  feedback_y = 0;
  feedback=newwin(feedback_h,feedback_w,feedback_x,feedback_y);
  wbkgd(feedback,COLOR_PAIR(FEEDBACK_COLOR));
 

    while(ch != 'q')  
    {
        init();
        stateFinderFunc();
        stateSeqFinderFunc();
        stateFreqFinderFunc();
        listDevices();  
      displayErrMsg();
      show(graph1);
      show(graph2); 
      show(data); 
      show(misc);
      show(feedback);         
      wmove(misc,misc_h-2,1);
      wprintw(misc,"                                    ");
      mvwprintw(misc,misc_h-2,1,">");
      sprintf(g_errMsg,"-");       
      wrefresh(misc);
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
      delwin(graph1);
      endwin();
}
// End of file
