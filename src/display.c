#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>

enum page CurrentPage;         // 当前页面 
int isShowRightMenu;           // 是否显示右键菜单 

double RightMenuX;			   // 右键菜单X坐标 
double RightMenuY; 			   // 右键菜单Y坐标 



/*
	设置右键菜单左上角坐标 
*/

void SetRightMenuXY(double x, double y)
{
	RightMenuX= x;
	RightMenuY= y;
}


/*
	显示菜单 
*/

void displayMenu()
{
	static int flag= 0;
	static int editFlag= 0;
 	SetFont("Consolas");
	SetPenColor("Gray");
	
	char *LabelsFile[] = {"文件",
        "新建                       Ctrl-N",
        "打开                       Ctrl-O",
        "保存                       Ctrl-S",
        "另存为                     Ctrl-P",
        "关闭                       Ctrl-W",
        "退出                       Alt-F4"
    };
	
	double winWidth= GetWindowWidth();
	double x=0;
	double y= GetWindowHeight();
	double w= TextStringWidth(LabelsFile[0])*2.5;
	double wlist= TextStringWidth(LabelsFile[1])*1.1;
	double h= GetFontHeight()*1.5;
	drawRectangle(x, y-h, winWidth, h, 1);
	
	SetPenColor("Menu Gray");
	
	char *LabelsEdit[] = {"编辑",
        "恢复                       Ctrl-Z",
        "重做                       Ctrl-Y",
        "剪切                       Ctrl-X",
        "复制                       Ctrl-C",
        "粘贴                       Ctrl-V"
    };
	
	char *LabelsHelp[] = {"帮助",
        "关于                       Ctrl-E",
        "使用说明                       F1"
    };
	
	char *LabelsCompile[] = {"运行",
		"编译                       F9",
		"运行                       F10",
		"编译运行                   F11" 
	};
	 
	int FileMenu= menuList(GenUIID(0), x, y-h, w, wlist, h, LabelsFile, sizeof(LabelsFile)/sizeof(LabelsFile[0]));
	
	int EditMenu= menuList(GenUIID(0), x+w, y-h, w, wlist, h, LabelsEdit, sizeof(LabelsEdit)/sizeof(LabelsEdit[0]));
	
	int HelpMenu= menuList(GenUIID(0), x+w*2.0, y-h, w, wlist, h, LabelsHelp, sizeof(LabelsHelp)/sizeof(LabelsHelp[0]));
	
	int CompileMenu= menuList(GenUIID(0), x+w*3.0, y-h, w, wlist, h, LabelsCompile, sizeof(LabelsCompile)/sizeof(LabelsCompile[0]));
	
	// 文件菜单 
	
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
				if(!isCreated) sprintf(s, "是否要保存对 无标题 的修改？");
				else sprintf(s, "是否要保存对 %s 的修改", ofn.lpstrFileTitle);
				int id= MessageBoxA(NULL, s, "轻量级C代码编辑器", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL);
				switch(id){
					case IDYES:
						saveFile();
						if(!openSuccess) editFlag=1;       // 如果没有成功获取保存路径就继续编辑 
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
	
	// 编辑菜单： 
	
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
	
	// 帮助菜单： 
	
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
	
	// 运行菜单：
	
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
	显示关于页面 
*/

void displayAboutPage()
{
	char *originFont= GetFont();
	int originPointSize= GetPointSize();
	
	SetFont("微软雅黑");
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
	
	if(button(GenUIID(0), x+TextStringWidth("  "), y-h, TextStringWidth("返回")*2, h, ("返回"))){
		CurrentPage= EditPage;
	}
	
	SetPointSize(20);
	MovePen(w/2.7, y-h*0.8);
	DrawTextString("关于轻量级C代码编辑器");
	
	SetPointSize(20);
	FontH= GetFontHeight();
	double FD= GetFontDescent();
	h= FontH*1.5;
	x= GetWindowWidth()/8;
	y= GetWindowHeight()*0.8-h*2;
	
	MovePen(x, y);
	DrawTextString("轻量级C代码编辑器由两位同学共同开发。");
	MovePen(x, y-h);
	DrawTextString("我们使用的是第三方图形库 libgraphics 以及浙江大学课程组开发的 simpleGUI 图形库。");
	MovePen(x, y-h*2);
	DrawTextString("我们对所使用的的图形库进行了一些修改，同时用到了Windows API。");
	setButtonColors("Button Blue", "Black", "Button Blue", "Black", 1);
	MovePen(x, y-h*3);
	DrawTextString("为乡村支教做些贡献:");
	if(button(GenUIID(0), x, y-h*4-FD, TextStringWidth("Donate"), FontH, "Donate")){
		WinExec("cmd.exe /k start https://love.bilibili.com/detail?uuid=f3289a42bc", SW_HIDE);
	}
	
	SetFont(originFont);
	SetPointSize(originPointSize);
}

/*
	显示使用说明界面(直接跳转到网页)
*/

void displayInstructionsPage()
{
	WinExec("cmd.exe /k start https://github.com/onezero-harmony/C-editor", SW_HIDE);
}

/*
	显示右键菜单 
*/

void displayRightMenu()
{
	setButtonColors("Button Gray", "Black", "Light Gray", "Black", 1); 
	
	MovePen(RightMenuX, RightMenuY);
	SetFont("Consolas");
	SetPointSize(13);
	
	char *RightMenu[]= {
		"恢复                       Ctrl-Z",
        "重做                       Ctrl-Y",
        "剪切                       Ctrl-X",
        "复制                       Ctrl-C",
        "粘贴                       Ctrl-V"
	};
	
	double h= GetFontHeight()*1.5;
	double w= TextStringWidth(RightMenu[0])*1.1;
	int i= 1;
	for(i; i<= sizeof(RightMenu)/sizeof(RightMenu[0]); i++){
		if(button(GenUIID(i), RightMenuX, RightMenuY-h*(i), w, h, RightMenu[i-1])){
			switch(i){
				case 1:         // 恢复 
					CANCEL();DisplayClear();display();break;
				case 2:			// 重做 
					REMAKE();DisplayClear();display();break;
				case 3:			// 剪切 
					ClipContent();DisplayClear();display();break;
				case 4:			// 复制 
					CopyContent();DisplayClear();display();break;
				case 5:			// 粘贴 
					PasteContent();DisplayClear();display();break;
			}
	//		isShowRightMenu= 0;
		}
	}
}



/*
	显示当前的代码编辑窗口 
*/

void displayEditPage()
{
	if(isShowEdit) displayEdit();
	
	/*
		显示菜单 
	*/
	
	displayMenu();
	
	/*
		显示文件名称 
	*/
	
	displayFileName();
	
	/*
		显示右键菜单 
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
