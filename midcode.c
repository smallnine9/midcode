#include"global.h"
#include"midcode.h"

int midcodeiter = 0;//��������Ԫʽ�����в�����Ԫʽ
int labelcnt = 0;//����������Ԫʽ��if��ת���ı��
char tempvar[200]; //���ڱ�����Ԫʽ��ʱ����������,���ڵݹ�������Ԫʽ
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

void traverToCreateMid(TreeNode * t)/*preProc��postProc�������Ǻ�����ָ��*/
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
char* nextvar() //����һ���µ���ʱ����
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
		fprintf(listing, "ת����Ԫʽʧ�ܣ����庯���ڵ�Ϊ��");
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
	t = t->child[0];//�����б��еĵ�һ������

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
void useFuncmidcode(TreeNode *t)//׼ȷ��˵�������ֻ�Ǹ���Щ�޷���ֵ��������������Ԫʽ��
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
		strcpy(tempvar, p3);//�����p3�����ָ���tempvar
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
/*��� ::= �����ӣ�{���˷�������������ӣ�}*/
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
				t = t->sibling->sibling; //tΪ��һ��factor�������еĻ���
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
		strcpy(p1, p3); //���ڱ��ʽ�������ӣ�����p3�õ��Ľ��Ҫ�ٸ�P1,

		t = t->sibling;//��ȡ��һ���ڵ�
		strcpy(tempvar, p3);//�ѱ��ʽ������õ�ֵ������tempvar��
	}
	
}

// p3 = p1 op p2  p3���������жϵĽ��
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
	insmidcode("jne", "", "",label1);//x86ϵͳ�����ڽ�����һ��ָ����и��Ĵ���ר�ű��ִ�н���Ƿ�Ϊ0
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
		insmidcode("givea", p1, p2, p3); //�ڸ�ֵ����У��Ⱥ���߳�������Ԫ�غ��ұ߳�������Ԫ�أ������Ĳ����ǲ�һ���ģ�����ǽ����ݸ�������Ԫ�����ڵ��ڴ棬�ұ��ǽ�����Ԫ�������ڴ�������ݸ������
		
	}
	t->child[0]->traverflag = 1;
	expmidcode(t->child[0]->sibling);//������ý�����tempvar�п�����һ��ʵ�εı�־�����������ķ��б��ʽ�����Բ��Ƕ�׵��µ�
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
	//��for��ı�־����ֵ
	
	strcpy(lab1, nextlab());
	insmidcode("lab", "", "", lab1);
	//���ѭ�����
	
	strcpy(Exitlab, nextlab());
	ifexpmidcode(t->child[0]->sibling);
	insmidcode("jne", "", "", Exitlab);
	//���������棬����ѭ��

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