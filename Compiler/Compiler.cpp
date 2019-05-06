#include "include.h"

using namespace std;

void setup() {  //建立关键字保留字表
				/* intcon,floatcon,charcon,stringcon,leqsy,geqsy,neqsy,equsy,ident 在词法分析时另行声明 */
	key[0] = "case";
	key[1] = "char";
	key[2] = "const";
	key[3] = "default";
	key[4] = "else";
	key[5] = "float";
	key[6] = "if";
	key[7] = "int";
	key[8] = "main";
	key[9] = "printf";
	key[10] = "return";
	key[11] = "scanf";
	key[12] = "switch";
	key[13] = "void";
	key[14] = "while";

	ksy[0] = casesy;
	ksy[1] = charsy;
	ksy[2] = constsy;
	ksy[3] = defaultsy;
	ksy[4] = elsesy;
	ksy[5] = floatsy;
	ksy[6] = ifsy;
	ksy[7] = intsy;
	ksy[8] = mainsy;
	ksy[9] = printfsy;
	ksy[10] = returnsy;
	ksy[11] = scanfsy;
	ksy[12] = switchsy;
	ksy[13] = voidsy;
	ksy[14] = whilesy;

	sps['('] = lparent;
	sps[')'] = rparent;
	sps['['] = lbrack;
	sps[']'] = rbrack;
	sps['{'] = lcurly;
	sps['}'] = rcurly;
	sps[','] = comma;
	sps[':'] = colon;
	sps[';'] = semicolon;
	sps['='] = becomes;
	sps['+'] = plussy;
	sps['-'] = minussy;
	sps['*'] = timessy;
	sps['/'] = divsy;
	sps['<'] = lsssy;
	sps['>'] = gtrsy;
}

int binSearch(char A[]) {
	int mid;
	int left = 0, right = nkw;
	while (left <= right) {
		mid = (left + right) / 2;
		if (strcmp(A, key[mid].c_str()) == 0) return mid;
		else if (strcmp(A, key[mid].c_str()) < 0) right = mid - 1;
		else left = mid + 1;
	}
	return -1;
}

char tolower(char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
	else return c;
}

void adjustscale(int e) {
	int s = e;
	double d = 10.0, t = 1.0;
	do {
		while (s % 2 == 0) {
			s = s / 2;
			d = d * d;
		}
		s = s - 1;
		t = d * t;
	} while (s != 0);
	rnum = rnum / t;
}

void nextch() {
	if (cc == ll) {
		if (feof(fp)) {
			printf("Source File Incomplete!\n");
			printf("Compiling Stopped!\n");
			system("pause");
			exit(0);
		}
		if (errpos != 0) {  //读到新行，将errpos置为0，如果skipflag，则接着endskip，输出换行
			if (skipflag) endskip();
			printf("\n");
			errpos = 0;
		}
		printf("%d     ", lc);  //每行开始前打印出当前pcode地址
		cc = 0;
		ll = 0;
		do {
			ch = fgetc(fp);
			printf("%c", ch);
			ll = ll + 1;
			line[ll] = ch;
		} while (ch != '\n' && ch != EOF);  //line的下标从1开始
		line[ll] = ' ';  //将读到的换行替换为空格
		printf("\n");  //读完每行，打印换行
	}
	cc = cc + 1;
	ch = line[cc];
}

void insymbol() {
	while (ch == ' ' || ch == '	') {
		nextch();
	}
	label:
	if (ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
		int k = 0;
		do {
			if (k < alen) {
				id[k++] = tolower(ch);  //标识符忽略大小写
			}
			nextch();
		} while ((ch == '_') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));
		id[k] = '\0';
		k = binSearch(id);
		if (k == -1) sy = ident;
		else sy = ksy[k];
		/*
		if (k == -1) printf("ident %s\n", id);
		else cout << key[k] << "sy " << id << endl;
		*/
	}
	else if (ch >= '0' && ch <= '9') {
		int k = 0;
		inum = 0;
		int zero = 0; bool ispre = true;
		sy = intcon;
		do {
			if (ispre && ch == '0') zero++;
			else ispre = false;
			inum = inum * 10 + ch - '0';
			k = k + 1;
			nextch();
		} while (ch >= '0' && ch <= '9');
		if (zero > 1 || (inum > 0 && zero >= 1)) error(48);  //不允许出现前导0
		if (ch == '.') {
			nextch();
			sy = floatcon;
			rnum = inum;
			int e = 0;
			while (ch >= '0' && ch <= '9') {
				e = e + 1;
				rnum = rnum * 10 + ch - '0';
				nextch();
			}
			if (e == 0) error(21);
			else adjustscale(e);
		}
		/*
		if (sy == floatcon) printf("floatcon %f\n", rnum);
		else printf("intcon %d\n", inum);
		*/
	}
	else if (ch == '=') {
		nextch();
		if (ch == '=') {
			sy = equsy;
			nextch();
		}
		else sy = becomes;
		/*
		if (sy == equsy) printf("equsy ==\n");
		else printf("becomes =\n");
		*/
	}
	else if (ch == '<') {
		nextch();
		if (ch == '=') {
			sy = leqsy;
			nextch();
		}
		else sy = lsssy;
		/*
		if (sy == leqsy) printf("leqsy <=\n");
		else printf("lsssy <\n");
		*/
	}
	else if (ch == '>') {
		nextch();
		if (ch == '=') {
			sy = geqsy;
			nextch();
		}
		else sy = gtrsy;
		/*
		if (sy == geqsy) printf("geqsy >=\n");
		else printf("gtrsy >\n");
		*/
	}
	else if (ch == '!') {
		nextch();
		if (ch == '=') {
			sy = neqsy;
			nextch();
		}
		/*
		if (sy == neqsy) printf("neqsy !=\n");
		*/
	}
	else if (ch == '\'') {
		nextch();
		if (ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
			(ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) || (ch >= '0' && ch <= '9')) {
			sy = charcon;
			inum = ch;
			nextch();
		}
		if (ch != '\'') error(31);  //应该是单引号
		else nextch();
		/*
		printf("charcon %d\n", inum);
		*/
	}
	else if (ch == '"') {
		sy = stringcon;
		nextch();
		int asc = ch, k = 0;
		while (asc == 32 || asc == 33 || (asc >= 35 && asc <= 126)) {
			if (sx + k == smax) fatal(3);
			stab[sx + k] = ch;
			k = k + 1;
			nextch();
			asc = ch;
		}
		inum = sx;
		slen = k;
		sx = sx + k;
		if (ch != '"') error(38);  //应该是双引号
		else nextch();
		/*
		printf("stringcon %s\n", str);
		*/
	}
	else if (ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '[' ||
		ch == ']' || ch == '{' || ch == '}' || ch == ',' || ch == ';' || ch == ':' || ch == '+' || ch == '-') {
		sy = sps[ch];
		/*
		printf("%c\n", ch);
		*/
		nextch();
	}
	else {
		error(11);
		nextch();
		goto label;  //使用goto语句跳过非法字符
	}
}

void errmsg() {
	string msg[55];
	msg[0] = "标识符未定义";
	msg[1] = "标识符重复定义";
	msg[2] = "应是标识符";
	msg[3] = "应是’)’";
	msg[4] = "非法符号，编译时将跳读";
	msg[5] = "应是’(’";
	msg[6] = "应是’[’";
	msg[7] = "应是’]’";
	msg[8] = "应是’;’";
	msg[9] = "数太大";
	msg[10] = "switch和case语句后面的标识必须是整形/字符型";
	msg[11] = "非法字符";
	msg[12] = "常量定义中，等号后面必须是常数";
	msg[13] = "数组说明中，数组大小必须大于0";
	msg[14] = "数组不存在";
	msg[15] = "应该是类型标识符";
	msg[16] = "算数表达式类型不合法，数组本身不能参与运算";
	msg[17] = "相比较的表达式类型必须相同";
	msg[18] = "参数不匹配";
	msg[19] = "字符串字符不合法";
	msg[20] = "参数个数不匹配";
	msg[21] = "小数点后没有数字";
	msg[22] = "scanf和printf参数类型不正确";
	msg[23] = "此处应该是变量";
	msg[24] = "赋值语句前后类型不匹配";
	msg[25] = "switch语句中标号与case中标号类型不匹配";
	msg[26] = "应该是’{’";
	msg[27] = "应该是’}’";
	msg[28] = "数组下标不合法，必须从0开始";
	msg[29] = "应该是’:’";
	msg[30] = "数字不允许出现多个前导0";
	msg[31] = "应该是’’’";
	msg[32] = "应该是’=’";
	msg[33] = "应该是’,’";
	msg[34] = "常量定义是左右类型不匹配";
	msg[35] = "被调用的函数类型不能是void";
	msg[36] = "主函数声明不正确";
	msg[37] = "因子必须以标识符，常量，’(’开始";
	msg[38] = "应该是’”’";
	msg[40] = "应该是常量";
	msg[41] = "数组声明中，表示数组大小的应该是整型常量";
	msg[42] = "字符型常量不能有前导加号或减号";
	msg[43] = "此处应该是整型";
	msg[44] = "赋值语句中，标识符后面必须是=/(/[";
	msg[45] = "常量声明语句前后类型不匹配";
	msg[46] = "主程序中标识符后面只可能出现,/;/(";
	msg[47] = "缺少main函数";
	msg[48] = "整数不允许出现前导0";
	msg[49] = "返回值类型必须与函数类型相同";
	msg[50] = "不是语句开始符号";
	msg[51] = "此处应该是表达式";
	msg[52] = "赋值语句左侧类型错误";
	msg[53] = "条件语句左右两边必须是整型或实型";
	msg[54] = "缺少return语句";
	printf("\n");
	for (int i = 0; i < er; i++) {
		cout << errs[i] << ":" << msg[errs[i]] << endl;
	}
}

void error(int n) {
	if (errpos == 0) printf("****");
	if (cc > errpos) {
		for (int i = errpos; i < cc; i++) printf(" ");
		printf("^%d", n);
		errpos = cc + 3;
		errs[er++] = n;
	}
}

void fatal(int n) {
	string msg[7];
	msg[0] = "实常数表溢出";
	msg[1] = "符号表溢出";
	msg[2] = "函数信息表溢出";
	msg[3] = "字符串表溢出";
	msg[4] = "数组信息表溢出";
	msg[5] = "P代码表溢出";
	msg[6] = "case表溢出";
	
	cout << msg[n] << endl;
	system("pause");
	exit(1);
}

void endskip() {
	while (errpos++ < cc) printf("-");
	skipflag = false;
}

void skip(vector<symbol> fsys, int n) {
	error(n);
	skipflag = true;
	while (true) {
		bool isin = false;
		for (vector<symbol>::iterator it = fsys.begin(); it != fsys.end(); it++) {
			if (sy == *it) {
				isin = true;
				break;
			}
		}
		if (isin) break;
		else insymbol();
	}
	if (skipflag) endskip();
}

void test(vector<symbol> s1, vector<symbol> s2, int n) {
	bool isin = false;
	for (vector<symbol>::iterator it = s1.begin(); it != s1.end(); it++) {
		if (sy == *it) {
			isin = true;
			break;
		}
	}
	if (!isin) {
		s1.insert(s1.end(), s2.begin(), s2.end());
		skip(s1, n);
	}
}

void testsemicolon(vector<symbol> fsys) {
	if (sy == semicolon) {
		insymbol();
	}
	else {
		error(8);
		test(fsys, fsys, 4);
	}
}

int loc(char name[]) {  //检索标识符在符号表中的位置，查找范围是当前函数+全局数据，若没找到，则返回-1
	//在当前函数数据区内查找
	int ind = ftab[f].last;
	if (tab[ind].obj != funktion) {  //如果当前函数没有参数/局部变量/常量，则last域指向自己在tab表中位置，类型是函数，且函数内不会再有函数
		do {
			if (strcmp(tab[ind].name, name) == 0) {
				isglobal = 0;
				return ind;
			}
			ind = tab[ind].link;
		} while (ind != 0);
	}
	//函数没有自己数据/没找到，则到全局数据处找，全局数据最后一个数据在符号表中位置是ftab[1].last
	ind = ftab[1].last;
	do {
		if (strcmp(tab[ind].name, name) == 0) {
			isglobal = 1;
			return ind;
		}
		ind = tab[ind].link;
	} while (ind != 0);
	//什么都没找到，return -1
	return -1;
}

int locfun(char name[]) {  //查找函数表ftab，获取函数在符号表中位置
	for (int i = 2; i <= f; i++) {  //ftab[1]预留给了主程序，用户定义函数下标从2开始，检索到当前定义的函数，之后定义的函数不可见
		//获取函数在符号表中位置公式为index = lastpar - psize + ISIZE
		if (strcmp(name, tab[ftab[i].lastpar - ftab[i].psize + ISIZE].name) == 0) {
			return (ftab[i].lastpar - ftab[i].psize + ISIZE);
		}
	}
	return -1;
}

int locglobal(char name[]) {
	for (int i = 1; i <= ftab[0].last; i++) {
		if (strcmp(name, tab[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

void emit(int fct) {
	if (lc == cmax) fatal(5);
	code[lc].f = fct;
	lc = lc + 1;
}

void emit1(int fct, int y) {
	if (lc == cmax) fatal(5);
	code[lc].f = fct;
	code[lc].y = y;
	lc = lc + 1;
}

void emit2(int fct, int x, int y) {
	if (lc == cmax) fatal(5);
	code[lc].f = fct;
	code[lc].x = x;
	code[lc].y = y;
	lc = lc + 1;
}

void enter(char name[], enum object k, enum type tp) {
	if (t == tmax) fatal(1);
	int l = ftab[f].last;  //last即函数最后一个标识符在符号表中的索引，初始值为0，表示该函数的第一个标识符
	t = t + 1;
	strcpy_s(tab[t].name, name);
	tab[t].link = l;
	tab[t].obj = k;
	tab[t].typ = tp;
	tab[t].ref = 0;
	tab[t].adr = 0;
	ftab[f].last = t;
}

void enterfloat(double x) {
	if (c2 == c2max - 1) fatal(0);
	fconst[c2 + 1] = x;  //fconst数组有效数据从索引1开始
	c1 = 1;
	while (fconst[c1] != x) {  //检索表中有没有相同元素
		c1 = c1 + 1;
	}
	if (c1 > c2) c2 = c1;  //如果之前没有相同元素，则c1 = c2 + 1，此时将c2自增
}

void enterarray(enum type eltyp, int size) {
	if (a == amax) fatal(4);
	a = a + 1;
	atab[a].eltyp = eltyp;
	atab[a].size = size;
}

void enterfun() {
	if (f == fbmax) fatal(2);
	f = f + 1;
	ftab[f].last = 0;
	ftab[f].lastpar = 0;
}

void constdec(vector<symbol> fsys) {  //处理常量声明
	while (sy == constsy) {
		insymbol();
		if (sy == intsy || sy == floatsy || sy == charsy) {
			enum symbol tp = sy;
			insymbol();
			vector<symbol> legal_1 = { ident };
			test(legal_1, fsys, 2);
			while (sy == ident) {
				if (loc(id) != -1 && isglobal != 1) error(1);  //常量重定义
				if (locfun(id) != -1) error(1);
				switch (tp) {
				case intsy: {
					enter(id, konstant, ints);
					break;
				}
				case floatsy: {
					enter(id, konstant, floats);
					break;
				}
				case charsy: {
					enter(id, konstant, chars);
					break;
				}
				}
				insymbol();
				if (sy == becomes) {
					insymbol();
				}
				else error(32);  //error
				bool isneg = false;
				if (sy == plussy || sy == minussy) {
					isneg = (sy == minussy);
					insymbol();
				}
				vector<symbol> legal_2 = { intcon, floatcon, charcon };
				vector<symbol> stop = { semicolon, comma, ident };
				stop.insert(stop.end(), fsys.begin(), fsys.end());
				test(legal_2, stop, 12);
				if ((tp == intsy && sy == intcon) || (tp == charsy && sy == charcon)) {
					tab[t].adr = (isneg) ? -inum : inum;
				}
				else if (tp == floatsy && sy == floatcon) {
					if (isneg) enterfloat(-rnum);
					else enterfloat(rnum);
					tab[t].adr = c1;
				}
				else error(45);  //error
				insymbol();
				if (sy == comma) {
					insymbol();
					vector<symbol> legal_3 = { ident };
					test(legal_3, fsys, 2);
				}
				else break;
			}
			testsemicolon(fsys);
		}
		else error(15); //error
	}

	//printf("This is Const Declaration\n");
}

void vardec(vector<symbol> fsys) {  //处理变量说明
	while (sy == floatsy || sy == intsy || sy == charsy) {
		enum type tp;
		switch (sy) {
		case floatsy: {
			tp = floats;
			break;
		}
		case intsy: {
			tp = ints;
			break;
		}
		case charsy: {
			tp = chars;
			break;
		}
		}
		insymbol();
		vector<symbol> legal = { ident };
		test(legal, fsys, 2);
		while (sy == ident) {
			if (loc(id) != -1 && isglobal != 1) error(1);
			if (locfun(id) != -1) error(1);
			int sz = 1;
			enter(id, vvariable, tp);  //先填表后判断
			insymbol();
			if (sy == lbrack) {
				insymbol();
				if (sy == intcon) {
					if (inum > 0) {
						enterarray(tp, inum);
						sz = inum;
						tab[t].typ = arrays;
						tab[t].ref = a;
						insymbol();
						if (sy == rbrack) {
							insymbol();
						}
						else error(7);  //error
					}
					else error(13);
				}
				else error(41);  //error
			}
			tab[t].adr = dx;
			dx = dx + sz;
			if (sy == comma) {
				insymbol();
				test(legal, fsys, 2);
			}
			else break;
		}
		testsemicolon(fsys);
	}
	test(fsys, fsys, 4);
	//printf("This is  Variables Declaration");
}

void paralist(vector<symbol> fsys) {
	while (sy == floatsy || sy == intsy || sy == charsy) {
		int sz = 1;
		enum type tp;
		switch (sy) {
		case floatsy: {
			tp = floats;
			break;
		}
		case intsy: {
			tp = ints;
			break;
		}
		case charsy: {
			tp = chars;
			break;
		}
		}
		insymbol();
		if (sy == ident) {
			enter(id, vvariable, tp);
			tab[t].adr = dx;
			dx = dx + sz;
			insymbol();
			if (sy == comma) {
				insymbol();
				vector<symbol> legal = { intsy, floatsy, charsy };
				test(legal, fsys, 15);
			}
			else break;
		}
		else error(2);  //error
	}
	if (sy == rparent) {
		insymbol();
	}
	else error(3);
	vector<symbol> legal = { lcurly };
	test(legal , fsys, 26);
	//printf("This is Parameter List\n");
}

void fundecident(vector<symbol> fsys, type tp) {  //函数定义时，读到标识符进入此处
	if (sy == lparent) {
		int prt = t;  //记录函数在符号表中位置
		insymbol();
		vector<symbol> legal_1(fsys.begin(), fsys.end());
		legal_1.push_back(rparent);
		paralist(legal_1);
		ftab[f].lastpar = t;
		ftab[f].psize = dx;
		if (sy == lcurly) {
			insymbol();
			vector<symbol> legal_2 = { rcurly };
			compoundstat(prt, legal_2);
			if ((tp == ints || tp == floats || tp == chars) && hasreturn != 1) error(54);
			hasreturn = 0;
			if (sy == rcurly) {
				insymbol();
			}
			else error(27);  //error
			emit(33);  //指令 - 退出当前函数
		}
		else error(26); //error
	}
	else error(5);  //error
	test(fsys, fsys, 4);
	//printf("This is Functions Declaration\n");
}

void fundec(vector<symbol> fsys) {
	if (sy == floatsy || sy == intsy || sy == charsy || sy == voidsy) {
		enum type tp;
		switch (sy) {
		case floatsy: {
			tp = floats;
			break;
		}
		case intsy: {
			tp = ints;
			break;
		}
		case charsy: {
			tp = chars;
			break;
		}
		case voidsy: {
			tp = voids;
			break;
		}
		}
		insymbol();
		if (sy == ident) {
			if (locfun(id) != -1) error(1);
			if (locglobal(id) != -1) error(1);
			enter(id, funktion, tp);
			ftab[f].last = ftab[f].last - 1;  //修正ftab的last域值(last域不应该包括下一个函数标识符)
			enterfun();
			tab[t].ref = f;
			dx = ISIZE;  //初始化内务信息区大小为4
			insymbol();
			fundecident(fsys, tp);
		}
		else error(2);  //error
	}
	test(fsys, fsys, 4);
}

void compoundstat(int prt, vector<symbol> fsys) {
	if (sy == constsy) {
		vector<symbol> legal(statbegsys.begin(), statbegsys.end());
		legal.push_back(constsy); legal.push_back(intsy);
		legal.push_back(floatsy); legal.push_back(charsy);
		legal.insert(legal.end(), fsys.begin(), fsys.end());
		constdec(legal);
	}
	if (sy == intsy || sy == floatsy || sy == charsy) {
		vector<symbol> legal(statbegsys.begin(), statbegsys.end());
		legal.push_back(intsy);
		legal.push_back(floatsy); legal.push_back(charsy);
		legal.insert(legal.end(), fsys.begin(), fsys.end());
		vardec(legal);
	}
	ftab[f].vsize = dx;
	tab[prt].adr = lc;
	if (strcmp(tab[prt].name, "main") == 0) {  //调用main函数指令，生成main函数数据区
		tab[0].adr = lc;
		emit1(18, prt);
		emit1(19, ftab[tab[prt].ref].psize - 1);
		emit(31);  //该指令相当于调用处的下一条指令，对于程序来说，调用main后，下一条指令是结束程序
		tab[prt].adr = lc;
	}
	statseries(fsys);
	//printf("This is Compound Statements\n");
}

void statseries(vector<symbol> fsys) {  //语句列
	while (true) {
		bool isin = false;
		for (vector<symbol>::iterator it = statbegsys.begin(); it != statbegsys.end(); it++) {
			if (sy == *it) {
				isin = true;
				break;
			}
		}
		if (isin) {
			vector<symbol> legal(fsys.begin(), fsys.end());
			legal.insert(legal.end(), statbegsys.begin(), statbegsys.end());  //复合语句的follow集合为statbegsys+rcurly
			statement(legal);
		}
		else break;
	}

	//printf("This is Statements Series\n");
}

void statement(vector<symbol> fsys) {
	switch (sy) {
	case ifsy: {
		ifstatement(fsys);
		break;
	}
	case whilesy: {
		whilestatement(fsys);
		break;
	}
	case switchsy: {
		switchstatement(fsys);
		break;
	}
	case returnsy: {
		standproc(fsys);
		hasreturn = 1;
		break;
	}
	case scanfsy: {
		standproc(fsys);
		break;
	}
	case printfsy: {
		standproc(fsys);
		break;
	}
	case lcurly: {
		insymbol();
		statseries(fsys);
		if (sy == rcurly) {
			insymbol();
		}
		else error(27);  //error
		break;
	}
	case ident: {
		char idname[alen + 1];  strcpy_s(idname, id);
		insymbol();
		if (sy == lparent) {
			int i = locfun(idname);
			if (i != -1) {
				call(fsys, i);  //调用函数
			}
			else error(0);  //error
			testsemicolon(fsys);

			//printf("This is Function Call\n");
		}
		else if (sy == lbrack) {
			int i = loc(idname);
			if (i == -1) error(0);
			emit2(0, isglobal, tab[i].adr);
			insymbol();
			vector<symbol> legal(fsys.begin(), fsys.end());
			legal.push_back(rbrack);
			legal.push_back(becomes);
			expression(legal);
			emit1(20, tab[i].ref);
			if (sy == rbrack) {
				insymbol();
				if (sy == becomes) {
					insymbol();
					item y = expression(fsys);
					if (atab[tab[i].ref].eltyp == y.typ) {  //等号左右两边类型相等才能赋值，除非左边是浮点型，右边是整型
						emit(38);
					}
					else error(24);  //error
					testsemicolon(fsys);
				}
				else error(32);  //error
			}
			else error(7);  //error

			//printf("This is Assignment\n");
		}
		else if (sy == becomes) {
			int i = loc(idname);
			if (i == -1) error(0);
			emit2(0, isglobal, tab[i].adr);
			insymbol();
			item y = expression(fsys);
			if (tab[i].obj == vvariable) {
				if (tab[i].typ == y.typ) {  //等号左右两边类型相等才能赋值，除非左边是浮点型，右边是整型
					emit(38);
				}
				else error(24);
			}
			else error(52);
			testsemicolon(fsys);

			//printf("This is Assignment\n");
		}
		else error(44);  //error
		break;
	}
	case semicolon: {
		insymbol();
		//printf("This is Null Statement\n");
		break;
	}
	default: {
		error(50);
	}
	}
	//printf("This is Statement\n");
}

item condition(vector<symbol> fsys) {
	item x, y;
	if (sy == lparent) {
		insymbol();
		vector<symbol> legal_1(fsys.begin(), fsys.end());
		vector<symbol> sub = { lsssy, leqsy, gtrsy, geqsy, neqsy, equsy, rparent };
		legal_1.insert(legal_1.end(), sub.begin(), sub.end());
		x = expression(legal_1);
		if (sy == lsssy || sy == leqsy || sy == gtrsy || sy == geqsy || sy == neqsy || sy == equsy) {
			enum symbol op = sy;
			insymbol();
			vector<symbol> legal_2(fsys.begin(), fsys.end());
			legal_2.push_back(rparent);
			y = expression(legal_2);
			if (x.typ == y.typ) {
				if (x.typ == ints) {
					switch (op) {
					case equsy: {
						emit(45);  //指令 - 整型/字符型相等比较
						break;
					}
					case neqsy: {
						emit(46);  //指令 - 整型/字符型不相等比较
						break;
					}
					case lsssy: {
						emit(47);  //指令 - 整型/字符型小于比较
						break;
					}
					case leqsy: {
						emit(48);  //指令 - 整型/字符型小于等于比较
						break;
					}
					case gtrsy: {
						emit(49);  //指令 - 整型/字符型大于比较 
						break;
					}
					case geqsy: {
						emit(50);  //指令 - 整型/字符型大于等于比较
						break;
					}
					}
				}
				else if (x.typ == floats) {
					switch (op) {
					case equsy: {
						emit(39);  //指令 - 实型相等比较
						break;
					}
					case neqsy: {
						emit(40);  //指令 - 实型不相等比较
						break;
					}
					case lsssy: {
						emit(41);  //指令 - 实型小于比较
						break;
					}
					case leqsy: {
						emit(42);  //指令 - 实型小于等于比较
						break;
					}
					case gtrsy: {
						emit(43);  //指令 - 实型大于比较 
						break;
					}
					case geqsy: {
						emit(44);  //指令 - 实型大于等于比较
						break;
					}
					}
				}
				else error(53);
			}
			else {
				error(17);  //相比较的表达式类型必须相同
			}  //error
		}
		else {
			if (x.typ != ints) error(43);
		}
		if (sy == rparent) insymbol();
		else error(3);
	}
	else error(5);
	return x;
}

void ifstatement(vector<symbol> fsys) {
	int lc1, lc2;
	if (sy == ifsy) {
		insymbol();
		condition(fsys);
		lc1 = lc;
		emit(11);  //指令 - 栈顶内容为假，跳转到y，此处y反填
		vector<symbol> legal(fsys.begin(), fsys.end());
		legal.push_back(elsesy);
		statement(legal);
		if (sy == elsesy) {
			insymbol();
			lc2 = lc;
			emit(10);  //指令 - 无条件跳转到y，此处y反填
			code[lc1].y = lc;   //反填，条件不成立，执行else语句
			statement(fsys);
			code[lc2].y = lc;  //反填，条件成立，跳过else语句
		}
		else {
			code[lc1].y = lc;  //没有else语句，跳转到此处
		}
	}

	//printf("This is IF Statement\n");
}

void whilestatement(vector<symbol> fsys) {
	int lc1, lc2;
	if (sy == whilesy) {
		insymbol();
		lc1 = lc;
		condition(fsys);
		lc2 = lc;
		emit(11);  //指令 - 条件为假，跳转出语句
		statement(fsys);
		emit1(10, lc1);  //指令 - 无条件跳转到<条件>判断
		code[lc2].y = lc;
	}

	//printf("This is While Statement\n");
}

void switchstatement(vector<symbol> fsys) {
	stru_casetab casetab[csmax];
	int exittab[csmax];  //每个case结束的pc值
	int i = 0, lc1, lc2;
	if (sy == switchsy) {
		insymbol();
		if (sy == lparent) {
			insymbol();
			vector<symbol> legal (fsys.begin(), fsys.end());
			legal.push_back(casesy);
			legal.push_back(defaultsy);
			//here
			legal.push_back(rparent);
			item x = expression(legal);
			if (x.typ != chars && x.typ != ints) error(10);  //必须是整型或字符型
			lc1 = lc;
			emit(12);  //指令 - 跳转到y，查询情况表，y反
			if (sy == rparent) {
				insymbol();
				if (sy == lcurly) {
					insymbol();
					while (sy == casesy) {
						onecase(i, casetab, exittab, x.typ, legal);
						i = i + 1;
					}
					if (sy == defaultsy) {
						if (i == csmax) fatal(6);
						insymbol();
						casetab[i].lc = lc;
						casetab[i].isdefault = true;
						if (sy == colon) {
							insymbol();
							statement(fsys);
							exittab[i] = lc;
							emit(10);
						}
						else error(29);  //error
						i = i + 1;
					}
					code[lc1].y = lc;
					for (int k = 0; k < i; k++) {
						if (!casetab[k].isdefault) {
							emit1(13, casetab[k].val);
							emit1(13, casetab[k].lc);
						}
						else {
							emit2(13, 1, casetab[k].val);  //如果是default语句，则x域填上1，解释执行的时候可以判断
							emit1(13, casetab[k].lc);
						}
					}
					lc2 = lc;
					emit(10);  //没有满足条件的情况，跳出switch结束
					code[lc2].y = lc;
					for (int k = 0; k < i; k++) {  //反填每个case语句结束时的无条件跳转指令
						code[exittab[k]].y = lc;
					}
					if (sy == rcurly) {
						insymbol();
					}
					else error(27);  //error
				}
				else error(26);  //error
			}
			else error(3);  //error
		}
		else error(5);  //error
	}

	//printf("This is Switch Statement\n");
}

void onecase(int i, stru_casetab *casetab, int *exittab, type tp, vector<symbol> fsys) {
	if (i == csmax) fatal(6);
	if (sy == casesy) {
		insymbol();
		if (sy == charcon) {
			if (tp != chars) error(25);
			casetab[i].val = inum;
			casetab[i].lc = lc;
			casetab[i].isdefault = false;
			int k = 0;
			while (casetab[k].val != casetab[i].val) k++;
			if (k < i) error(1);  //重定义
			insymbol();
			if (sy == colon) {
				insymbol();
				statement(fsys);
				exittab[i] = lc;
				emit(10);
			}
			else error(29);  //error
		}
		else if (sy == plussy || sy == minussy) {
			enum symbol op = sy;
			insymbol();
			if (sy == intcon) {
				if (tp != ints) error(25);
				casetab[i].val = (op == minussy) ? -inum : inum;
				casetab[i].lc = lc;
				casetab[i].isdefault = false;
				int k = 0;
				while (casetab[k].val != casetab[i].val) k++;
				if (k < i) error(1);  //重定义
				insymbol();
				if (sy == colon) {
					insymbol();
					statement(fsys);
					exittab[i] = lc;
					emit(10);
				}
				else error(29);  //error
			}
			else error(43);  //error
		}
		else if (sy == intcon) {
			if (tp != ints) error(25);
			casetab[i].val = inum;
			casetab[i].lc = lc;
			casetab[i].isdefault = false;
			int k = 0;
			while (casetab[k].val != casetab[i].val) k++;
			if (k < i) error(1);  //重定义
			insymbol();
			if (sy == colon) {
				insymbol();
				statement(fsys);
				exittab[i] = lc;
				emit(10);
			}
			else error(29);  //error
		}
		else error(10);
	}

	//printf("This is Case Tab\n");
}

void standproc(vector<symbol> fsys) {
	if (sy == scanfsy) {
		insymbol();
		if (sy == lparent) {
			insymbol();
			while (sy == ident) {
				int i = loc(id);
				if (1 != -1) {
					if (tab[i].obj == vvariable) {
						emit2(0, isglobal, tab[i].adr);
						switch (tab[i].typ) {
						case ints: {
							emit1(27, 1);
							break;
						}
						case floats: {
							emit1(27, 2);
							break;
						}
						case chars: {
							emit1(27, 3);
							break;
						}
						default: {
							error(22);  //参数类型不正确
							break;
						}
						}
					}
					else error(23);  //应该是变量
				}
				else error(0);  //error
				insymbol();
				if (sy == comma) {
					insymbol();
					vector<symbol> legal;
					legal.push_back(ident);
					test(legal, fsys, 2);
				}
			}
			if (sy == rparent) {
				insymbol();
				testsemicolon(fsys);
			}
			else error(3);  //error
		}
		else error(5);  //error

		//printf("This is Scan Statement\n");
	}
	else if (sy == printfsy) {
		insymbol();
		if (sy == lparent) {
			insymbol();
			vector<symbol> legal(fsys.begin(), fsys.end());
			legal.push_back(rparent);
			if (sy == stringcon) {
				emit2(24, 0, slen);  //指令 - 装入字符串长度
				emit1(28, inum);  //指令 - 输出字符串，inum为字符串在字符串表中起始位置
				insymbol();
				if (sy == comma) {
					insymbol();
					item x = expression(legal);
					switch (x.typ) {
					case ints: {
						emit1(29, 1);
						break;
					}
					case floats: {
						emit1(29, 2);
						break;
					}
					case chars: {
						emit1(29, 3);
						break;
					}
					default: {
						error(22);  //error
						break;
					}
					}
				}
				if (sy == rparent) {
					insymbol();
					testsemicolon(fsys);
				}
				else error(3);  //error
			}
			else {
				item x = expression(legal);
				switch (x.typ) {
				case ints: {
					emit1(29, 1);
					break;
				}
				case floats: {
					emit1(29, 2);
					break;
				}
				case chars: {
					emit1(29, 3);
					break;
				}
				default: {
					error(22);
					break;
				}  //error
				}
				if (sy == rparent) {
					insymbol();
					testsemicolon(fsys);
				}
				else error(3);  //error
			}
		}
		else error(5);  //error

		//printf("This is Print Statement\n");
	}
	else if (sy == returnsy) {
		insymbol();
		//获取函数在符号表中位置
		int i = ftab[f].lastpar;
		while (tab[i].obj != funktion) i--;
		if (sy == lparent) {
			insymbol();
			vector<symbol> legal(fsys.begin(), fsys.end());
			legal.push_back(rparent);
			item x = expression(legal);
			if (tab[i].typ == x.typ) {
				switch (x.typ) {
				case ints: {
					emit1(32, 1);  //指令 - 装入函数返回值
					emit(33);  //指令 - 退出当前函数
					break;
				}
				case floats: {
					emit1(32, 2);
					emit(33);
					break;
				}
				case chars: {
					emit1(32, 3);
					emit(33);
					break;
				}
				default: {
					error(49);
					break;
				}  //error
				}
			}
			else error(49);
			if (sy == rparent) {
				insymbol();
				testsemicolon(fsys);
			}
			else error(3);  //error
		}
		else if (sy == semicolon) {
			if (tab[i].typ != voids) error(49);
			emit(33);  //指令 - 退出当前函数
			insymbol();
		}
		else error(8);  //error

		//printf("This is Return Statement\n");
	}
}

type resulttype(enum type a, enum type b) {  //整型与浮点型运算乘除运算，转换为浮点型，字符型参与运算，结果为整型
	if (a == ints) {
		if (b == ints) {
			return ints;
		}
		else if (b == floats) {
			emit1(26, 1);  //指令 - 转换为浮点数
			return floats;
		}
		else if (b == chars) {
			emit1(23, 0);
			return ints;
		}
		else return nottyp;  //error
	}
	else if (a == floats) {
		if (b == ints || b == chars) {
			emit1(26, 0);  //指令，转换为浮点数
			return floats;
		}
		else if (b == floats) {
			return floats;
		}
		else return nottyp;  //error
	}
	else if (a == chars) {
		if (b == chars) {
			emit1(23, 1);
			emit1(23, 0);
			return ints;
		}
		else if (b == ints) {
			emit1(23, 1);
			return ints;
		}
		else if (b == floats) {
			emit1(26, 1);
			return floats;
		}
		else return nottyp;  //error
	}
	else return nottyp;
}

void call(vector<symbol> fsys, int i) {
	item x;
	emit1(18, i);  //指令 - 标记栈
	int lastp = ftab[tab[i].ref].lastpar;
	int cp = i;
	if (cp != lastp) {  //函数有参数
		do {
			if (cp >= lastp) {
				error(20);  //参数个数不匹配
				//跳读
				vector<symbol> legal = { rparent };
				test(legal, fsys, 4);  //测试
			}
			else {
				insymbol();
				bool isin = false;
				for (int i = 0; i < expbegsys.size(); i++) {
					if (sy == expbegsys[i]) {
						isin = true;
						break;
					}
				}
				if (isin) {
					cp = cp + 1;
					vector<symbol> legal(fsys.begin(), fsys.end());
					legal.push_back(comma);
					legal.push_back(rparent);
					x = expression(legal);
					if (x.typ != tab[cp].typ) error(18); // 参数类型不匹配，强条件
				}
				else break;
			}
			vector<symbol> legal = { comma, rparent };
			test(legal, fsys, 4);  //测试
		} while (sy == comma);
	}
	else insymbol();
	if (sy == rparent) {
		insymbol();
	}
	else {
		error(3);  //应该是)
	}  //error
	if (cp < lastp) error(20);  //参数个数不匹配
	emit1(19, ftab[tab[i].ref].psize - 1);
}

item expression(vector<symbol> fsys) {  //表达式
	item x, y;
	x.typ = nottyp;
	x.ref = 0;
	bool isin = false;
	for (int i = 0; i < expbegsys.size(); i++) {
		if (sy == expbegsys[i]) {
			isin = true;
			break;
		}
	}
	if (!isin) {
		error(51);
		return x;
	}
	enum symbol op;
	vector<symbol> legal(fsys.begin(), fsys.end());
	legal.push_back(plussy);
	legal.push_back(minussy);
	if (sy == plussy || sy == minussy) {
		op = sy;
		insymbol();
		x = term(legal);
		if (op == minussy) {
			emit(36);  //指令 - 取负数
			if (x.typ == chars) {
				x.typ = ints;
			}
		}
	}
	else {
		x = term(legal);
	}
	while (sy == plussy || sy == minussy) {
		op = sy;
		insymbol();
		y = term(legal);
		x.typ = resulttype(x.typ, y.typ);
		if (x.typ == ints) {
			if (op == plussy) {
				emit(52);  //指令 - 整数/字符加
			}
			else {
				emit(53);  //指令 - 整数/字符减
			}
		}
		else if (x.typ == floats) {
			if (op == plussy) {
				emit(54);  //实数加
			}
			else {
				emit(55);  //实数减
			}
		}
		else error(16);
	}

	return x;
	//printf("This is Expression\n");
}

item term(vector<symbol> fsys) {
	item x, y;
	vector<symbol> legal(fsys.begin(), fsys.end());
	legal.push_back(timessy);
	legal.push_back(divsy);
	x = factor(legal);
	while (sy == timessy || sy == divsy) {
		enum symbol op = sy;
		insymbol();
		y = factor(legal);
		if (op == timessy) {
			x.typ = resulttype(x.typ, y.typ);
			switch (x.typ) {
			case ints: {
				emit(57);  //指令 - 整数乘法运算
				break;
			}
			case floats: {
				emit(60);  //指令 - 实数乘法运算
				break;
			}
			default: {
				error(16);
				break;
			}
			}
		}
		else {
			x.typ = resulttype(x.typ, y.typ);
			switch (x.typ) {
			case ints: {
				emit(58);  //指令 - 整数除
				break;
			}
			case floats: {
				emit(61);  //指令 - 实数除
				break;
			}
			default: {
				error(16);
				break;
			}
			}
		}
	}

	return x;
	//printf("This is Term\n");
}

item factor(vector<symbol> fsys) {
	item x;
	test(facbegsys, fsys, 37);
	if (sy == ident) {
		char idname[alen + 1]; strcpy_s(idname, id);
		int i = loc(id);  //获取该标识符在符号表中位置
		insymbol();
		if (sy == lbrack) {  //数组变量
			if (i == -1) error(0);
			emit2(0, isglobal, tab[i].adr);
			insymbol();
			vector<symbol> legal(fsys.begin(), fsys.end());
			legal.push_back(rbrack);
			expression(legal);
			emit1(20, tab[i].ref);  //指令 - 取下标变量地址
			emit(34);  //指令 - 将 栈顶单元内容 设置为 以栈顶单元内容 为 地址 的 单元内容
			x.typ = atab[tab[i].ref].eltyp;
			x.ref = 0;
			if (sy == rbrack) {
				insymbol();
			}
			else {
				error(7);  //应该是]
			}  //error
			test(fsys, fsys, 4); 
		}
		else if (sy == lparent) {  //函数调用
			i = locfun(idname);
			if (i != -1) {
				x.typ = tab[i].typ;
				x.ref = 0;
				call(fsys, i);
			}
			else error(0);  //error
		}
		else {
			if (i == -1) error(0);
			if (tab[i].obj == konstant) {
				x.typ = tab[i].typ;
				x.ref = 0;
				if (x.typ == floats) {
					emit1(25, tab[i].adr);  //指令 - 装入实数
				}
				else if (x.typ == ints) {
					emit2(24, 0, tab[i].adr);  //指令 - 装入整数/字符
				}
				else if (x.typ == chars) {
					emit2(24, 1, tab[i].adr);
				}
			}
			else {
				x.typ = tab[i].typ;
				x.ref = tab[i].ref;
				emit2(1, isglobal, tab[i].adr);  //指令 - 把变量值装入栈顶(f为函数在ftab中位置，adr为相对地址)
			}
		}
	}
	else if (sy == lparent) {
		insymbol();
		vector<symbol> legal(fsys.begin(), fsys.end());
		legal.push_back(rparent);
		x = expression(legal);
		if (x.typ == chars) {
			x.typ = ints;
			emit1(23, 0);
		}
		if (sy == rparent) {
			insymbol();
		}
		else {
			error(3);
		}  //error
	}
	else if (sy == charcon || sy == floatcon || sy == intcon) {
		if (sy == floatcon) {
			x.typ = floats;
			enterfloat(rnum);
			emit1(25, c1);
		}
		else {
			if (sy == charcon) {
				x.typ = chars;
				emit2(24, 1, inum);
			}
			else {
				x.typ = ints;
				emit2(24, 0, inum);
			}
		}
		x.ref = 0;
		insymbol();
	}
	else if (sy == plussy || sy == minussy) {  //<整数>开头可以是正负
		bool isneg = (sy == minussy);
		insymbol();
		if (sy == floatcon || sy == intcon) {
			if (sy == floatcon) {
				if (isneg) enterfloat(rnum*(-1));
				else enterfloat(rnum);
				x.typ = floats;
				emit1(25, c1);
			}
			else {
				x.typ = ints;
				if (isneg) {
					emit2(24, 0, -inum);
				}
				else emit2(24, 0, inum);
			} 
			x.ref = 0;
			insymbol();
		}
		else {
			error(42);  //字符型常量前面不能有加减号
		}  //error
	}

	return x;
	//printf("This is Factor\n");
}

void programs() {
	bool iscoin = false;  //变量声明后是否紧跟int/float/char型函数
	dx = ISIZE;
	//填写程序在函数表和符号表中信息，程序在函数表中占据1，在符号表中占据0
	ftab[f].psize = dx;
	ftab[f].lastpar = t;
	tab[t].link = 0;
	strcpy_s(tab[t].name, "program");
	tab[t].obj = funktion;
	tab[t].ref = 1;
	tab[t].typ = voids;
	vector<symbol> fsys_1 = { constsy, intsy, floatsy, charsy, voidsy };
	if (sy == constsy) {
		constdec(fsys_1);
	}
	while (sy == intsy || sy == floatsy || sy == charsy) {
		enum type tp;
		switch (sy) {
		case intsy: {
			tp = ints;
			break;
		}
		case floatsy: {
			tp = floats;
			break;
		}
		case charsy: {
			tp = chars;
			break;
		}
		}
		insymbol();
		if (sy == ident) {
			if (loc(id) != -1) error(1);
			enter(id, vvariable, tp);
			insymbol();
			if (sy == lparent) {  //函数定义部分，部分重用上面定义的函数
				if (locglobal(id) != -1) error(1);
				iscoin = true;
				ftab[f].vsize = dx;
				ftab[f].last = ftab[f].last - 1;  //修正ftab的last域值(last域不应该包括下一个函数标识符)
				tab[t].obj = funktion;
				dx = ISIZE;
				enterfun();
				tab[t].ref = f;
				vector<symbol> fsys_2 = { intsy, floatsy, charsy, voidsy };
				fundecident(fsys_2, tp);
				break;  //如果遇到函数定义部分，只判断一次，跳转到下面进行判断，让逻辑更顺畅
			}
			//变量定义部分，无法重用上面定义的函数
			else if (sy == lbrack || sy == comma || sy == semicolon) {
				int sz = 1;
				if (sy == lbrack) {
					insymbol();
					if (sy == intcon) {
						if (inum > 0) {
							enterarray(tp, inum);
							tab[t].typ = arrays;
							tab[t].ref = a;
							sz = inum;
							insymbol();
							if (sy == rbrack) {
								insymbol();
							}
							else error(7);  //error
						}
						else error(13);
					}
					else error(41);  //error
				}
				tab[t].adr = dx;
				dx = dx + sz;
				if (sy == comma) {
					insymbol();
					vector<symbol> legal = { ident };
					vector<symbol> fsys_3 = { intsy, floatsy, charsy, voidsy };
					test(legal, fsys_3, 2);
					while (sy == ident) {
						if (loc(id) != -1) error(1);
						enter(id, vvariable, tp);
						sz = 1;
						insymbol();
						if (sy == lbrack) {
							insymbol();
							if (sy == intcon) {
								if (inum > 0) {
									enterarray(tp, inum);
									tab[t].typ = arrays;
									tab[t].ref = a;
									sz = inum;
									insymbol();
									if (sy == rbrack) {
										insymbol();
									}
									else error(7);  //error
								}
								else error(13);
							}
							else error(41);  //error
						}
						tab[t].adr = dx;
						dx = dx + sz;
						if (sy == comma) {
							insymbol();
							test(legal, fsys_3, 2);
						}
						else break;
					}
				}
				vector<symbol> fsys_4 = { intsy, floatsy, charsy, voidsy };
				testsemicolon(fsys_4);
				//printf("This is Variables Declaration\n");
			}
			else error(46);  //error
		}
		else error(2);  //error
	}
	if (!iscoin) ftab[f].vsize = dx;
	vector<symbol> fsys_4 = { intsy, floatsy, charsy, voidsy };
	while (sy == voidsy || sy == intsy || sy == charsy || sy == floatsy) {
		if (sy != voidsy) {
			fundec(fsys_4);
		}
		else {  //主函数与无返回值函数定义有相同头部void，这里做一个区分
			insymbol();
			if (sy != mainsy) {
				enter(id, funktion, voids);
				ftab[f].last = ftab[f].last - 1;  //修正ftab的last域值(last域不应该包括下一个函数标识符)
				enterfun();
				tab[t].ref = f;
				dx = ISIZE;
				insymbol();
				fundecident(fsys_4, voids);
			}
			else break;
		}
	}
	if (sy == mainsy) {
		enter(id, funktion, voids);
		int prt = t;
		ftab[f].last = ftab[f].last - 1;  //修正ftab的last域值(last域不应该包括下一个函数标识符)
		enterfun();
		tab[t].ref = f;
		dx = ISIZE;
		ftab[f].lastpar = t;
		ftab[f].psize = dx;
		insymbol();
		if (sy == lparent) {
			insymbol();
			if (sy == rparent) {
				insymbol();
				if (sy == lcurly) {
					insymbol();
					vector<symbol> fsys_5 = { rcurly };
					compoundstat(prt, fsys_5);
					if (sy == rcurly) {
						//printf("This is Main Function\n");
					}
					else error(27);  //error
					emit(33);  //退出main函数
					emit(31);  //指令 - 程序停止
				}
				else {
					error(26);  //error
				}
			}
			else {
				error(3);  //error
			}
		}
		else {
			error(5);  //error
		}
	}
	else error(47);  //error
}

void stkcpy(stru_s &s1, stru_s &s2) {
	switch (s2.datatype) {
	case 0: {
		s1.i = s2.i;
		break;
	}
	case 1: {
		s1.f = s2.f;
		break;
	}
	case 2: {
		s1.i = s2.i;  //字符的i域与c域都是有效域
		s1.c = s2.c;
	}
	}
}

void completeCopy(stru_s &s1, stru_s &s2) {
	s1.datatype = s2.datatype;
	switch (s2.datatype) {
	case 0: {
		s1.i = s2.i;
		break;
	}
	case 1: {
		s1.f = s2.f;
		break;
	}
	case 2: {
		s1.i = s2.i;
		s1.c = s2.c;
		break;
	}
	}
}

void inter0() {
	switch (ir.f) {
	case 0: {  /* LDA - 将变量地址装入栈顶 */
		top = top + 1;
		if (top > stacksize) ps = stkchk;
		else {
			if (ir.x) s[top].i = ir.y;  //全局变量，b = 0
			else s[top].i = base + ir.y;  //b是当前数据区基地址，y是地址偏移
		}
		break;
	}
	case 1: {  /* LOD - 将变量值装入栈顶 */
		top = top + 1;
		if (top > stacksize) ps = stkchk;
		else {
			if (ir.x) {  //全局变量
				completeCopy(s[top], s[ir.y]);
			}
			else {
				completeCopy(s[top], s[base + ir.y]);
			}
		}
		break;
	}
	}
}

void inter1() {
	switch (ir.f) {
	case 10: {  /* JMP - 无条件跳转 */
		pc = ir.y;
		break;
	}
	case 11: {  /* JPC - 如果栈顶内容为假，跳转 */
		bool tojump = false;
		switch (s[top].datatype) {
		case 0: {
			if (s[top].i == 0) tojump = true;
			break;
		}
		case 1: {
			if (s[top].f == 0) tojump = true;
			break;
		}
		case 2: {  //字符型的i域存储其ASCII值
			if (s[top].i == 0) tojump = true;
			break;
		}
		}
		if (tojump) pc = ir.y;
		top = top - 1;
		break;
	}
	case 12: {  /* SWT - 跳转到y，查找情况表 */
		int h1 = s[top].i;
		top = top - 1;
		int h2 = ir.y;
		int h3 = 0;
		do {
			if (code[h2].f != 13) {  //找不到满足条件的情况，此时code[h2].f应该是10，表示跳转到switch结束
				h3 = 1;
				pc = code[h2].y;
			}
			else if (code[h2].y == h1 || code[h2].x == 1) {  //情况匹配或读到缺省
				h3 = 1;
				pc = code[h2 + 1].y;
			}
			else h2 = h2 + 2;  //加2，情况表会登记两项
		} while (h3 == 0);
		break;
	}
	case 18: {  /* MKS - 标记栈 */
		int h1 = ftab[tab[ir.y].ref].vsize;
		if (top + h1 > stacksize) ps = stkchk;
		else {
			top = top + ISIZE;
			s[top - 1].i = h1 - 1;
			s[top].i = ir.y;
		}
		break;
	}
	case 19: {  /* CAL - 调用用户过程或函数 */
		int h1 = top - ir.y;  //h1指向函数数据栈低
		int h2 = s[h1 + 3].i;  //h2指向函数在tab符号表中位置
		int h4 = s[h1 + 2].i + h1;  //h4指向 栈低 加上 内务信息区+参数区+变量区 后的位置
		s[h1 + 1].i = pc;  //调用点下一条指令地址
		s[h1 + 2].i = base;  //调用者数据区基地址
		int firstv = ftab[tab[h2].ref].lastpar, open;
		while (true) {
			if (tab[firstv + 1].obj == konstant) firstv++;
			else break;
		}
		for (int i = top + 1; i <= h4; i++) {
			s[i].i = 0;  //将被调用函数的变量全部初始化为0
			switch (tab[++firstv].typ) {
			case ints: {
				s[i].datatype = 0;
				break;
			}
			case floats: {
				s[i].datatype = 1;
				break;
			}
			case chars: {
				s[i].datatype = 2;
				break;
			}
			case arrays: {
				open = i + atab[tab[firstv].ref].size;
				for (int k = i; k < open; k++) {
					s[k].i = 0;
					switch (atab[tab[firstv].ref].eltyp) {
					case ints: {
						s[k].datatype = 0;
						break;
					}
					case floats: {
						s[k].datatype = 1;
						break;
					}
					case chars: {
						s[k].datatype = 2;
						break;
					}
					}
				}
				i = open - 1;
				break;
			}
			}
		}
		base = h1;  //base赋值为当前函数数据区基地址
		top = h4;  //top指向最后一个函数变量位置
		pc = tab[h2].adr;  //pc定向到当前函数代码地址
		break;
	}
	}
}

void inter2() {
	switch (ir.f) {
	case 20: {  /* IDX - 根据下标得到数组元素地址 */
		int h1 = ir.y;  //h1指向数组在atab中位置
		int h2 = atab[h1].size;  //h2是数组大小
		int h3 = s[top].i;  //下标一定是整型
		if (h3 >= h2 || h3 < 0) {  //数组下标越界
			ps = inxchk;
		}
		else {
			top = top - 1;
			s[top].i = s[top].i + h3;  //此时s[top].i表示数组基地址，加上下标得到数组元素实际地址
		}
		break;
	}
	case 23: {
		int h1 = top - ir.y;
		s[h1].i = s[h1].c;
		s[h1].datatype = 0;
		break;
	}
	case 24: {  /* LDC - 装入字面常量(整型/字符型) */
		top = top + 1;
		if (top > stacksize) ps = stkchk;
		else {
			if (ir.x == 0) {  //整型
				s[top].datatype = 0;
				s[top].i = ir.y;
			}
			if (ir.x == 1) {  //字符型
				s[top].datatype = 2;
				s[top].i = ir.y;
				s[top].c = ir.y;
			}
		}
		break;
	}
	case 25: {  /* LDR - 装入实数 */
		top = top + 1;
		if (top > stacksize) ps = stkchk;
		else {
			s[top].datatype = 1;
			s[top].f = fconst[ir.y];
		}
		break;
	}
	case 26: {  /* FLT - 转换为浮点数 */
		int h1 = top - ir.y;
		if (s[h1].datatype == 0) {
			s[h1].f = s[h1].i;
		}
		else if (s[h1].datatype == 2) {
			s[h1].f = s[h1].c;
		}
		s[h1].datatype = 1;
		break;
	}
	case 27: {  /* RED - 读指令 - y表示类型(1-整型;2-浮点型;3-字符型) */
		switch (ir.y) {
		case 1: {
			scanf_s("%d", &s[s[top].i].i, sizeof(s[s[top].i].i));
			s[s[top].i].datatype = 0;
			break;
		}
		case 2: {
			scanf_s("%lf", &s[s[top].i].f, sizeof(s[s[top].i].f));
			s[s[top].i].datatype = 1;
			break;
		}
		case 3: {  //字符运算都是以整数ASCII值进行，所以读入字符后，要在其i域填上相应的ASCII码值
			char filter = getchar(); //过滤换行符
			if (filter == '\n')
				scanf_s("%c", &s[s[top].i].c, sizeof(s[s[top].i].c));
			else
				s[s[top].i].c = filter;
			s[s[top].i].datatype = 2;
			s[s[top].i].i = s[s[top].i].c;
			break;
		}
		}
		top = top - 1;
		break;
	}
	case 28: {  /* WRS - 写指令 - 输出字符串 */
		int h1 = s[top].i;  //h1表示字符串长度
		int h2 = ir.y;  //h2表示字符串在stab起始位置
		top = top - 1;
		while (h1 != 0) {
			printf_s("%c", stab[h2]);
			h2 = h2 + 1;
			h1 = h1 - 1;
		}
		printf_s("\n");
		break;
	}
	case 29: {  /* WRW - 写指令 - 输出表达式(1-整型;2-浮点型;3-字符型) */
		switch (ir.y) {
		case 1: {
			printf_s("%d\n", s[top].i);
			break;
		}
		case 2: {
			printf_s("%f\n", s[top].f);
			break;
		}
		case 3: {  //单个字符型常量/变量打印字符
			printf_s("%c\n", s[top].c);
			break;
		}
		}
		top = top - 1;
		break;
	}
	}
}

void inter3() {
	switch (ir.f) {
	case 31: {  /* HLT - 程序停止 */
		ps = fin;
		break;
	}
	case 32: {  /* LDR - 装入函数结果(返回值) - (1:整型;2实型;3字符型) */
		switch (ir.y) {
		case 1: {
			s[base].i = s[top].i;
			s[base].datatype = 0;
			break;
		}
		case 2: {
			s[base].f = s[top].f;
			s[base].datatype = 1;
			break;
		}
		case 3: {
			s[base].i = s[top].i;
			s[base].c = s[top].c;
			s[base].datatype = 2;
			break;
		}
		}
		break;
	}
	case 33: {  /* EXF - 退出函数 */
		top = base;  //top指向函数结果
		pc = s[top + 1].i;  //pc指向调用处下一条语句
		base = s[base + 2].i;  //base指向调用者数据区基地址
		break;
	}
	case 34: {  /* LDT - 将栈顶单元设置为以栈顶单元内容为地址的单元的值 */
		completeCopy(s[top], s[s[top].i]);
		break;
	}
	case 36: {  /* MUS - 求负数 */
		switch (s[top].datatype) {
		case 0: {
			s[top].i = -s[top].i;
			break;
		}
		case 1: {
			s[top].f = -s[top].f;
			break;
		}
		case 2: {  //字符型参与运算，结果为整型
			s[top].i = -s[top].i;
			s[top].datatype = 0;
			break;
		}
		}
		break;
	}
	case 38: {  /* STO - 将栈顶内容存入以栈顶次高元为地址的单元 */
		stkcpy(s[s[top - 1].i], s[top]);
		top = top - 2;
		break;
	}
	case 39: {  /* EQR - 实型相等比较 */
		top = top - 1;
		s[top].datatype = 0;  //整型值为0表示false，整型值为1表示true
		s[top].i = (s[top].f == s[top + 1].f) ? 1 : 0;
		break;
	}
	}
}

void inter4() {
	switch (ir.f) {
	case 40: {  /* NER - 实型不等于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].f != s[top + 1].f) ? 1 : 0;
		break;
	}
	case 41: {  /* LSR - 实型小于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].f < s[top + 1].f) ? 1 : 0;
		break;
	}
	case 42: {  /* LER - 实型小于等于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].f <= s[top + 1].f) ? 1 : 0;
		break;
	}
	case 43: {  /* GTR - 实型大于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].f > s[top + 1].f) ? 1 : 0;
		break;
	}
	case 44: {  /* GER - 实型大于等于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].f >= s[top + 1].f) ? 1 : 0;
		break;
	}
	case 45: {  /* EQL - 整型相等比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i == s[top + 1].i) ? 1 : 0;
		break;
	}
	case 46: {  /* NEQ - 整型不等比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i != s[top + 1].i) ? 1 : 0;
		break;
	}
	case 47: {  /* LSS - 整型小于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i < s[top + 1].i) ? 1 : 0;
		break;
	}
	case 48: {  /* LER - 整型小于等于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i <= s[top + 1].i) ? 1 : 0;
		break;
	}
	case 49: {  /* GRT - 整型大于比较 */
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i > s[top + 1].i) ? 1 : 0;
		break;
	}
	}
}

void inter5() {
	switch (ir.f) {
	case 50: {  /* GEQ - 整型大于等于比较*/
		top = top - 1;
		s[top].datatype = 0;
		s[top].i = (s[top].i >= s[top + 1].i) ? 1 : 0;
		break;
	}
	case 52: {  /* ADD - 整型加 */
		top = top - 1;
		s[top].i = s[top].i + s[top + 1].i;
		s[top].datatype = 0;
		break;
	}
	case 53: {  /* SUB - 整型减 */
		top = top - 1;
		s[top].i = s[top].i - s[top + 1].i;
		s[top].datatype = 0;
		break;
	}
	case 54: {  /* ADR - 实型加 */
		top = top - 1;
		s[top].f = s[top].f + s[top + 1].f;
		break;
	}
	case 55: {  /* SUR - 实型减 */
		top = top - 1;
		s[top].f = s[top].f - s[top + 1].f;
		break;
	}
	case 57: {  /* MUL - 整型乘 */
		top = top - 1;
		s[top].i = s[top].i * s[top + 1].i;
		s[top].datatype = 0;
		break;
	}
	case 58: {  /* DIV - 整型除 */
		top = top - 1;
		if (s[top + 1].i == 0) {
			ps = divchk;
		}
		else {
			s[top].i = s[top].i / s[top + 1].i;
			s[top].datatype = 0;
		}
		break;
	}
	}
}

void inter6() {
	switch (ir.f) {
	case 60: {  /* MUR - 实型乘 */
		top = top - 1;
		s[top].f = s[top].f * s[top + 1].f;
		break;
	}
	case 61: {  /* DIR - 实型除 */
		top = top - 1;
		if (s[top + 1].f == 0) ps = divchk;
		else s[top].f = s[top].f / s[top + 1].f;
		break;
	}
	}
}

void interpret() {
	//解释执行初启，初始化全局变量区内容
	s[0].i = 0;
	s[1].i = lc - 1;
	s[2].i = -1;
	s[3].i = ftab[1].last;

	int firstv = 0;
	while (true) {
		if (tab[firstv + 1].obj == konstant) firstv++;
		else break;
	}
	for (int i = 4; i < ftab[tab[0].ref].vsize; i++) {
		s[i].i = 0;  //将被调用函数的变量全部初始化为0
		switch (tab[++firstv].typ) {
		case ints: {
			s[i].datatype = 0;
			break;
		}
		case floats: {
			s[i].datatype = 1;
			break;
		}
		case chars: {
			s[i].datatype = 2;
			break;
		}
		case arrays: {
			int open = i + atab[tab[firstv].ref].size;
			for (int k = i; k < open; k++) {
				s[k].i = 0;
				switch (atab[tab[firstv].ref].eltyp) {
				case ints: {
					s[k].datatype = 0;
					break;
				}
				case floats: {
					s[k].datatype = 1;
					break;
				}
				case chars: {
					s[k].datatype = 2;
					break;
				}
				}
			}
			i = open - 1;
			break;
		}
		}
	}

	base = 0;
	top = ftab[1].vsize - 1;  //top指向的数据必定是当前有效数据，不是指向顶部数据的上面一个位置
	pc = tab[0].adr;
	ps = run;
	while (ps == run) {
		ir = code[pc];
		pc = pc + 1;
		switch (ir.f / 10) {
		case 0: {
			inter0();
			break;
		}
		case 1: {
			inter1();
			break;
		}
		case 2: {
			inter2();
			break;
		}
		case 3: {
			inter3();
			break;
		}
		case 4: {
			inter4();
			break;
		}
		case 5: {
			inter5();
			break;
		}
		case 6: {
			inter6();
			break;
		}
		}
	}
	if (ps != fin) {
		printf_s("解释执行异常，异常处PC值为 %d\n", pc);
		switch (ps) {
		case divchk: {
			printf_s("除数为0\n");
			break;
		}
		case inxchk: {
			printf_s("数组下标越界\n");
			break;
		}
		case stkchk: {
			printf_s("运行栈溢出\n");
			break;
		}
		}
	}
}

void printtables() {
	char objlist[3][16] = { "constant", "variable", "function" };
	char typlist[5][16] = { "int", "char", "float", "array", "void" };
	char mne[64][4];
	strcpy_s(mne[0], "LDA"); strcpy_s(mne[1], "LOD"); strcpy_s(mne[2], "LDI");
	strcpy_s(mne[3], "DIS"); strcpy_s(mne[8], "FCT"); strcpy_s(mne[9], "INT");
	strcpy_s(mne[10], "JMP"); strcpy_s(mne[11], "JPC"); strcpy_s(mne[12], "SWT");
	strcpy_s(mne[13], "CAS"); strcpy_s(mne[14], "F1U"); strcpy_s(mne[15], "F2U");
	strcpy_s(mne[16], "F1D"); strcpy_s(mne[17], "F2D"); strcpy_s(mne[18], "MKS");
	strcpy_s(mne[19], "CAL"); strcpy_s(mne[20], "IDX"); strcpy_s(mne[21], "IXX");
	strcpy_s(mne[22], "LDB"); strcpy_s(mne[23], "INT"); strcpy_s(mne[24], "LDC");
	strcpy_s(mne[25], "LDR"); strcpy_s(mne[26], "FLT"); strcpy_s(mne[27], "RED");
	strcpy_s(mne[28], "WRS"); strcpy_s(mne[29], "WRW"); strcpy_s(mne[30], "WRU");
	strcpy_s(mne[31], "HLT"); strcpy_s(mne[32], "LDR"); strcpy_s(mne[33], "EXF");
	strcpy_s(mne[34], "LDT"); strcpy_s(mne[35], "NOT"); strcpy_s(mne[36], "MUS");
	strcpy_s(mne[37], "WRR"); strcpy_s(mne[38], "STO"); strcpy_s(mne[39], "EQR");
	strcpy_s(mne[40], "NER"); strcpy_s(mne[41], "LSR"); strcpy_s(mne[42], "LER");
	strcpy_s(mne[43], "GTR"); strcpy_s(mne[44], "GER"); strcpy_s(mne[45], "EQL");
	strcpy_s(mne[46], "NEQ"); strcpy_s(mne[47], "LSS"); strcpy_s(mne[48], "LEQ");
	strcpy_s(mne[49], "GRT"); strcpy_s(mne[50], "GEQ"); strcpy_s(mne[51], "ORR");
	strcpy_s(mne[52], "ADD"); strcpy_s(mne[53], "SUB"); strcpy_s(mne[54], "ADR");
	strcpy_s(mne[55], "SUR"); strcpy_s(mne[56], "AND"); strcpy_s(mne[57], "MUL");
	strcpy_s(mne[58], "DIV"); strcpy_s(mne[59], "MOD"); strcpy_s(mne[60], "MUR");
	strcpy_s(mne[61], "DIR"); strcpy_s(mne[62], "RDL"); strcpy_s(mne[63], "WRL");
	printf_s("符号表内容如下：\n");
	printf_s("index \t name \t link \t object \t type \t ref \t adr \t\n");
	for (int i = 0; i <= t; i++) {
		printf_s("%d \t %s \t %d \t %s \t %s \t %d \t %d \t\n", i, tab[i].name, tab[i].link, objlist[tab[i].obj], typlist[tab[i].typ], tab[i].ref, tab[i].adr);
	}
	printf_s("函数信息表内容如下：\n");
	printf_s("index\tlastpar\tlast\tpsize\tvsize\t\n");
	for (int i = 1; i <= f; i++) {
		printf_s("%d\t%d\t%d\t%d\t%d\t\n", i, ftab[i].lastpar, ftab[i].last, ftab[i].psize, ftab[i].vsize);
	}
	printf_s("数组信息表内容如下：\n");
	printf_s("index \t eltyp \t size \t\n");
	for (int i = 1; i <= a; i++) {
		printf_s("%d \t %s \t %d \t\n", i, typlist[atab[i].eltyp], atab[i].size);
	}
	printf_s("实常数表内容如下：\n");
	printf_s("index\tvalue\t\n");
	for (int i = 1; i <= c2; i++) {
		printf_s("%d\t%lf\t", i, fconst[i]);
	}
	printf("\n");
	printf_s("字符串常数表内容如下：\n");
	for (int i = 0; i <= sx; i++) {
		printf_s("%c", stab[i]);
	}
	printf("\n");
	printf_s("P代码表内容如下：\n");
	printf_s("地址\t助记符\t操作码\tx域\ty域\t\n");
	for (int i = 0; i < lc; i++) {
		if (code[i].x != INF && code[i].y != INF) {
			printf_s("%d\t%s\t%d\t%d\t%d\t\n", i, mne[code[i].f], code[i].f, code[i].x, code[i].y);
		}
		else if (code[i].y != INF) {
			printf_s("%d\t%s\t%d\t \t%d\t\n", i, mne[code[i].f], code[i].f, code[i].y);
		}
		else if (code[i].x != INF) {
			printf_s("%d\t%s\t%d\t%d\t \t\n", i, mne[code[i].f], code[i].f, code[i].x);
		}
		else {
			printf_s("%d\t%s\t%d\t \t \t\n", i, mne[code[i].f], code[i].f);
		}
	}
	printf("\n");
}

int main() {
	char path[100];
	gets_s(path);
	if (fopen_s(&fp, path, "r") != 0) {
		printf_s("Cannot Open Source File! \n");
		fclose(fp);
		system("pause");
		return -1;
	}
	setup();
	ch = ' ';
	insymbol();
	programs();
	fclose(fp);
	if (er == 0) {
		printtables();
		interpret();
	}
	else {
		errmsg();
	}
	system("pause");
	return 0;
}