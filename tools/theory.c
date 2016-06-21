//===========================================
// theory.c
// 2016-06-21
//===========================================

#include <stdio.h>
#include <math.h>

#define IMAX 4  // Number of devices
int mx[IMAX] = {1,10,70,400};  


  // list of device power
int ssv[100],pr[100][100],tr[100][100];
int svv[65535];
int g_sum[100];

int g_ndevices = 4;
int g_nstates  = 16;
int g_state_vector[16] = {225,310,350,365,440,450,490,590};
int g_state_trans_stat[16][16] = 
		{
				{1,2,3,4,5,6,7},
				{1,2,3,4,5,6,7},
				{1,2,3,4,5,6,7},
				{1,2,3,4,5,6,7},
				{1,2,3,4,5,6,7},
				{1,1,3,4,5,6,7},
				{1,2,1,4,5,6,7}
		};

//============================================
int getBit(int value, int bit)
//============================================
{
	int res = 0;
	value = value >> bit;
	res = value & 1;
	return(res);
}
//============================================
void showBits(int value, int nbits)
//============================================
{
	int i,res;
	
	printf("( ");
	res = value & 1;
	printf("%d ",res);

	for(i=1;i<nbits;i++)
	{
		value = value >> 1;
		res = value & 1;
		printf("%d ",res);
		g_sum[i] = g_sum[i] + res;;
		if((i+1)%4 == 0)printf(" ");
	}
	printf(")");
	return;
}

//============================================
void genStateVector(int v[], int ndev)
//============================================
{
	int i,j,temp,bp;
	
	int dim = 1;
	for(i=0;i<ndev;i++)dim = dim*2;
	g_nstates = dim;
	
	for (i=0;i<dim;i++)
	{
		temp = 0;
		printf("( ");
		for(j=0;j<IMAX;j++)
		{
			bp = getBit(i,j);
			temp = temp + v[j]*bp;
			printf("%1d ",bp);
		}
		printf(")  ");
		g_state_vector[i] = temp;
		printf("%2d %5d\n",i,temp);
	}
	printf("\n");
}
//============================================
eval(int dim, int sv[16], int freq[16][16])
{
	int i,j,k,temp,temp1,temp2,sum,bp,dimm,nb;

//	for(i=0;i<IMAX;i++)
//	{
//		printf("%3d ",mx[i]);
//	}
//	printf("\n");

//	dim = 1;
//	for(i=0;i<IMAX;i++)dim = dim*2;
	dimm = 1;
	for(i=0;i<dim;i++)dimm = dimm*2;
	dimm--;

	// remove bias from state vector
	temp = 9999999;
	for (i=0;i<dim;i++)if(sv[i] < temp)temp = sv[i];	
	printf("State Vector Bias = %d\n",temp);
	for (i=0;i<dim;i++)sv[i] = sv[i] - temp;	



	// generate transition matrix
	//------------------------------
	printf("\n Transition Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<dim;j++)
		{
			tr[i][j] = sv[i] - sv[j];
			printf("%5d ",tr[i][j]);
		}
		printf("\n");
	}

	// generate probability matrix
	//--------------------------------
	printf("\nProbability Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<=i;j++)
		{
			sum = 0;
			for(k=0;k<IMAX;k++)
			{
				temp1 = getBit(i,k);
				temp2 = getBit(j,k);
				if(temp1 != temp2)sum++;
			}
			freq[i][j] = sum;
			printf("%4d ",freq[i][j]);
		}
		printf("\n");
	}

	// generate transition matrix
	//-------------------------------
	printf("\n Likely Transition Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<=i;j++)
		{
			if(freq[i][j] == 1)
			{
				printf("%4d ",tr[i][j]);
			}
			else
				printf("---- ");
		}
		printf("\n");
	}

	//========================================================
	// Check possible sums of states
	//========================================================

	for(i=1;i<dim;i++)g_sum[i] = 0;
	for (i=0;i<dimm;i++)
	{
		temp = 0;
		//printf("( ");
		nb = 0;
		for(j=1;j<dim;j++)
		{
			bp = getBit(i,j);
			if(bp == 1)nb++;
			temp = temp + sv[j]*bp;
			//printf("%1d ",bp);
		}
		//printf(")  ");
		svv[i] = temp;
		for(k=1;k<dim;k++)
		{
			if(svv[i] == sv[k] && !(i&0x0001))
				//if(!(i&0x0001))
				{
				printf("%5x %2d %5d nb=%2d ",i,k,temp,nb);
				showBits(i, dim);
				printf("\n");
				}
		}
	}
	printf("\n");
	for(i=0;i<dim;i++)printf("%2d %5d freq=%3d\n",i,sv[i],g_sum[i]);
	printf("\n");

}

main()
{
	// Prepare best guess of possible states - state_vector[nstates]
	// Collect state transition statistics, i.e. frequency - state_trans_stat[nstates][nstates]
	
	//int v[4] = {1000,10,100,1};
	//genStateVector(v,4);
	g_nstates = 8;
	eval(g_nstates, g_state_vector, g_state_trans_stat);
}

