#include"PerformanceAnalyzer.h"

//Release re;

PPSection* PerformanceAnalyzer::CreateMap(const char* filename, const char* function, int line, const char* desc)
{
	lock_guard<mutex> lock(_mtx);
	PPNode ppnode(filename,function,line,desc);

	PPSection* ppsection = NULL;

	map<PPNode, PPSection*>::iterator it = _ppMap.find(ppnode);
	if (it != _ppMap.end())
	{
		ppsection = it->second;
	}
	else
	{
		ppsection = new PPSection;
		_ppMap.insert(pair<PPNode,PPSection*>(ppnode, ppsection));
	}

	return ppsection;
}
bool PerformanceAnalyzer::CompareByCallCount(Performance::iterator lt, Performance::iterator rt)
{
	return lt->second->_totalCallCount > rt->second->_totalCallCount;
}
bool PerformanceAnalyzer::CompareByCostTime(Performance::iterator lt, Performance::iterator rt)
{
	return lt->second->_totalCostTime > rt->second->_totalCostTime;
}
void PerformanceAnalyzer::Output()
{
	lock_guard<mutex> lock(_mtx);
	int option = ConfigManager::GetInstance()->GetOptions();
	if (option & PPCO_SAVE_TO_CONSOLE)
	{
		ConsoleSave csa;
		_Output(csa);
	}

	if (option & PPCO_SAVE_TO_FILE)
	{
		FileSave fsa("Performance.txt");
		_Output(fsa);
	}
}
void PerformanceAnalyzer::_Output(SaveAdapt& sa)
{
	int num = 1;
	map<PPNode, PPSection*>::iterator it = _ppMap.begin();
	vector<Performance::iterator> vinfos;
	while (it != _ppMap.end())
	{
		vinfos.push_back(it);
		it++;
	}

	int flag = ConfigManager::GetInstance()->GetOptions();
	if (flag & PPCO_SAVE_BY_CALLTIME)
	{
		sort(vinfos.begin(), vinfos.end(), CompareByCallCount);
	}
	if (flag & PPCO_SAVE_BY_COSTTIME)
	{
		sort(vinfos.begin(), vinfos.end(), CompareByCostTime);
	}

	sa.Save("====================Performance Profiler Report====================\n\n");
	for (size_t index = 0; index < vinfos.size();++index)
	{
		sa.Save("NO.%d Des:%s\n", num++, vinfos[index]->first._desc.c_str());
		sa.Save("Filename:%s Function:%s Line:%d\n",
			vinfos[index]->first._filename.c_str(),
			vinfos[index]->first._function.c_str(),
			vinfos[index]->first._line);

		const PPNode& ppnode = vinfos[index]->first;
		PPSection* section = vinfos[index]->second;
		map<int, LongType>::iterator MapIt = section->_callCountMap.begin();
		while (MapIt != section->_callCountMap.end())
		{
			int id = MapIt->first;
			sa.Save("TreadId:%d CostTime:%0.2f CallCount:%d\n",
				id,
				(double)section->_costTimeMap[id] / 1000,
				section->_callCountMap[id]);

			MapIt++;
		}

		sa.Save("TotalCostTime:%0.2f TotalCount:%d\n",
			(double)section->_totalCostTime / 1000,
			section->_totalCallCount);
	}
	sa.Save("================================End================================\n\n");
}


