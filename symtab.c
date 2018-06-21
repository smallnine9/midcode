#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"global.h"
#include"symtab.h"

/*
���ű��е�type��
����0������1��������2������3������4 */

#define SymSize 211
#define SHIFT 4

typedef struct BucketListRec
{
	char *name;

	int level;//�ɼ��ԣ�0��ʾȫ�ֱ���
	int scope;//�����򣬿ɼ���һ����������һ��һ��
	int life_flag; //�����ڣ�0��ʾ�ñ�������δ����״̬��1��ʾ�ñ�����������������


	int type;//���C0�ķ����ʶ�����Ա�ʶ������������������������������  
	ExpType basetype;//�������͵�Ԫ�أ�����intA[]����basetypeΪint
	int memloc; //��������ڷ��ű���ĵ�ַ(?)
	int nelts; //Ԫ�ظ����������type�Ǻ��������ʾ��������
	int val; //���������ĳ�ֵ
	struct BucketListRec *next;
}*BucketList;

typedef struct FunParaRec
{
	char *name;
	ExpType paratype[SymSize];//����ÿ��������SIZE������ 

}FunPara;

static BucketList SymTable[SymSize];
static FunPara  ParaTable[SymSize];


/*������ʹ�õ�������hash����������hash�����ѵ����Ա����ͻô*/
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





/*st_insert ��������ű����һ����������ʹ�õ��ǿ���ɢ�з�ɢ�б��еĴʷ�������st_insert������ŵ�ԭ�����ж��ڵ�ǰlevel��scope���Ƿ�����ͬ�ķ��ţ�
���У�������ض���������ޣ���ɹ�����һ������
*/
void pt_insert(char *name,TreeNode *t) //����һ����ͷ�Ĳ����ڵ�
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
	if (i == num) //˵����paratype������в�������Ӧ�ж���һ��֮��û������
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
	int index = hash(name);/*ʹ��hash������nameӳ�䵽һ������ֵ*/
	BucketList l = (BucketList)malloc(sizeof(struct BucketListRec));
	if (SymTable[index] != NULL)
	{
		if (SymTable[index]->level == level&&SymTable[index]->scope == scope)
		{
			fprintf(listing, "�ض������");
			return;
 		}

	}
	/*�²���ķ��ŵ�һЩ����ֵ*/
	l->name = name;/*���Ǳ�ʶ��*/
	l->level = level;/*�ɼ��ȼ������������ɻ�ȫ�ֱ����ܼ򵥣�����0������
						   ������������е�if�������������䣬Ӧ�ö���һ��level�ģ�����level����2
						   һ����2a��һ����2b������˳�2a�������򣬶���Ҫ����2a��ķ��ţ���ô����2b
						   ��ʱ�򣬷��ű�����Щ2a��Ԫ��Ӧ����α����أ��ǰ�������������ı���
						   ��level�����ó�2ô��(���ּ���һ��scope)������ļ����ǰ�life_flag��
						   Ϊ0*/
	l->scope = scope;
	l->life_flag = 1;
	l->type = type;  /*type��basetype����������ģ������Ǹ�ֻ������type��ȴû�п���basetype����������
					 ���������ʱ���ǲ��Ͻ���*/
	l->basetype = basetype;
	l->memloc = meloc;/*���meloc��Ҳ�����ɻ󣬱����Ǹ�˵�Ƿ��ű��е�λ�ã���ôʲô�Ƿ��ű��е�λ�ã�
					  level2�е�һ�����ֵ��Ǳ���x���ڶ������ֵı�����y����ôx��meloc����0��y�ľ���1ô��
					   ������������и���̬���꣨���ɼ���+�洢λ�ã��ĸ��*/
	l->nelts = nelts;
	l->val = val;
	l->next = SymTable[index];
	SymTable[index] = l;
}
/*�ڷ��ű��в����Ƿ����name����
���ص��Ƿ��ŵĵ�ַ�������ַ������������Ԫʽ��(����������ôд��)
*/
int st_lookup(char *name)
{
	int h = hash(name);
	BucketList l = SymTable[h];
	/*str1=str2ʱ��strcmp����0*/
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