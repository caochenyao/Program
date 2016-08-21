#include<iostream>
using namespace std;

#include<map>
#include<thread>
#include<mutex>
#include<vector>
#include<algorithm>

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


//保存适配器基类
class SaveAdapt
{
public:
	virtual void Save(const char* format, ...) = 0;
};
//控制台保存适配器
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
//文件保存适配器
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

//单例基类
template<class T>
class Simpleton
{
public:
	static T* GetInstance()
	{
		//双重检查保证线程的安全与效率
		if (_sInstance == NULL)
		{
			lock_guard<mutex> lock(_mutex);
			if (_sInstance == NULL)
			{
				_sInstance = new T();
			}
		}
		return _sInstance;
	}
protected:
	Simpleton()
	{}

	static T* _sInstance;
	static mutex _mutex;
};

template<class T>
T* Simpleton<T>::_sInstance = new T;

template<class T>
mutex Simpleton<T>::_mutex;
//配置管理选项
enum PP_CONFIG_OPTION
{
	PPCO_NONE = 0,               //不做剖析
	PPCO_PERFORMANCE = 2,        //开始剖析
	PPCO_SAVE_TO_CONSOLE = 4,    //保存到控制台
	PPCO_SAVE_TO_FILE = 8,       //保存到文件
	PPCO_SAVE_BY_CALLTIME = 16,  //按调用次数排序
	PPCO_SAVE_BY_COSTTIME = 32,  //按花费时间排序
};

//配置管理
class ConfigManager :public Simpleton<ConfigManager>
{
public:
	ConfigManager()
		:_option(PPCO_NONE)
	{}
	void SetOptions(int option)
	{
		_option |= option;
	}
	int GetOptions()
	{
		return _option;
	}
private:
	int _option;
};

//获取路径中最后的文件名。
static string GetFileName(const string& path)
{
	char ch = '/';

#ifdef _WIN32
	ch = '\\';
#endif

	size_t pos = path.rfind(ch);
	if (pos == string::npos)
	{
		return path;
	}
	else
	{
		return path.substr(pos + 1);
	}
}

struct PPNode
{
	string _filename;   //文件名
	string _function;   //函数名
	int _line;          //行号

	string _desc;       //描述

	PPNode(const char* filename, const char* function, int line, const char* desc)
		:_filename(GetFileName(filename))
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

//性能剖析
class PerformanceAnalyzer:public Simpleton<PerformanceAnalyzer>
{
	friend class Simpleton<PerformanceAnalyzer>;
	typedef map<PPNode, PPSection*> Performance;
public:
	PPSection* CreateMap(const char* filename,const char* function,int line,const char* desc);

	void Output();

protected:
	void _Output(SaveAdapt& sa);
	static bool CompareByCallCount(Performance::iterator lt, Performance::iterator rt);
	static bool CompareByCostTime(Performance::iterator lt, Performance::iterator rt);
protected:
	map<PPNode, PPSection*> _ppMap;
	mutex _mtx;
};

class Release
{
	~Release()
	{
		PerformanceAnalyzer::GetInstance()->Output();
	}
};
//配置管理的设置
#define CONFIG_MANAGER_SETOPTIONS(option)                      \
	ConfigManager::GetInstance()->SetOptions(option);
//开始剖析
#define PERFORMANCE_ANALYER_EE_BEGIN(sign,desc) \
	PPSection* sign##section = NULL;            \
	if (ConfigManager::GetInstance()->GetOptions() & PPCO_PERFORMANCE)\
	{                                           \
		sign##section = PerformanceAnalyzer::GetInstance()->CreateMap(__FILE__, __FUNCTION__, __LINE__, desc); \
		sign##section->Begin(GetThreadId());    \
	}
//结束剖析
#define PERFORMANCE_ANALYER_EE_END(sign)        \
	if (sign##section)                          \
	{                                           \
		sign##section->End(GetThreadId());      \
	}
//输出剖析结果
#define PERFORMANCE_RESULT_DISPLAY()                               \
	PerformanceAnalyzer::GetInstance()->Output();
	