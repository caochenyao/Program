#include<iostream>
using namespace std;

#include<map>
#include<thread>
#include<mutex>

#include<assert.h>
#include<windows.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>


typedef long long LongType;

//获取线程id
static int GetThreadId()
{
#ifdef _WIN32
	return ::GetCurrentThreadId();
#else
	return ::thread_self();
#endif
}

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
		if (_filename < ppnode._filename)
		{
			return true;
		}
		else if (_filename > ppnode._filename)
		{
			return false;
		}
		else
		{
			if (_function < ppnode._function)
			{
				return true;
			}
			else if (_function > ppnode._function)
			{
				return false;
			}
			else
			{
				if (_line < ppnode._line)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
};

struct PPSection
{
public:
	PPSection()
		:_totalCostTime(0)
		, _totalCallCount(0)
		, _totalRefCount(0)
	{}
	void Begin(int id)
	{
		if (_refCountMap[id] == 0)
		{
			_beginTimeMap[id] = clock();
		}
		_refCountMap[id]++;
		_callCountMap[id]++;
		_totalCallCount++;
		_totalRefCount++;
	}
	void End(int id)
	{
		_refCountMap[id]--;
		LongType costtime = clock() - _beginTimeMap[id];
		if (_refCountMap[id] == 0)
		{
			_costTimeMap[id] += costtime;
		}

		_totalRefCount--;
		_totalCostTime += costtime;
	}
public:
	map<int, LongType> _beginTimeMap;
	map<int, LongType> _costTimeMap;
	map<int, LongType> _callCountMap;
	map<int, LongType> _refCountMap;

	LongType _totalCallCount;
	LongType _totalCostTime;
	LongType _totalRefCount;
};

class SaveAdapt
{
public:
	virtual void Save(const char* format, ...) = 0;
};

class ConsoleSave:public SaveAdapt
{
public:
	virtual void Save(const char* format,...)
	{
		va_list list;
		va_start(list,format);
		vfprintf(stdout,format,list);
		va_end(list);
	}
};

class FileSave :public SaveAdapt
{
public:
	FileSave(const char* filename)
	{
		file = fopen(filename,"w");
		if (file == NULL)
		{
			perror("fopen");
			exit(EXIT_FAILURE);
		}
	}
	~FileSave()
	{
		if (file)
		{
			if (fclose(file) != 0)
			{
				perror("fclose");
				exit(EXIT_FAILURE);
			}
		}
	}
	virtual void Save(const char* format,...)
	{
		va_list list;
		va_start(list, format);
		vfprintf(file, format, list);
		va_end(list);
	}
protected:
	//防拷贝
	FileSave(const FileSave&);
	FileSave& operator = (const FileSave&);
protected:
	FILE* file;
};

template<class T>
class Simpleton
{
public:
	static T* GetInstance()
	{
		assert(_sInstance);
		return _sInstance;
	}
protected:
	static T* _sInstance;
};

template<class T>
T* Simpleton<T>::_sInstance = new T;

class PerformanceAnalyzer:public Simpleton<PerformanceAnalyzer>
{
	friend class Simpleton<PerformanceAnalyzer>;
public:
	PPSection* CreateMap(const char* filename,const char* function,int line,const char* desc);

	void Output()
	{
		ConsoleSave csa;
		_Output(csa);

		FileSave fsa("Performance.txt");
		_Output(fsa);
	}
protected:
	void _Output(SaveAdapt& sa)
	{
		int num = 1;
		map<PPNode, PPSection*>::iterator it = _ppMap.begin();
		while (it != _ppMap.end())
		{
			sa.Save("NO.%d Des:%s\n", num++, it->first._desc.c_str());
			sa.Save("Filename:%s Function:%s Line:%d\n",
				it->first._filename.c_str(),
				it->first._function.c_str(),
				it->first._line);

			const PPNode& ppnode = it->first;
			PPSection* section = it->second;
			map<int, LongType>::iterator MapIt = section->_callCountMap.begin();
			while (MapIt != section->_callCountMap.end())
			{
				int id = MapIt->first;
				sa.Save("TreadId:%d CostTime:%0.2f CallCount:%d\n",
					id,
					(double)section->_costTimeMap[id]/1000,
					section->_callCountMap[id]);	

				MapIt++;
			}

			sa.Save("TotalCostTime:%0.2f TotalCount:%d\n",
				(double)section->_totalCostTime / 1000,
				section->_totalCallCount);
			
			it++;
		}
	}
protected:
	map<PPNode, PPSection*> _ppMap;
	mutex _mtx;
};

struct Release
{
	~Release()
	{
		PerformanceAnalyzer::GetInstance()->Output();
	}
};

#define PERFORMANCE_ANALYER_EE_BEGIN(sign,desc) \
	PPSection* sign##section = PerformanceAnalyzer::GetInstance()->CreateMap(__FILE__, __FUNCTION__, __LINE__, desc); \
	sign##section->Begin(GetThreadId());

#define PERFORMANCE_ANALYER_EE_END(sign) \
	sign##section->End(GetThreadId());