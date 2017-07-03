//===========================================
// nilm_backprop.c
// 2017-07-03
//===========================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_OK        0
#define M_NO_FILE   1

#define MAX_LAYERS 10
#define MAX_ROWS   10
#define MAX_COLS   10

struct neuralNetwork {
  int   nlayer;
  int   layer[MAX_LAYERS];
  float relax;
  float nnw[MAX_LAYERS][MAX_ROWS][MAX_COLS];
};

struct neuralNetwork *nn, network1;

int readNeuralNetwork(char *inpFile);
int writeNeuralNetwork(char *outFile);
void showNeuralNetwork();
int forwardCalc(float *inp, float *out);

//=====================================================
//===========================================
int main(int argc, char **argv)
//===========================================
//=====================================================
{
  int i,j,k;
  int res = M_OK;
  char inpFile[80],outFile[80];
  nn = &network1;

  // Create neural network
  if(argc == 1)
  {
    nn->nlayer  = 3;
    nn->layer[0] = 5;
    nn->layer[1] = 8;
    nn->layer[2] = 4;
    nn->relax    = 0.4;

    for (k=1;k<nn->nlayer;k++)
    {
      for (i=1;i<=nn->layer[k-1];i++)
      {
        for(j=1;j<=nn->layer[k];j++)
        {
          nn->nnw[k][i][j] = (float)((rand()%1000)*0.001);
        }
      }
    }

  }
  // Read neural network
  if(argc == 2)
  {
    sscanf(argv[1],"%s",inpFile);
    res = readNeuralNetwork(inpFile);
  }

  showNeuralNetwork();
  // ==========================================

  // Read training data
  float inp1[10] = {0.,1.,1.,1.,1.,1.};
  float out1[10];
  res = forwardCalc(inp1,out1);

  // Train neural network


  // Save neural network
  strcpy(outFile,"temp.txt");
  writeNeuralNetwork(outFile);
  printf("Backprop terminated with result: %d\n",res);
}

//=====================================================
// Functions
//=====================================================

//=====================================================
int forwardCalc(float *inp, float *out)
//=====================================================
{
  int i,j,k;
  int res = M_OK;
  float p[MAX_LAYERS][MAX_ROWS];

  for (j=1;j<=nn->layer[0];j++)
  {
    p[0][j] = inp[j];
    printf("%d %f,",j,p[0][j]);
  }
  printf("\n");

  for (k=1;k<nn->nlayer;k++)
  {
    for (j=1;j<=nn->layer[k];j++)
    {
      p[k][j] = 0.0;
      for(i=1;i<=nn->layer[k-1];i++)
      {
        p[k][j] = p[k][j] + p[k-1][i]*nn->nnw[k][i][j];
      }
    }
  }

  k = nn->nlayer-1;
  for (i=1;i<=nn->layer[k];i++)
  {
    printf("%d %f,",i,p[k][i]);
    out[i] = p[k][i];
  }
  printf("\n");
  return res;
}

//=====================================================
int writeNeuralNetwork(char *outFile)
//=====================================================
{
  int i,j,k;
  int res = M_OK;

  FILE *out;

  printf("Write neural network : %s ....\n",outFile);
  out = fopen(outFile,"w");
  if(out == NULL)
  {
    res = M_NO_FILE;
  }
  else
  {
    fprintf(out,"NLAYER %d\n", nn->nlayer);
    for (i=0;i<nn->nlayer;i++)
    {
      fprintf(out,"C_LAYER %d %d\n", i,nn->layer[i]);
    }
    fprintf(out,"RELAX %f\n", nn->relax);


    for (k=1;k<nn->nlayer;k++)
    {
      fprintf(out,"DATA %5d %5d %5d \n",k,nn->layer[k-1],nn->layer[k]);
      for (i=1;i<=nn->layer[k-1];i++)
      {
        //fprintf(out,"WT ");
        for(j=1;j<=nn->layer[k];j++)
        {
          fprintf(out,"WT %d %d %d %5.3f\n",k,i,j,nn->nnw[k][i][j]);
        }
        //fprintf(out,"\n");
      }
    }
  }

  fclose(out);
  printf("writeNeuralNetwork terminated with result: %d\n",res);
  return res;
}
//=====================================================
int readNeuralNetwork(char *inpFile)
//=====================================================
{
  int i,j,k,n,data_found;
  int layer,rows,cols;
  int index;
  int layer_ix,row_ix,col_ix;
  int res = M_OK;
  int itemp;
  float ftemp;
  char line[80];
  char key[12];
  FILE *in;

  printf("Read neural network : %s ....\n",inpFile);
  in = fopen(inpFile,"r");
  if(in == NULL)
  {
    res = M_NO_FILE;
  }
  else
  {
    n = 0;
    data_found = 0;
    while (fgets(line,sizeof line,in) != NULL)
    {
      n++;
      sscanf(line,"%s",key);
      if(strcmp(key,"#") != 0)
      {
        if (strcmp(key,"RELAX") == 0)
        {
          sscanf(line,"%s %f",key,&ftemp);
          printf("%s %f\n",key,ftemp);
          nn->relax = ftemp;
        }
        if (strcmp(key,"NLAYER") == 0)
        {
          sscanf(line,"%s %d",key,&itemp);
          printf("%s %d\n",key,itemp);
          nn->nlayer = itemp;
        }
        if (strcmp(key,"C_LAYER") == 0)
        {
          sscanf(line,"%s %d %d",key,&index,&itemp);
          printf("%s %d %d\n",key,index,itemp);
          nn->layer[index] = itemp;
        }
        if (strcmp(key,"DATA") == 0)
        {
          data_found = 1;
          sscanf(line,"%s %d %d %d",key,&layer_ix,&rows,&cols);
          printf("%s %d %d %d\n",key,layer_ix,rows,cols);
        }
        if (strcmp(key,"WT") == 0)
        {
          sscanf(line,"%s %d %d %d %f",key,&layer_ix,&row_ix,&col_ix,&ftemp);
          //printf("%s %d %d %f\n",key,row_ix,col_ix,ftemp);
          nn->nnw[layer_ix][row_ix][col_ix] = ftemp;
        }
      }
    }
  }

  fclose(in);
  if(data_found == 0)
  {
    printf("No data found ... random values generated\n");
    for (k=1;k<nn->nlayer;k++)
    {
      for (i=1;i<=nn->layer[k-1];i++)
      {
        for(j=1;j<=nn->layer[k];j++)
        {
          nn->nnw[k][i][j] = (float)((rand()%1000)*0.001);
        }
      }
    }
  }
  printf("readNeuralNetwork terminated with result: %d\n",res);
  return res;
}

//=====================================================
void showNeuralNetwork()
//=====================================================
{
  int i,j,k;
  printf("Show neural network : Layers=%d\n",nn->nlayer);
  for (k=1;k<nn->nlayer;k++)
  {
    printf("Layer = %5d \n",k);
    printf("%5d ",0);
    for(j=1;j<=nn->layer[k];j++)printf("%5d ",j);
    printf("\n");
    for (i=1;i<=nn->layer[k-1];i++)
    {
      printf("%5d ",i);
      for(j=1;j<=nn->layer[k];j++)
      {
        printf("%5.3f ",nn->nnw[k][i][j]);
      }
      printf("\n");
    }
  }
  printf("End show neural network\n");
}
