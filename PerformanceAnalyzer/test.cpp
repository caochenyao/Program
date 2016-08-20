#include"PerformanceAnalyzer.h"

void Run(int n)
{
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

int main()
{
	Release re;
	thread p1(Run, 1);
	thread p2(Run, 2);
	thread p3(Run, 3);

	p1.join();
	p2.join();
	p3.join();

	re.~Release();
	system("pause");
	return 0;
}