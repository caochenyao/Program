#include<iostream>
using namespace std;

#include<string.h>
#include<time.h>
#include<map>

typedef long long LongType;

struct PPNode
{
	string _filename;   //文件名
	string _function;   //函数名
	int _line;          //行号

	string _desc;       //描述

	PPNode(const char* filename, const char* function, int line, const char* desc)
		:_filename(filename)
		, _function(function)
		, _line(line)
		, _desc(desc)
	{}

	bool operator < (const PPNode& ppnode)const
	{
		return _filename < ppnode._filename
			|| _function < ppnode._function
			|| _line < ppnode._line
			|| _desc < ppnode._desc;
	}
};

struct PPSection
{
public:
	PPSection()
		:_begintime(0)
		, _costtime(0)
		, _callcount(0)
	{}
	void Begin()
	{
		_begintime = clock();
		_callcount++;
	}
	void End()
	{
		_costtime += clock() - _begintime;
	}
protected:
	int _begintime;
	int _costtime;
	int _callcount;
};

class PerformanceAnalyzer
{
public:
	PPSection* CreateMap(const char* filename,const char* function,int line,const char* desc);
protected:
	map<PPNode, PPSection*> _ppMap;
};