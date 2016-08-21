#include"PerformanceAnalyzer.h"

void Run(int n)
{
	SETOPTIONS(PPCO_PERFORMANCE);
	SETOPTIONS(PPCO_SAVE_TO_CONSOLE);
	SETOPTIONS(PPCO_SAVE_BY_COSTTIME);

	while (n--)
	{
		PERFORMANCE_ANALYER_EE_BEGIN(sql, "���ݿ�");
		Sleep(200);
		PERFORMANCE_ANALYER_EE_END(sql);

		PERFORMANCE_ANALYER_EE_BEGIN(net, "����");
		Sleep(300);
		PERFORMANCE_ANALYER_EE_END(net);

		PERFORMANCE_ANALYER_EE_BEGIN(data, "����");
		Sleep(500);
		PERFORMANCE_ANALYER_EE_END(data);
	}	
}

void Test()
{
	//Release re;
	thread p1(Run, 1);
	thread p2(Run, 2);
	thread p3(Run, 3);

	p1.join();
	p2.join();
	p3.join();
}

int main()
{
	Test();
	DISPLAY();

	system("pause");
	return 0;
}