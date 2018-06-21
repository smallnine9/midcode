#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 13
typedef enum
{	
	ENDFILE,ERROR,
	/*������*/
	INT,VOID,CHAR,MAIN,
	IF,ELSE,WHILE,RETURN,
	SCANF,PRINTF,CONST,FOR,VAR,

	/*�������*/
    PLUS,MINUS,MULTI,DIV,SEMI,LPAREN,EQL,
	RPAREN,LBRACKET,RBRACKET,GRE,GEQ,LSS,LEQ,
	NOT,NEQ,RBRACE,LBRACE,EQ,COMMA,
	/**/
	NUM,CCHAR,ID,SSTR
} TokenType;

typedef enum {constdeclareK, constdefineK, 
variabledeclareK, variabledefineK, parameterlistK,paraK,
notvoidfunctionK,voidfunctionK, mainfunctionK,

compstatementK, statementK, if_stmtK, while_stmtK, for_stmtK, return_stmtK,
useFunctionK, printf_stmtK, scanf_stmtK, factorK, AssignK,

ConstK, varK,
intK,charK,statementlistK,expK, termK,if_expK,OpK,
val_parameterK,ArrayK,IdK,
TypeK,CompStmt} NodeKind;


typedef enum { Null,Integer, Char,Void } ExpType;

extern FILE* source;
extern FILE* listing;
extern FILE* code;
#define MAXCHILDREN 1

/*ʵ�����ڵ�Ľṹ*/

typedef struct treeNode
{
	struct treeNode * child[MAXCHILDREN];
	struct treeNode * sibling;/*����ʵ�����ı�׼��������ͬ���ṹ*/
	int lineno;
	NodeKind nodekind;
	struct {
		TokenType op;
		int offset ; //��¼�����е�Ԫ�ص�������Ϊ-1��ʾ��������Ԫ��
		int val; //��¼������ֵ�����������ʶ����Ԫ�ظ���
		char * name;
	} attr;
	ExpType type; /* for type checking of exps */
	int traverflag;
} TreeNode;

extern int EchoSource;

extern int lineno; //��Դ�ļ������к��õ�

extern int Error;

extern int TraceScan;

/**************************** ��Ԫʽ��˵����
* op:      symbol of operator
* var1:    op1
* var2:    op2
* var3:    distination var
* e.g.
*      =   , 2 ,   , temp  ==> temp = 2;
*       []= , a , i , t       ==> t = a[i]
*      int ,   ,   , a     ==> int  a;
*      const,int,5 , a     ==> const int a = 5;
*      char,   , 30, a     ==> char a[30];
*      fupa,   ,   , a     ==> a is a function parameter
*      call, f ,   , a     ==> a = f()
*      call, f ,   ,       ==> f()
*      <=.., a , b ,       ==> a <= b
*      jne ,   ,   , lable ==> if not satisfy(==false) then jump
*      jmp ,   ,   , label ==> jump to label
*      lab:,   ,   , labx  ==> set label
*      geta, a , n , b     ==> b = a[n]
*      ret ,   ,   , (a)   ==> return a / return
*      prt , a , b , symb  ==> print("a", b)
*      scf ,   ,   , a     ==> scanf(a)
*      func,int,   , f     ==> start of function int f()
*      para,int,   , a     ==> f(int a, ...)
*      end ,   ,   , f     ==> end of function f
****************************/

typedef struct
{
	char op[10];
	char var1[200];
	char var2[30];
	char var3[30];
}fourvarcode;
#define MAXMIDCODE  1000

  fourvarcode midcode[MAXMIDCODE];



#endif