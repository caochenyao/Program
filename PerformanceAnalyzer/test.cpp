#include"PerformanceAnalyzer.h"

void Run(int n)
{
	CONFIG_MANAGER_SETOPTIONS(PPCO_PERFORMANCE);
	CONFIG_MANAGER_SETOPTIONS(PPCO_SAVE_TO_CONSOLE);
	CONFIG_MANAGER_SETOPTIONS(PPCO_SAVE_BY_COSTTIME);

	while (n--)
	{
		PERFORMANCE_ANALYER_EE_BEGIN(sql, "数据库");
		Sleep(200);
		PERFORMANCE_ANALYER_EE_END(sql);

		PERFORMANCE_ANALYER_EE_BEGIN(net, "网络");
		Sleep(300);
		PERFORMANCE_ANALYER_EE_END(net);

		PERFORMANCE_ANALYER_EE_BEGIN(data, "数据");
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
	PERFORMANCE_RESULT_DISPLAY();

	system("pause");
	return 0;
}