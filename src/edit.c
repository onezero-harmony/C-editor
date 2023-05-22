#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>

RENode *CancelNodeStack, *RemakeNodeStack;
RELINE *CancelLineStack, *RemakeLineStack;
REpos  *CancelposStack,  *RemakeposStack;	//用三个栈来维护需要撤销和重做的内容 

//linenum: 总行数  col：当前列  row：当前行   CursorExist：光标是否出现   lastcol,lastrow：选中区域时起始行列
//uprow,leftcol   当前显示的最上方一行与最左侧一列
//nowline当前行，nownode当前列，head永远指向最上方一行，theother括号匹配的另一个括号 
int linenum, col, row, CursorExist, lastcol, lastrow;	
int uprow, leftcol;
LINE *nowline, *lastline, *head, *displayhead;
Node *nownode, *lastnode, *theother;
int ChooseRange, ClickEdit;
double textw = 9.5, texth = 6.68, leftw; 	//编辑区范围 
int Operatorid;			//操作的id 
int PointSize = 15; 		//字号 

void savepos(){			//存下当前光标的位置信息 
	isSaved = 0;
	while(RemakeLineStack->next != NULL){
		RELINE *p = RemakeLineStack->next;
		RemakeLineStack->next = p->next;
		free(p);
	}
	while(RemakeNodeStack->next != NULL){		//加入了新信息，因此把重做的栈直接清空 
		RENode *p = RemakeNodeStack->next;
		RemakeNodeStack->next = p->next;
		free(p);
	}
	REpos *p = (REpos*)calloc(1,sizeof(REpos));
	p->id = Operatorid;
	p->tcol = col;
	p->trow = row;
	p->tline = nowline;
	p->tnode = nownode;
	p->tlinenum = linenum;
	p->next = CancelposStack->next;
	CancelposStack->next = p;
}
void savenode(Node *node,Node *nodenext, Node *nodepre, int id){		//存下当前节点 
	RENode *p = (RENode*)calloc(1,sizeof(RENode));
	p->id = id;
	p->node = node;
	p->nodenext = nodenext;
	p->nodepre = nodepre;
	p->next = CancelNodeStack->next;
	CancelNodeStack->next = p;
}
void saveline(LINE *node,LINE *nodenext, LINE *nodepre, int id){		//存下当前行的信息 
	RELINE *p = (RELINE*)calloc(1,sizeof(RELINE));
	p->id = id;
	p->node = node;
	p->nodenext = nodenext;
	p->nodepre = nodepre;
	p->next = CancelLineStack->next;
	CancelLineStack->next = p;
}

void CANCEL(){				//撤销 ，将与撤销相关的三个栈的栈首元素取出，塞入与重做相关的栈中  
	int maxid = -1;	
	ChooseRange = ClickEdit = 0;
	if(CancelposStack->next == NULL) return;
	REpos *t = CancelposStack->next, *t2 = (REpos*)calloc(1, sizeof(REpos));
	t2->id = t->id;
	t2->tcol = col;
	t2->trow = row;
	t2->tlinenum = linenum;
	t2->tline = nowline;
	t2->tnode = nownode;
	t2->next = RemakeposStack->next;
	RemakeposStack->next = t2;
	maxid = t->id;
	col = t->tcol;
	row = t->trow;
	linenum = t->tlinenum;
	nowline = t->tline;
	nownode = t->tnode;
	CancelposStack->next = t->next;
	free(t);
	while(CancelNodeStack->next != NULL && CancelNodeStack->next->id == maxid){
		RENode *p = CancelNodeStack->next, *q = (RENode*)calloc(1,sizeof(RENode));
		q->id = p->id;
		q->node = p->node;
		q->nodenext = p->node->next;
		q->nodepre  = p->node->pre;
		q->next = RemakeNodeStack->next;
		RemakeNodeStack->next = q;
		p->node->next = p->nodenext;
		p->node->pre  = p->nodepre ;
		CancelNodeStack->next = p->next;
		free(p);
	}
	while(CancelLineStack->next != NULL && CancelLineStack->next->id == maxid){
		RELINE *p = CancelLineStack->next, *q = (RELINE*)calloc(1,sizeof(RELINE));
		q->id = p->id;
		q->node = p->node;
		q->nodenext = p->node->next;
		q->nodepre  = p->node->pre;
		q->next = RemakeLineStack->next;
		RemakeLineStack->next = q;
		p->node->next = p->nodenext;
		p->node->pre  = p->nodepre ;
		CancelLineStack->next = p->next;
		free(p);
	}
}

void REMAKE(){							//重做，将与重做相关的三个栈的栈首元素取出，塞入与撤销相关的栈中 
	ChooseRange = ClickEdit = 0;
	int minid = 1000000000;
	if(RemakeposStack->next == NULL) return;
	REpos *t = RemakeposStack->next, *t2 = (REpos*)calloc(1, sizeof(REpos));
	t2->id = t->id;
	t2->tcol = col;
	t2->trow = row;
	t2->tlinenum = linenum;
	t2->tline = nowline;
	t2->tnode = nownode;
	t2->next = CancelposStack->next;
	CancelposStack->next = t2;
	minid = t->id;
	col = t->tcol;
	row = t->trow;
	linenum = t->tlinenum;
	nowline = t->tline;
	nownode = t->tnode;
	RemakeposStack->next = t->next;
	free(t);
	while(RemakeNodeStack->next != NULL && RemakeNodeStack->next->id == minid){
		RENode *p = RemakeNodeStack->next, *q = (RENode*)calloc(1,sizeof(RENode));
		q->id = p->id;
		q->node = p->node;
		q->nodenext = p->node->next;
		q->nodepre  = p->node->pre;
		q->next = CancelNodeStack->next;
		CancelNodeStack->next = q;
		p->node->next = p->nodenext;
		p->node->pre  = p->nodepre ;
		RemakeNodeStack->next = p->next;
		free(p);
	}
	while(RemakeLineStack->next != NULL && RemakeLineStack->next->id == minid){
		RELINE *p = RemakeLineStack->next, *q = (RELINE*)calloc(1,sizeof(RELINE));
		q->id = p->id;
		q->node = p->node;
		q->nodenext = p->node->next;
		q->nodepre  = p->node->pre;
		q->next = CancelLineStack->next;
		CancelLineStack->next = q;
		p->node->next = p->nodenext;
		p->node->pre  = p->nodepre ;
		RemakeLineStack->next = p->next;
		free(p);
	}
}

static int issleep = 0;
void modifylocate(){			//修改屏幕上显示的范围 
	double h = GetFontHeight(), w = TextStringWidth("A");
	char tmp[30];
	sprintf(tmp,"%d",uprow+90);
	leftw = w * ((int)strlen(tmp)+2);
	if(leftw<0.5) leftw = 0.5;
	textw = 10 - leftw;
	int x = (int)(textw / w - 0.00001) - 1, y = (int)(texth / h - 0.00001) - 1;
	x = max(x, 0);
	y = max(y, 0);
	issleep = 0;
	if(row <= uprow || row > uprow + y - 1){
		if(row < uprow){
			for(int i = 0; i < uprow - row && displayhead->pre; i++, uprow--) displayhead = displayhead->pre;
		}
		else if(row == uprow && uprow != 1){
			displayhead = displayhead->pre;
			uprow = row - 1;
			issleep = 1;
		}
		else if(row > uprow + y - 1){
			for(int i = 0; i < -uprow - y + 1 + row && displayhead->next; i++, uprow++) 
				displayhead = displayhead->next;
		}
	}
	leftcol = max(0, col - x);
}

int GetNewCursor(double x, double y){			//获取光标位置 
	CalculateText();
	SetPointSize(PointSize);
	if(!(x >= leftw && x <= leftw + textw && y >= 0 && y<=texth)) {
		SetPointSize(15);
		return 0;
	}
	CursorExist = 1;
	double h = GetFontHeight(), w = TextStringWidth("A");
	int xnum = (int)((x-leftw) / w + 0.48), ynum = (int)((texth - y) / h);
	int flag = (ynum > (linenum - uprow));
	ynum = min(ynum, linenum - uprow);
//	row = uprow + ynum;
	row = uprow;
	nowline = displayhead;
	for(int i = 0; i < ynum && nowline->next; i++, row++) nowline = nowline->next;
	xnum = min(xnum + leftcol, nowline->chnum);
	if(flag) xnum = nowline->chnum;
	col = xnum;
	nownode = nowline->head;
	int i;
	for(i = 0; i < xnum && nownode && nownode->next ;i += strlen(nownode->c)) nownode = nownode->next;
	col = i;
	SetPointSize(15);
	return 1;
} 
int ischosen(int r, int c){				//判断第r行第c列的字符是否被选中 
	int minr,minc,maxr,maxc;
	if(!ChooseRange) return 0; 
	if(row < lastrow || (row == lastrow && col < lastcol)){
		minr = row;
		minc = col;
		maxr = lastrow;
		maxc = lastcol;
	}
	else{
		minr = lastrow;
		minc = lastcol;
		maxr = row;
		maxc = col;
	}
	if(maxr != minr && ((r == minr && c >= minc) || (r>minr && r < maxr) || (r == maxr && c < maxc))) return 1;
	if(maxr == minr && r==maxr && c >= minc && c<maxc) return 1;
	return 0;
}

void DeleteRange(){					//删除选中区域 
	ClickEdit = ChooseRange = 0;
	if(lastnode == nownode) return;
	if(lastrow<row||(lastrow == row && lastcol < col)){		//处理一下哪一端是开头，哪一端是结尾 
		int tmp = lastrow;
		lastrow = row;
		row = tmp;
		tmp = lastcol;
		lastcol = col;
		col = tmp;
		LINE *tmp2 = lastline;
		lastline = nowline;
		nowline = tmp2;
		Node *tmp3 = lastnode;
		lastnode = nownode;
		nownode = tmp3;
	}
	//区间删除，存档用于撤销与重做 
	if(row != lastrow){
		saveline(nowline, nowline->next, nowline->pre, Operatorid);
		nowline->next = lastline->next;
		if(lastline->next != NULL) {
			saveline(lastline->next, lastline->next->next, lastline->next->pre, Operatorid);
			lastline->next->pre = nowline;
		}
		nowline->chnum = col + lastline->chnum - lastcol;
	}
	else nowline->chnum -= lastcol - col;
	savenode(nownode, nownode->next, nownode->pre, Operatorid);
	nownode->next = lastnode->next;
	if(lastnode->next != NULL) {
		savenode(lastnode->next, nownode->next->next, nownode->next->pre, Operatorid);
		lastnode->next->pre = nownode;
	}
	CalculateText();
}

LINE *RangeStartLine, *RangeEndLine;		//选中区域 
Node *RangeStartNode, *RangeEndNode;

int GetRange(){
	if(ChooseRange == 0) return -1;
	if(lastrow<row||(lastrow == row && lastcol < col)){
		RangeStartLine = lastline;
		RangeStartNode = lastnode;
		RangeEndLine   = nowline;
		RangeEndNode   = nownode;
	}
	else{
		RangeStartLine = nowline;
		RangeStartNode = nownode;
		RangeEndLine   = lastline;
		RangeEndNode   = lastnode;
	}
	return 0;
}


/*
	复制选中的区域 
*/
void CopyContent()
{
	if((GetRange())==-1) return ;
	int length= 0;
	LINE *p= RangeStartLine;
	Node *q= RangeStartNode;
	q = q->next;
	
	// 统计字符串的长度 
	while(p){
		while(q && q != RangeEndNode){
			length+= strlen(q->c);
			q= q->next;
		}
		length += 2;
		if(q== RangeEndNode) break;
		else{
			p= p->next;
			q= p->head->next;
		}
	}
	// 将范围内的数据写入字符串中 
	char *s= calloc(length * 2 + 3, sizeof(char));
	p= RangeStartLine;
	q= RangeStartNode->next;
	while(p){
		while(q && q != RangeEndNode){
			strcat(s, q->c);
			q= q->next;
		}
		if(q== RangeEndNode){
			strcat(s, q->c);
			break;
		}
		else{
			s[strlen(s)]= '\n';
			p= p->next;
			q= p->head->next;
		}
	}
	Sleep(50);
	// 将字符串写到内存区域中 
	OpenClipboard(NULL);     // 打开剪贴板 
	EmptyClipboard();        // 清空剪贴板
	HGLOBAL hMem= GlobalAlloc(GMEM_MOVEABLE, (strlen(s)+1)*sizeof(char));    // 分配内存区域 
	LPSTR pMem= GlobalLock(hMem);      // 锁定内存区域 
	memcpy(pMem, s, (strlen(s))*sizeof(char)); // 复制字符串到内存区域中 
	pMem[strlen(s)]= '\0';            // 设置结束符 
	GlobalUnlock(hMem);                   // 释放锁
	SetClipboardData(CF_TEXT, hMem);      // 将内存中的数据复制到剪贴板上
	CloseClipboard();                     // 关闭剪贴板 
	free(s);
}

/*
	从当前光标所在位置开始粘贴剪贴板中的内容 
*/
void PasteContent()
{
	char *s;                       // 储存剪贴板中数据的字符串 
	OpenClipboard(NULL);
	HGLOBAL hMem= GetClipboardData(CF_TEXT);   // 从剪贴板中获取数据
	char *pMem= GlobalLock(hMem);              // 锁住内存区域 
	s= (char *)calloc((strlen(pMem)+1), sizeof(char));
	strcpy(s, pMem);
	GlobalUnlock(hMem);         // 释放内存锁 
	CloseClipboard();           // 关闭剪贴板 
	if(strlen(s) == 0) return;
//	Sleep(50);
	Operatorid++;savepos();
	if(ChooseRange) DeleteRange(); 
	
	savenode(nownode, nownode->next, nownode->pre, Operatorid);
	if(nownode->next != NULL) savenode(nownode->next, nownode->next->next, nownode->next->pre, Operatorid);
	saveline(nowline, nowline->next, nowline->pre, Operatorid);
	if(nowline->next != NULL) saveline(nowline->next, nowline->next->next, nowline->next->pre, Operatorid);
	int i;
	int flagChinese = 0;         // 是否是中文 
	for(i=0; i<strlen(s); i++){
		if(s[i]>0){                  // 不是中文 
			nownode->line= nowline;
			if(s[i]=='\n'){          // 需要换行 
				linenum++;
				row++;
				col = 0; 
				LINE *p= calloc(1, sizeof(LINE));
				p->pre= nowline;
				p->next= nowline->next;
				if(nowline->next != NULL) nowline->next->pre = p;
				nowline->next = p;
				nowline = p;
				Node *q = calloc(1, sizeof(Node));
				q->pre = NULL;
				q->next = nownode->next;
				nownode->next = NULL;    // 每一行的最后一个node的next为NULL 
				nowline->head = q;
				nownode = q;
				q->line = nowline;
			}
			else{                   
				
				Node *q= calloc(1, sizeof(Node));
				if(s[i] == '\t'){
					col += 4;
					q->c[0] = q->c[1] = q->c[2] = q->c[3] = ' ';
				}
				else{
					col++;
					q->c[0] = s[i];	
				}
				q->pre= nownode;
				q->next= nownode->next;
				if(nownode->next != NULL) nownode->next->pre = q;
				nownode->next = q;
				nownode= q;
			}
		}
		else{                        // 是中文 
			nownode->line= nowline;
			if(flagChinese){
				nownode->c[1]= s[i];
				flagChinese = 0;
			}
			else{
				col+=2;
				Node *q = calloc(1, sizeof(Node));
				q->pre = nownode;
				q->next = nownode->next;
				if(nownode->next != NULL) nownode->next->pre = q;
				nownode->next = q;
				nownode= q;
				nownode->c[0]= s[i];
				flagChinese = 1;
			}
		}
 	}
 	free(s);
}
//剪切 
void ClipContent()
{
	if((GetRange())==-1) return ;
	int length= 0;
	Operatorid++;
	LINE *p= RangeStartLine;
	Node *q= RangeStartNode;
	q = q->next;
	
	// 统计字符串的长度 
	while(p){
		while(q && q != RangeEndNode){
			length+= strlen(q->c);
			q= q->next;
		}
		length += 2;
		if(q== RangeEndNode) break;
		else{
			p= p->next;
			q= p->head->next;
		}
	}
//	printf("aaaaaaa\n");
	// 将范围内的数据写入字符串中 
	char *s= calloc(length * 2 + 3, sizeof(char));
	p= RangeStartLine;
	q= RangeStartNode->next;
	while(p){
		while(q && q != RangeEndNode){
			strcat(s, q->c);
			q= q->next;
		}
		if(q== RangeEndNode){
			strcat(s, q->c);
			break;
		}
		else{
			s[strlen(s)]= '\n';
			p= p->next;
			q= p->head->next;
		}
	}
	savepos();
	DeleteRange();
	// 将字符串写到内存区域中 
	OpenClipboard(NULL);     // 打开剪贴板 
	EmptyClipboard();        // 清空剪贴板
	HGLOBAL hMem= GlobalAlloc(GMEM_MOVEABLE, (strlen(s)+1)*sizeof(char));    // 分配内存区域 
	LPSTR pMem= GlobalLock(hMem);      // 锁定内存区域 
	memcpy(pMem, s, (strlen(s))*sizeof(char)); // 复制字符串到内存区域中 
	pMem[strlen(s)]= '\0';            // 设置结束符 
	GlobalUnlock(hMem);                   // 释放锁
	SetClipboardData(CF_TEXT, hMem);      // 将内存中的数据复制到剪贴板上
	CloseClipboard();                     // 关闭剪贴板 
	free(s);
}

static int issym(char c){			//判断是否为字符 
	char s[]=",./<>?;:[{]}|+=-)(*&^%!~";
	for(int i = 0; i < strlen(s); i++){
		if(s[i] == c) return 1;
	}
	return 0;
}

matchNode *matchStack;			//一个用于括号匹配的栈 

void CalculateText(){				//Calculate中完成对颜色的渲染与括号匹配 
	int trow = 1, tcol = 0;
	LINE *tline = head;
	//自动纠正程序中可能出现的错误 ，增加程序稳定性 
	while(tline != NULL){
		tcol = 0;
		if(tline == displayhead && uprow != trow) {
			uprow = trow; 
		}
		Node *tnode = tline->head;
		tnode->line = tline;
		if(tline->next != NULL && tline->next->pre != tline) tline->next->pre = tline;
		tnode = tnode->next;
		while(tnode != NULL){
			tnode->line = tline;
			if(ischosen(trow, tcol)) tnode->ischoose = 1; else tnode->ischoose = 0;
			tcol += strlen(tnode->c);
			if(tnode == nownode){
				if(tcol != col || trow != row) col = tcol, row = trow;
			}
			if(tnode->next != NULL &&tnode->next->pre != tnode) tnode->next->pre = tnode;
			tnode = tnode->next;
		}
		tline->chnum = tcol;
		trow++;
		tline = tline->next;
	}
	linenum = trow - 1;
	//括号匹配与颜色渲染 
	trow = 1, tcol = 0;
	tline = head;
	int iscom = 0; 
	theother = NULL;
	while(tline != NULL){
		int isletter = 0, isnum = 0;
		Node *tnode = tline->head->next;
		
		if(tnode == NULL){
			tline = tline->next;
			trow++;
			continue;
		}
		if(tnode->c[0] == '#'){				//如果改行开头是# 
			while(tnode != NULL){
				tnode->iscom = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
				tnode->ispound = 1;
				tnode = tnode->next;
			}
			tline = tline->next;
			trow++;
			continue;
		}
		tcol = strlen(tnode->c);
		while(tnode != NULL){
			if(iscom){ 		//处理注释/**/ 
				isletter = isnum = 0;
				while(tnode != NULL){
					tnode->ispound = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
					tnode->iscom = 1;
					if(tnode->c[0] == '*' && tnode->next != NULL &&tnode->next->c[0] == '/'){
						tnode = tnode->next;
						tcol += strlen(tnode->c);
						tnode->ispound = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
						tnode->iscom = 1;
						tnode = tnode->next;
						if(tnode != NULL) tcol += strlen(tnode->c);
						iscom = 0;
						break;
					}
					tnode = tnode->next;
					if(tnode != NULL) tcol += strlen(tnode->c);
				}
			}
			else if(tnode->c[0] == '/' && tnode->next != NULL && tnode->next->c[0] =='*'){
				isletter = isnum = 0;
				iscom = 1;
				tnode->ispound = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
				tnode->iscom = 1;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
				tnode->ispound = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
				tnode->iscom = 1;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
			else if(tnode->c[0] == '/' && tnode->next != NULL && tnode->next->c[0] == '/'){		//处理//型注释 
				isletter = isnum = 0;
				while(tnode != NULL){
					tnode->ispound = tnode->isbold = tnode->ischar = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
					tnode->iscom = 1;
					tnode = tnode->next;
					if(tnode != NULL) tcol += strlen(tnode->c);
				}
				break;
			}
			else if(tnode->c[0] == '\''){		//处理单引号与字符 
				isletter = isnum = 0;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
				tnode->ischar = 1;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
				while(tnode != NULL){
					tnode->ispound = tnode->isbold = tnode->iscom = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
					tnode->ischar = 1;
					if(tnode->c[0] == '\\'){
						tnode = tnode->next;
						if(tnode != NULL) tcol += strlen(tnode->c);
						tnode->ispound = tnode->isbold = tnode->iscom = tnode->isstr = tnode->iskey = tnode->issym = tnode->isnum = 0;
						tnode->ischar = 1;
						if(tnode != NULL) {
							tnode = tnode->next;
							if(tnode != NULL)tcol += strlen(tnode->c);
						}
						continue;
					}
					else if(tnode->c[0] == '\''){
						tnode = tnode->next;
						if(tnode != NULL) tcol += strlen(tnode->c);
						break;
					}
					tnode = tnode->next;
					if(tnode != NULL) tcol += strlen(tnode->c);
				}
			}
			else if(tnode->c[0] == '"'){		//处理双引号与字符串 
				isletter = isnum = 0;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->issym = tnode->isnum = 0;
				tnode->isstr = 1;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
				while(tnode != NULL){
					tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->issym = tnode->isnum = 0;
					tnode->isstr = 1;
					if(tnode->c[0] == '\\'){
						tnode = tnode->next;
						if(tnode != NULL) tcol += strlen(tnode->c);
						tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->issym = tnode->isnum = 0;
						tnode->isstr = 1;
						if(tnode != NULL){
							tnode = tnode->next;
							if(tnode != NULL) tcol += strlen(tnode->c);
						}
						continue;
					}
					else if(tnode->c[0] == '"'){
						tnode = tnode->next;
						if(tnode != NULL) tcol += strlen(tnode->c);
						break;
					}
					tnode = tnode->next;
					if(tnode != NULL) tcol += strlen(tnode->c);
				}
			}
			else if(issym(tnode->c[0])){		//符号 
				isletter = isnum = 0;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->isnum = 0;
				tnode->issym = 1;
				//使用栈进行括号匹配 
				if(nownode->c[0] == '('){
					tnode->issym = 2;
					if(tnode->c[0] == ')' )
						if(matchStack->next != NULL){
							if(matchStack->next->node == nownode) theother = tnode;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '('){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				if(nownode->c[0] == '['){
					tnode->issym = 2;
					if(tnode->c[0] == ']' )
						if(matchStack->next != NULL){
							if(matchStack->next->node == nownode) theother = tnode;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '['){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				if(nownode->c[0] == '{'){
					tnode->issym = 2;
					if(tnode->c[0] == '}' )
						if(matchStack->next != NULL){
							if(matchStack->next->node == nownode) theother = tnode;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '{'){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				if(nownode->c[0] == ')'){
					tnode->issym = 2;
					if(tnode->c[0] == ')' )
						if(matchStack->next != NULL){
							if(tnode == nownode) theother = matchStack->next->node;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '('){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				if(nownode->c[0] == ']'){
					tnode->issym = 2;
					if(tnode->c[0] == ']' )
						if(matchStack->next != NULL){
							if(tnode == nownode) theother = matchStack->next->node;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '['){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				if(nownode->c[0] == '}'){
					tnode->issym = 2;
					if(tnode->c[0] == '}' )
						if(matchStack->next != NULL){
							if(tnode == nownode) theother = matchStack->next->node;
							matchNode *p = matchStack->next;
							matchStack->next = p->next;
							free(p);
						}
					if(tnode->c[0] == '{'){
						matchNode *p = (matchNode*)calloc(1,sizeof(matchNode));
						p->node = tnode;
						p->next = matchStack->next;
						matchStack->next = p;
					}
				}
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
			else if(tnode->c[0] >= '0' && tnode->c[0] <= '9'){			//数字 
				isletter = 0;
				isnum = 1;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = 0;
				tnode->isnum = 1;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
			else if(tnode->c[0] == ' '){			//空格或者tab 
				isletter = isnum = 0;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = tnode->isnum = 0;
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
			else if(!isletter && !isnum){		//C语言保留字 
				isnum = 0;
				isletter = 1;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = tnode->isnum = 0;	
				char *s[] = {"int", "float", "double", "long", "short", "char", "enum", "struct", "typedef", "return", "if", "else"
				            ,"signed", "union", "void", "for", "do", "while", "break", "continue", "goto", "switch", "case", "default",
							"auto", "extern", "register", "static", "const", "sizeof", "volatile"};
				for(int i = 0; i < sizeof(s)/sizeof(char*); i++){
					int len = strlen(s[i]);
					Node *q = tnode;
					int flag = 0;
					for(int j = 0; j < len; j++, q = q->next){
						if(q == NULL || q->c[0] != s[i][j]){
							flag = 1;
							break;
						}
					}
					if(flag || 
					(q != NULL && q->c[0] >= 'a' && q->c[0] <= 'z') ||
					(q != NULL && q->c[0] >= 'A' && q->c[0] <= 'Z')) continue;
					
					q = tnode->next;
					tnode->ispound = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = tnode->isnum = 0;
					tnode->isbold = 1;
					for(int j = 1; j < len; j++, q = q->next){
						tnode = q;
						if(tnode != NULL) tcol += strlen(tnode->c);
						tnode->ispound = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = tnode->isnum = 0;
						tnode->isbold = 1;
					}
					break;
				}
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
			else{
				isnum = 0;
				isletter = 1;
				tnode->ispound = tnode->isbold = tnode->iscom = tnode->ischar = tnode->iskey = tnode->isstr = tnode->issym = tnode->isnum = 0;	
				tnode = tnode->next;
				if(tnode != NULL) tcol += strlen(tnode->c);
			}
		}
		tline = tline->next;
		trow++;
	}
	while(matchStack->next) {			//清空栈 
		matchNode *p = matchStack->next;
		matchStack->next = p->next;
		free(p);
	}
}


void setsize(){		
	//利用快捷键设置字号 
	char * menuListFile[]={"","| Ctrl-=","| Ctrl--"};
	int select =menuList(GenUIID(0), -100, -100, 0, 0, 0, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if(select == 1){
		if(PointSize <= 35) PointSize += 5 ;
	}
	else if(select == 2){
		if(PointSize >= 15) PointSize -= 5;
	}
}

int getreturn(int key, int event){	
	//处理回车（注意缩进） 
	if(key == VK_RETURN && event == KEY_DOWN){
		
		Operatorid++;savepos();
		if(ChooseRange){
			DeleteRange();
		}
		CursorExist = 1;
		LINE *p = calloc(1, sizeof(LINE));
		linenum++;
		int blanknum = 0;
		//计算当前行首空格数，用于自动缩进 
		for(Node *i = nowline->head->next; i != NULL && i->c[0] == ' '; i = i->next){
			blanknum += strlen(i->c);
		}
		p->pre  = nowline;
		p->next = nowline->next;
		if(nowline->next != NULL) {
			saveline(nowline->next,nowline->next->next,nowline->next->pre,Operatorid);
			nowline->next->pre = p;
		}
		saveline(nowline,nowline->next,nowline->pre,Operatorid);
		nowline->next = p;
		nowline = p;
		Node *q = calloc(1, sizeof(Node));
		nowline->head = q;
		q->line = nowline;
		
		nowline->chnum = nowline->pre->chnum - col; 
		nowline->pre->chnum = col;
		if(nownode->next != NULL) {
			savenode(nownode->next,nownode->next->next,nownode->next->pre,Operatorid);
			q->next = nownode->next;
			nownode->next->pre = q;
		}
		savenode(nownode, nownode->next, nownode->pre, Operatorid);
		nownode->next = NULL;
		nownode = q;
		row ++; col = 0;
		//自动缩进 
		for(int i = 0; i < blanknum / 4; i++){
			q = calloc(1, sizeof(Node));
			q->next = nownode->next;
			q->pre = nownode;
			q->c[0] = q->c[1] = q->c[2] = q->c[3] = ' ';
			if(nownode->next != NULL) nownode->next->pre = q;
			nownode->next = q;
			nownode = q;
			col += 4;
		}
		for(int i = 0; i < blanknum % 4; i++){
			q = calloc(1, sizeof(Node));
			q->next = nownode->next;
			q->pre = nownode;
			q->c[0] = ' ';
			if(nownode->next != NULL) nownode->next->pre = q;
			nownode->next = q;
			nownode = q;
			col++;
		}
		return 1;
	}
	return 0;


}

Node* gettail(LINE *p){		
	//辅助函数，找到当前行的末尾 
	Node* q = p->head;
	while(q->next != NULL) q = q->next;
	return q;
}

int getdirect(int key, int event){	
	//处理方向键，通过方向键移动光标 
	if(event == KEY_UP) return 0; 
	CalculateText();
	CursorExist = 1;
	if(key == VK_UP){
		ClickEdit = ChooseRange = 0;
		if(row == 1) return 1;
		nowline = nowline->pre;
		row--;
		col = min(col, nowline->chnum);
		nownode = nowline->head;
		int i;
		for(i = 0; i < col ;i+=strlen(nownode->c)) nownode = nownode->next;
		col = i;
		return 1;
	}
	if(key == VK_DOWN){
		ClickEdit = ChooseRange = 0;
		if(row == linenum) return 2;
		nowline = nowline->next;
		row++;
		col = min(col, nowline->chnum);
		nownode = nowline->head;
		int i;
		for(i = 0; i < col ;i+=strlen(nownode->c)) nownode = nownode->next;
		col = i;
		return 2;
	}
	if(key == VK_LEFT){
		ClickEdit = ChooseRange = 0;
		if(row == 1 && !nownode->c[0]) return 3;
		if(!nownode->c[0]) {
			col = nowline->pre->chnum;
			row--;
			nownode = gettail(nowline->pre);
			nowline = nowline->pre;
		}
		else {
			col-=strlen(nownode->c);
			nownode = nownode->pre;
		}
		return 3;
	}
	if(key == VK_RIGHT){
		ClickEdit = ChooseRange = 0;
		if(row == linenum && nownode->next == NULL) return 4;
		if(nownode->next == NULL) {
			col = 0;
			row++;
			nownode = nowline->next->head;
			nowline = nowline->next;
		}
		else {
			col += strlen(nownode->next->c);
			nownode = nownode->next;
		}
		return 4;
	}
}

int getback(int key, int event){			//处理回退键 
	if(event == KEY_UP || key != VK_BACK) return 0;
	CursorExist = 1;
	Operatorid++;
	savepos();
	if(ChooseRange){
		DeleteRange();
		return 1;
	}
	if(row == 1 && !nownode->c[0]) return 1;
	if(!nownode->c[0]) {			
		//在行首按下回退键 
		linenum--;
		Node *tail = gettail(nowline->pre);
		savenode(tail,tail->next,tail->pre,Operatorid);
		tail->next = nownode->next;
		if(nownode->next != NULL) {
			savenode(nownode->next, nownode->next->next, nownode->next->pre, Operatorid);
			nownode->next->pre = tail;
		}
		savenode(nownode, nownode->next, nownode->pre, Operatorid);
		nownode->next = NULL;
		saveline(nowline->pre, nowline->pre->next, nowline->pre->pre, Operatorid);
		nowline->pre->next = nowline->next;
		if(nowline->next != NULL) {
			saveline(nowline->next, nowline->next->next, nowline->next->pre, Operatorid);
			nowline->next->pre = nowline->pre;
		}
		col = nowline->pre->chnum;
		row--;
		nowline->pre->chnum += nowline->chnum;
		nownode = tail;
		nowline = nowline->pre;
	}
	else {					
		//非行首 
		col-=strlen(nownode->c);
		nowline->chnum -= strlen(nownode->c);
		savenode(nownode->pre, nownode->pre->next, nownode->pre->pre, Operatorid);
		nownode->pre->next = nownode->next;
		if(nownode->next != NULL) {
			savenode(nownode->next, nownode->next->next, nownode->next->pre, Operatorid);
			nownode->next->pre = nownode->pre;
		}
		nownode = nownode->pre;
	}
	return 1;
}

//处理delete键 
int getdel(int key, int event){	
	if(event == KEY_UP || key != VK_DELETE) return 0;
	CursorExist = 1;
	Operatorid++;
	savepos();
	if(ChooseRange){
		DeleteRange();
		return 1;
	}
	if(nowline->next == NULL && nownode->next == NULL) return 1;
	if(nownode->next == NULL) {				
		//在行末按下del 
		linenum--;
		savenode(nownode,nownode->next,nownode->pre,Operatorid);
		nownode->next = nowline->next->head->next;
		Node *q = nowline->next->head->next;
		if(q != NULL) {
			savenode(q, q->next, q->pre, Operatorid);
			q->pre = nownode;
		}
		if(nowline->next->next != NULL) {
			saveline(nowline->next->next, nowline->next->next->next, nowline->next->next->pre, Operatorid);
			nowline->next->next->pre = nowline;
		}
		saveline(nowline, nowline->next, nowline->pre, Operatorid);
		nowline->chnum += nowline->next->chnum;
		nowline->next = nowline->next->next;
		
	}
	else {			
		//非行末 
		nowline->chnum -= strlen(nownode->next->c);
		savenode(nownode, nownode->next, nownode->pre, Operatorid);
		if(nownode->next->next != NULL) {
			savenode(nownode->next->next, nownode->next->next->next, nownode->next->next->pre, Operatorid);
			nownode->next->next->pre = nownode;
		}
		nownode->next = nownode->next->next;
	}
	return 1;
}


static double Cursorw, Cursorh, Cursordw, Cursordh;		//光标的信息 
void displayEdit(){		
	//绘制编辑区 
	SetPointSize(PointSize);
	SetFont("Consolas");
	CalculateText();
	modifylocate();
	CalculateText();
	SetPenColor("White");
	double height = GetWindowHeight();
	drawRectangle(leftw,0,10,texth,1);
	SetPenColor("Black");
	MovePen(leftw,texth - GetFontHeight());
	
	double x = GetCurrentX(), x0 = x;
	double y = GetCurrentY(), y0 = y;
	const double deltax = TextStringWidth("A"), deltay = GetFontHeight();
	//编辑区的绘制 
	int i = uprow;
	int flag = 0;
	for(LINE *p = displayhead; p != NULL && y >= -1; p = p->next, x = x0, y -= deltay ,i++){
		int j = 0;
		if(p == nowline){
			SetPenColor("Highlight");
			drawRectangle(leftw, y - GetFontDescent(), 10, deltay, 1);
		}
		Node *q = p->head;
		while(q->next!=NULL && j + strlen(q->next->c) <= leftcol){
			j += strlen(q->next->c);
			q  = q->next;
		}
		if(q->next != NULL && j != leftcol){
			if(q->ischoose){
				SetPenColor("Blue");
				drawRectangle(x0 - deltax,y - GetFontDescent(),deltax*strlen(q->c),deltay,1);
				SetPenColor("White");
			}
			else if(q == theother){
				SetPenColor("Red");
				drawRectangle(x0 - deltax,y - GetFontDescent(),deltax*strlen(q->c),deltay,1);
				SetPenColor("White");
			}
			else if(q->issym) SetPenColor("Red");
			else if(q->iscom) SetPenColor("Comment Blue");
			else if(q->ispound) SetPenColor("Header File Green");
			else if(q->isstr) SetPenColor("Blue");
			else if(q->isnum) SetPenColor("Purple");
			else SetPenColor("Black");
			if(q->iscom) SetStyle(Italic);
			else if(q->isbold || q->isstr) SetStyle(Bold);
			else SetStyle(Normal);
			MovePen(x0 - deltax, y);
			x = x0 + deltax;
			j += strlen(q->next->c);
			q = q->next;
			DrawTextString(q->c);
		}
		if(q != NULL) q = q->next;
		for(; q != NULL && x <= textw + 0.5;x = x + deltax*strlen(q->c),j+=(int)strlen(q->c), q = q->next){
			if(q->ischoose){
				SetPenColor("Blue");
				drawRectangle(x,y - GetFontDescent(),deltax*strlen(q->c),deltay,1);
				SetPenColor("White");
			}
			else if(q == theother || (q->issym == 2 && q == nownode)){
				SetPenColor("Red");
				drawRectangle(x,y - GetFontDescent(),deltax*strlen(q->c),deltay,1);
				SetPenColor("White");
			}
			else if(q->issym) SetPenColor("Red");
			else if(q->iscom) SetPenColor("Comment Blue");
			else if(q->ispound) SetPenColor("Header File Green");
			else if(q->isstr) SetPenColor("Blue");
			else if(q->isnum) SetPenColor("Purple");
			else SetPenColor("Black");
			if(q->iscom) SetStyle(Italic);
			else if(q->isbold || q->isstr) SetStyle(Bold);
			else SetStyle(Normal);
			MovePen(x, y);
			DrawTextString(q->c);
		}
	} 
	SetStyle(Normal);
	SetPenColor("Black");
	MovePen(leftw,texth - GetFontHeight());
	x = GetCurrentX(), x0 = x;
	y = GetCurrentY(), y0 = y;
	i = uprow;
	flag = 0;
	//光标  
	for(LINE *p = displayhead; p != NULL && y >= -1; p = p->next, x = x0, y -= deltay ,i++){
		int j = 0;
		Node *q = p->head;
		while(q->next!=NULL && j + strlen(q->next->c) <= leftcol){
			j += strlen(q->next->c);
			q  = q->next;
		}
		if(q->next != NULL && j != leftcol){
			x = x0 + deltax;
			j += strlen(q->next->c);
			q = q->next;
		}
		if(q != NULL) q = q->next;
		for(; q != NULL && x <= textw + 0.5;x = x + deltax*strlen(q->c),j+=(int)strlen(q->c), q = q->next){
			if(row == i && col == j) {
				Cursorw = x, Cursorh = y - GetFontDescent(), Cursordw = 0, Cursordh = deltay;
				if(CursorExist){
					
					MovePen(Cursorw, Cursorh);
					DrawLine(Cursordw, Cursordh);
				}printf("mdmd%f,%f\n",x,y);
				flag = 1; 
			}
		}
		if(row == i && col >= j && !flag) {
			Cursorw = x, Cursorh = y - GetFontDescent(), Cursordw = 0, Cursordh = deltay;
			if(CursorExist){
				MovePen(Cursorw, Cursorh);
				DrawLine(Cursordw, Cursordh);
			}
		}
	} 
	//行号 
	SetPenColor("Button Gray");
	drawRectangle(0,0,leftw - 0.02,texth,1);
	MovePen(0.02,texth - GetFontHeight());
	x = GetCurrentX(), x0 = x;
	y = GetCurrentY(), y0 = y;
	i = uprow;
	SetPenColor("Black");
	for(LINE *p = displayhead; p != NULL && y >= -1; p = p->next, x = 0.02, y -= deltay ,i++){
		char tmp[30];
		sprintf(tmp,"%d",i);
		MovePen(x,y);
		DrawTextString(tmp);
	}
	
	
	SetPointSize(15);
//	if(issleep) Sleep(40);
}

void initEdit(){			//编辑区初始化 
	SetPointSize(15);
	PointSize = 15;
	textw = 9.5, texth = 6.68;
	Operatorid = 3;
	col = leftcol = 0;
	row = uprow = linenum = 1;
	ChooseRange = ClickEdit = 0;
	CursorExist = 1;
	LINE *p = calloc(1, sizeof(LINE));
	nowline = head = displayhead = p;
	Node *q = calloc(1, sizeof(Node));
	nowline->head = q;
	q->line = nowline;
	nownode = q;
	Cursorw = leftw, Cursorh = texth - GetFontHeight() - GetFontDescent(), Cursordw = 0, Cursordh = 0;
	CancelNodeStack = (RENode*)calloc(1, sizeof(RENode));
	CancelLineStack = (RELINE*)calloc(1, sizeof(RELINE));
	CancelposStack = (REpos*)calloc(1, sizeof(REpos));
	RemakeNodeStack = (RENode*)calloc(1, sizeof(RENode));
	RemakeLineStack = (RELINE*)calloc(1, sizeof(RELINE));
	RemakeposStack = (REpos*)calloc(1, sizeof(REpos));
	matchStack = (matchNode*)calloc(1, sizeof(matchNode));
} 

void editMouseEvent (int x, int y, int button, int event)	
//处理鼠标事件 
{
	
	double mouse_x = ScaleXInches(x);
	double mouse_y = ScaleYInches(y);
	if(!(mouse_x >= leftw && mouse_y <= texth)) return;
	if(button == LEFT_BUTTON && event == BUTTON_DOWN && GetNewCursor(mouse_x, mouse_y)){
		ClickEdit = 1;
		lastcol = col;
		lastrow = row;
		lastline = nowline;
		lastnode = nownode;
		ChooseRange = 0; 
	}
	if(ClickEdit && !(button == LEFT_BUTTON && event == BUTTON_UP)){
		GetNewCursor(mouse_x, mouse_y); 
		if(col != lastcol || row != lastrow) ChooseRange = 1;
	}
	if(button == LEFT_BUTTON && event == BUTTON_UP){
		ClickEdit = 0;
	}
}

void editchar(char ch){			//处理字符事件 
	static int oneChinese = 0;
	if(ch!= '\t' && ch >0 && ch <= 31) {
		display();
		return;
	}
	if(!oneChinese) {
		Operatorid++;
		savepos();
	}
	if(ChooseRange) DeleteRange();
	CursorExist = 1;
	if(ch == '\t'){			//输入一个Tab转换成四个空格 
		nowline->chnum += 4;
		col += 4;
		Node *p = calloc(1, sizeof(Node));
		p->pre  = nownode;
		if(nownode->next != NULL) {
			savenode(nownode->next,nownode->next->next,nownode->next->pre,Operatorid);
			nownode->next->pre = p;
		}
		p->next = nownode->next;
		savenode(nownode,nownode->next,nownode->pre,Operatorid);
		nownode->next = p;
		nownode = p;
		p->c[0] = p->c[1] = p->c[2] = p->c[3] = ' ';
		display();
		return;
	}
	nowline->chnum++;
	col++;
	if(oneChinese){
		oneChinese ^= 1;
		nownode->c[1] = ch;
		display();
		return;
	}
	Node *p = calloc(1, sizeof(Node));
	p->pre  = nownode;
	if(nownode->next != NULL) {
		savenode(nownode->next,nownode->next->next,nownode->next->pre,Operatorid);
		nownode->next->pre = p;
	}
	p->next = nownode->next;
	savenode(nownode,nownode->next,nownode->pre,Operatorid);
	nownode->next = p;
	nownode = p;
	p->c[0] = ch;
	if(ch > 0) display(); else oneChinese^=1;
}

void editkey(int key, int event){
	setsize();
	getreturn(key, event);
	getdirect(key, event);
	getback(key,event);
	getdel(key, event);
}

void editTimer(int timerID){		//处理光标闪烁，画光标 
	CursorExist ^= 1;
	if(CursorExist == 0){
		SetPenColor("Highlight");
		MovePen(Cursorw, Cursorh);
		DrawLine(Cursordw, Cursordh);
		SetPenColor("Black");
	}
	else{
		SetPenColor("Black");
		MovePen(Cursorw, Cursorh);
		DrawLine(Cursordw, Cursordh);
	}
}


