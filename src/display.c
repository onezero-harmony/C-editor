#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>

enum page CurrentPage;         // ��ǰҳ�� 
int isShowRightMenu;           // �Ƿ���ʾ�Ҽ��˵� 

double RightMenuX;			   // �Ҽ��˵�X���� 
double RightMenuY; 			   // �Ҽ��˵�Y���� 



/*
	�����Ҽ��˵����Ͻ����� 
*/

void SetRightMenuXY(double x, double y)
{
	RightMenuX= x;
	RightMenuY= y;
}


/*
	��ʾ�˵� 
*/

void displayMenu()
{
	static int flag= 0;
	static int editFlag= 0;
 	SetFont("Consolas");
	SetPenColor("Gray");
	
	char *LabelsFile[] = {"�ļ�",
        "�½�                       Ctrl-N",
        "��                       Ctrl-O",
        "����                       Ctrl-S",
        "���Ϊ                     Ctrl-P",
        "�ر�                       Ctrl-W",
        "�˳�                       Alt-F4"
    };
	
	double winWidth= GetWindowWidth();
	double x=0;
	double y= GetWindowHeight();
	double w= TextStringWidth(LabelsFile[0])*2.5;
	double wlist= TextStringWidth(LabelsFile[1])*1.1;
	double h= GetFontHeight()*1.5;
	drawRectangle(x, y-h, winWidth, h, 1);
	
	SetPenColor("Menu Gray");
	
	char *LabelsEdit[] = {"�༭",
        "�ָ�                       Ctrl-Z",
        "����                       Ctrl-Y",
        "����                       Ctrl-X",
        "����                       Ctrl-C",
        "ճ��                       Ctrl-V"
    };
	
	char *LabelsHelp[] = {"����",
        "����                       Ctrl-E",
        "ʹ��˵��                       F1"
    };
	
	char *LabelsCompile[] = {"����",
		"����                       F9",
		"����                       F10",
		"��������                   F11" 
	};
	 
	int FileMenu= menuList(GenUIID(0), x, y-h, w, wlist, h, LabelsFile, sizeof(LabelsFile)/sizeof(LabelsFile[0]));
	
	int EditMenu= menuList(GenUIID(0), x+w, y-h, w, wlist, h, LabelsEdit, sizeof(LabelsEdit)/sizeof(LabelsEdit[0]));
	
	int HelpMenu= menuList(GenUIID(0), x+w*2.0, y-h, w, wlist, h, LabelsHelp, sizeof(LabelsHelp)/sizeof(LabelsHelp[0]));
	
	int CompileMenu= menuList(GenUIID(0), x+w*3.0, y-h, w, wlist, h, LabelsCompile, sizeof(LabelsCompile)/sizeof(LabelsCompile[0]));
	
	// �ļ��˵� 
	
	switch(FileMenu){
		case 1:
			DisplayClear();display();createFile();DisplayClear();display();
			break;
		case 2:
			DisplayClear();display();readFile();DisplayClear();display();
			break;
		case 3:
			DisplayClear();display();saveFile();DisplayClear();display();
			break;
		case 4:
			DisplayClear();display();saveAsFile();DisplayClear();display();
			break;
		case 5:
			DisplayClear();
			display();
			if(!isSaved){
				char s[100];
				if(!isCreated) sprintf(s, "�Ƿ�Ҫ����� �ޱ��� ���޸ģ�");
				else sprintf(s, "�Ƿ�Ҫ����� %s ���޸�", ofn.lpstrFileTitle);
				int id= MessageBoxA(NULL, s, "������C����༭��", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL);
				switch(id){
					case IDYES:
						saveFile();
						if(!openSuccess) editFlag=1;       // ���û�гɹ���ȡ����·���ͼ����༭ 
						break;
					case IDNO:
						break;
					case IDCANCEL:
						flag= 1;
						break;
				}			
			}
			if(editFlag) break;
			if(flag){
				flag= 0;
				break;
			} 
			isShowEdit= 0;
			isSaved = 1; 
			DisplayClear();
			display();
			break;
		case 6:
			DisplayClear();display();exitRemind();DisplayClear();display();
			break;
	}
	
	// �༭�˵��� 
	
	switch(EditMenu){
		case 1:
			DisplayClear();display();CANCEL();DisplayClear();display();break;
		case 2:
			DisplayClear();display();REMAKE();DisplayClear();display();break;
		case 3:
			DisplayClear();display();ClipContent();DisplayClear();display();break;
		case 4:
			DisplayClear();display();CopyContent();DisplayClear();display();break;
	    case 5:
	    	DisplayClear();display();PasteContent();DisplayClear();display();break;
	}
	
	// �����˵��� 
	
	switch(HelpMenu){
		case 1:
			CurrentPage= AboutPage;
			DisplayClear();display();
			break;
		case 2:
			displayInstructionsPage();
			DisplayClear();display();
			break;
	}
	
	// ���в˵���
	
	switch(CompileMenu){
		case 1:
			DisplayClear();display();compileFile();break;
		case 2:
			DisplayClear();display();runFile();break;
		case 3:
			DisplayClear();display();if(compileFile()) runFile();break;
	} 
}

/*
	��ʾ����ҳ�� 
*/

void displayAboutPage()
{
	char *originFont= GetFont();
	int originPointSize= GetPointSize();
	
	SetFont("΢���ź�");
	SetPointSize(13);
	
	double FontH= GetFontHeight();
	double x= 0;
	double y= GetWindowHeight();
	double h= FontH*1.5;
	double w= GetWindowWidth();
	
	SetPenColor("Menu Gray");
	drawRectangle(x, y-h, w, h, 1);
	SetPenColor("Black");
	
	setButtonColors("Menu Gray", "Black", "Menu Hot Gray", "Black", 1);
	
	SetPointSize(13);
	
	if(button(GenUIID(0), x+TextStringWidth("  "), y-h, TextStringWidth("����")*2, h, ("����"))){
		CurrentPage= EditPage;
	}
	
	SetPointSize(20);
	MovePen(w/2.7, y-h*0.8);
	DrawTextString("����������C����༭��");
	
	SetPointSize(20);
	FontH= GetFontHeight();
	double FD= GetFontDescent();
	h= FontH*1.5;
	x= GetWindowWidth()/8;
	y= GetWindowHeight()*0.8-h*2;
	
	MovePen(x, y);
	DrawTextString("������C����༭������λͬѧ��ͬ������");
	MovePen(x, y-h);
	DrawTextString("����ʹ�õ��ǵ�����ͼ�ο� libgraphics �Լ��㽭��ѧ�γ��鿪���� simpleGUI ͼ�ο⡣");
	MovePen(x, y-h*2);
	DrawTextString("���Ƕ���ʹ�õĵ�ͼ�ο������һЩ�޸ģ�ͬʱ�õ���Windows API��");
	setButtonColors("Button Blue", "Black", "Button Blue", "Black", 1);
	MovePen(x, y-h*3);
	DrawTextString("Ϊ���֧����Щ����:");
	if(button(GenUIID(0), x, y-h*4-FD, TextStringWidth("Donate"), FontH, "Donate")){
		WinExec("cmd.exe /k start https://love.bilibili.com/detail?uuid=f3289a42bc", SW_HIDE);
	}
	
	SetFont(originFont);
	SetPointSize(originPointSize);
}

/*
	��ʾʹ��˵������(ֱ����ת����ҳ)
*/

void displayInstructionsPage()
{
	WinExec("cmd.exe /k start https://github.com/onezero-harmony/C-editor", SW_HIDE);
}

/*
	��ʾ�Ҽ��˵� 
*/

void displayRightMenu()
{
	setButtonColors("Button Gray", "Black", "Light Gray", "Black", 1); 
	
	MovePen(RightMenuX, RightMenuY);
	SetFont("Consolas");
	SetPointSize(13);
	
	char *RightMenu[]= {
		"�ָ�                       Ctrl-Z",
        "����                       Ctrl-Y",
        "����                       Ctrl-X",
        "����                       Ctrl-C",
        "ճ��                       Ctrl-V"
	};
	
	double h= GetFontHeight()*1.5;
	double w= TextStringWidth(RightMenu[0])*1.1;
	int i= 1;
	for(i; i<= sizeof(RightMenu)/sizeof(RightMenu[0]); i++){
		if(button(GenUIID(i), RightMenuX, RightMenuY-h*(i), w, h, RightMenu[i-1])){
			switch(i){
				case 1:         // �ָ� 
					CANCEL();DisplayClear();display();break;
				case 2:			// ���� 
					REMAKE();DisplayClear();display();break;
				case 3:			// ���� 
					ClipContent();DisplayClear();display();break;
				case 4:			// ���� 
					CopyContent();DisplayClear();display();break;
				case 5:			// ճ�� 
					PasteContent();DisplayClear();display();break;
			}
	//		isShowRightMenu= 0;
		}
	}
}



/*
	��ʾ��ǰ�Ĵ���༭���� 
*/

void displayEditPage()
{
	if(isShowEdit) displayEdit();
	
	/*
		��ʾ�˵� 
	*/
	
	displayMenu();
	
	/*
		��ʾ�ļ����� 
	*/
	
	displayFileName();
	
	/*
		��ʾ�Ҽ��˵� 
	*/ 
	
	if(isShowRightMenu){
		displayRightMenu();
	}
}

void display()
{
	switch(CurrentPage){
		case EditPage:
			displayEditPage();
			break;
		case AboutPage:
			displayAboutPage();
			break;
	}
} 
