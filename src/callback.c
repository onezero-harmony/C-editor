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

	//������˲˵����߲��ڱ༭ҳ�������༭����mouseevent
	//���ֻ����������ƶ�����ɶ�������Խ�Լ������Դ 
	DisplayClear();
	uiGetMouse(x,y,button,event);
	if(unfoldMenu) { 		//�޸Ĵ� 
		display();
		return;
	}
	if(!isShowRightMenu && isShowEdit)editMouseEvent(x, y, button, event);
	if(button == LEFT_BUTTON && event == BUTTON_UP && isShowRightMenu){
		display();
		DisplayClear();
		isShowRightMenu= 0;
	}
	
	if(button == RIGHT_BUTTON && event == BUTTON_UP){     // �Ҽ� 
		isShowRightMenu= 1;
		SetRightMenuXY(mouse_x, mouse_y);  // �����Ҽ��˵����Ͻ���ʼ���� 
	}
	
	display();
}

void mykey(int key, int event){
	if(CurrentPage != EditPage || !isShowEdit) return;
	if(key == VK_F1  && event == KEY_UP)  displayInstructionsPage();
	if(key == VK_F9  && event == KEY_UP)  compileFile();
	if(key == VK_F10 && event == KEY_UP)  runFile();
	if(key == VK_F11 && event == KEY_UP)  if(compileFile()) runFile();
	//������˲˵����߲��ڱ༭ҳ�������༭����editkey
	DisplayClear();
	uiGetKeyboard(key,event);
	editkey(key, event);
	display();
}

void mychar(char ch){
	//������˲˵����߲��ڱ༭ҳ�������༭����editchar
	if(CurrentPage != EditPage || !isShowEdit) return;
	editchar(ch);
}

void myTimer(int timerID){
	//������˲˵����߲��ڱ༭ҳ�������༭����timer 
	if(!isShowRightMenu && !unfoldMenu && CurrentPage == EditPage) {
		if(isShowEdit) editTimer(timerID);        // �޸Ĵ� 
	}
}
