/*本文件使用的是手工编码的方式来实现DFA*/
/*本文件的接口为getToken函数*/
#include "global.h"
#include "scan.h"
#include "util.h"


typedef enum
{START,DONE,INLSS, INGRE, INNOT,INNUM,INID,INCHAR,INSTR,INASSIGN}
StateType;

#define BUFLEN 256
char tokenString[MAXTOKENLEN + 1]; //一个单词，最长只能有MAXTOKENLEN个字符

static char lineBuf[BUFLEN]; /* holds the current line  一个行缓冲区*/
static int linepos = 0; /* current position in LineBuf 记录行缓冲中当前位置的指针 */
static int bufsize = 0; /* current size of buffer string  行缓冲的大小*/

static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/*保留字表*/
static struct
{
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED]
= { { "if",IF },{ "int",INT },{ "else",ELSE },{ "void",VOID },
{ "main",MAIN },{ "while",WHILE },{"for",FOR}, { "return",RETURN },
{ "scanf",SCANF },{"printf",PRINTF },{"var",VAR},
{"const",CONST },{"char",CHAR} };


/*采用行缓冲的方式从源文件中读取字符*/
static int getNextChar(void)
{
	if (!(linepos < bufsize))//linepos超出了lineBuf的长度
	{
		lineno++;  
		if (fgets(lineBuf, BUFLEN - 1, source)) //fgets函数每次读取一行
		{
			/*给源文件加上了行号，并且在屏幕中输出*/
			if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
			
			bufsize = strlen(lineBuf);//bufsize表示lineBuf的长度
			linepos = 0;

			return lineBuf[linepos++];
		}
		else //整个源文件已经读取完了
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
	
}



static void  ungetNextChar(void) //回溯一个字符
{
	if (!EOF_flag) linepos--; //防止linepos跌出数组的下界
}


static TokenType reservedLookup(char *s)
{
	int i;
	for (i = 0; i<MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/*scan.c的主要函数*/
TokenType getToken(void) //用DFA进行单词提取
{
	int save; //用来表示是否存储读取的字符

	int tokenStringIndex = 0;//存储到tokenString时用到的索引

	StateType state = START; //DFA中的初始状态

	TokenType currentToken; //当前读取的单词

	while (state != DONE) //没到终止状态
	{
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{

		case START:
			if (c == '<')
				state = INLSS;
			else if (c == '>')
				state = INGRE;
			else if (c == '=')
				state = INASSIGN;
			else if (c == '!')
				state = INNOT;
			else if (isdigit(c))
				state = INNUM;
			else if (isalpha(c) || c == '_')
				state = INID;
			else if (c == '\'')
				state = INCHAR;
			else if (c == '\"')
				state = INSTR;
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = FALSE;
			else
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = MULTI;
					break;
				case '/':
					currentToken = DIV;
					break;
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case';':
					currentToken = SEMI;
					break;
				case'[':
					currentToken = LBRACKET;
					break;
				case']':
					currentToken = RBRACKET;
					break;
				case'{':
					currentToken = LBRACE;
					break;
				case'}':
					currentToken = RBRACE;
					break;
			//	case'=':
			//		currentToken = EQ;
			//		break;
				case',':
					currentToken = COMMA;
					break;

				default:
					fprintf(listing, "Scanner Error:单词首字母不存在");
					currentToken = ERROR;
					break;
				}
			}
			break;

		case INLSS:
			if (!(c =='='))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = LSS;
			}
			else
			{
				state = DONE;
				currentToken = LEQ;
				
			}
			break;
		case INGRE:
			if (!(c=='='))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = GRE;
			}
			else
			{
				state = DONE;
				currentToken = GEQ;
			}
			break;
		case INASSIGN:
			if (!(c == '='))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = EQ;
			}
			else
			{
				state = DONE;
				currentToken = EQL;
			}
			break;
		case INNOT:
			if (!(c == '='))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NOT;
			}
			else
			{
				state = DONE;
				currentToken = NEQ;
			}
			break;
		case INNUM:
			if (!isdigit(c))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c)&&(c!='_')&&(!isdigit(c)))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case INCHAR:
			if (c == '+' || c == '-' || isdigit(c) || isalpha(c))
			{
				c = getNextChar();
				if (c == '\'')
				{
					state = DONE;
					currentToken = CCHAR;
				}
				else
				{
					fprintf(listing, "Scanner Error:字符常量中不止一个字符");
					state = DONE;
					currentToken = ERROR;
				}
			}
			else
			{ 
				fprintf(listing, "Scanner Error:不是合法的字符常量");
				state = DONE;
				currentToken = ERROR;
			}
			break;
		case INSTR:
			if (c == 32 || c == 33 || (c <= 126 && c >= 35));
			else
			{
				if (c == '\"')
				{
					state = DONE;
					currentToken = SSTR;
				}
				else
				{
					fprintf(listing, "Scanner Error:不是合法的字符串常量");
					state = DONE;
					currentToken = ERROR;
				}
			}
					
		break;
		case DONE:
			break;

		default:
			fprintf(listing, "Scanner Crash:state = %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;


		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
			{
				currentToken = reservedLookup(tokenString);
			}
		}
	
	} //while

   if (TraceScan)
	{
		fprintf(listing, "%d:", lineno);
		printToken(currentToken, tokenString);
	}
	//printf("%d\n", currentToken);

	return currentToken;
}