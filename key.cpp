#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<thread>
using namespace std;
//G()
//#include<mmsystem.h>
//#pragma comment(lib,"winmm.lib")

int err=0;

HHOOK g_hHook = NULL ;
//set up com3
HANDLE com3;

void com3Init(){
	com3 = CreateFile(
		"COM3",
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if(com3==INVALID_HANDLE_VALUE){
		printf("shit!");
		err=1;
	}
	//DCB setting
	DCB dcb;
	dcb.BaudRate=115200;
	dcb.Parity=0;
	dcb.StopBits=1;
	dcb.ByteSize=8;
	//timeout setting
	COMMTIMEOUTS cto;
	cto.ReadIntervalTimeout = 0xFFFFFFFF;
	cto.WriteTotalTimeoutConstant=5000;
	cto.WriteTotalTimeoutMultiplier=0;
	cto.ReadTotalTimeoutMultiplier=0;
	cto.ReadTotalTimeoutConstant=0;
	SetCommTimeouts(com3,&cto);
	SetCommState(com3,&dcb);
	SetupComm(com3,200,200);
}


void myBeep(){
	Beep(1100,100);
}

char data[5]="ab\r\n";
DWORD  numToWrite = strlen(data);
DWORD  writtenNum;
BOOL STA;
void MCUBeep(){
	STA=WriteFile(com3,(LPVOID)data,numToWrite,&writtenNum,NULL);
}
/*
void G(){
	PlaySound(TEXT("D:\\ShitT_Files\\Coding\\Soft-nicalKeyboard\\G.wav"),NULL,SND_FILENAME | SND_NOWAIT | SND_ASYNC);
}*/

LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam){
	if (code == HC_ACTION){
		char msg[100];
		sprintf_s(msg,"StringRecieved: %c \r\n",wParam);
		switch (wParam){
			case WM_KEYDOWN:
				if(err==0){
					thread mcuB(MCUBeep);
					mcuB.detach();	
				}
				else{
					thread myB(myBeep);
					myB.detach();
				}
				
				//thread GI(G);
				//GI.detach();
		}	
		OutputDebugString(msg);
	}
	return CallNextHookEx(g_hHook,code,wParam,lParam);
}

void Hook(){
	g_hHook = SetWindowsHookExA(
		WH_KEYBOARD_LL,
		KeyboardProc,
		GetModuleHandle(0),
		0
	);
	
	if (g_hHook == NULL){
		printf("hook failed");
		int i = GetLastError();
		printf("%d",i);
	}else{
		printf("Hooked!");
	}

}


void UnHook(){
	UnhookWindowsHookEx(g_hHook);
}

/*
void Delay(int time){
	clock_t now = clock();
	while(clock() - now < time);
}
*/

int main(void){
	//HWND cu = FindWindow(NULL,"D:\ShitT_Files\Coding\keyDetective\key.exe");
	com3Init();
	Hook();
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);		
	}
}
/*
BOOL WINAPI ConsoleHandler(DWORD CEvent){
	switch(CEvent){
		case CTRL_C_EVENT:
			MessageBox(NULL,"key","CEvent",MB_OK);
			break;
	}
	return TRUE;
}
*/
//LRESULT CALLBACK WindowProc(HWND FindWindow(NULL,"D:\ShitT_Files\Coding\keyDetective\key.exe"),UNIT uMsg)

