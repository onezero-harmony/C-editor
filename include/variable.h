#ifndef variable
#define variable
#include "linkedlist.h"
#include "boolean.h"
#include "strlib.h"
//存储当前页类型 
enum page{ 
	EditPage,
	InstructionsPage,
	AboutPage,
	SavePage
};

//一个LINE变量表示一行 
typedef struct LINE {
	int chnum;
	struct LINE *next, *pre;
	struct Node *head;
} LINE;

//一个Node变量表示一个字符 
typedef struct Node {
	char c[5];
	int ischoose, isbold, isstr, ischar, iscom, iskey, issym, isnum, ispound;
	struct Node *next, *pre;
	struct LINE *line; 
} Node;

//用于撤销操作的链表 
typedef struct RENode{
	Node *node,*nodenext,*nodepre;
	int id;
	struct RENode *next;
} RENode;
 
//用于撤销操作的链表 
typedef struct RELINE{
	LINE *node,*nodenext,*nodepre;
	int id;
	struct RELINE *next;
} RELINE;
//用于括号匹配的链表 
typedef struct matchNode{
	Node *node;
	struct matchNode *next;
} matchNode;
//用于撤销操作的链表 
typedef struct REpos{
	int trow, tcol, tlinenum, id;
	Node *tnode;
	LINE *tline;
	struct REpos *next;
} REpos;

extern OPENFILENAME ofn;

extern LINE *nowline, *head;
extern int openSuccess;
extern int ChooseRange, ClickEdit;
extern Node *nownode;
extern int CursorExist, linenum;
extern int isShowRightMenu;
extern int isSaved;
extern int isCreated;
extern int isShowEdit;
extern enum page CurrentPage;
extern double textw, texth, leftw; 

void myMouseEvent (int x, int y, int button, int event);
void mykey(int key, int event);
void mychar(char ch);
void myTimer(int timerID); 


//修改当前显示范围 
void modifylocate();
//获取光标位置 
int GetNewCursor(double x, double y);
//设置字号 
void setsize();
//显示编辑区 
void displayEdit();
//初始化编辑区 
void initEdit();
//编辑区的各类event 
void editMouseEvent (int x, int y, int button, int event);
void editchar(char ch);
void editkey(int key, int event);
void editTimer(int timerID);
//渲染编辑区 
void CalculateText();
//撤销 
void CANCEL();
//重做 
void REMAKE();
//复制 
void CopyContent();
//粘贴 
void PasteContent();
//剪贴 
void ClipContent();



//文件操作 
void readFile();
void saveFile();
void saveAsFile();
void InitCeditor();
void createFile();
void exitRemind();
char *getCurrentFileName();
int getisSaved();
void displayFileName();
void initfile();
int compileFile();
void runFile();

//显示函数 
void SetRightMenuXY(double x, double y);
void displayMenu();
void displayAboutPage();
void displayInstructionsPage();
void displayRightMenu();
void displayEditPage();
void display();
#endif
