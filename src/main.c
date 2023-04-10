#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include<string.h>

int oneChinese;


typedef struct LINE {
	int chnum;
	struct LINE *next, *pre;
	struct Node *head;
} LINE;

typedef struct Node {
	char c[3];
	struct Node *next, *pre;
	struct LINE *line; 
} Node;

int linenum = 1, col, row, CursorExist;
int uprow, leftcol;
LINE *nowline, *head;
Node *nownode;
const double textw = 9.5, texth = 6.5; 
//#define max(a,b) ((a)>(b)?(a):(b))
//#define min(a,b) ((a)<(b)?(a):(b))

void modifylocate(){
	double h = GetFontHeight(), w = TextStringWidth("A");
	int x = (int)(textw / w - 0.00001) - 1, y = (int)(texth / h - 0.00001) - 1;
	x = max(x, 0);
	y = max(y, 0);
	if(row < uprow || row > uprow + y - 1){
		if(row < uprow){
			for(int i = 0; i < uprow - row; i++) head = head->pre;
			uprow = row;
		}
		else{
			for(int i = 0; i < -uprow - y + 1 + row; i++) head = head->next;
			uprow = row - y + 1;
		}
	}
	leftcol = max(0, col - x);
}

int GetNewCursor(double x, double y){
	if(!(x >= 0.5 && x <= 0.5 + textw && y >= 0 && y<=texth)) return 0;
	CursorExist = 1;
	double h = GetFontHeight(), w = TextStringWidth("A");
	int xnum = (int)((x-0.5) / w + 0.48), ynum = (int)((texth - y) / h);
	int flag = (ynum > (linenum - uprow));
	ynum = min(ynum, linenum - uprow);
	row = uprow + ynum;
	nowline = head;
	for(int i = 0; i < ynum; i++) nowline = nowline->next;
	xnum = min(xnum + leftcol, nowline->chnum);
	if(flag) xnum = nowline->chnum;
	col = xnum;
	nownode = nowline->head;
	int i;
	for(i = 0; i < xnum ;i += strlen(nownode->c)) nownode = nownode->next;
	col = i;
	modifylocate();
	return 1;
} 

void display()
{
/*	DisplayClear();
	int height = GetWindowHeight();
	SetPointSize(20);
	SetFont("Arial Black");
	SetStyle(Bold);
	printf("%lf %lf %s %d\n",GetWindowHeight(),GetWindowWidth(),GetFont(),GetPointSize());
	char * menuListFile[] = {"File",  
		"Open  | Ctrl-O",  // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"Close",           
		"Exit   | Ctrl-E"};// 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾

	int selection;

	selection = menuList(GenUIID(0), 0, height-0.2, 0.5, 1, 0.2, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));

	if( selection==3 )
		exit(-1); // user choose to exit
	double w = 1.0;
	double h = GetFontHeight() * 2;
	// 清除屏幕
	//draw a rect/box to trace the mouse
	drawRectangle(mouse_x, mouse_y, w, h, 0);
	SetPenColor("Blue");
	drawBox(mouse_x, mouse_y - 1, w, h, 1, "This box follows the mouse", 'L', "Red");
	// draw a square
	SetPenColor("Red");
	drawLabel(label_x, label_y, "Lable is Here");
*/
	SetFont("Consolas");
	SetPenColor("White");
	double height = GetWindowHeight();
	drawRectangle(0.5,0,10,6.5,1);
	SetPenColor("Black");
	MovePen(0.5,6.5 - GetFontHeight());
	
	double x = GetCurrentX(), x0 = x;
	double y = GetCurrentY(), y0 = y;
	double deltax = TextStringWidth("A"), deltay = GetFontHeight();
	modifylocate();
	int i = uprow;
	int flag = 0;
	for(LINE *p = head; p != NULL && y >= -1; p = p->next, x = x0, y -= deltay ,i++){
		int j = 0;
		Node *q = p->head;
		while(q->next!=NULL && j + strlen(q->next->c) <= leftcol){
			j += strlen(q->next->c);
			q  = q->next;
		}
		if(q->next != NULL && j != leftcol){
			MovePen(x0 - deltax, y);
			x = x0 + deltax;
			j += strlen(q->next->c);
			q = q->next;
			DrawTextString(q->c);
		}
		if(q != NULL) q = q->next;
		for(; q != NULL && x <= textw + 0.5;x = x + deltax*strlen(q->c),j+=(int)strlen(q->c), q = q->next){
			if(row == i && col == j &&CursorExist) {
				MovePen(x, y - GetFontDescent());
				flag = 1; 
				DrawLine(0 , deltay);
			}
			MovePen(x, y);
			DrawTextString(q->c);
		}
		if(row == i && col >= j && CursorExist) {
			MovePen(x, y - GetFontDescent());
			DrawLine(0 , deltay);
		}
	} 
}

void myMouseEvent (int x, int y, int button, int event)
{
	DisplayClear();
	uiGetMouse(x,y,button,event);
	double mouse_x = ScaleXInches(x);
	double mouse_y = ScaleXInches(y);
	mouse_y = 7.0 - mouse_y;
	if(button == LEFT_BUTTON && event == BUTTON_DOWN){
		GetNewCursor(mouse_x, mouse_y); 
	}
	display();
}
void mychar(char ch){
	uiGetChar(ch);
	DisplayClear();
	if(ch >0 && ch <= 31) {
		display();
		return;
	}
	CursorExist = 1;
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
	if(nownode->next != NULL) nownode->next->pre = p;
	p->next = nownode->next;
	nownode->next = p;
	nownode = p;
	p->c[0] = ch;
	if(ch > 0) display(); else oneChinese^=1;
}

void setsize(){
	char * menuListFile[]={"","| Ctrl-=","| Ctrl--"};
	int select =menuList(GenUIID(0), -100, -100, 0, 0, 0, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if(select == 1){
		if(GetPointSize()<=35)SetPointSize(GetPointSize()+5);
	}
	else if(select == 2){
		if(GetPointSize()>=15)SetPointSize(GetPointSize()-5);
	}
}

int getreturn(int key, int event){
	if(key == VK_RETURN && event == KEY_DOWN){
		CursorExist = 1;
		LINE *p = calloc(1, sizeof(LINE));
		linenum++;
		p->pre  = nowline;
		p->next = nowline->next;
		if(nowline->next != NULL) nowline->next->pre = p;
		nowline->next = p;
		nowline = p;
		Node *q = calloc(1, sizeof(Node));
		nowline->head = q;
		q->line = nowline;
		
		nowline->chnum = nowline->pre->chnum - col; 
		nowline->pre->chnum = col;
		if(nownode->next != NULL) {
			q->next = nownode->next;
			nownode->next->pre = q;
		}
		nownode->next = NULL;
		nownode = q;
		row ++; col = 0;
		return 1;
	}
	return 0;

}

Node* gettail(LINE *p){
	Node* q = p->head;
	while(q->next != NULL) q = q->next;
	return q;
}

int getdirect(int key, int event){
	if(event == KEY_UP) return 0; 
	CursorExist = 1;
	if(key == VK_UP){
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

int getback(int key, int event){
	if(event == KEY_UP || key != VK_BACK) return 0;
	CursorExist = 1;
	if(row == 1 && !nownode->c[0]) return 1;
	if(!nownode->c[0]) {
		linenum--;
		Node *tail = gettail(nowline->pre);
		tail->next = nownode->next;
		if(nownode->next != NULL) nownode->next->pre = tail;
		nownode->next = NULL;
		nowline->pre->next = nowline->next;
		if(nowline->next != NULL) nowline->next->pre = nowline->pre;
		col = nowline->pre->chnum;
		row--;
		nowline->pre->chnum += nowline->chnum;
		nownode = tail;
		nowline = nowline->pre;
	}
	else {
		col-=strlen(nownode->c);
		nownode->pre->next = nownode->next;
		if(nownode->next != NULL) nownode->next->pre = nownode->pre;
		nownode = nownode->pre;
	}
	return 1;
}

void mykey(int key, int event){
	DisplayClear();
	uiGetKeyboard(key,event);
	setsize();
	getreturn(key, event);
	getdirect(key, event);
	getback(key,event);
	display();
}

void myTimer(int timerID){
	CursorExist ^= 1;
	DisplayClear();
	display();
}
void Main()
{
	col = leftcol = 0;
	row =uprow = 1;
	CursorExist = 1;
//	InitConsole(); 
	SetWindowTitle("轻量级C语言编辑器");
	InitGraphics();
	InitGUI();
	
	registerMouseEvent(myMouseEvent);
	registerCharEvent(mychar);
	registerKeyboardEvent(mykey);
	registerTimerEvent(myTimer);
	startTimer(1,700); 
	
	LINE *p = calloc(1, sizeof(LINE));
	nowline = head = p;
	Node *q = calloc(1, sizeof(Node));
	nowline->head = q;
	q->line = nowline;
	nownode = q;
	display();
}
