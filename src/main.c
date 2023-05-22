#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>


/*
	自定义颜色 
*/
void initcolor()
{
	// 括号、运算符直接用默认的Red即可
	// 变量名、函数名等标识符用默认的黑色即可
	// 关键字要黑体
	// 字符串用默认的Blue即可 
	// 头文件用默认的Green即可 
    
	DefineColor("Button Gray", 243 * 1./255, 243 * 1./255, 243 * 1./255);
	DefineColor("Menu Gray", 221 * 1./255, 221 * 1./255, 221 * 1./255);
	DefineColor("Menu Hot Gray", 198 * 1./255, 198 * 1./255, 198 * 1./255);
	DefineColor("Textbox Hot Blue", 0, 144 * 1./255, 241 * 1./255);
	DefineColor("Button Blue", 87 * 1./255, 123 * 1./255, 242 * 1./255);
	DefineColor("Comment Blue", 0, 120 * 1./255, 215 * 1./255);         // 注释颜色，要斜体 
	DefineColor("Header File Green", 22 * 1./255, 152 * 1./255, 43 * 1./255);  // 头文件颜色 
	DefineColor("Purple",128 * 1./255, 0, 128 * 1./255);
	DefineColor("Highlight", 204 * 1./255, 255 * 1./255, 255 * 1./255);  // 高亮颜色 
}

/*
	定义样式 
*/
void initDisplay()
{
	setButtonColors("Button Gray", "Black", "Light Gray", "Black", 1);
    setTextBoxColors("White", "Black", "Textbox Hot Blue", "Black", 0);
    setMenuColors("Menu Gray", "Black", "Menu Hot Gray", "Black", 1);
}
void Main()
{
	initfile();
	CurrentPage= EditPage;
	isShowRightMenu= 0;
//	InitConsole(); 
	SetWindowTitle("轻量级C语言编辑器");
	InitGraphics();
	InitGUI();
	initcolor();
    initDisplay();
	initEdit();
	
	registerMouseEvent(myMouseEvent);
	registerCharEvent(mychar);
	registerKeyboardEvent(mykey);
	registerTimerEvent(myTimer);
	startTimer(1,700); 
	
	
	display();
}
