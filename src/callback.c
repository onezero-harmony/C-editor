#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>


void myMouseEvent (int x, int y, int button, int event)
{
	
	double mouse_x = ScaleXInches(x);
	double mouse_y = ScaleYInches(y);
	
		if(CurrentPage == AboutPage){
		uiGetMouse(x,y,button,event);
		DisplayClear();
		display();
		DisplayClear();
		display();
		return;
	}
	if(event == MOUSEMOVE && !unfoldMenu && !isShowRightMenu && !ClickEdit && mouse_x >= leftw && mouse_y <= texth) return;  

	//如果打开了菜单或者不在编辑页就锁死编辑区的mouseevent
	//如果只是无意义的移动鼠标就啥都不干以节约算力资源 
	DisplayClear();
	uiGetMouse(x,y,button,event);
	if(unfoldMenu) { 		//修改处 
		display();
		return;
	}
	if(!isShowRightMenu && isShowEdit)editMouseEvent(x, y, button, event);
	if(button == LEFT_BUTTON && event == BUTTON_UP && isShowRightMenu){
		display();
		DisplayClear();
		isShowRightMenu= 0;
	}
	
	if(button == RIGHT_BUTTON && event == BUTTON_UP){     // 右键 
		isShowRightMenu= 1;
		SetRightMenuXY(mouse_x, mouse_y);  // 设置右键菜单左上角起始坐标 
	}
	
	display();
}

void mykey(int key, int event){
	if(CurrentPage != EditPage || !isShowEdit) return;
	if(key == VK_F1  && event == KEY_UP)  displayInstructionsPage();
	if(key == VK_F9  && event == KEY_UP)  compileFile();
	if(key == VK_F10 && event == KEY_UP)  runFile();
	if(key == VK_F11 && event == KEY_UP)  if(compileFile()) runFile();
	//如果打开了菜单或者不在编辑页就锁死编辑区的editkey
	DisplayClear();
	uiGetKeyboard(key,event);
	editkey(key, event);
	display();
}

void mychar(char ch){
	//如果打开了菜单或者不在编辑页就锁死编辑区的editchar
	if(CurrentPage != EditPage || !isShowEdit) return;
	editchar(ch);
}

void myTimer(int timerID){
	//如果打开了菜单或者不在编辑页就锁死编辑区的timer 
	if(!isShowRightMenu && !unfoldMenu && CurrentPage == EditPage) {
		if(isShowEdit) editTimer(timerID);        // 修改处 
	}
}
