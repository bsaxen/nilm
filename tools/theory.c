//===========================================
// theory.c
// 2016-06-20
//===========================================

#include <stdio.h>
#include <math.h>

#define IMAX 4

int mx[IMAX] = {10,100,150,200};
int mult[IMAX];
int sv[100],ssv[100],tr[100][100],pr[100][100];


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
main()
{
	int i,j,k,dim,temp,temp1,temp2,sum;

	for(i=0;i<IMAX;i++)
	{
		printf("%d ",mx[i]);
	}
	printf("\n");

	dim = 1;
	for(i=0;i<IMAX;i++)dim = dim*2;

	

	// generate state vector
	for (i=0;i<dim;i++)
	{
		temp = 0;
		for(j=0;j<IMAX;j++)
		{
			temp = temp + mx[j]*getBit(i,j);
		}
		sv[i] = temp;
		printf("%2d %3d\n",i,temp);
	}
	printf("\n");

	// generate transition matrix
	printf("\n Transition Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<=i;j++)
		{
			tr[i][j] = sv[i] - sv[j];
			printf("%3d ",tr[i][j]);
		}
		printf("\n");
	}

	// generate probability matrix
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
			printf("%3d ",pr[i][j]);
		}
		printf("\n");
	}

	// generate transition matrix
	printf("\n Likely Transition Matrix\n");
	for (i=0;i<dim;i++)
	{
		for(j=0;j<=i;j++)
		{
			if(pr[i][j] == 1)
			{
				printf("%3d ",tr[i][j]);
			}
			else
				printf("--- ");
		}
		printf("\n");
	}		
}
