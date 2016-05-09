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
	case NO_STATE:
		EventProAtNo(ch);
		break;
	case C_STATE:
		EventProAtC(ch);
		break;
	case CPP_STATE:
		EventProAtCpp(ch);
		break;
	case NEW_STATE:
        EventProAtNEW(ch);
		break;
	case MARK_STATE:
        EventProAtMARK(ch);
		break;
	case END_STATE:
		break;
	default:
		break;
	}
}

void EventProAtNo(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
        state.ulstate = END_STATE;
		break;
	case '/':
		nextch = fgetc(state.inputfile);
		if(nextch == '/') //C++
		{
			fputc(ch,state.outputfile);
			fputc('*',state.outputfile);
			state.ulstate = CPP_STATE;
		}
		else if(nextch == '*') // C
		{
			fputc(ch,state.outputfile);
			fputc(nextch,state.outputfile);
			state.ulstate = C_STATE;
		}
		else
		{
			fputc(ch,state.outputfile);
		}
		break;
	case '"':
		fputc(ch,state.outputfile);
		state.sign='A';
        state.ulstate = NEW_STATE;
		break;
	case '\'':
		fputc(ch,state.outputfile);
		state.sign_mark='D';
        state.ulstate = MARK_STATE;
		break;
	default:
        fputc(ch,state.outputfile);
		break;
	}
}
void EventProAtC(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
		state.ulstate = END_STATE;
		break;
	case '/':
		nextch = fgetc(state.inputfile);
		if(nextch=='/'  || nextch=='*')
		{
			fputc(' ',state.outputfile);
			fputc(' ',state.outputfile);
		}
		else 
		{
			fputc(' ',state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	case '*':
		nextch = fgetc(state.inputfile);
		if(nextch=='/')
		{
		    fputc(ch,state.outputfile);
			fputc('/',state.outputfile);
            state.ulstate = NO_STATE;
		}
		else
		{
			fputc(' ',state.outputfile);
            fputc(nextch,state.outputfile);
		}
		break;
	case '"':
		fputc(ch,state.outputfile);
		state.sign='B';
        state.ulstate = NEW_STATE;
		break;
	case '\'':
		fputc(ch,state.outputfile);
		state.sign_mark='E';
        state.ulstate = MARK_STATE;
		break;
	default:
    	fputc(ch,state.outputfile);
		break;
	}
}
void EventProAtCpp(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
		fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		state.ulstate = END_STATE;
		break;
	case '/':
		nextch = fgetc(state.inputfile);
        if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		else if(nextch=='*')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		else
		{
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
    case '\n':
        fputc('*',state.outputfile);
		fputc('/',state.outputfile);
		fputc('\n',state.outputfile);
		state.ulstate = NO_STATE;
        break;
	case '*':
		nextch = fgetc(state.inputfile);
		if(nextch=='/')
		{
            fputc(' ',state.outputfile);
		    fputc(' ',state.outputfile);
		}
		else
		{
			fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		break;
	case '"':
		fputc(ch,state.outputfile);
		state.sign='C';
        state.ulstate = NEW_STATE;
		break;
	case '\'':
		fputc(ch,state.outputfile);
		state.sign_mark='F';
        state.ulstate = MARK_STATE;
		break;
	default:
		fputc(ch,state.outputfile);
		break;
	}
	
}
void EventProAtNEW(char ch)
{
	char nextch;
    switch(ch)
	{
	case EOF:
        state.ulstate = END_STATE;
		break;
	case '"':
		fputc(ch,state.outputfile);
        if('A'==state.sign)
		{
            state.ulstate = NO_STATE;
		}
		else if('B'==state.sign)
		{
            state.ulstate = C_STATE;
		}
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	case '*':
		nextch = fgetc(state.inputfile);
		if(nextch=='/')
		{
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	default:
        fputc(ch,state.outputfile);
		break;
	}
}

void EventProAtMARK(char ch)
{
	char nextch;
    switch(ch)
	{
	case EOF:
        state.ulstate = END_STATE;
		break;
	case '\'':
		fputc(ch,state.outputfile);
        if('D'==state.sign_mark)
		{
            state.ulstate = NO_STATE;
		}
		else if('E'==state.sign_mark)
		{
            state.ulstate = C_STATE;
		}
		else
		{
			state.ulstate = CPP_STATE;
		}
		break;
	case '*':
		nextch = fgetc(state.inputfile);
		if(nextch=='/')
		{
			fputc(ch,state.outputfile);
            fputc(' ',state.outputfile);
        	fputc(nextch,state.outputfile);
		}
		else
		{
            fputc(ch,state.outputfile);
        	fputc(nextch,state.outputfile);
			fputc(' ',state.outputfile);
		}
		break;
	default:
        fputc(ch,state.outputfile);
		break;
	}
}