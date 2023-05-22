#ifndef variable
#define variable
#include "linkedlist.h"
#include "boolean.h"
#include "strlib.h"
//�洢��ǰҳ���� 
enum page{ 
	EditPage,
	InstructionsPage,
	AboutPage,
	SavePage
};

//һ��LINE������ʾһ�� 
typedef struct LINE {
	int chnum;
	struct LINE *next, *pre;
	struct Node *head;
} LINE;

//һ��Node������ʾһ���ַ� 
typedef struct Node {
	char c[5];
	int ischoose, isbold, isstr, ischar, iscom, iskey, issym, isnum, ispound;
	struct Node *next, *pre;
	struct LINE *line; 
} Node;

//���ڳ������������� 
typedef struct RENode{
	Node *node,*nodenext,*nodepre;
	int id;
	struct RENode *next;
} RENode;
 
//���ڳ������������� 
typedef struct RELINE{
	LINE *node,*nodenext,*nodepre;
	int id;
	struct RELINE *next;
} RELINE;
//��������ƥ������� 
typedef struct matchNode{
	Node *node;
	struct matchNode *next;
} matchNode;
//���ڳ������������� 
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


//�޸ĵ�ǰ��ʾ��Χ 
void modifylocate();
//��ȡ���λ�� 
int GetNewCursor(double x, double y);
//�����ֺ� 
void setsize();
//��ʾ�༭�� 
void displayEdit();
//��ʼ���༭�� 
void initEdit();
//�༭���ĸ���event 
void editMouseEvent (int x, int y, int button, int event);
void editchar(char ch);
void editkey(int key, int event);
void editTimer(int timerID);
//��Ⱦ�༭�� 
void CalculateText();
//���� 
void CANCEL();
//���� 
void REMAKE();
//���� 
void CopyContent();
//ճ�� 
void PasteContent();
//���� 
void ClipContent();



//�ļ����� 
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

//��ʾ���� 
void SetRightMenuXY(double x, double y);
void displayMenu();
void displayAboutPage();
void displayInstructionsPage();
void displayRightMenu();
void displayEditPage();
void display();
#endif
