//===========================================
// theory.c
// 2016-06-21
//===========================================

#include <stdio.h>
#include <math.h>

#define IMAX 4  // Number of devices

int mx[IMAX] = {1,10,70,400};  // list of device power
int sv[100],ssv[100],tr[100][100],pr[100][100];
int svv[65535];
int g_sum[100];

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
main()
{
	int i,j,k,dim,temp,temp1,temp2,sum,bp,dimm,nb;

	for(i=0;i<IMAX;i++)
	{
		printf("%3d ",mx[i]);
	}
	printf("\n");

	dim = 1;
	for(i=0;i<IMAX;i++)dim = dim*2;
	dimm = 1;
	for(i=0;i<dim;i++)dimm = dimm*2;
	dimm--;



	// generate state vector
	//---------------------------- 
	// sv[0] = 0, sv[dim] = max state value, i.e. sum of all devices power
	//
	for (i=0;i<dim;i++)
	{
		temp = 0;
		printf("( ");
		for(j=0;j<IMAX;j++)
		{
			bp = getBit(i,j);
			temp = temp + mx[j]*bp;
			printf("%1d ",bp);
		}
		printf(")  ");
		sv[i] = temp;
		printf("%2d %3d\n",i,temp);
	}
	printf("\n");

	// generate transition matrix
	//------------------------------
	printf("\n Transition Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<=i;j++)
		{
			tr[i][j] = sv[i] - sv[j];
			printf("%4d ",tr[i][j]);
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
			pr[i][j] = sum;
			printf("%4d ",pr[i][j]);
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
			if(pr[i][j] == 1)
			{
				printf("%4d ",tr[i][j]);
			}
			else
				printf("---- ");
		}
		printf("\n");
	}

	//========================================================
	// Reverse Engineering
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
				{
				printf("%5x %2d %3d nb=%d ",i,k,temp,nb);
				showBits(i, dim);
				printf("\n");
				}
		}
	}
	printf("\n");
	for(i=0;i<dim;i++)printf("%2d %3d freq=%3d\n",i,sv[i],g_sum[i]);
	printf("\n");

}

