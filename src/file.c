#include "graphics.h"
#include "extgraph.h"
#include "imgui.h"
#include "windows.h"
#include "variable.h"
#include <string.h>
#include <Commdlg.h>

int openSuccess;               // ����ʱ�Ƿ�ɹ���ȡ��·�� 
int isProcessing;              // �����Ƿ����ڶ�ȡ/д������ 
int isCreated;                 // �ļ��Ƿ��Ѵ��� 
int isSaved;                   // �ļ��Ƿ��ѱ��� 
int isShowEdit;                // �Ƿ���ʾ�༭�� 
OPENFILENAME ofn;
char ssaveFile[MAX_PATH];       // �����ļ�·���� 
char ssaveFileTitle[MAX_PATH];  // �����ļ����� 
FILE *fp1;
char tmpName[MAX_PATH];
/*
	��(��ȡ)һ���ļ� 
*/



void readFile()
{
	char s[10010];
	if(isProcessing) return;
	isProcessing= 1;
	
	int id;
	if(!isSaved){
		if(!isCreated){
			sprintf(s, "�Ƿ�Ҫ����� �ޱ��� �ĸ��ģ�");
		}
		else{
			sprintf(s, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
		}
		id= MessageBoxA(NULL, s, "������C����༭��", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL);
		switch(id){
			case IDYES:
				saveFile();
				break;
			case IDNO:
				break;
			case IDCANCEL:
				isProcessing= 0;
				return;
		}
	}
	
	char oopenFile[MAX_PATH];          // ���� 
	char oopenFileTitle[MAX_PATH];
	strcpy(oopenFile, ssaveFile);
	strcpy(oopenFileTitle, ssaveFileTitle);
	
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize= sizeof(OPENFILENAME);
	ofn.hwndOwner= NULL;
	ofn.lpstrFile= ssaveFile;
	strcpy(ofn.lpstrFile, "*.c");
	ofn.lpstrFilter= "�ı��ĵ�(*.c)\0*.c\0�����ļ�(*.*)\0*.*\0";
	ofn.lpstrFileTitle= ssaveFileTitle;
	ofn.lpstrFileTitle[0]= '\0';
	ofn.nMaxFileTitle= MAX_PATH;
	ofn.nMaxFile= MAX_PATH;
	ofn.Flags= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	
	if(GetOpenFileName(&ofn)){
		fp1= fopen(ofn.lpstrFile, "r+");
		if(fp1==NULL){
			MessageBoxA(NULL, "��ʧ��", ".c", MB_OK | MB_APPLMODAL);
			strcpy(ssaveFile, oopenFile);
			strcpy(ssaveFileTitle, oopenFileTitle);
			isProcessing= 0;
			return ;
		}
		isShowEdit= 1;
	}
	else{
		strcpy(ssaveFile, oopenFile);
		strcpy(ssaveFileTitle, oopenFileTitle);
		isProcessing= 0;
		return;
	}
	
	// ���ļ��ж�ȡ���� 
	modifylocate();
	CursorExist= 1;
	fseek(fp1, 0, SEEK_SET);
	int flagReturn= 0;       // ����Ƿ��� 
	int flagChinese= 0;       // ����Ƿ������� 
	char ch;
	initEdit();
	while((ch= fgetc(fp1))&&ch!=EOF){
		if(ch>0){                    // �����������
			if(ch=='\n'){
				linenum++;
				LINE *p= (LINE*)calloc(1, sizeof(LINE));
				p->pre= nowline;
				nowline->next= p;
				nowline= p;
				nowline->next= NULL;
				nownode->next= NULL;
				Node *q= (Node*)calloc(1, sizeof(Node));
				q->pre= NULL;
				nownode= q;
				nowline->head = q;
				nownode->next= NULL;
			}
			else{
				if(ch == '\t'){
					nowline->chnum += 4;
					Node *q= (Node*)calloc(1, sizeof(Node));
					q->c[0] = q->c[1] = q->c[2] = q->c[3] = ch;
					q->pre= nownode;
					nownode->next= q;
					nownode= q;
					nownode->next= NULL;
				}
				else{
					Node *q= (Node*)calloc(1, sizeof(Node));
					q->c[0] = ch;
					q->pre= nownode;
					nownode->next= q;
					nownode= q;
					nownode->next= NULL;
				}
			}
			nownode->line= nowline;
		}	
		else{                        // ��������� 
			if(flagChinese){
				flagChinese^= 1;
				nownode->c[1]= ch;
			}
			else{
				nowline->chnum += 2;
				flagChinese^= 1;
				Node *q= (Node*)calloc(1, sizeof(Node));
				q->pre= nownode;
				nownode->next= q;
				nownode= q;
				nownode->next= NULL;
				nownode->c[0]= ch;
			}
			nownode->line= nowline;
		}
	}
	nownode = head->head;
	nowline = head;
	fclose(fp1);
	
	isSaved= 1;
	isCreated= 1;
	isProcessing= 0;
}
/*
	�����ļ�����������½����Ա�����ԭ·������������windows api���� 
*/

void saveFile()
{
	isProcessing= 1;
	
	if(!isCreated){
		memset(&ofn,0,sizeof(OPENFILENAME));
		ofn.lStructSize= sizeof(OPENFILENAME);
		ofn.hwndOwner= NULL;
		ofn.lpstrFile= ssaveFile;
		strcpy(ofn.lpstrFile, "*.c");
		ofn.lpstrFilter= "�ı��ĵ�(*.c)\0*.c\0�����ļ�(*.*)\0*.*\0";
		ofn.lpstrFileTitle= ssaveFileTitle;
		ofn.lpstrFileTitle[0]= '\0';
		ofn.nMaxFileTitle= MAX_PATH;
		ofn.nMaxFile= MAX_PATH;
		ofn.Flags= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		if(GetSaveFileName(&ofn)){
			openSuccess= 1;
			int l= strlen(ofn.lpstrFileTitle);
			if (strcmp(ofn.lpstrFileTitle+l-2, ".c")){
				strcat(ofn.lpstrFileTitle, ".c");
				fp1= fopen(strcat(ofn.lpstrFile, ".c"), "w+");
			}
			else fp1= fopen(ofn.lpstrFile, "w+");
			isCreated= 1;
			fclose(fp1);
		}
		else{
			openSuccess= 0;
			isProcessing= 0;
			return ;
		}
	} 
	if(strcmp(ofn.lpstrFileTitle+strlen(ofn.lpstrFileTitle)-2, ".c")){
		strcat(ofn.lpstrFileTitle, ".c");
		strcat(ofn.lpstrFile, ".c");
	}
	// д��fp1�� 
	fp1= fopen(ofn.lpstrFile, "w");
	LINE *lline= head;
	Node *q= head->head->next;
	while(lline->next){
		while(q && q->next){
			fputs(q->c, fp1); 
			q= q->next;
		}
		if(q) fputs(q->c, fp1);
		fputc('\n',fp1);
		lline= lline->next;
		q= lline->head->next;
	}
	
	while(q){
		fputs(q->c, fp1);
		q= q->next;
	}
	isSaved= 1;
	fclose(fp1);
	isProcessing= 0;
}

/*
	���Ϊ�����������ʱд���ļ���ͬʱ���� 
*/ 

void saveAsFile()
{
	isProcessing= 1;
	if(!isCreated){
		saveFile();
		isProcessing= 0;
		return;
	}
	OPENFILENAME ofn2;
	char ssaveFile2[MAX_PATH];
	FILE *fp2;                 // ���ļ� 
	int flag= 0;
	memset(&ofn2, 0, sizeof(OPENFILENAME));
	ofn2.lStructSize= sizeof(OPENFILENAME);
	ofn2.hwndOwner= NULL;
	ofn2.lpstrFile= ssaveFile2;
	strcpy(ofn2.lpstrFile, "*.c");
	ofn2.lpstrFilter= "�ı��ĵ�(*.c)\0*.c\0�����ļ�(*.*)\0*.*\0";
	ofn2.lpstrFileTitle= ssaveFileTitle;
	ofn2.lpstrFileTitle[0]= '\0';
	ofn2.nMaxFileTitle= MAX_PATH;
	ofn2.nMaxFile= MAX_PATH;
	ofn2.Flags= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if(GetSaveFileName(&ofn2)){
		int l= strlen(ofn2.lpstrFileTitle);
		if(strcmp(ofn2.lpstrFileTitle+l-2, ".c")){
			strcat(ofn2.lpstrFileTitle, ".c");
			strcat(ofn2.lpstrFile, ".c");
		}
		fp2= fopen(ofn2.lpstrFile, "w+");
		if(isCreated){
			fp1= fopen(ofn.lpstrFile, "w+");
			flag= 1;
			isSaved= 1;
		}
		LINE *lline= head;
		Node *q= head->head;
		while(lline->next){
			while(q && q->next){
				if(flag){
					fputs(q->c, fp1);
				}
				fputs(q->c, fp2);
				q= q->next;
			}
			if(flag){
				if(q != NULL)fputs(q->c, fp1);
				fputc('\n', fp1);
			} 
			if(q != NULL)fputs(q->c, fp2);
			fputc('\n',fp2);
			lline= lline->next;
			q= lline->head;
		}
	
		while(q != NULL){
			if(flag) fputs(q->c, fp1);
			fputs(q->c, fp2);
			q= q->next;
		}
	
		fclose(fp2);
		if(flag) fclose(fp1);
	}
	
	isProcessing= 0;
}

/*
	��ʼ��C-editor 
*/

void InitCeditor()
{
	isCreated= 0;
	isSaved= 0;
	isShowEdit= 1;
}

/*
	�½�һ���ļ� 
*/

void createFile()
{
	isShowEdit= 1;
	char s[10010]; 
	if(isProcessing) return;
	if(!isSaved){
		if(!isCreated){
			sprintf(s, "�Ƿ�Ҫ����� �ޱ��� ���޸ģ�");
		}
		else{
			sprintf(s, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
		}
		int id= MessageBoxA(NULL, s, "������C����༭��", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL);
		switch(id){
			case IDYES:
				saveFile();
			case IDNO:
				InitCeditor();initEdit();
				break;
			case IDCANCEL:
				break;
		}
	}
	else{
		InitCeditor();
		initEdit(); 
	} 
} 

/*
	�˳�ʱ���ޱ�������ʾ	
*/

void exitRemind()
{ 
	char s[10010];             // ��ʱ�� 
//	printf("%d", isSaved);
	if(!isSaved){
		if(!isCreated){        // ����������� 
			sprintf(s, "�Ƿ�Ҫ����� �ޱ��� �ĸ��ģ�");	
		}
		else{                  // û�д����� 
			sprintf(s, "�Ƿ�Ҫ����� %s �ĸ��ģ�", ofn.lpstrFileTitle);
		}
		int id= MessageBoxA(NULL, s, "������C����༭��", MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_APPLMODAL);
		switch(id){
			case IDYES:
				saveFile();
				ExitGraphics();
				break;
			case IDNO:
				ExitGraphics();
				break;
			case IDCANCEL:
				break;
		}
	}
	else{
		ExitGraphics();
	}
}

/*
	���ص�ǰ�ļ����� 
*/

char *getCurrentFileName()
{
	if(isCreated){
		return ofn.lpstrFileTitle;
	}
	else{
		return NULL;
	}
}

/*
	���ص�ǰ�Ƿ񱣴� 
*/

int getisSaved()
{
	if(isSaved) return 1;
	return 0;
}


/*
	��ʾ��ǰ�ļ�����	
*/

void displayFileName()
{
	SetPenColor("Black");
	string filename;
	char name[10050];
	if(!(filename= getCurrentFileName())){
		if(isSaved) sprintf(name, "�ޱ���.c");
		else sprintf(name, "*�ޱ���.c"); 
	}
	else{
		if(isSaved) sprintf(name, "%s", filename);
		else sprintf(name, "*%s", filename);
	}
	if(isShowEdit){
		SetWindowTitle(name);
		double fw = TextStringWidth(name);
		double w= TextStringWidth("�ļ�")*2;
    	double x = max(GetWindowWidth() / 2 - fw / 2, w * 3.5);
    	double h= GetFontHeight()*1.5;
    	MovePen(x, GetWindowHeight() - h / 2 - GetFontAscent() / 2);
    	DrawTextString(name);
	}
	else{
		SetWindowTitle("������C����༭��");
	}
}

/*
	�����ļ� 
*/



int ExecuteCmd(char *str, char *output)
{
	SECURITY_ATTRIBUTES sa;
    HANDLE hRead,hWrite;
 
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&hRead,&hWrite,&sa,0))
    {
        MessageBox(NULL,"Error on CreatePipe()!","Error",MB_OK);
        return 0;
    }
	STARTUPINFO si={sizeof(si)};
    PROCESS_INFORMATION pi;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    if (!CreateProcess(NULL,str,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))
    {
        MessageBox(NULL,"����ʧ�ܣ������Ƿ�װgcc����","����",MB_OK);
        return 0;
    }
    
    
    CloseHandle(hWrite);
 
    char buffer[4096];
    memset(buffer,0,sizeof buffer);
    DWORD byteRead;
	ReadFile(hRead,buffer,4095,&byteRead,NULL);
    strcat(output,buffer);
    CloseHandle(hRead);
    return 1;
}

int compileFile()
{
	if(!isSaved){
		MessageBox(NULL, "���ȱ��棬�ٱ��룡", "������ʾ", MB_OK);
		return 0; 
	}
	char *s= ofn.lpstrFile;
	strcpy(tmpName,s);
	if(!strcmp(tmpName+strlen(tmpName)-2, ".c")){
		tmpName[strlen(tmpName)-2]= '\0';
	}
	char command[1024];
	memset(command, 0, sizeof(command));
	sprintf(command, "gcc %s.c -o %s.exe", tmpName, tmpName);
	char result[4096]={};
	if(!ExecuteCmd(command, result)) return 0;
	 
	if(strlen(result)) {
		MessageBox(NULL, result, "������ʾ", MB_OK);
		return 0;
	}
	return 1;
}

/*
	�����ļ� 
*/

void runFile()
{
	char *s= ofn.lpstrFile;
	strcpy(tmpName,s);
	if(!strcmp(tmpName+strlen(tmpName)-2, ".c")){
		tmpName[strlen(tmpName)-2]= '\0';
	}
	strcat(tmpName,".exe");
	STARTUPINFO si = {sizeof(si)}; //��¼�ṹ���ж�󣬱���Ҫ����
    PROCESS_INFORMATION pi;    //����id�����̾�����߳�id���߳̾������������ṹ��
    if(CreateProcess(NULL,tmpName,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi) == NULL){
    	MessageBox(NULL, "����ʧ�ܣ����ȱ������", "������ʾ", MB_OK);
	}
}


/*	
	��ʼ���ļ� 
*/
void initfile(){
	isShowEdit= 1;
	isCreated= 0;
	isSaved= 0;
}

