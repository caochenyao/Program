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

	//��ȡ�ļ�
	fpIn = fopen("input.cpp","r");
	//ΪNULL,���ļ�ʧ��
	if(NULL == fpIn)
	{
		printf("Error: Open input.cpp file fail!\n");
		return -1;
	}

	//д���ļ�
	fpOut = fopen("output.cpp","w");
	//ΪNULL,���ļ�ʧ��
	if(NULL == fpOut)
	{
		printf("Error: Open output.cpp file fail!\n");
		return -1;
	}

	//ע��ת��
	CommentConvert(fpIn,fpOut);//

	fclose(fpIn);
	fclose(fpOut);
	return 0;
}