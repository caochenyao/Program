#include<stdio.h>

//����״̬
typedef enum
{
	NO_STATE,
	C_STATE,
	CPP_STATE,
	NEW_STATE,
	MARK_STATE,
	END_STATE
}STATE_ENUM;

//����״̬��
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
	//״̬Ϊ��ʼ״̬
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
	//״̬Ϊ��ʼ״̬,��û�ж���ע��(����Ҳ�����Ƕ���ע��)
	case NO_STATE:
		EventProAtNo(ch);
		break;
	//����Cע��
	case C_STATE:
		EventProAtC(ch);
		break;
	//����C++ע��
	case CPP_STATE:
		EventProAtCpp(ch);
		break;
	//����˫����
	case NEW_STATE:
        EventProAtNEW(ch);
		break;
	//����������
	case MARK_STATE:
        EventProAtMARK(ch);
		break;
	case END_STATE:
		break;
	default:
		break;
	}
}

//δ����ע��,���øú���(����Ҳ�����Ƕ���ע��)
void EventProAtNo(char ch)
{
	char nextch;
	switch(ch)
	{
	//��ǰ�ַ�ΪEOF,״̬Ϊ����״̬
	case EOF:
        state.ulstate = END_STATE;
		break;
	//��ǰ�ַ�Ϊ'/'
	case '/':
		//��ȡ��һ���ַ�
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'��ΪC++ע��
		if(nextch == '/') 
		{
			//д�뵱ǰ�ַ�,��д���ַ�'*',״̬��ΪC++״̬
			fputc(ch,state.outputfile);
			fputc('*',state.outputfile);
			state.ulstate = CPP_STATE;
		}
		//��һ���ַ�Ϊ'*'��ΪCע��
		else if(nextch == '*')
		{
			//����ǰ�ַ�����һ���ַ�д���ļ�,״̬��ΪCע��״̬
			fputc(ch,state.outputfile);
			fputc(nextch,state.outputfile);
			state.ulstate = C_STATE;
		}
		else
		{
			//�������ֱ�ӽ���ǰ�ַ�д���ļ�
			fputc(ch,state.outputfile);
		}
		break;
	//��ǰ�ַ�Ϊ'"'
	case '"':
		//д�뵱ǰ�ַ�,��־�ַ���ΪA,״̬��Ϊ��ȡ��˫���ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign='A';
        state.ulstate = NEW_STATE;
		break;
	//��ǰ�ַ�Ϊ'\''
	case '\'':
		//д�뵱ǰ�ַ�,��־�ַ���ΪD,״̬��Ϊ��ȡ�������ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign_mark='D';
        state.ulstate = MARK_STATE;
		break;
	default:
		//�������ֱ�ӽ���ǰ�ַ�д���ļ�
        fputc(ch,state.outputfile);
		break;
	}
}

//������Cע��,���øú���
void EventProAtC(char ch)
{
	char nextch;
	switch(ch)
	{
	//����ʱ״̬��Ϊ����״̬
	case EOF:
		state.ulstate = END_STATE;
		break;
	//��ǰ�ַ�Ϊ'/'
	case '/':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'��'*',�������ո�д���ļ�
		if(nextch=='/'  || nextch=='*')
		{
			fputc(' ',state.outputfile);
			fputc(' ',state.outputfile);
		}
		//����״̬��һ���ո����һ���ַ�д���ļ�
		else 
		{
			fputc(ch,state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	//��ǰ�ַ�Ϊ'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'����ǰ�ַ�����һ���ַ�д���ļ�,״̬Ҫ��Ϊ��ʼ״̬
		if(nextch=='/')
		{
		    fputc(ch,state.outputfile);
			fputc(nextch,state.outputfile);
            state.ulstate = NO_STATE;
		}
		//�����������ǰ�ַ�����һ���ַ�д���ļ�
		else
		{
			fputc(ch,state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	//��ǰ�ַ�Ϊ'"'
	case '"':
		//д�뵱ǰ�ַ�,��־�ַ���ΪB,״̬��Ϊ��ȡ��˫���ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign='B';
        state.ulstate = NEW_STATE;
		break;
	//��ǰ�ַ�Ϊ'\''
	case '\'':
		//д�뵱ǰ�ַ�,��־�ַ���ΪE,״̬��Ϊ��ȡ�������ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign_mark='E';
        state.ulstate = MARK_STATE;
		break;
	//���������ǰ�ַ�д���ļ�
	default:
    	fputc(ch,state.outputfile);
		break;
	}
}

//������C++ע��,���øú���
void EventProAtCpp(char ch)
{
	char nextch;
	switch(ch)
	{
    //����ʱ��ĩβд��'*''/'
	case EOF:
		fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		state.ulstate = END_STATE;
		break;
	//��ǰ�ַ�Ϊ'/'
	case '/':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'�������ո�д���ļ�
        if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//��һ���ַ�Ϊ'*'�������ո�д���ļ�
		else if(nextch=='*')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//���������һ���ո����һ���ַ�д���ļ�
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
	//��ǰ�ַ�Ϊ'\n',д���ַ�'*''/''\n',״̬��Ϊ��ʼ״̬
    case '\n':
        fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		fputc('\n',state.outputfile);
		state.ulstate = NO_STATE;
        break;
    //��ǰ�ַ�Ϊ'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'��д�������ո�
		if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		//�������д��һ���ո����һ���ַ�
		else
		{
			fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
	//��ǰ�ַ�Ϊ'"'
	case '"':
		//д�뵱ǰ�ڵ�,��־�ַ���ΪC,״̬��Ϊ��ȡ��˫���ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign='C';
        state.ulstate = NEW_STATE;
		break;
	//��ǰ�ַ�Ϊ'\''
	case '\'':
		//д�뵱ǰ�ַ�,��־�ַ���ΪF,״̬��Ϊ��ȡ�������ŵ�״̬
		fputc(ch,state.outputfile);
		state.sign_mark='F';
        state.ulstate = MARK_STATE;
		break;
	//�����������ǰ�ַ�д���ļ�
	default:
		fputc(ch,state.outputfile);
		break;
	}
	
}

//����˫����
void EventProAtNEW(char ch)
{
	char nextch;
    switch(ch)
	{
	//����ʱ״̬��Ϊ����״̬
	case EOF:
        state.ulstate = END_STATE;
		break;
	//��ǰ�ַ�Ϊ'"'
	case '"':
		fputc(ch,state.outputfile);
		//��־�ַ�ΪA,˵�����������������ݲ���ע����,״̬��Ϊ��ʼ״̬
        if('A'==state.sign)
		{
            state.ulstate = NO_STATE;
		}
		//��־�ַ�ΪB,˵��������������������Cע����,״̬��ΪCע��״̬
		else if('B'==state.sign)
		{
            state.ulstate = C_STATE;
		}
		//��־�ַ�ΪC,˵��������������������C++ע����,״̬��ΪC++ע��״̬
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	//��ǰ�ַ�Ϊ'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'
		if(nextch=='/')
		{
			//����ǰ�ַ�����һ���ַ����һ���ո���д���ļ�
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		//�������д�뵱ǰ�ַ�,��һ���ַ���һ���ո�
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	//�����������ǰ�ַ�д���ļ�
	default:
        fputc(ch,state.outputfile);
		break;
	}
}

//����������
void EventProAtMARK(char ch)
{
	char nextch;
    switch(ch)
	{
	//����ʱ״̬��Ϊ����״̬
	case EOF:
        state.ulstate = END_STATE;
		break;
	//��ǰ�ַ�Ϊ'\''
	case '\'':
		fputc(ch,state.outputfile);
		//��־�ַ�ΪD,˵�����������������ݲ���ע����,״̬��Ϊ��ʼ״̬
        if('D'==state.sign_mark)
		{
            state.ulstate = NO_STATE;
		}
		//��־�ַ�ΪE,˵��������������������Cע����,״̬��ΪCע��״̬
		else if('E'==state.sign_mark)
		{
            state.ulstate = C_STATE;
		}
		//��־�ַ�ΪF,˵��������������������C++ע����,״̬��ΪC++ע��״̬
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	//��ǰ�ַ�Ϊ'*'
	case '*':
		nextch = fgetc(state.inputfile);
		//��һ���ַ�Ϊ'/'
		if(nextch=='/')
		{
			//����ǰ�ַ�����һ���ַ����һ���ո���д���ļ�
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		//�������д�뵱ǰ�ַ�,��һ���ַ���һ���ո�
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	//�����������ǰ�ַ�д���ļ�
	default:
        fputc(ch,state.outputfile);
		break;
	}
}