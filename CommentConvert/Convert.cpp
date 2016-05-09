#include<stdio.h>

/****************************************
 * Function name:  main
 * Argument     :  void
 * Destribute   :  
 * Return value :
 * when   why   what    who
 * 
 ****************************************/

extern int CommentConvert(FILE *, FILE *);

int main()
{
	FILE *fpIn;
	FILE *fpOut;
	fpIn = fopen("input.cpp","r");
	if(NULL == fpIn)
	{
		printf("Error: Open input.cpp file fail!\n");
		return -1;
	}
	fpOut = fopen("output.cpp","w");
	if(NULL == fpOut)
	{
		printf("Error: Open output.cpp file fail!\n");
		return -1;
	}

	CommentConvert(fpIn,fpOut);//

	fclose(fpIn);
	fclose(fpOut);
	return 0;
}