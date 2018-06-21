#include "global.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
* generate code
*/
#define NO_CODE FALSE
#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#include"midcode.h"

#if !NO_CODE
#include "cgen.h"
#endif
#endif


int lineno = 0;
FILE *source;
FILE *code;
FILE *listing;
/*���������ļ�ָ��*/



/*5��flag*/
int Error = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int EchoSource = FALSE;


int main(int argc, char *argv[])
{
	TreeNode *syntaxTree = NULL;//�����﷨�����ڵ�
	
 /*  argc: ����,����ͳ�������г���ʱ�͸�main�����������в����ĸ���
     argv[ ]: ָ�����飬�������ָ������ַ���������ָ�룬ÿһ��Ԫ��ָ��һ������
     argv[0] ָ��������е�ȫ·����
     argv[1] ָ����DOS��������ִ�г�������ĵ�һ���ַ���
     argv[2] ָ��ִ�г�������ĵڶ����ַ���
*/
	char pgm[120];
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgm, argv[1]);//copy argv[1] to pgm

	if (strchr(pgm, '.') == NULL) //����pgm���Ƿ��к�׺
		strcat(pgm, ".tg");//���û�У���pgm(argv[1]��.tg��������)

	source = fopen(pgm, "r");//open the source file��ֻ���ļ�
	
	/*sourceԴ�ļ�ָ��մ���*/
	if (source == NULL)
	{
		fprintf(stderr, "0000");
		exit(1);
	}
	/*fprintf��������ļ� stdout��׼�����Ĭ��Ҳ���������Ļ*/
	/*stdout�����ض�λ���ļ���ʹ��freopen*/
	listing = stdout;
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE //scan-onlyɨ����


	while (getToken() != ENDFILE);
	

#else


	syntaxTree = parse(); /*����parse��������﷨��*/

	traverToCreateMid(syntaxTree);

	if (TraceParse)
	{
		fprintf(listing, "\nSyntax tree:\n");
	}


#if !NO_CODE

	/*�޴����ʱ�򣬲��ܽ�������Ĵ���飬���ӳ���Ŀɿ���*/
	if (!Error)
	{
		char * codefile;
		int fnlen = strcspn(pgm, ".");

		codefile = (char *)calloc(fnlen + 4, sizeof(char));
		//calloc�ڶ�̬�������ڴ���Զ���ʼ�����ڴ�ռ�Ϊ�㣬
		//��malloc����ʼ��������������������������

		strncpy(codefile, pgm, fnlen);
		//��pgm����fnlen���ַ����Ƶ�codefile��
		strcat(codefile, ".asm");
		//��codefile���׺.asm���


		code = fopen(codefile, "w");//ֻд�ļ�

		if (code == NULL)
		{
			printf("Unable to open %s\n", codefile);
			exit(1);
		}

		/*codeGen���ɴ���*/
		codeGen(codefile);
		fclose(code);
	}

#endif//NO_CODE
#endif // !NO_ANALYZE

	fclose(source);

	return 0;
}

