#include "global.h"
#include "util.h"

void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	case IF:
	case WHILE:
	case ELSE:
	case RETURN:
	case CONST:
	case INT:
	case VOID:
	case MAIN:
	case CHAR:
	case SCANF:
	case PRINTF:
	case VAR:
		fprintf(listing,
			"reserved word: %s\n", tokenString);
		break;
	case EQ: fprintf(listing, "=\n"); break;
	case LSS: fprintf(listing, "<\n"); break;
	case LPAREN: fprintf(listing, "(\n"); break;
	case RPAREN: fprintf(listing, ")\n"); break;
	case SEMI: fprintf(listing, ";\n"); break;
	case PLUS: fprintf(listing, "+\n"); break;
	case MINUS: fprintf(listing, "-\n"); break;
	case MULTI: fprintf(listing, "*\n"); break;
	case DIV: fprintf(listing, "/\n"); break;
	case LBRACE:fprintf(listing, "{\n"); break;
	case RBRACE:fprintf(listing, "}\n"); break;
	case LBRACKET:fprintf(listing, "[\n"); break;
	case RBRACKET:fprintf(listing, "]\n"); break;
	case ENDFILE: fprintf(listing, "EOF\n"); break;
	case COMMA: fprintf(listing, ",\n"); break;
	case CCHAR:
		fprintf(listing,
			"CCHAR, val= %s\n", tokenString);
		break;
	case SSTR:
		fprintf(listing,
			"SSTR, val= %s\n", tokenString);
		break;
	case NUM:
		fprintf(listing,
			"NUM, val= %s\n", tokenString);
		break;
	case ID:
		fprintf(listing,
			"ID, name= %s\n", tokenString);
		break;
	case ERROR:
		fprintf(listing,
			"ERROR: %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	
	}

}

TreeNode * makeNode(NodeKind kind)
{
	TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	}
	else
	{
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = kind;
		t->lineno = lineno;
		t->attr.val = 0;
		t->attr.offset = -1;
		t->traverflag = 0;
	}
	return t;

}

/*复制一个token的名字*/
char * copyString(char * s)
{
	int n;
	char * t;
	if (s == NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else strcpy(t, s);
	return t;
}

static indentno = 0;
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/*void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL)
	{
		printSpces();

	}
}*/