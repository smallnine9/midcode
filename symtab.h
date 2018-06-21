#ifndef  _SYMTAB_H_
#define  _SYMTAB_H_


void st_insert(char * name, int level, int scope, int type, ExpType basetype, int meloc, int nelts, int val);
int st_lookup(char *name);
void st_kill(int level, int scope);
int pt_lookup(char *name, TreeNode *t);
void pt_insert(char *name, TreeNode *t); //����һ����ͷ�Ĳ����ڵ�

int typecheck(char *name);

ExpType basetypecheck(char *name);
int returncheck(char *name);
#endif