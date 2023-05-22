#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>


/*
	�Զ�����ɫ 
*/
void initcolor()
{
	// ���š������ֱ����Ĭ�ϵ�Red����
	// ���������������ȱ�ʶ����Ĭ�ϵĺ�ɫ����
	// �ؼ���Ҫ����
	// �ַ�����Ĭ�ϵ�Blue���� 
	// ͷ�ļ���Ĭ�ϵ�Green���� 
    
	DefineColor("Button Gray", 243 * 1./255, 243 * 1./255, 243 * 1./255);
	DefineColor("Menu Gray", 221 * 1./255, 221 * 1./255, 221 * 1./255);
	DefineColor("Menu Hot Gray", 198 * 1./255, 198 * 1./255, 198 * 1./255);
	DefineColor("Textbox Hot Blue", 0, 144 * 1./255, 241 * 1./255);
	DefineColor("Button Blue", 87 * 1./255, 123 * 1./255, 242 * 1./255);
	DefineColor("Comment Blue", 0, 120 * 1./255, 215 * 1./255);         // ע����ɫ��Ҫб�� 
	DefineColor("Header File Green", 22 * 1./255, 152 * 1./255, 43 * 1./255);  // ͷ�ļ���ɫ 
	DefineColor("Purple",128 * 1./255, 0, 128 * 1./255);
	DefineColor("Highlight", 204 * 1./255, 255 * 1./255, 255 * 1./255);  // ������ɫ 
}

/*
	������ʽ 
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
	SetWindowTitle("������C���Ա༭��");
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
