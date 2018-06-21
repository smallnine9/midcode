/*������һЩʵ�ú���*/

#ifndef _UTIL_H_
#define _UTIL_H_

/* Procedure printToken prints a token
* and its lexeme to the listing file
*/
void printToken(TokenType, const char*);


/*��������newNode����������������Ҷ�ڵ��*/
/* Function newStmtNode creates a new statement
* node for syntax tree construction
*/

/* Function newExpNode creates a new expression
* node for syntax tree construction
*/
TreeNode * makeNode(NodeKind);


/* Function copyString allocates and makes a new
* copy of an existing string
*/
char * copyString(char *);

/* procedure printTree prints a syntax tree to the
* listing file using indentation to indicate subtrees
*/
void printTree(TreeNode *);

void printTree(TreeNode *);


#endif