#include "global.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
#include "symtab.h"


char *id_reserve;
int ParseBack = FALSE;

char *key = NULL;//当前要插入符号表的符号名称
int level = 0;//默认的可见等级
//int scope = 0;//默认为第一个作用域
//int meloc = 0;//记录符号在符号表的位置
int paranum = 0;
typedef struct symbolMeloc
{
	int symmeloc;
} SymbolScope;

typedef struct symbolLevel
{
	SymbolScope scope[1000];
} SymboLevel;

int levelscope[1000] = {0};//记录某个level的scope计数到哪了

SymboLevel SaveClass[1000]; //实现存储类的一个数据结构，SaveClass[0]用来记录可见性等级为0的符号的位置

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

/*syntaxError用来输出错误信息*/
static void syntaxError(char * message)
{
	fprintf(listing, "\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}

/*match是用来匹配一个token然后读取下一个token*/
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

/*节点函数是用来创建节点，并且指明节点的子节点和子节点的兄弟节点*/
/*＜程序＞ ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞*/
TreeNode * program() /*我省略了program节点，节省一点空间*/
{
	TreeNode *t = NULL;
	TreeNode *p = NULL;
	switch (token)  //读取第一个字符
	{
	case CONST: //常量说明的Fisrt是CONST
		
		t = constdeclare();
		break;
	case INT:  //INT或CHAR开头是有返回值函数定义
		
		t = notvoidfunction();
		break;
	case CHAR:
	
		t = notvoidfunction();
		break;
	case VAR://VAR开头是全局变量说明
	
		t = variabledeclare();
		break;
	case VOID://VOID开头有可能是无返回值函数定义，也有可能是主函数
	{
		match(VOID);  /*警告：这里已经match掉了VOID，所以在mainfunction函数里和voidfunction函数里不需要再match（MAIN)*/
		if (token == MAIN)  //是主函数
		{
			
			t = mainfunction();
		}
		else if (token == ID) //是无返回值函数定义
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
	/*非主函数可以有多个*/
	if (t == NULL)  //节点为空说明程序的开头就写错了
	{
		syntaxError("语法分析错误，请检查程序开头");
		return t;
	}
	while (token != ENDFILE) //程序没有读到文件末尾
	{
		if (token == VAR)
		{
			p ->sibling = variabledeclare();
			p = p->sibling;
		}
		if (token == INT || token == CHAR)
		{
			/*使用的是最左孩子同属结构，p的用处在这里体现*/
		
			p->sibling = notvoidfunction();
			p = p->sibling; 
		}
		else if (token == VOID)
		{    
			/*跟上面一样，判断是主函数还是无返回值函数定义*/
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
/*常量声明*/
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
/*常量定义*/
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
	/*声明了一个全局常量，插入符号表*/

	match(ID);

	match(EQ);

	p->child[0]->attr.val = atoi(tokenString);//给这个常量节点赋常量名和常量值

	st_insert(key, level, levelscope[level], 0, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, p->child[0]->attr.val);

	match(NUM);
	while (token == COMMA)
	{
		match(COMMA);

		p->child[0]->sibling = makeNode(ConstK);
		p = p->child[0]->sibling;
		key = p->attr.name = copyString(tokenString);

		/*声明了一个全局常量，插入符号表*/
		match(ID);
		match(EQ);

		p->attr.val = atoi(tokenString);

		st_insert(key, level, levelscope[level], 0, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, p->child[0]->attr.val);
		SaveClass[level].scope[levelscope[level]].symmeloc++;//使用了一个位置

		match(NUM);

	}
	return t;
}

TreeNode * notvoidfunction()
{   
	level++;//进入一个函数，那么可见性加一级，作用域加1
	levelscope[level]++;//当前level的scope计数+1

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

	st_insert(id_reserve, level, levelscope[level], 2, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, p->child[0]->attr.val, 0);//插入一个有返回值函数名,p->child[0]->attr.val保存参数个数
	SaveClass[level].scope[levelscope[level]].symmeloc++;

	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);


	
	st_kill(level, levelscope[level]);//把当前level当前scope的所有标识符的生命杀死

	level = level - 1;//退出一个作用域
	return t;

}
TreeNode * voidfunction()
{
	level++;
	levelscope[level]++;//当前level的scope计数+1
	TreeNode *t = makeNode(voidfunctionK);
	TreeNode *p = t;
	id_reserve = t->attr.name = copyString(tokenString);
	
	match(ID);
	match(LPAREN);
	p->child[0] = parameter();
	pt_insert(id_reserve, p->child[0]);

	st_insert(key, level, levelscope[level], 2, Void, SaveClass[level].scope[levelscope[level]].symmeloc, p->child[0]->attr.val, 0);//插入一个无返回值函数名
	SaveClass[level].scope[levelscope[level]].symmeloc++;

	match(RPAREN);
	match(LBRACE);
	p->child[0]->sibling = compstatement();
	match(RBRACE);

	st_kill(level, levelscope[level]);
	level = level - 1;
	return t;
}
//main函数里面就是一个复合语句块，没有别的，所以进入main的时候可以先不用+level，等到进入复合语句的时候再加也不迟
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

/*＜参数表＞ ::= ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞*/
TreeNode* parameter()
{
	TreeNode *t = makeNode(parameterlistK);
	TreeNode *p = t;
	if (token == INT)
	{
		match(INT);
		t ->child[0]= makeNode(paraK);//生成一个整数节点	 改正：应该是一个参数节点
		t->child[0]->type = Integer;
	    key = t->child[0]->attr.name = copyString(tokenString);
		t->child[0]->attr.val = atoi(tokenString);

		st_insert(key,level,levelscope[level],3,Integer,SaveClass[level].scope[levelscope[level]].symmeloc,0,0);//插入一个参数名
		SaveClass[level].scope[levelscope[level]].symmeloc++;

		match(ID);
		paranum++;
	}
	else if (token == CHAR)
	{
		match(CHAR);
		t->child[0] = makeNode(paraK);//生成一个字符节点	
		t->child[0]->type = Char;
		key = t->child[0]->attr.name = copyString(tokenString);

		st_insert(key, level, levelscope[level], 3, Char, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个参数名
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
			t->child[0] = makeNode(paraK);//生成一个整数节点
			t->child[0]->type = Integer;
			key = t->child[0]->attr.name = copyString(tokenString);
			t->child[0]->attr.val = atoi(tokenString);
			st_insert(key, level, levelscope[level], 3, Integer, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个参数名
			SaveClass[level].scope[levelscope[level]].symmeloc++;
			match(ID);
			paranum++;
		}
		else if (token == CHAR)
		{
			match(CHAR);
			t->child[0] = makeNode(paraK);//生成一个字符节点	
			t->child[0]->type = Char;
			key = t->child[0]->attr.name = copyString(tokenString);

			st_insert(key, level, levelscope[level], 3, Char, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个参数名
			SaveClass[level].scope[levelscope[level]].symmeloc++;

			match(ID);
			paranum++;
		}
	}
	if (t->child[0] == NULL)
	{
		t = NULL;
	}
	t->attr.val = paranum;//参数表节点保存了参数个数信息
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
		  //t = statementlist() ,好像可以替换掉上面这个语句，简化树
	}
	else
	{
		syntaxError("复合语句书写错误");
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
		return t;//不要break，否则会再次匹配一个semi，然而大括号之后不需要semi
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
//＜条件语句＞ :: = if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
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
//＜循环语句＞ :: = while ‘(’＜条件＞‘)’＜语句＞ |

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
//for‘(’＜标识符＞＝＜表达式＞; ＜条件＞; ＜标识符＞＝＜标识符＞(+| -)＜步长＞‘)’＜语句＞
TreeNode * for_stmt()
{
	TreeNode *t = makeNode(for_stmtK);
	match(LPAREN);
	key = copyString(tokenString);
	if(st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
	{
		syntaxError("未声明错误");
	}
	t->attr.name = copyString(tokenString);
	match(ID);
	match(EQ);
	t->child[0] = exp();
	match(SEMI);
	t->child[0]->sibling = if_exp();
	match(SEMI);
	key = copyString(tokenString);
	if (st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
	{
		syntaxError("未声明错误");
	}
	match(ID);
	match(EQ);
	key = copyString(tokenString);
	if (st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
	{
		syntaxError("未声明错误");
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
//＜标识符＞‘(’＜值参数表＞‘)’;
TreeNode * useFunction()
{
	TreeNode * t = makeNode(useFunctionK);
    id_reserve = t->attr.name = copyString(tokenString);

	match(ID);
	if (token == LPAREN)
	{
		if (st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
		{
			syntaxError("未声明错误");
		}
		match(LPAREN);
		t->child[0] = val_parameter();

		if (pt_lookup(id_reserve, t->child[0]->child[0]) == -1)
		{
			syntaxError("调用函数实参不对应");
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
//＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}｜＜空＞
TreeNode * val_parameter()
{
	TreeNode *t = makeNode(val_parameterK);
	TreeNode *p;
	if (token == ID||token==NUM||token == LPAREN)
	{
		//＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}｜＜空＞
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
//＜返回语句＞ ::= return[‘(’＜表达式＞‘)’];
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

//＜读语句＞ :: = scanf ‘(’＜标识符＞{ ,＜标识符＞ }‘)’ 这个语句有点问题
//这样写scanf的功能究竟是什么呢？
TreeNode * scanf_stmt()
{
	TreeNode *t = makeNode(scanf_stmtK);
	match(SCANF);
	match(LPAREN);
	key = t->attr.name = copyString(tokenString);
	if (st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
	{
		syntaxError("未声明错误");
	}
	match(ID);
	match(RPAREN);

	match(SEMI);
	return t;

}
/*printf_stmt现在还只能输出字符串*/
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
//＜表达式＞ ::= ＜项＞{＜加法运算符＞＜项＞} //exp是有类型的
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
				syntaxError("请检查表达式，保证表达式中的数据为同一类型");
			}
		}
		p = p->sibling;
	}
	return t;
}
/*＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}*/

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
				syntaxError("请检查表达式，保证表达式中的数据为同一类型");
			}
		}
		p = p->sibling;
	}
	return t;
}
//＜因子＞ :: = ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞ | ‘(’＜表达式＞‘)’
TreeNode *factor()
{
	TreeNode *t = makeNode(factorK);
	if (token == ID)  //因子表示一个标识符，有可能是一个常量也有可能是一个变量，也有可能是一个数组，也有可能是函数调用
	{
		id_reserve = key = t->attr.name = copyString(tokenString);

		if (st_lookup(key)==-1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
		{
			syntaxError("未声明错误");
		}
		match(ID);
		t->type = basetypecheck(key);//根据符号表赋予类型
		
		if (token == LBRACKET) //这时候因子是一个数组元素
		{
			match(LBRACKET);
			t->nodekind = ArrayK;
			if (typecheck(key) != 4)
			{
				syntaxError("标识符不是数组！");
			}
			t->child[0] = exp();  //表达式里保存的是数组的偏移量
			match(RBRACKET);
		}
		else if (token == LPAREN)//是函数调用语句
		{
			match(LPAREN);
			t->nodekind = useFunctionK;
			t->child[0] = val_parameter();
			if (!returncheck(t->attr.name))
			{
				syntaxError("函数缺少返回值！");
		   }
			if (pt_lookup(t->attr.name, t->child[0]->child[0]) == -1)
			{
				syntaxError("调用函数参数不匹配\n");
			}
			match(RPAREN);

		}
	}

	else if (token == NUM)
	{
		t->attr.val = atoi(tokenString);
		t->nodekind = ConstK;//数字当然是一个常量
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
//＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
static TreeNode * assign_stmt()
{
	TreeNode* t = makeNode(AssignK);
	t->attr.op = EQ;
	t->child[0] = makeNode(IdK);
	if (ParseBack == TRUE)
	{
	    t->child[0]->attr.name = copyString(id_reserve);
		if (st_lookup(id_reserve)== -1)//查找一下符号表中是否有下面这个ID的记录，若有，符合语义，若无，判断出错
		{
			syntaxError("未声明错误");
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
//＜变量说明＞ ::= ＜变量定义＞;{＜变量定义＞;}
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
//＜变量定义＞ ::= ＜类型标识符＞(＜标识符＞|
//＜标识符＞‘[’＜无符号整数＞‘]’)
//{,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’ )}   

/*不是定一个变量，就是定义一个数组*/
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
		st_insert(key, level, levelscope[level], 4, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个定义的数组
		match(LBRACKET);
		p->child[0]->nodekind = ArrayK;
		p->child[0]->attr.val = atoi(tokenString);
		match(NUM);
		match(RBRACKET);
	}
	else
	{
		st_insert(key, level, levelscope[level], 1, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个定义的变量
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
			st_insert(key, level, levelscope[level], 4, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//插入一个定义的数组
			match(LBRACKET);
			p->sibling->nodekind = ArrayK;
			p->sibling->attr.val = atoi(tokenString);
			match(NUM);
			match(RBRACKET);
		}
		else 
		{
			st_insert(key, level, levelscope[level], 1, t->type, SaveClass[level].scope[levelscope[level]].symmeloc, 0, 0);//再插入一个定义的变量
		}
		p = p->sibling;
	
	}

	return t;
}


/*主函数，parse,从开始节点进入递归下降*/
TreeNode * parse()
{
	TreeNode *t;
	token = getToken();//取单词

	for (int i = 0; i < 1000; i++)
	{
		for (int j = 0; j < 1000; j++)
		{
			SaveClass[i].scope[j].symmeloc = 0;
		}
	}

	t = program();//进入递归下降分析

	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	if (token == ENDFILE)
	{
		fprintf(listing, "语法分析成功！！！！\n");
	    
	}
	return t;
}