#include"global.h"
#include"midcode.h"

int midcodeiter = 0;//用于向四元式数组中插入四元式
int labelcnt = 0;//用于生成四元式中if跳转语句的标记
char tempvar[200]; //用于保存四元式临时变量的名字,便于递归生成四元式
int varcnt = 0;

void createMidCode(TreeNode *t);
void paramidcode(TreeNode *t);
void stmtmidcode(TreeNode *t);
void expmidcode(TreeNode *t);
void ifmidcode(TreeNode *t);
void assignmidcode(TreeNode *t);
void whilemidcode(TreeNode *t);
void formidcode(TreeNode *t);
void returnmidcode(TreeNode *t);
void scanfmidcode(TreeNode *t);
void printfmidcode(TreeNode *t);

void traverToCreateMid(TreeNode * t)/*preProc和postProc两个都是函数的指针*/
{
	if (t != NULL)
	{
		if (t->traverflag == 0)
		{
			createMidCode(t);
		}
		else if (t->traverflag == 1)
		{
			return;
		}
		
		int i;
		for (i = 0; i < MAXCHILDREN; i++)
		{ 
			traverToCreateMid(t->child[i]);
		}
		traverToCreateMid(t->sibling);
	}
}
char *nextlab()
{
	char *label = (char*)malloc(sizeof(char) * 20);
	strcpy(label, "_LABEL_");
	sprintf(label, "_LABEL_%d", labelcnt++);
	return label;
}
char* nextvar() //生成一个新的临时变量
{
	char *var = (char*)malloc(sizeof(char) * 20);
	sprintf(var, "$_%d", varcnt++);
	return var;
}

void insmidcode(char* op, char* t1, char* t2, char* t3) {
	
	strcpy(midcode[midcodeiter].op, op);
	strcpy(midcode[midcodeiter].var1, t1);
	strcpy(midcode[midcodeiter].var2, t2);
	strcpy(midcode[midcodeiter].var3, t3);
	midcodeiter++;
}
void constmidcode(TreeNode *t)
{

	char  v1[200], v2[30], v3[30];
	t->traverflag = 1;

	if (t->type == Integer)
	{
		strcpy(v1, "int");
	}
	else if (t->type == Char)
	{
		strcpy(v1, "char");
	}
	t = t->child[0];
	
	while (t != NULL)
	{
		t->traverflag = 1;
		itoa(t->attr.val, v2, 10);
		insmidcode("CONST", v1, v2, t->attr.name);
		t = t->sibling;
	}
}
void varmidcode(TreeNode*t)
{
	t->traverflag = 1;
	char  v1[200], v2[30], v3[30];
	if (t->type == Integer)
	{
		strcpy(v1, "int");
	}
	else if (t->type == Char)
	{
		strcpy(v1, "char");
	}
	
	t = t->child[0];
	while (t != NULL)
	{
		t->traverflag = 1;
		if (t->type == ArrayK)
		{
			itoa(t->attr.val, v2, 10);
			insmidcode(v1, "", v2, t->attr.name);
		}
		else
		{

			insmidcode(v1, "", "", t->attr.name);
		}
		t = t->sibling;
	}
}
void compstmtmidcode(TreeNode *t)
{
	TreeNode *p = t->child[0];
	t->traverflag = 1;
	p->traverflag = 1;
	if (p->nodekind == constdefineK)
	{
		constmidcode(p);
		stmtmidcode(p->sibling);
	}
	else if (p->nodekind == variabledefineK)
	{
		varmidcode(p);
		stmtmidcode(p->sibling);
	}
	else
	{
		stmtmidcode(p);
	}
	
}
void funcmidcode(TreeNode*t)
{
	TreeNode *p = t;
	p->traverflag = 1;
	if (t == NULL)
	{
		fprintf(listing, "转换四元式失败，定义函数节点为空");
	}
	if(t->type==Integer)
	insmidcode("func", "int", "", t->attr.name);
	if (t->type == Char)
		insmidcode("func", "char", "", t->attr.name);
	if (t->type == Void)
		insmidcode("func", "void", "", t->attr.name);

	p= t->child[0];
	if (p->nodekind == parameterlistK)
	{
		paramidcode(p);
		compstmtmidcode(p->sibling);
	}
	else if (p->nodekind == compstatementK)
	{
		compstmtmidcode(p);
	}
	insmidcode("end", "", "", t->attr.name);
}
void paramidcode(TreeNode *t)
{
	t->traverflag = 1;
	t = t->child[0];//参数列表中的第一个参数

	while (t != NULL)
	{
		t->traverflag = 1;
		switch (t->type)
		{
		case Integer:
			insmidcode("para", "int", "", t->attr.name);
			break;
		case Char:
			insmidcode("para", "char", "", t->attr.name);
			break;
		default:
			break;
		}
		t = t->sibling;
	}

}
void valparamidcode(TreeNode *t)
{
	char type[100];
	if (t == NULL)
	{
		return;
	}
	else
	{
		t->traverflag = 1;
		t = t->child[0];
		while (t != NULL)
		{
			t->traverflag = 1;
			expmidcode(t);
			insmidcode("fupa", "", "", tempvar);
			t = t->sibling;
		}
	}
}
void useFuncmidcode(TreeNode *t)//准确地说这个函数只是给那些无返回值函数调用生成四元式的
{
	t->traverflag = 1;
	char p1[100], p2[100], p3[100];
	strcpy(p3, t->attr.name);
	insmidcode("call", "", "",p3 );
	valparamidcode(t->child[0]);
}
void factormidcode(TreeNode *t)
{
	char p1[100], p2[100], p3[100];
	t->traverflag = 1;
	if (t->nodekind == factorK)
	{
		strcpy(tempvar, t->attr.name);
		return;
	}
	else if (t->nodekind == ArrayK)
	{
		strcpy(p1, t->attr.name);
		itoa(t->attr.val, p2, 10);
		strcpy(p3, nextvar());
		insmidcode("geta", p1, p2, p3);
		strcpy(tempvar, p3);//将结果p3的名字赋给tempvar
		return;
	}
	else if (t->nodekind == useFunctionK)
	{
		strcpy(p1, t->attr.name);
		strcpy(p3, nextvar());
		valparamidcode(t->child[0]);
		insmidcode("call", p1, "", p3);
		strcpy(tempvar, p3);
		return;
	}
	else if (t->nodekind == ConstK)
	{
		itoa(t->attr.val, tempvar, 10);
	}
	else if (t->nodekind == expK)
	{
		expmidcode(t);
	}
}
/*＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}*/
void termmidcode(TreeNode *t)
{

	t->traverflag = 1;
	char p1[100], p2[100], op[20], p3[100];
	t = t->child[0];

		while (t != NULL)  //
		{
			
			factormidcode(t);
			strcpy(p1, tempvar);
			 if (t->sibling != NULL)
			{
				switch (t->sibling->attr.op)
				{
				case MULTI:
					strcpy(op, "*");
					break;
				case DIV:
					strcpy(op, "/");
					break;
				default:
					break;
				}
				t = t->sibling->sibling; //t为下一个factor（假如有的话）
				factormidcode(t);
				strcpy(p2, tempvar);
				strcpy(p3, nextvar());
				insmidcode(op, p1, p2, p3);
				strcpy(p1, p3);
			}  //if
			 else
			 {
				 break;
			 }

		}
	

		strcpy(tempvar, p1);
}
void expmidcode(TreeNode *t)
{
	t->traverflag = 1;
	char p1[100], p2[100], op[20],p3[100];
	t = t->child[0];
	termmidcode(t);
	strcpy(p1, tempvar);
	t = t->sibling;
	while (t != NULL)
	{
		switch (t->attr.op)
		{
		case PLUS:
			strcpy(op, "+");
			break;
		case MINUS:
			strcpy(op, "-");
			break;
		default:
			break;
		}
		t = t->sibling;
		termmidcode(t);
		strcpy(p2, tempvar);
		strcpy(p3, nextvar());
		insmidcode(op, p1, p2, p3);
		strcpy(p1, p3); //由于表达式可以连加，所以p3得到的结果要再给P1,

		t = t->sibling;//读取下一个节点
		strcpy(tempvar, p3);//把表达式最终求得的值，放在tempvar中
	}
	
}

// p3 = p1 op p2  p3保存条件判断的结果
void ifexpmidcode(TreeNode*t)
{
	t->traverflag = 1;
	char p1[100], p2[100],op[20],p3[100];
	expmidcode(t->child[0]);
	strcpy(p1, tempvar);
	switch (t->attr.op)
	{
	case EQL:
		strcpy(op, "==");
		break;
	case NOT:
		strcpy(op, "!");
		break;
	case NEQ:
		strcpy(op, "!=");
		break;
	case GRE:
		strcpy(op, ">");
		break;
	case GEQ:
		strcpy(op, ">=");
		break;
	case LSS:
		strcpy(op, "<");
		break;
	case LEQ:
		strcpy(op, "<=");
		break;
	default:
		break;
	}
	expmidcode(t->child[0]->sibling->sibling);
	strcpy(p2, tempvar);
	insmidcode(op, p1, p2, "");
}

void stmtmidcode(TreeNode *t)
{
	t->traverflag = 1;
	t = t->child[0];
	while (t != NULL)
	{
		switch (t->nodekind)
		{
		case if_stmtK:
			ifmidcode(t);
			break;
		case while_stmtK:
			whilemidcode(t);
			break;
		case for_stmtK:
			formidcode(t);
			break;
		case return_stmtK:
			returnmidcode(t);
			break;
		case scanf_stmtK:
			scanfmidcode(t);
			break;
		case printf_stmtK:
			printfmidcode(t);
			break;
		case AssignK:
			assignmidcode(t);
			break;
		case useFunctionK:
			useFuncmidcode(t);
			break;
		case statementlistK:
		{
			t = t->child[0];
			while (t != NULL)
			{
				stmtmidcode(t);
				t = t->sibling;
			}
		}
		default:
			break;
		}
		t = t->sibling;
	}
}
void ifmidcode(TreeNode *t)
{
	t->traverflag = 1;
	char label1[100], label2[100];
	strcpy(label1, nextlab());
	strcpy(label2, nextlab());
	ifexpmidcode(t->child[0]);
	insmidcode("jne", "", "",label1);//x86系统好像在进行完一条指令后有个寄存器专门标记执行结果是否为0
	t = t->child[0]->sibling;
	stmtmidcode(t);
	insmidcode("jump", "", "", label2);
	insmidcode("lab", "", "", label1);
	if (t->sibling != NULL)
	{
		stmtmidcode(t->sibling);
	}
	insmidcode("lab", "", "", label2);
	
}
void assignmidcode(TreeNode *t)
{
	t->traverflag = 1;
	char p1[100], p2[100],p3[100];
	strcpy(p3, t->child[0]->attr.name);
	if (t->child[0]->nodekind == ArrayK)
	{
		strcpy(p1, t->attr.name);
		itoa(t->attr.val, p2, 10);
		strcpy(p3, nextvar());
		insmidcode("givea", p1, p2, p3); //在赋值语句中，等号左边出现数组元素和右边出现数组元素，机器的操作是不一样的，左边是将数据赋给数组元素所在的内存，右边是将数组元素所在内存里的数据赋给左边
		
	}
	t->child[0]->traverflag = 1;
	expmidcode(t->child[0]->sibling);//这个调用结束后tempvar有可能是一个实参的标志。这是由于文法中表达式语句可以层层嵌套导致的
	insmidcode("=", tempvar, "", p3);
}
void whilemidcode(TreeNode *t)
{
	t->traverflag = 1;
	char lab1[100],Exitlab[100];
	strcpy(lab1, nextlab());
	strcpy(Exitlab, nextlab());
	insmidcode("lab", "", "", lab1);
	
	ifexpmidcode(t->child[0]);
	
	insmidcode("jne", "", "", Exitlab);
	stmtmidcode(t->child[0]->sibling);
	insmidcode("jump", "", "", lab1);
	insmidcode("lab", "", "", Exitlab);
	
}
void formidcode(TreeNode *t)
{
	t->traverflag = 1;
	char id[100],lab1[100],Exitlab[100],step[100];
	expmidcode(t->child[0]);
	
	insmidcode("=", tempvar, "", id);
	//给for里的标志符赋值
	
	strcpy(lab1, nextlab());
	insmidcode("lab", "", "", lab1);
	//设计循环标记
	
	strcpy(Exitlab, nextlab());
	ifexpmidcode(t->child[0]->sibling);
	insmidcode("jne", "", "", Exitlab);
	//若条件不真，跳出循环

	if (t->attr.val > 0)
	{
		itoa(t->attr.val, step, 10);
		insmidcode("+", step, id, id);
	}
	else if (t->attr.val < 0)
	{
		int a = -t->attr.val;
		itoa(a, step, 10);
		insmidcode("-", step, id, id);
	}

	stmtmidcode(t->child[0]->sibling->sibling);
	insmidcode("jump", "", "", lab1);
	insmidcode("lab", "", "", Exitlab);

}
void returnmidcode(TreeNode *t)
{
	t->traverflag = 1;
	if (t->child[0] != NULL)
	{
		expmidcode(t->child[0]);
		insmidcode("ret","" ,"" ,tempvar);
	}
	else
	{
		insmidcode("ret","" ,"" ,"" );
	}
}
void scanfmidcode(TreeNode *t)
{
	t->traverflag = 1;
	insmidcode("scf","","",t->attr.name);
}

void printfmidcode(TreeNode *t)
{
	t->traverflag = 1;
	insmidcode("ptf","","",t->attr.name);
}
void createMidCode(TreeNode *t)
{
	t->traverflag = 1;
	switch (t->nodekind)
	{
	case constdefineK:
		constmidcode(t);
		break;
	case variabledefineK:
		varmidcode(t);
		break;
	case notvoidfunctionK:
		funcmidcode(t);
		break;
	case voidfunctionK:
		funcmidcode(t);
		break;
	case mainfunctionK:
		funcmidcode(t);
		break;
	case if_stmtK:
		ifmidcode(t);
		break;
	case while_stmtK:
		whilemidcode(t);
		break;
	case for_stmtK:
		formidcode(t);
		break;
	case return_stmtK:
		returnmidcode(t);
		break;
	case scanf_stmtK:
		scanfmidcode(t);
		break;
	case printf_stmtK:
		printfmidcode(t);
		break;
	case AssignK:
		assignmidcode(t);
		break;
	case useFunctionK:
		useFuncmidcode(t);
		break;
	default:
		break;
	}
}