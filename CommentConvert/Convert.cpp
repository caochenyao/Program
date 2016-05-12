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

	//读取文件
	fpIn = fopen("input.cpp","r");
	//为NULL,打开文件失败
	if(NULL == fpIn)
	{
		printf("Error: Open input.cpp file fail!\n");
		return -1;
	}

	//写入文件
	fpOut = fopen("output.cpp","w");
	//为NULL,打开文件失败
	if(NULL == fpOut)
	{
		printf("Error: Open output.cpp file fail!\n");
		return -1;
	}

	//注释转换
	CommentConvert(fpIn,fpOut);//

	fclose(fpIn);
	fclose(fpOut);
	return 0;
}