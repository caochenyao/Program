#include"PerformanceAnalyzer.h"

PPSection* PerformanceAnalyzer::CreateMap(const char* filename, const char* function, int line, const char* desc)
{
	PPNode ppnode(filename,function,line,desc);

	PPSection* ppsection = NULL;

	map<PPNode, PPSection*>::iterator it = _ppMap.find(ppnode);
	if (it != _ppMap.end())
	{
		ppsection = it->second;
	}
	else
	{
		_ppMap.insert(pair<PPNode,PPSection*>(ppnode, ppsection));
	}

	return ppsection;
}