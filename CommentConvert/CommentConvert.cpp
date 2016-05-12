#include<stdio.h>

//定义状态
typedef enum
{
	NO_STATE,
	C_STATE,
	CPP_STATE,
	NEW_STATE,
	MARK_STATE,
	END_STATE
}STATE_ENUM;

//定义状态机
typedef struct STATE_MACHINE
{
	FILE *inputfile;
	FILE *outputfile;
	STATE_ENUM ulstate;
	char sign;
	char sign_mark;
}STATE_MACHINE;

STATE_MACHINE  state = {0};
void EventPro(char ch);
void EventProAtNo(char ch);
void EventProAtC(char ch);
void EventProAtCpp(char ch);
void EventProAtNEW(char ch);
void EventProAtMARK(char ch);

int CommentConvert(FILE *inputfile, FILE *outputfile)
{
	if(inputfile==NULL || outputfile==NULL)
	{
		printf("Error: Argument Invalid!\n");
		return -1;
	}

	state.inputfile = inputfile;
	state.outputfile = outputfile;
	//状态为初始状态
	state.ulstate = NO_STATE;
	
	char ch;
	while(state.ulstate != END_STATE)
	{
		ch = fgetc(state.inputfile);
		EventPro(ch);
	}
	return 0;
}

void EventPro(char ch)
{
	switch(state.ulstate)
	{
	//状态为初始状态,即没有读到注释(或者也可能是读完注释)
	case NO_STATE:
		EventProAtNo(ch);
		break;
	//读到C注释
	case C_STATE:
		EventProAtC(ch);
		break;
	//读到C++注释
	case CPP_STATE:
		EventProAtCpp(ch);
		break;
	//读到双引号
	case NEW_STATE:
        EventProAtNEW(ch);
		break;
	//读到单引号
	case MARK_STATE:
        EventProAtMARK(ch);
		break;
	case END_STATE:
		break;
	default:
		break;
	}
}

//未读到注释,调用该函数(或者也可能是读完注释)
void EventProAtNo(char ch)
{
	char nextch;
	switch(ch)
	{
	//当前字符为EOF,状态为结束状态
	case EOF:
        state.ulstate = END_STATE;
		break;
	//当前字符为'/'
	case '/':
		//读取下一个字符
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'则为C++注释
		if(nextch == '/') 
		{
			//写入当前字符,在写入字符'*',状态变为C++状态
			fputc(ch,state.outputfile);
			fputc('*',state.outputfile);
			state.ulstate = CPP_STATE;
		}
		//下一个字符为'*'则为C注释
		else if(nextch == '*')
		{
			//将当前字符和下一个字符写入文件,状态变为C注释状态
			fputc(ch,state.outputfile);
			fputc(nextch,state.outputfile);
			state.ulstate = C_STATE;
		}
		else
		{
			//其他情况直接将当前字符写入文件
			fputc(ch,state.outputfile);
		}
		break;
	//当前字符为'"'
	case '"':
		//写入当前字符,标志字符置为A,状态置为读取到双引号的状态
		fputc(ch,state.outputfile);
		state.sign='A';
        state.ulstate = NEW_STATE;
		break;
	//当前字符为'\''
	case '\'':
		//写入当前字符,标志字符置为D,状态置为读取到单引号的状态
		fputc(ch,state.outputfile);
		state.sign_mark='D';
        state.ulstate = MARK_STATE;
		break;
	default:
		//其他情况直接将当前字符写入文件
        fputc(ch,state.outputfile);
		break;
	}
}

//若读到C注释,调用该函数
void EventProAtC(char ch)
{
	char nextch;
	switch(ch)
	{
	//结束时状态置为结束状态
	case EOF:
		state.ulstate = END_STATE;
		break;
	//当前字符为'/'
	case '/':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'或'*',将两个空格写入文件
		if(nextch=='/'  || nextch=='*')
		{
			fputc(' ',state.outputfile);
			fputc(' ',state.outputfile);
		}
		//其他状态将一个空格和下一个字符写入文件
		else 
		{
			fputc(ch,state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	//当前字符为'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'将当前字符与下一个字符写入文件,状态要置为初始状态
		if(nextch=='/')
		{
		    fputc(ch,state.outputfile);
			fputc(nextch,state.outputfile);
            state.ulstate = NO_STATE;
		}
		//其他情况将当前字符与下一个字符写入文件
		else
		{
			fputc(ch,state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	//当前字符为'"'
	case '"':
		//写入当前字符,标志字符置为B,状态置为读取到双引号的状态
		fputc(ch,state.outputfile);
		state.sign='B';
        state.ulstate = NEW_STATE;
		break;
	//当前字符为'\''
	case '\'':
		//写入当前字符,标志字符置为E,状态置为读取到单引号的状态
		fputc(ch,state.outputfile);
		state.sign_mark='E';
        state.ulstate = MARK_STATE;
		break;
	//其他情况当前字符写入文件
	default:
    	fputc(ch,state.outputfile);
		break;
	}
}

//若读到C++注释,调用该函数
void EventProAtCpp(char ch)
{
	char nextch;
	switch(ch)
	{
    //结束时在末尾写入'*''/'
	case EOF:
		fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		state.ulstate = END_STATE;
		break;
	//当前字符为'/'
	case '/':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'将两个空格写入文件
        if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//下一个字符为'*'将两个空格写入文件
		else if(nextch=='*')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//其他情况将一个空格和下一个字符写入文件
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
	//当前字符为'\n',写入字符'*''/''\n',状态置为初始状态
    case '\n':
        fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		fputc('\n',state.outputfile);
		state.ulstate = NO_STATE;
        break;
    //当前字符为'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'则写入两个空格
		if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//其他情况写入一个空格和下一个字符
		else
		{
			fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
	//当前字符为'"'
	case '"':
		//写入当前节点,标志字符置为C,状态置为读取到双引号的状态
		fputc(ch,state.outputfile);
		state.sign='C';
        state.ulstate = NEW_STATE;
		break;
	//当前字符为'\''
	case '\'':
		//写入当前字符,标志字符置为F,状态置为读取到单引号的状态
		fputc(ch,state.outputfile);
		state.sign_mark='F';
        state.ulstate = MARK_STATE;
		break;
	//其他情况将当前字符写入文件
	default:
		fputc(ch,state.outputfile);
		break;
	}
	
}

//读到双引号
void EventProAtNEW(char ch)
{
	char nextch;
    switch(ch)
	{
	//结束时状态置为结束状态
	case EOF:
        state.ulstate = END_STATE;
		break;
	//当前字符为'"'
	case '"':
		fputc(ch,state.outputfile);
		//标志字符为A,说明该引号所引的内容不在注释中,状态置为初始状态
        if('A'==state.sign)
		{
            state.ulstate = NO_STATE;
		}
		//标志字符为B,说明该引号所引的内容在C注释中,状态置为C注释状态
		else if('B'==state.sign)
		{
            state.ulstate = C_STATE;
		}
		//标志字符为C,说明该引号所引的内容在C++注释中,状态置为C++注释状态
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	//当前字符为'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'
		if(nextch=='/')
		{
			//将当前字符和下一个字符间加一个空格再写入文件
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		//其他情况写入当前字符,下一个字符和一个空格
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	//其他情况将当前字符写入文件
	default:
        fputc(ch,state.outputfile);
		break;
	}
}

//读到单引号
void EventProAtMARK(char ch)
{
	char nextch;
    switch(ch)
	{
	//结束时状态置为结束状态
	case EOF:
        state.ulstate = END_STATE;
		break;
	//当前字符为'\''
	case '\'':
		fputc(ch,state.outputfile);
		//标志字符为D,说明该引号所引的内容不在注释中,状态置为初始状态
        if('D'==state.sign_mark)
		{
            state.ulstate = NO_STATE;
		}
		//标志字符为E,说明该引号所引的内容在C注释中,状态置为C注释状态
		else if('E'==state.sign_mark)
		{
            state.ulstate = C_STATE;
		}
		//标志字符为F,说明该引号所引的内容在C++注释中,状态置为C++注释状态
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	//当前字符为'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//下一个字符为'/'
		if(nextch=='/')
		{
			//将当前字符和下一个字符间加一个空格再写入文件
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		//其他情况写入当前字符,下一个字符和一个空格
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	//其他情况将当前字符写入文件
	default:
        fputc(ch,state.outputfile);
		break;
	}
}