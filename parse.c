#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"


char *id_reserve;
int ParseBack = FALSE;

char *key = NULL;//��ǰҪ������ű�ķ�������
int level = 0;//Ĭ�ϵĿɼ��ȼ�
//int scope = 0;//Ĭ��Ϊ��һ��������
//int meloc = 0;//��¼�����ڷ��ű��λ��
int paranum = 0;
typedef struct symbolMeloc
{
	int symmeloc;
} SymbolScope;

typedef struct symbolLevel
{
	SymbolScope scope[1000];
} SymboLevel;

int levelscope[1000] = {0};//��¼ĳ��level��scope����������

SymboLevel SaveClass[1000]; //ʵ�ִ洢���һ�����ݽṹ��SaveClass[0]������¼�ɼ��Եȼ�Ϊ0�ķ��ŵ�λ��

static TokenType token;

static TreeNode * program(void);
static TreeNode * constdeclare(void);
static TreeNode * variabledeclare(void);
static TreeNode * variabledefine(void);
static TreeNode * notvoidfunction(void);
static TreeNode * voidfunction(void);
static TreeNode * mainfunction(void);
static TreeNode * constdefine(void);
//static TreeNode * typeid(void);
static TreeNode * parameter(void);
static TreeNode * compstatement(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * while_stmt(void);
static TreeNode * for_stmt(void);
static TreeNode * useFunction(void);
static TreeNode * val_parameter(void);
static TreeNode * return_stmt(void);
static TreeNode * scanf_stmt(void);
static TreeNode * printf_stmt(void);
static TreeNode * exp(void);
static TreeNode * factor(void);
static TreeNode * statementlist(void);
static TreeNode * if_exp(void);
static TreeNode * assign_stmt(void);
static TreeNode * term(void);

/*syntaxError�������������Ϣ*/
static void syntaxError(char * message)
{
	fprintf(listing, "\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}

/*match������ƥ��һ��tokenȻ���ȡ��һ��token*/
static void match(TokenType expected)
{
	if (token == expected)
	{
		token = getToken();
	//	printToken(token, tokenString);
	}
	else
	{
		syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		fprintf(listing, "      ");
	}
}

/*�ڵ㺯�������������ڵ㣬����ָ���ڵ���ӽڵ���ӽڵ���ֵܽڵ�*/
/*������ ::= �ۣ�����˵�����ݣۣ�����˵������{���з���ֵ�������壾|���޷���ֵ�������壾}����������*/
TreeNode * program() /*��ʡ����program�ڵ㣬��ʡһ��ռ�*/
{
	TreeNode *t = NULL;
	TreeNode *p = NULL;
	switch (token)  //��ȡ��һ���ַ�
	{
	case CONST: //����˵����Fisrt��CONST
		
		t = constdeclare();
		break;
	case INT:  //INT��CHAR��ͷ���з���ֵ��������
		
		t = notvoidfunction();
		break;
	case CHAR:
	
		t = notvoidfunction();
		break;
	case VAR://VAR��ͷ��ȫ�ֱ���˵��
	
		t = variabledeclare();
		break;
	case VOID://VOID��ͷ�п������޷���ֵ�������壬Ҳ�п�����������
	{
		match(VOID);  /*���棺�����Ѿ�match����VOID��������mainfunction�������voidfunction�����ﲻ��Ҫ��match��MAIN)*/
		if (token == MAIN)  //��������
		{
			
			t = mainfunction();
		}
		else if (token == ID) //���޷���ֵ��������
		{
			
			t = voidfunction();
		}
		else { syntaxError("voidf or main write  error"); }
	    break; 
	}
	default:
		syntaxError("first token program error!");
		return t;
	}
	p = t;
	/*�������������ж��*/
	if (t == NULL)  //�ڵ�Ϊ��˵������Ŀ�ͷ��д����
	{
		syntaxError("�﷨���������������ͷ");
		return t;
	}
	while (token != ENDFILE) //����û�ж����ļ�ĩβ
	{
		if (token == VAR)
		{
			p ->sibling = variabledeclare();
			p = p->sibling;
		}
		if (token == INT || token == CHAR)
		{
			/*ʹ�õ���������ͬ���ṹ��p���ô�����������*/
		
			p->sibling = notvoidfunction();
			p = p->sibling; 
		}
		else if (token == VOID)
		{    
			/*������һ�����ж��������������޷���ֵ��������*/
			match(VOID);
			if (token == MAIN)
			{  
			
				p->sibling = mainfunction();
				p = p->sibling;
			}
			else if (token == ID)
			{
		
				p->sibling = voidfunction();
				p = p->sibling;
			}
			else
			{
				syntaxError("voidf or main not find  error");
				return t;
			}
		}
	/*	else if (token == VAR)
		{
			p->sibling = variabledeclare();
			p = p->sibling;
		}*/
		else
		{ 
		 syntaxError("program error");
		 return t;
		}
	}
	return t;
}
/*��������*/
TreeNode *constdeclare()
{   
	
	match(CONST);
	TreeNode *t = makeNode(constdeclareK);
	TreeNode *p = t;
	t->child[0] = constdefine();
	match(SEMI);
	while (token == CONST)
	{
		p->child[0]->sibling = constdefine();
		p = p->child[0]->sibling;
		match(SEMI);
	}
	return t;
}
/*��������*/
TreeNode * constdefine()
{
	TreeNode *t = makeNode(constdefineK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t->type = Integer;
		
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->type = Char;
	}
	p->child[0] = makeNode(ConstK);
	key = p->child[0]->attr.name = copyString(tokenString);
	/*������һ��ȫ�ֳ�����������ű�*/

	match(ID);

	match(EQ);

	p->child[0]->attr.val = atoi(tokenString);//����������ڵ㸳�������ͳ���ֵ

	st_insert(key, level, levelscope[level], 0, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, p->child[0]->attr.val);

	match(NUM);
	while (token == COMMA)
	{
		match(COMMA);

		p->child[0]->sibling = makeNode(ConstK);
		p = p->child[0]->sibling;
		key = p->attr.name = copyString(tokenString);

		/*������һ��ȫ�ֳ�����������ű�*/
		match(ID);
		match(EQ);

		p->attr.val = atoi(tokenString);

		st_insert(key, level, levelscope[level], 0, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, p->child[0]->attr.val);
		SaveClass[level].scope[levelscope[level]].symmeloc++;//ʹ����һ��λ��

		match(NUM);

	}
	return t;
}

TreeNode * notvoidfunction()
{   
	level++;//����һ����������ô�ɼ��Լ�һ�����������1
	levelscope[level]++;//��ǰlevel��scope����+1

	TreeNode *t = makeNode(notvoidfunctionK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t->type = Integer;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->type = Char;
	}
	id_reserve = t->attr.name = copyString(tokenString);

	
	
	match(ID);
	match(LPAREN);
	p->child[0] = parameter();

	pt_insert(id_reserve, p->child[0]->child[0]);

	paranum = 0;

	st_insert(id_reserve, level, levelscope[level], 2, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, p->child[0]->attr.val, 0);//����һ���з���ֵ������,p->child[0]->attr.val�����������
	SaveClass[level].scope[levelscope[level]].symmeloc++;

	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);


	
	st_kill(level, levelscope[level]);//�ѵ�ǰlevel��ǰscope�����б�ʶ��������ɱ��

	level = level - 1;//�˳�һ��������
	return t;

}
TreeNode * voidfunction()
{
	level++;
	levelscope[level]++;//��ǰlevel��scope����+1
	TreeNode *t = makeNode(voidfunctionK);
	TreeNode *p = t;
	id_reserve = t->attr.name = copyString(tokenString);
	
	match(ID);
	match(LPAREN);
	p->child[0] = parameter();
	pt_insert(id_reserve, p->child[0]);

	st_insert(key, level, levelscope[level], 2, Void, SaveClass[level].scope[levelscope[level]].symmeloc, p->child[0]->attr.val, 0);//����һ���޷���ֵ������
	SaveClass[level].scope[levelscope[level]].symmeloc++;

	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);

	st_kill(level, levelscope[level]);
	level = level - 1;
	return t;
}
//main�����������һ���������飬û�б�ģ����Խ���main��ʱ������Ȳ���+level���ȵ����븴������ʱ���ټ�Ҳ����
TreeNode * mainfunction()
{

	TreeNode * t = makeNode(mainfunctionK);
	t->attr.name = copyString(tokenString);
	t->type = Void;
	match(MAIN);
	match(LPAREN);
	match(RPAREN);
	match(LBRACE);
	level++;
	levelscope[level]++;
	t->child[0] = compstatement();

	st_kill(level, levelscope[level]);
	level = level - 1;
	match(RBRACE);

	return t;
}

/*�������� ::= �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}| ���գ�*/
TreeNode* parameter()
{
	TreeNode *t = makeNode(parameterlistK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t ->child[0]= makeNode(paraK);//����һ�������ڵ�	 ������Ӧ����һ�������ڵ�
		t->child[0]->type = Integer;
	    key = t->child[0]->attr.name = copyString(tokenString);
		t->child[0]->attr.val = atoi(tokenString);

		st_insert(key,level,levelscope[level],3,Integer,SaveClass[level].scope[levelscope[level]].symmeloc,0,0);//����һ��������
		SaveClass[level].scope[levelscope[level]].symmeloc++;

		match(ID);
		paranum++;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->child[0] = makeNode(paraK);//����һ���ַ��ڵ�	
		t->child[0]->type = Char;
		key = t->child[0]->attr.name = copyString(tokenString);

		st_insert(key, level, levelscope[level], 3, Char, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ��������
		SaveClass[level].scope[levelscope[level]].symmeloc++;

		match(ID);
		paranum++;
	}
	while (token == COMMA)
	{
		match(COMMA);
		if (token == INT)
		{
			match(INT);
			t->child[0] = makeNode(paraK);//����һ�������ڵ�
			t->child[0]->type = Integer;
			key = t->child[0]->attr.name = copyString(tokenString);
			t->child[0]->attr.val = atoi(tokenString);
			st_insert(key, level, levelscope[level], 3, Integer, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ��������
			SaveClass[level].scope[levelscope[level]].symmeloc++;
			match(ID);
			paranum++;
		}
		else if (token == CHAR)
		{
			match(CHAR);
			t->child[0] = makeNode(paraK);//����һ���ַ��ڵ�	
			t->child[0]->type = Char;
			key = t->child[0]->attr.name = copyString(tokenString);

			st_insert(key, level, levelscope[level], 3, Char, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ��������
			SaveClass[level].scope[levelscope[level]].symmeloc++;

			match(ID);
			paranum++;
		}
	}
	if (t->child[0] == NULL)
	{
		t = NULL;
	}
	t->attr.val = paranum;//������ڵ㱣���˲���������Ϣ
	return t;

}

TreeNode* compstatement()
{

	TreeNode *t =makeNode(compstatementK);
	TreeNode *p = t;
	if (token == CONST)
	{
		t->child[0] = constdeclare();
		
	}
	else if (token == VAR)
	{
		t->child[0] = variabledeclare();
	}
    if (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		if (t->child[0] != NULL)
			t->child[0]->sibling = statementlist();
		else
			t->child[0] = statementlist();
		  //t = statementlist() ,��������滻�����������䣬����
	}
	else
	{
		syntaxError("���������д����");
	}

	return t;
}
TreeNode * statementlist()
{
	TreeNode* t = makeNode(statementlistK);

	TreeNode* p = t;
	if (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		p->child[0] = statement();
	}
	p = p->child[0];
	while (token == IF || token == WHILE || token == FOR ||
		token == LBRACE || token == ID || token == RETURN || token == SCANF || token == PRINTF)
	{
		p->sibling = statement();
		p = p->sibling;
	}

	return t;
}
TreeNode * statement()
{
	TreeNode *t = NULL;
	switch (token)
	{
	case IF:
		t = if_stmt();
		break;
	case WHILE:
		t = while_stmt();
		break;
	case FOR:
		t = for_stmt();
		break;
	case LBRACE:
		match(LBRACE);

		level++;
		levelscope[level]++;
		t = statementlist();
		st_kill(level, levelscope[level]);
		level = level - 1;
		match(RBRACE);
		return t;//��Ҫbreak��������ٴ�ƥ��һ��semi��Ȼ��������֮����Ҫsemi
		break;
	case ID:
		id_reserve = copyString(tokenString);
		t = useFunction();
		if (t == NULL)
		{
			ParseBack = TRUE;
			t = assign_stmt();
		}
		break;
	case RETURN:
		t = return_stmt();
		break;
	case SCANF:
		t = scanf_stmt();
		break;
	case PRINTF:
		t = printf_stmt();
		break;
	default:
		syntaxError("statement error");
		break;

	}
//	match(SEMI);
	return t;

}
//��������䣾 :: = if ��(������������)������䣾��else����䣾��
TreeNode * if_stmt()
{
	TreeNode *t = makeNode(if_stmtK);
	match(IF);
	match(LPAREN);
	t->child[0] = if_exp();
	match(RPAREN);
	t->child[0]->sibling = statement();
	if (token == ELSE)
	{
		match(ELSE);
		t->child[0]->sibling->sibling = statement();
	}
	return t;
}

TreeNode * if_exp()
{
	TreeNode *t = makeNode(if_expK);
	t->child[0] = exp();
	if (token == LSS || token == LEQ || token == GRE
		|| token == GEQ || token == NOT || token == NEQ||token == EQL)
	{
		TreeNode *p = t;
		p->child[0]->sibling = makeNode(OpK);
		p->attr.op = token;
		switch (token)
		{
		case LSS:
			match(LSS);
			break;
		case LEQ:
			match(LEQ);
			break;
		case GEQ:
			match(GEQ);
			break;
		case NOT:
			match(NOT);
			break;
		case NEQ:
			match(NEQ);
			break;
		case EQL:
			match(EQL);
			break;
		}
		p = p->child[0]->sibling;
		p->sibling = exp();
	}
	return t;
}
//��ѭ����䣾 :: = while ��(������������)������䣾 |

TreeNode * while_stmt()
{
	TreeNode *t = makeNode(while_stmtK);
	match(WHILE);
	match(LPAREN);
	t->child[0] = if_exp();
	match(RPAREN);
	t->child[0]->sibling = statement();
	return t;
}
//for��(������ʶ�����������ʽ��; ��������; ����ʶ����������ʶ����(+| -)����������)������䣾
TreeNode * for_stmt()
{
	TreeNode *t = makeNode(for_stmtK);
	match(LPAREN);
	key = copyString(tokenString);
	if(st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
	{
		syntaxError("δ��������");
	}
	t->attr.name = copyString(tokenString);
	match(ID);
	match(EQ);
	t->child[0] = exp();
	match(SEMI);
	t->child[0]->sibling = if_exp();
	match(SEMI);
	key = copyString(tokenString);
	if (st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
	{
		syntaxError("δ��������");
	}
	match(ID);
	match(EQ);
	key = copyString(tokenString);
	if (st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
	{
		syntaxError("δ��������");
	}
	match(ID);

    if(token==PLUS)
	{
		match(PLUS);
		t->attr.val = atoi(tokenString);
		t->child[0]->sibling->sibling = statement();
	}
	else if (token == MINUS)
	{
		match(MINUS);
		t->attr.val = -atoi(tokenString);
		t->child[0]->sibling->sibling = statement();
	}
	else
	{
		syntaxError("for_stmt error!");
	}
	return t;
	
}
//����ʶ������(����ֵ��������)��;
TreeNode * useFunction()
{
	TreeNode * t = makeNode(useFunctionK);
    id_reserve = t->attr.name = copyString(tokenString);

	match(ID);
	if (token == LPAREN)
	{
		if (st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
		{
			syntaxError("δ��������");
		}
		match(LPAREN);
		t->child[0] = val_parameter();

		if (pt_lookup(id_reserve, t->child[0]->child[0]) == -1)
		{
			syntaxError("���ú���ʵ�β���Ӧ");
		}
		match(RPAREN);
	}
	else
	{
		t = NULL;
		return t;
	}
	match(SEMI);
	return t;
}
//��ֵ������ ::= �����ʽ��{,�����ʽ��}�����գ�
TreeNode * val_parameter()
{
	TreeNode *t = makeNode(val_parameterK);
	TreeNode *p;
	if (token == ID||token==NUM||token == LPAREN)
	{
		//��ֵ������ ::= �����ʽ��{,�����ʽ��}�����գ�
		t->child[0] = exp();
		 p = t->child[0];
		while (token == COMMA)
		{
			match(COMMA);
			p->sibling = exp();
			p = p->sibling;
		}
	}
	else
	{
		t = NULL;
	}
	return t;
}
//��������䣾 ::= return[��(�������ʽ����)��];
TreeNode *return_stmt()
{
	TreeNode *t = makeNode(return_stmtK);
	match(RETURN);
	if (token == LPAREN)
	{
		match(LPAREN);
		t->child[0] = exp();
		match(RPAREN);
	}
	else
	{
		t->child[0] = NULL;
	}
	match(SEMI);
	return t;
}

//������䣾 :: = scanf ��(������ʶ����{ ,����ʶ���� }��)�� �������е�����
//����дscanf�Ĺ��ܾ�����ʲô�أ�
TreeNode * scanf_stmt()
{
	TreeNode *t = makeNode(scanf_stmtK);
	match(SCANF);
	match(LPAREN);
	key = t->attr.name = copyString(tokenString);
	if (st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
	{
		syntaxError("δ��������");
	}
	match(ID);
	match(RPAREN);

	match(SEMI);
	return t;

}
/*printf_stmt���ڻ�ֻ������ַ���*/
TreeNode * printf_stmt()
{
	TreeNode *t = makeNode(printf_stmtK);
	match(PRINTF);
	match(LPAREN);
	t->attr.name = copyString(tokenString);
	match(SSTR);
	match(RPAREN);
	match(SEMI);
	return t;


}
//�����ʽ�� ::= ���{���ӷ�����������} //exp�������͵�
TreeNode *exp()
{
	TreeNode *t = makeNode(expK);
	TreeNode *p = t;
	t->child[0] = term();
	t->type = t->child[0]->type;
	p = t->child[0];
	while (token == PLUS || token == MINUS)
	{
		if (token == PLUS)
		{
			match(PLUS);
			p->sibling = makeNode(OpK);
			p->sibling->attr.op = PLUS;
			p = p->sibling;
			p->sibling = term();


		}
		if (token == MINUS)
		{
			match(PLUS);
			p->sibling = makeNode(OpK);
			p->sibling->attr.op = MINUS;
			p = p->sibling;
			p->sibling = term();
		}
	}

	p = t->child[0];
	while (p != NULL)
	{
		if (p->nodekind == termK)
		{
			if (p->type != t->type)
			{
				syntaxError("������ʽ����֤���ʽ�е�����Ϊͬһ����");
			}
		}
		p = p->sibling;
	}
	return t;
}
/*��� ::= �����ӣ�{���˷�������������ӣ�}*/

TreeNode *term()
{ 
	TreeNode *t = makeNode(termK);
	t->child[0] = factor();
	t->type = t->child[0]->type;
	TreeNode * p = t->child[0];
	while (token == MULTI || token == DIV)
	{
		p->sibling = makeNode(OpK);
		if (token == MULTI)
		{
			p->sibling->attr.op = MULTI;
			match(MULTI);
		}
		if (token == DIV)
		{
			p->sibling->attr.op = DIV;
			match(DIV);
		}
		p = p->sibling;
		p->sibling = factor();
	}
	p = t->child[0];
	while (p != NULL)
	{
		if (p->nodekind == factorK)
		{
			if (p->type != t->type)
			{
				syntaxError("������ʽ����֤���ʽ�е�����Ϊͬһ����");
			}
		}
		p = p->sibling;
	}
	return t;
}
//�����ӣ� :: = ����ʶ����������ʶ������[�������ʽ����]������������ | ���ַ��������з���ֵ����������䣾 | ��(�������ʽ����)��
TreeNode *factor()
{
	TreeNode *t = makeNode(factorK);
	if (token == ID)  //���ӱ�ʾһ����ʶ�����п�����һ������Ҳ�п�����һ��������Ҳ�п�����һ�����飬Ҳ�п����Ǻ�������
	{
		id_reserve = key = t->attr.name = copyString(tokenString);

		if (st_lookup(key)==-1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
		{
			syntaxError("δ��������");
		}
		match(ID);
		t->type = basetypecheck(key);//���ݷ��ű�������
		
		if (token == LBRACKET) //��ʱ��������һ������Ԫ��
		{
			match(LBRACKET);
			t->nodekind = ArrayK;
			if (typecheck(key) != 4)
			{
				syntaxError("��ʶ���������飡");
			}
			t->child[0] = exp();  //���ʽ�ﱣ����������ƫ����
			match(RBRACKET);
		}
		else if (token == LPAREN)//�Ǻ����������
		{
			match(LPAREN);
			t->nodekind = useFunctionK;
			t->child[0] = val_parameter();
			if (!returncheck(t->attr.name))
			{
				syntaxError("����ȱ�ٷ���ֵ��");
		   }
			if (pt_lookup(t->attr.name, t->child[0]->child[0]) == -1)
			{
				syntaxError("���ú���������ƥ��\n");
			}
			match(RPAREN);

		}
	}

	else if (token == NUM)
	{
		t->attr.val = atoi(tokenString);
		t->nodekind = ConstK;//���ֵ�Ȼ��һ������
		t->type = Integer;
		match(NUM);
		return t;

	}
	else if (token == LPAREN)
	{
		match(LPAREN);
		t = exp();
		match(RPAREN);
	}
	return t;
}
//����ֵ��䣾 ::= ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
static TreeNode * assign_stmt()
{
	TreeNode* t = makeNode(AssignK);
	t->attr.op = EQ;
	t->child[0] = makeNode(IdK);
	if (ParseBack == TRUE)
	{
	    t->child[0]->attr.name = copyString(id_reserve);
		if (st_lookup(id_reserve)== -1)//����һ�·��ű����Ƿ����������ID�ļ�¼�����У��������壬���ޣ��жϳ���
		{
			syntaxError("δ��������");
		}
		ParseBack = FALSE;
	}
	if (token == LBRACKET)
	{
		match(LBRACKET);
		t->child[0]->nodekind = ArrayK;
		t->child[0]->sibling = exp();
		match(RBRACKET);
	}
	
	match(EQ);
	t->child[0]->sibling = exp();
	match(SEMI);
	return t;
}
//������˵���� ::= ���������壾;{���������壾;}
TreeNode * variabledeclare()
{
	match(VAR);
	TreeNode *t = makeNode(variabledeclareK);
	TreeNode *p = t;
	t->child[0] = variabledefine();
	match(SEMI);
	while (token == VAR)
	{
		match(VAR);
		p->child[0]->sibling = variabledefine();
		p = p->child[0]->sibling;
		match(SEMI);
	}
	return t;

}
//���������壾 ::= �����ͱ�ʶ����(����ʶ����|
//����ʶ������[�����޷�����������]��)
//{,(����ʶ����|����ʶ������[�����޷�����������]�� )}   

/*���Ƕ�һ�����������Ƕ���һ������*/
TreeNode * variabledefine()
{
	TreeNode * t = makeNode(variabledefineK);
	TreeNode * p = t;
	if (token == INT)
	{
		match(INT);
		t->type = Integer;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->type = Char;
	}
	p->child[0] = makeNode(varK);
	key = p->child[0]->attr.name = copyString(tokenString);

	match(ID);

	if (token == LBRACKET)
	{  
		st_insert(key, level, levelscope[level], 4, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ�����������
		match(LBRACKET);
		p->child[0]->nodekind = ArrayK;
		p->child[0]->attr.val = atoi(tokenString);
		match(NUM);
		match(RBRACKET);
	}
	else
	{
		st_insert(key, level, levelscope[level], 1, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ������ı���
	}
	p = p->child[0];
	while (token == COMMA)
	{
		match(COMMA);
		p->sibling= makeNode(varK);
		key = p->sibling->attr.name = copyString(tokenString);
		
		match(ID);
		if (token == LBRACKET)
		{
			st_insert(key, level, levelscope[level], 4, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//����һ�����������
			match(LBRACKET);
			p->sibling->nodekind = ArrayK;
			p->sibling->attr.val = atoi(tokenString);
			match(NUM);
			match(RBRACKET);
		}
		else 
		{
			st_insert(key, level, levelscope[level], 1, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//�ٲ���һ������ı���
		}
		p = p->sibling;
	
	}

	return t;
}


/*��������parse,�ӿ�ʼ�ڵ����ݹ��½�*/
TreeNode * parse()
{
	TreeNode *t;
	token = getToken();//ȡ����

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			SaveClass[i].scope[j].symmeloc = 0;
		}
	}

	t = program();//����ݹ��½�����

	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	if (token == ENDFILE)
	{
		fprintf(listing, "�﷨�����ɹ���������\n");
	    
	}
	return t;
}