#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"global.h"
#include"symtab.h"

/*
符号表中的type：
常量0、变量1、函数名2、参数3、数组4 */

#define SymSize 211
#define SHIFT 4

typedef struct BucketListRec
{
	char *name;

	int level;//可见性，0表示全局变量
	int scope;//作用域，可见性一样，作用域不一定一样
	int life_flag; //生命期，0表示该变量处于未激活状态，1表示该变量处于生命周期内


	int type;//这个C0文法里标识符可以标识常量、变量、函数名、参数、数组  
	ExpType basetype;//构造类型的元素，比如intA[]，则basetype为int
	int memloc; //这个变量在符号表里的地址(?)
	int nelts; //元素个数，如果是type是函数，则表示参数个数
	int val; //变量或常量的初值
	struct BucketListRec *next;
}*BucketList;

typedef struct FunParaRec
{
	char *name;
	ExpType paratype[SymSize];//假如每个函数有SIZE个参数 

}FunPara;

static BucketList SymTable[SymSize];
static FunPara  ParaTable[SymSize];


/*虎书中使用的是这种hash函数，这种hash函数难道可以避免冲突么*/
static int hash(char *key)
{
	int temp = 0;
	int i = 0;
	while (key[i] != '\0')
	{
		temp = temp * 65599 + key[i];
		i++;
	}
	temp = temp %SymSize;
	return temp;

}





/*st_insert 用来向符号表插入一个变量，我使用的是开放散列法散列表中的词法作用域，st_insert插入符号的原则是判断在当前level和scope中是否有相同的符号，
若有，则输出重定义错误，若无，则成功插入一个符号
*/
void pt_insert(char *name,TreeNode *t) //传入一个开头的参数节点
{
	int index = hash(name);
	ParaTable[index].name = name;
	int i = 0;
	TreeNode *p = t;

	for (; p != NULL; i++)
	{
		
		ParaTable[index].paratype[i] = p->type;
		p = p->sibling;
	}
}
int pt_lookup(char *name,TreeNode *t)
{
	int index = hash(name);
	int i = 0;
	TreeNode *p = t;
	int num = SymTable[index]->nelts;
	while (p!=NULL&&i<num)
	{
		if (ParaTable[index].paratype[i] == p->type)
		{
			p = p->sibling;
			i++;
			continue;
		}
		else
		{
			return -1;
		}
	}
	if (i == num) //说明吧paratype里的所有参数都对应判断了一遍之后，没有问题
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
void st_insert(char * name, int level,int scope, int type, ExpType basetype, int meloc, int nelts, int val)
{
	int index = hash(name);/*使用hash函数将name映射到一个索引值*/
	BucketList l = (BucketList)malloc(sizeof(struct BucketListRec));
	if (SymTable[index] != NULL)
	{
		if (SymTable[index]->level == level&&SymTable[index]->scope == scope)
		{
			fprintf(listing, "重定义错误！");
			return;
 		}

	}
	/*新插入的符号的一些属性值*/
	l->name = name;/*这是标识符*/
	l->level = level;/*可见等级，这里我有疑惑，全局变量很简单，就是0，但是
						   如果有两个并列的if，大括号里的语句，应该都是一个level的，假设level都是2
						   一个是2a，一个是2b，如果退出2a的作用域，而且要保存2a里的符号，那么进入2b
						   的时候，符号表里那些2a的元素应该如何保存呢，是把这两个作用域的变量
						   的level都设置成2么？(我又加了一个scope)我这里的假想是把life_flag置
						   为0*/
	l->scope = scope;
	l->life_flag = 1;
	l->type = type;  /*type和basetype还是有区别的，北航那个只考虑了type，却没有考虑basetype，这样进行
					 语义分析的时候是不严谨的*/
	l->basetype = basetype;
	l->memloc = meloc;/*这个meloc我也很有疑惑，北航那个说是符号表中的位置，那么什么是符号表中的位置？
					  level2中第一个出现的是变量x，第二个出现的变量是y，那么x的meloc就是0，y的就是1么？
					   编译器设计中有个静态坐标（即可见性+存储位置）的概念。*/
	l->nelts = nelts;
	l->val = val;
	l->next = SymTable[index];
	SymTable[index] = l;
}
/*在符号表中查找是否存在name符号
返回的是符号的地址，这个地址是用于生成四元式的(北航的是这么写的)
*/
int st_lookup(char *name)
{
	int h = hash(name);
	BucketList l = SymTable[h];
	/*str1=str2时，strcmp返回0*/
	while ((l != NULL) && (strcmp(name, l->name) != 0)&&(l->life_flag != 1))
	l = l->next;

	if (l == NULL)
	{
		
		return -1;
	}
	else return l->memloc;
}
void st_kill(int level, int scope)
{
	int i = 0;
	BucketList l;
	for (; i < SymSize; i++)
	{
		l = SymTable[i];
		while (l != NULL)
		{
			if (l->level == level&&l->scope==scope)
			{
				l->life_flag = -1;
			}
			l = l->next;
		}
	}
}

int typecheck(char *name)
{
	int index = hash(name);
	return SymTable[index]->type;
}

ExpType basetypecheck(char *name)
{
	int index = hash(name);
	return SymTable[index]->basetype;
}

int returncheck(char *name)
{
	int  index = hash(name);  
	if (SymTable[index]->basetype != Void)
		return 1;
	else
		return 0;
}