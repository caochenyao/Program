#pragma once
#include"HaffmanTree.h"
#include<assert.h>
#include<string>
#include <algorithm>
#include<windows.h>

typedef long LongType;

struct FileInfo
{
	unsigned char _ch;      //�ַ�
	LongType _count;        //�ַ����ֵĴ���
	string _code;           //�ַ���Ӧ��Haffman����
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

		//1.���ļ�,ͳ���ļ��и����ַ����ֵĴ���
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}

		//2.���ɶ�Ӧ��Haffman����
		HaffmanTree<FileInfo> Haffman;
		FileInfo invalid;
		HaffmanTreeNode<FileInfo>* root = Haffman.Create(_infos, 256, invalid);

		//�ݹ�����Haffman����
		_GenerateHaffmanCode(root);

		//3.ѹ���ļ�
		string Compress = filename;
		Compress += ".compress";
		FILE* fIn = fopen(Compress.c_str(),"wb");
		assert(fIn);

		fseek(fOut,0L,SEEK_SET);//��λ���ļ��Ŀ�ͷλ��
		char read = fgetc(fOut);//��ȡ�ļ���ǰ�ַ�
		unsigned char fch = 0;  //�����ļ����ַ�(*����Ϊ�޷�������)
		int index = 0;          //����λ������8λ��һ��
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

		//4.���������ļ�,�����ѹ��
		Config(filename);

		fclose(fIn);
		fclose(fOut);

		return true;
	}
	bool UnCompress(const char* filename)
	{
		assert(filename);

		//��ȡ�����ļ�,һ��һ�еض�ȡ
		string Config = filename;
		Config += ".config";
		FILE* fOutConfig = fopen(Config.c_str(), "rb");
		assert(fOutConfig);

		string line;
		//�ȶ�ȡ�ַ��ܸ���
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

		//��ȡ�����ַ�
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

		//�ؽ�Haffman��
		HaffmanTree<FileInfo> newHaffman;
		FileInfo invalid;
		HaffmanTreeNode<FileInfo>* root = newHaffman.Create(_infos,256,invalid);

		//��ȡHaffman���뻹ԭ�����ַ�
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

		//���㱻ѹ���ļ����ַ����ܸ���
		long long count = 0;
		for (int i = 0; i < 256; ++i)
		{
			count += _infos[i]._count;
		}

		//д�������ļ�
		string Config = filename;
		Config += ".config";
		FILE* fInConfig = fopen(Config.c_str(), "wb");
		assert(fInConfig);

		//д���ַ�����,��һ��д���32λ,��2��д���32λ
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

		//����ýڵ�ΪҶ�ӽڵ�,��������Ӧ��Haffman����
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
			//��������,��Ϊ�Ǵ�Ҷ�ӽڵ����������ɵı���
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