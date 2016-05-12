#pragma once
#include"HaffmanTree.h"
#include<assert.h>
#include<string>
#include <algorithm>
#include<windows.h>

typedef long LongType;

struct FileInfo
{
	unsigned char _ch;      //字符
	LongType _count;        //字符出现的次数
	string _code;           //字符对应的Haffman编码
	FileInfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
	{}
	bool operator< (const FileInfo& info)const
	{
		return _count < info._count;
	}
	bool operator!=(const FileInfo& invalid)const
	{
		return _count != invalid._count;
	}
	FileInfo operator+(const FileInfo& info)
	{
		FileInfo newinfo;
	    newinfo._count = _count + info._count;
		return newinfo;
	}
};

class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256;++i)
		{
			_infos[i]._ch = (unsigned char)i;
		}
	}
public:
	bool Compress(const char* filename)
	{	
		assert(filename);

		//1.打开文件,统计文件中各个字符出现的次数
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//2.生成对应的Haffman编码
		HaffmanTree<FileInfo> Haffman;
		FileInfo invalid;
		HaffmanTreeNode<FileInfo>* root = Haffman.Create(_infos, 256, invalid);

		//递归生成Haffman编码
		_GenerateHaffmanCode(root);

		//3.压缩文件
		string Compress = filename;
		Compress += ".compress";
		FILE* fIn = fopen(Compress.c_str(),"wb");
		assert(fIn);

		fseek(fOut,0L,SEEK_SET);//定位到文件的开头位置
		char read = fgetc(fOut);//读取文件当前字符
		unsigned char fch = 0;  //存入文件的字符(*必须为无符号类型)
		int index = 0;          //计算位数，够8位存一次
		while (read != EOF)
		{
			string code = _infos[(unsigned char)read]._code;
			int in = 0;
			while (in < code.size())
			{
				fch <<= 1;
				fch |= (code[in++]- '0');	
				++index;
				if (index == 8)
				{
					index = 0;
					fputc(fch,fIn);
					fch = 0;
				}		    
			}
			read = fgetc(fOut);
		}
		if (index < 8)
		{
			fch <<= (8-index);
			fputc(fch, fIn);
		}

		//4.生成配置文件,方便解压缩
		Config(filename);

		fclose(fIn);
		fclose(fOut);

		return true;
	}
	bool UnCompress(const char* filename)
	{
		assert(filename);

		//读取配置文件,一行一行地读取
		string Config = filename;
		Config += ".config";
		FILE* fOutConfig = fopen(Config.c_str(), "rb");
		assert(fOutConfig);

		string line;
		//先读取字符总个数
		long long count = 0;
		count >>= 32;
		line = _ReadLineByLine(fOutConfig);
		int tmp = atoi(line.c_str());
		count |= tmp;
		count <<= 32;
		line.clear();
		line = _ReadLineByLine(fOutConfig);
		tmp = atoi(line.c_str());
		count |= tmp;
		line.clear();

		//读取各个字符
		line = _ReadLineByLine(fOutConfig);
		while (!line.empty())
		{
			string ch;
			_infos[(unsigned char)line[0]]._ch = (unsigned char)line[0];
			ch = line.substr(2);
			_infos[(unsigned char)line[0]]._count = atoi(ch.c_str());
			line.clear();
			line = _ReadLineByLine(fOutConfig);
		}

		//重建Haffman树
		HaffmanTree<FileInfo> newHaffman;
		FileInfo invalid;
		HaffmanTreeNode<FileInfo>* root = newHaffman.Create(_infos,256,invalid);

		//读取Haffman编码还原各个字符
		string Compress = filename;
		Compress += ".compress";
		FILE* fOutCompress = fopen(Compress.c_str(),"rb");
		assert(fOutCompress);

		string UnCompress = filename;
		UnCompress += ".uncompress";
		FILE* fIn = fopen(UnCompress.c_str(), "wb");
		assert(fIn);

		char fch = fgetc(fOutCompress);
		int index = 8;
		HaffmanTreeNode<FileInfo>* cur = root;
		while (!feof(fOutCompress))
		{
			char fchs = (fch >> (--index)) & 1;
			if (index == 0)
			{
				index = 8;
				fch = fgetc(fOutCompress);
			}
			
			if (fchs == 0)
			{
				cur = cur->_left;
			}
			else
			{
				cur = cur->_right;
			}
			if (cur && cur->_left == NULL && cur->_right == NULL)
			{
				fputc(cur->_weight._ch, fIn);
				--count;
				if (count == 0)
				{
					break;
				}
				cur = root;
			}
		}

		fclose(fOutConfig);
		fclose(fOutCompress);
		fclose(fIn);
		return true;
	}
public:
	string _ReadLineByLine(FILE* fOutConfig)
	{
		assert(fOutConfig);
		string line = "";

		char ch = fgetc(fOutConfig);
		if (ch == '\n')
		{
			ch = fgetc(fOutConfig);
		}
		while (ch != EOF && ch != '\n')
		{
			line += ch;
			ch = fgetc(fOutConfig);
		}
		return line;
	}
	void Config(const char* filename)
	{
		assert(filename);

		//计算被压缩文件中字符的总个数
		long long count = 0;
		for (int i = 0; i < 256; ++i)
		{
			count += _infos[i]._count;
		}

		//写入配置文件
		string Config = filename;
		Config += ".config";
		FILE* fInConfig = fopen(Config.c_str(), "wb");
		assert(fInConfig);

		//写入字符个数,第一行写入高32位,第2行写入低32位
		char ch[128];
		//int High = count >> 32;
		_itoa_s(count>>32, ch,10);
		fputs(ch, fInConfig);
		fputc('\n', fInConfig);
		_itoa_s(count&0xffffffff, ch, 10);
		fputs(ch, fInConfig);
		fputc('\n', fInConfig);

		for (int i = 0; i < 256; ++i)
		{
			if (_infos[i]._count)
			{
				char ch[128];
				_itoa_s(_infos[i]._count, ch, 10);
				fputc(_infos[i]._ch, fInConfig);
				fputc(',', fInConfig);
				fputs(ch, fInConfig);
				fputc('\n', fInConfig);
			}
		}
		fclose(fInConfig);
	}
	void _GenerateHaffmanCode(HaffmanTreeNode<FileInfo>* root)
	{
		if (root == NULL)
		{
			return;
		}
		_GenerateHaffmanCode(root->_left);
		_GenerateHaffmanCode(root->_right);

		//如果该节点为叶子节点,则生成相应的Haffman编码
		if (root->_left == NULL && root->_right == NULL)
		{
			HaffmanTreeNode<FileInfo>* cur = root;
			HaffmanTreeNode<FileInfo>* parent = cur->_parent;
			string& code = _infos[cur->_weight._ch]._code;
			while (parent)
			{
				if (cur == parent->_left)
				{
					code += '0';
				}
				else
				{
					code += '1';
				}
				cur = parent;
				parent = cur->_parent;
			}
			//编码逆置,因为是从叶子节点向上找生成的编码
			reverse(code.begin(),code.end());
		}
	}
protected:
	FileInfo _infos[256];
};

void Test()
{
	FileCompress file;

	int begin1 = GetTickCount();
	file.Compress("123.txt");
	int end1 = GetTickCount();
	cout << "Compress:" << end1 - begin1 << endl;

	int begin2 = GetTickCount();
	file.UnCompress("123.txt");
	int end2 = GetTickCount();
	cout << "UnCompress:" << end2 - begin2 << endl;
}