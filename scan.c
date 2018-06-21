/*���ļ�ʹ�õ����ֹ�����ķ�ʽ��ʵ��DFA*/
/*���ļ��Ľӿ�ΪgetToken����*/
#include "global.h"
#include "scan.h"
#include "util.h"


typedef enum
{START,DONE,INLSS, INGRE, INNOT,INNUM,INID,INCHAR,INSTR,INASSIGN}
StateType;

#define BUFLEN 256
char tokenString[MAXTOKENLEN + 1]; //һ�����ʣ��ֻ����MAXTOKENLEN���ַ�

static char lineBuf[BUFLEN]; /* holds the current line  һ���л�����*/
static int linepos = 0; /* current position in LineBuf ��¼�л����е�ǰλ�õ�ָ�� */
static int bufsize = 0; /* current size of buffer string  �л���Ĵ�С*/

static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/*�����ֱ�*/
static struct
{
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED]
= { { "if",IF },{ "int",INT },{ "else",ELSE },{ "void",VOID },
{ "main",MAIN },{ "while",WHILE },{"for",FOR}, { "return",RETURN },
{ "scanf",SCANF },{"printf",PRINTF },{"var",VAR},
{"const",CONST },{"char",CHAR} };


/*�����л���ķ�ʽ��Դ�ļ��ж�ȡ�ַ�*/
static int getNextChar(void)
{
	if (!(linepos < bufsize))//linepos������lineBuf�ĳ���
	{
		lineno++;  
		if (fgets(lineBuf, BUFLEN - 1, source)) //fgets����ÿ�ζ�ȡһ��
		{
			/*��Դ�ļ��������кţ���������Ļ�����*/
			if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
			
			bufsize = strlen(lineBuf);//bufsize��ʾlineBuf�ĳ���
			linepos = 0;

			return lineBuf[linepos++];
		}
		else //����Դ�ļ��Ѿ���ȡ����
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
	
}



static void  ungetNextChar(void) //����һ���ַ�
{
	if (!EOF_flag) linepos--; //��ֹlinepos����������½�
}


static TokenType reservedLookup(char *s)
{
	int i;
	for (i = 0; i<MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/*scan.c����Ҫ����*/
TokenType getToken(void) //��DFA���е�����ȡ
{
	int save; //������ʾ�Ƿ�洢��ȡ���ַ�

	int tokenStringIndex = 0;//�洢��tokenStringʱ�õ�������

	StateType state = START; //DFA�еĳ�ʼ״̬

	TokenType currentToken; //��ǰ��ȡ�ĵ���

	while (state != DONE) //û����ֹ״̬
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
					fprintf(listing, "Scanner Error:��������ĸ������");
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
					fprintf(listing, "Scanner Error:�ַ������в�ֹһ���ַ�");
					state = DONE;
					currentToken = ERROR;
				}
			}
			else
			{ 
				fprintf(listing, "Scanner Error:���ǺϷ����ַ�����");
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
					fprintf(listing, "Scanner Error:���ǺϷ����ַ�������");
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