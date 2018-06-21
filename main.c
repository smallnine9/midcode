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
/*定义三个文件指针*/



/*5个flag*/
int Error = FALSE;
int TraceScan = FALSE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int EchoSource = FALSE;


int main(int argc, char *argv[])
{
	TreeNode *syntaxTree = NULL;//抽象语法树根节点
	
 /*  argc: 整数,用来统计你运行程序时送给main函数的命令行参数的个数
     argv[ ]: 指针数组，用来存放指向你的字符串参数的指针，每一个元素指向一个参数
     argv[0] 指向程序运行的全路径名
     argv[1] 指向在DOS命令行中执行程序名后的第一个字符串
     argv[2] 指向执行程序名后的第二个字符串
*/
	char pgm[120];
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgm, argv[1]);//copy argv[1] to pgm

	if (strchr(pgm, '.') == NULL) //查找pgm中是否有后缀
		strcat(pgm, ".tg");//如果没有，将pgm(argv[1]和.tg连接起来)

	source = fopen(pgm, "r");//open the source file，只读文件
	
	/*source源文件指针空处理*/
	if (source == NULL)
	{
		fprintf(stderr, "0000");
		exit(1);
	}
	/*fprintf是输出到文件 stdout标准输出，默认也是输出到屏幕*/
	/*stdout可以重定位到文件，使用freopen*/
	listing = stdout;
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE //scan-only扫描器


	while (getToken() != ENDFILE);
	

#else


	syntaxTree = parse(); /*调用parse构造抽象语法树*/

	traverToCreateMid(syntaxTree);

	if (TraceParse)
	{
		fprintf(listing, "\nSyntax tree:\n");
	}


#if !NO_CODE

	/*无错误的时候，才能进入下面的代码块，增加程序的可靠性*/
	if (!Error)
	{
		char * codefile;
		int fnlen = strcspn(pgm, ".");

		codefile = (char *)calloc(fnlen + 4, sizeof(char));
		//calloc在动态分配完内存后，自动初始化该内存空间为零，
		//而malloc不初始化，里边数据是随机的垃圾数据

		strncpy(codefile, pgm, fnlen);
		//将pgm里面fnlen个字符复制到codefile里
		strcat(codefile, ".asm");
		//和codefile与后缀.asm相接


		code = fopen(codefile, "w");//只写文件

		if (code == NULL)
		{
			printf("Unable to open %s\n", codefile);
			exit(1);
		}

		/*codeGen生成代码*/
		codeGen(codefile);
		fclose(code);
	}

#endif//NO_CODE
#endif // !NO_ANALYZE

	fclose(source);

	return 0;
}

