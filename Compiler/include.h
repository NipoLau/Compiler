#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <vector>

#define ISIZE 4  //������Ϣ����ʼ��С
#define INF (1 << 30) - 1

using namespace std;

const int nkw = 15;  //��������Ŀ
const int llen = 256;  //Դ�ļ�һ����󳤶�
const int alen = 124;  //��ʶ����¼����󳤶�
const int c2max = 256;  //ʵ����������
const int tmax = 512;  //���ű���
const int fbmax = 128;  //��������
const int smax = 1024;  //�ַ�������
const int amax = 128;  //������Ϣ����
const int cmax = 2048;  //P�������
const int csmax = 256;  //case����
const int stacksize = 2048;  //����ջ��С
const int ermax = 256;  //������С

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
};  //����ִ��ʱ����״̬

FILE *fp;  //Դ�ļ�ָ��
enum symbol sy;  //��һ����������
char id[alen + 1];  //id��¼��ʶ������
int inum;	/* ���sy��intcon�� inum��������ֵ
			���sy��charcon��inum����ASCII��ֵ */
double rnum;	//���sy��floatcon, rnum���Ǹ���ֵ
string key[nkw];  //��¼�����ؼ��ֵ�����
enum symbol ksy[nkw];  //��key����һһ��Ӧ����¼�ؼ��ֶ�Ӧ��symbol
map<char, enum symbol> sps;  //�������ַ�����symbol��Ӧ������map
char ch;  //�ʷ���������һ���ַ�
int cc = 0, ll = 0;  //cc�ǵ�ǰ�ַ��ڵ�ǰ�е��±꣬ll�ǵ�ǰ�г���
int dx;  //����ջָ��
char line[llen];  //�����ȡ��һ��
int isglobal;  //��ǰ�����Ƿ�����ȫ�ֱ�����1��ʾ����ȫ�ֱ�����0��ʾ���ھֲ�����
int hasreturn;  //��ʾ�������Ƿ����return���
int errpos = 0;
int errs[ermax], er = 0;
bool skipflag;
vector<symbol> statbegsys = { ifsy, whilesy, lcurly, ident, scanfsy, printfsy, switchsy, returnsy, semicolon };  //���FIRST��
vector<symbol> termbegsys = { ident, intcon, floatcon, charcon, lparent, plussy, minussy };  //��FIRST��
vector<symbol> facbegsys = { ident, intcon, floatcon, charcon, lparent, plussy, minussy };  //����FIRST��
vector<symbol> expbegsys = { plussy, minussy, ident, intcon, floatcon, charcon, lparent };  //���ʽFIRST��

/* ���ű����ݽṹ */
/* ��������ʼ��ָ�����һ��Ԫ�أ��������һ��Ԫ�غ�һ�����������ڷ���������ܳ�ʼ��ַ��1 */
double fconst[c2max];  int c1, c2 = 0; //ʵ��������������
struct stru_tab {
	char name[alen];
	int link;
	enum object obj;
	enum type typ;
	int ref;
	int adr;
}tab[tmax]; int t = 0;  //���ű�������
struct stru_ftab {
	int last;
	int lastpar;
	int psize;
	int vsize;
} ftab[fbmax]; int f = 1;  //��������������ftab[1]Ԥ��������main��������������һ��ͬ��
char stab[smax]; int slen, sx = 0;  //�ַ�����
struct stru_atab {
	enum type eltyp;
	int size;
}atab[amax]; int a = 0;  //������Ϣ��������
struct pcode {
	int f;
	int x;  //x��δʹ��
	int y;
	pcode() {  //��ʼ��ΪNULL���������ָ��
		x = INF;
		y = INF;
	}
}code[cmax]; int lc = 0;  //P�������������lcʼ��ָ����һ�������ɵ�ָ���ַ
/* ���ű����ݽṹ */

/* �������Ͷ��� */
struct item {  //��Ϊ��������������/ref��Ϣ
	enum type typ;
	int ref;
};
struct stru_casetab {  //switch��������
	int val;  //����ֵ
	int lc;  //pcֵ
	bool isdefault;  //�Ƿ���default��
};
/* �������Ͷ��� */

/* ����ջ���Ͷ��� */
struct stru_s {
	int datatype;  //0 - int ; 1 - double ; 2 - char ;
	int i;
	double f;
	char c;
}s[stacksize];  //����ջ
pcode ir;  //����ָ��
int pc, top, base;  //pc�����������tָ��ջ����bָ��ǰ����
enum progstat ps;  //ps����ִ����״̬
/* ����ջ���Ͷ��� */

//��������
int loc(char name[]);
void emit(int fct);
void emit1(int fct, int y);
void emit2(int fct, int x, int y);  //x��δʹ�ã��ú���δʹ��
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