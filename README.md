# SoftMechKeyboard
使用软件手段模拟机械键盘的声音(A program imitate mechanical keboard sounds)

##12.14 16:03
接下来是要试图用多线程解决掉这个按键卡顿的bug，
原理我自个想的是截获按键消息后创建一个线程让蜂鸣器自己去响，
不要影响按键消息的流通，不知道行不行得通，我先做做！！

##12.14 15:51
这个原理其实就是通过截取windows下发给应用程序的按键信息，然后触发程序里的Bepp(freq,time)，之后放行这个信息
所以这样linear的操作就使得打字太快会有延迟...目前就是想要解决这个延迟问题

具体原理，截取信息就是用钩子把信息钓到：
	```c
//这部分是Hook()函数里面需要用到的一个可回调的函数
LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam){
	if (code == HC_ACTION){
		char msg[100];
		sprintf_s(msg,"StringRecieved: %c \r\n",wParam);
		//这个switch就是截获到键盘消息后的事情啦！
		switch (wParam){
			case WM_KEYDOWN:
				Beep(1000,60);
		}
		OutputDebugString(msg);
	}
	return CallNextHookEx(g_hHook,code,wParam,lParam);
}
void Hook(){
	//以下就是在做一个Windows的钩子
	g_hHook = SetWindowsHookExA(
		WH_KEYBOARD_LL,//要截取的消息
		KeyboardProc,//截取到这个消息调用的函数
		GetModuleHandle(0),//句柄啥的 不清楚
		0//全局都可用
	);
	//以下是检测hook有没有设定成功
	if (g_hHook == NULL){
		printf("hook failed");
		int i = GetLastError();
		printf("%d",i);
	}else{
		printf("Hooked!");
	}
}
//除此二者之外就是要在main()写一个消息循环
	MSG msg;
	while(GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);//翻译
		DispatchMessage(&msg);//分发消息
	}
//没有这个循环就没办法触发响应
	```
这里感谢好朋友Buger404帮我弄明白到底咋做这个事情，看了不少参考资料，
水平还很菜，希望在寒假里面能学到如何做这样子自己的一个项目


##12.14 15:43
正在学习如何用git和github
initialize:
	git init
	git add repo url

pull:
	git pull repo branch //我用的是main的名称 别搞错多弄了branch
push:
	git add . //将目录下所有文件加入，或把“.”替换为文件名
	git commit -m "new things"
	git push repo branch
出现error: src refspec main does not match any：
	大概是文件中有冲突无法合并的，需要去文件中修改冲突
rename:
	git rename old new
没有使用ssh 配置老是不成功 先这样凑合用吧

