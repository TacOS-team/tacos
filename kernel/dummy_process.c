#include <stdio.h>
#include <dummy_process.h>

void sleep()
{
	int i,j,k;
	float paf;
	
	for(i=0 ; i<999999999 ; i++)
	{
		for(k=0 ; j<99999999 ; j++)
		{
			for(k=0 ; k<99999999 ; k++)
			{
				paf *= (float)i*(float)j*(float)k;
			}
		}
	}
}

int dummy_process_main(int argc, char* argv[])
{	

		if(argc>1)
		{
			printf("Process %s %s\n",argv[0],argv[1]);
			//sleep();
		}
		else
		{
			printf("Unknown process\n");
			//sleep();
		}
		return 0;
}
