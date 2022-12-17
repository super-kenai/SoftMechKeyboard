#include<stdio.h>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
//#include<ctime>
#include<thread>
using namespace std;

HHOOK g_hHook = NULL ;

void myBeep(){
	Beep(1100,50);
}

void G(){
	PlaySound(TEXT("D:\\ShitT_Files\\Coding\\Soft-nicalKeyboard\\G.wav"),NULL,SND_FILENAME | SND_NOWAIT | SND_ASYNC);
}

LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam){
	if (code == HC_ACTION){
		char msg[100];
		sprintf_s(msg,"StringRecieved: %c \r\n",wParam);
		switch (wParam){
			case WM_KEYDOWN:
				thread myB(myBeep);
				myB.detach();
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

