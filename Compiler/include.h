#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <vector>

#define ISIZE 4  //内务信息区初始大小
#define INF (1 << 30) - 1

using namespace std;

const int nkw = 15;  //保留字数目
const int llen = 256;  //源文件一行最大长度
const int alen = 124;  //标识符记录的最大长度
const int c2max = 256;  //实数常量表长度
const int tmax = 512;  //符号表长度
const int fbmax = 128;  //函数表长度
const int smax = 1024;  //字符串表长度
const int amax = 128;  //数组信息表长度
const int cmax = 2048;  //P代码表长度
const int csmax = 256;  //case表长度
const int stacksize = 2048;  //运行栈大小
const int ermax = 256;  //错误表大小

enum symbol {
	intcon, floatcon, charcon, stringcon,
	plussy, minussy, timessy, divsy, lsssy, leqsy, gtrsy, geqsy, neqsy, equsy,
	ident, constsy, voidsy, mainsy, ifsy, elsesy, whilesy, switchsy, casesy, defaultsy, scanfsy, printfsy, returnsy, charsy, floatsy, intsy,
	lparent, rparent, lbrack, rbrack, lcurly, rcurly, comma, semicolon, colon, becomes
};
enum object {
	konstant, vvariable, funktion
};
enum type {
	ints, chars, floats, arrays, voids, nottyp
};
enum progstat {
	run, fin, caschk, divchk, inxchk, stkchk, linchk, lngchk, redchk
};  //解释执行时程序状态

FILE *fp;  //源文件指针
enum symbol sy;  //下一个单词类型
char id[alen + 1];  //id记录标识符内容
int inum;	/* 如果sy是intcon， inum就是整型值
			如果sy是charcon，inum就是ASCII码值 */
double rnum;	//如果sy是floatcon, rnum就是浮点值
string key[nkw];  //记录保留关键字的数组
enum symbol ksy[nkw];  //与key数组一一对应，记录关键字对应的symbol
map<char, enum symbol> sps;  //将特殊字符与其symbol对应起来的map
char ch;  //词法分析中下一个字符
int cc = 0, ll = 0;  //cc是当前字符在当前行的下标，ll是当前行长度
int dx;  //运行栈指针
char line[llen];  //缓存读取的一行
int isglobal;  //当前变量是否属于全局变量，1表示属于全局变量，0表示属于局部变量
int hasreturn;  //表示函数中是否出现return语句
int errpos = 0;
int errs[ermax], er = 0;
bool skipflag;
vector<symbol> statbegsys = { ifsy, whilesy, lcurly, ident, scanfsy, printfsy, switchsy, returnsy, semicolon };  //语句FIRST集
vector<symbol> termbegsys = { ident, intcon, floatcon, charcon, lparent, plussy, minussy };  //项FIRST集
vector<symbol> facbegsys = { ident, intcon, floatcon, charcon, lparent, plussy, minussy };  //因子FIRST集
vector<symbol> expbegsys = { plussy, minussy, ident, intcon, floatcon, charcon, lparent };  //表达式FIRST集

/* 符号表数据结构 */
/* 各表索引始终指向最后一个元素，而非最后一个元素后一个，这样便于反填，这样可能初始地址是1 */
double fconst[c2max];  int c1, c2 = 0; //实数常量表及其索引
struct stru_tab {
	char name[alen];
	int link;
	enum object obj;
	enum type typ;
	int ref;
	int adr;
}tab[tmax]; int t = 0;  //符号表及其索引
struct stru_ftab {
	int last;
	int lastpar;
	int psize;
	int vsize;
} ftab[fbmax]; int f = 1;  //函数表及其索引，ftab[1]预留给程序，main函数与其他函数一视同仁
char stab[smax]; int slen, sx = 0;  //字符串表
struct stru_atab {
	enum type eltyp;
	int size;
}atab[amax]; int a = 0;  //数组信息表及其索引
struct pcode {
	int f;
	int x;  //x域未使用
	int y;
	pcode() {  //初始化为NULL，方便输出指令
		x = INF;
		y = INF;
	}
}code[cmax]; int lc = 0;  //P代码表及其索引，lc始终指向下一条将生成的指令地址
/* 符号表数据结构 */

/* 其余类型定义 */
struct item {  //作为参数，传递类型/ref信息
	enum type typ;
	int ref;
};
struct stru_casetab {  //switch语句情况表
	int val;  //常量值
	int lc;  //pc值
	bool isdefault;  //是否是default型
};
/* 其余类型定义 */

/* 运行栈类型定义 */
struct stru_s {
	int datatype;  //0 - int ; 1 - double ; 2 - char ;
	int i;
	double f;
	char c;
}s[stacksize];  //运行栈
pcode ir;  //缓存指令
int pc, top, base;  //pc程序计数器，t指向栈顶，b指向当前区低
enum progstat ps;  //ps解释执行器状态
/* 运行栈类型定义 */

//函数定义
int loc(char name[]);
void emit(int fct);
void emit1(int fct, int y);
void emit2(int fct, int x, int y);  //x域未使用，该函数未使用
void enter(char name[], enum object k, enum type tp);
void enterfloat(double x);
void enterarray(enum type eltyp, int size);
void enterfun();
void setup();
int binSearch(char A[]);
char tolower(char c);
void adjustscale(int e);
void nextch();
void insymbol();
void endskip();
void error(int n);
void fatal(int n);
void skip(vector<symbol> fsys, int n);
void test(vector<symbol> s1, vector<symbol> s2, int n);
void constdec(vector<symbol> fsys);
void vardec(vector<symbol> fsys);
void paralist(vector<symbol> fsys);
void fundecident(vector<symbol> fsys);
void fundec(vector<symbol> fsys, type tp);
void compoundstat(int prt, vector<symbol> fsys);
void statseries(vector<symbol> fsys);
void statement(vector<symbol> fsys);
item condition(vector<symbol> fsys);
void ifstatement(vector<symbol> fsys);
void whilestatement(vector<symbol> fsys);
void switchstatement(vector<symbol> fsys);
void onecase(int i, stru_casetab *casetab, int *exittab, type tp, vector<symbol> fsys);
void standproc(vector<symbol> fsys);
enum type resulttype(enum type a, enum type b);
void call(vector<symbol> fsys, int i);
item expression(vector<symbol> fsys);
item term(vector<symbol> fsys);
item factor(vector<symbol> fsys);
void programs();
void completeCopy(stru_s &s1, stru_s &s2);
void stkcpy(stru_s &s1, stru_s &s2);
void inter0();
void inter1();
void inter2();
void inter3();
void inter4();
void inter5();
void inter6();
void interpret();
void printtables();