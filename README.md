# SoftMechKeyboard
使用软件手段模拟机械键盘的声音(A program imitate mechanical keboard sounds)

## 12.16 23:20
555刚出门吃碗粉电动车车胎爆了，明天跟hxd去吃早饭顺便修车

接下来可能要进行硬件部分的开发，其实没有想很好怎么做，大概率用stm32F4的开发板，然后在板子里写程序和电脑用usb接上然后接受电脑信息控制电磁阀通断

但是感觉好复杂，而且stm32做这个这么简单的事情好像没必要，

所以还有一种方法就是用c++直接控制usb接口输出信号，然后自个画一块板子焊接啥的，软硬件衔接我比较持怀疑态度，毕竟要是延迟太大或者根本无法实现就很难搞

硬件方面emmmmm，慢慢磨也还是可以的，so明天先做一点调查吧！

## 12.16 20:09
哈哈 刚刚用多线程技术改好了原来的卡键盘的bug 也就是说现在不会长按删除键把不想删的东西删掉了23333
原来会卡的原因是：
```cpp
//KeyboardProc()里
	case WM_KEYDOWN:
		Beep();
...
//会在如下一步之前阻塞整个线程，导致按键信息无法继续传递，造成打字的时候卡顿
	return CallNextHookEx(g_hHook,code,wParam,lParam);
```
解决办法就是新创建一个线程来运行Beep（）

需要用到thread头文件（include<thread>）

```cpp
void myBeep(){
	Beep(1000,60)
}
//线程需要入口的函数，所以这里的myBeep（）就是一个入口函数
//在proc（）里写：
	case WM_KEYDOWN:
		thread myB(myBeep);
		myB.detach()
//这里detach（）是让主线程不要等子线程的意思
//若使用join（）则是让主线程在这里等子线程，那就又会阻塞
```

## 12.14 16:25
readme上传上来才发先显示好奇怪！甚至换行都不一样，难道markdown也有方言嘛？

## 12.14 16:03
接下来是要试图用多线程解决掉这个按键卡顿的bug，

原理我自个想的是截获按键消息后创建一个线程让蜂鸣器自己去响，

不要影响按键消息的流通，不知道行不行得通，我先做做！！

## 12.14 15:51
这个原理其实就是通过截取windows下发给应用程序的按键信息，然后触发程序里的Bepp(freq,time)，之后放行这个信息

所以这样linear的操作就使得打字太快会有延迟...目前就是想要解决这个延迟问题

具体原理，截取信息就是用钩子把信息钓到：

```cpp
==这部分是Hook()函数里面需要用到的一个可回调的函数==  
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
	return CallNextHookEx(g_hHook,code,Param,lParam);  
}  
void Hook(){  
	以下就是在做一个Windows的钩子  
	g_hHook = SetWindowsHookExA(  
		WH_KEYBOARD_LL,//要截取的消息  
		KeyboardProc,//截取到这个消息调用的函数  
		GetModuleHandle(0),//句柄啥的 不清楚  
		0//全局都可用  
	);  
	==以下是检测hook有没有设定成功==  
	if (g_hHook == NULL){  
		printf("hook failed");  
		int i = GetLastError();  
		printf("%d",i);  
	}else{  
		printf("Hooked!");  
	}  
}  
```

```cpp
==除此二者之外就是要在main()写一个消息循环==  
	MSG msg;  
	while(GetMessage(&msg,NULL,0,0)){  
		TranslateMessage(&msg);//翻译  
		DispatchMessage(&msg);//分发消息  
	}  
```
没有这个循环就没办法触发响应  

这里感谢好朋友Buger404帮我弄明白到底咋做这个事情，看了不少参考资料，

水平还很菜，希望在寒假里面能学到如何做这样子自己的一个项目


## 12.14 15:43
正在学习如何用git和github

* initialize:  

  ```bash
  git init  
  git add repo url  
  ```

* pull:  

  ```bash
  git pull repo branch //我用的是main的名称 别搞错多弄了branch  
  ```

* push:  

  ```bash
  git add . //将目录下所有文件加入，或把“.”替换为文件名  
  git commit -m "new things"  
  git push repo branch  
  ```

* 出现error：`src refspec main does not match any：  `

  大概是文件中有冲突无法合并的，需要去文件中修改冲突  

* rename:  

  ```bash
  git rename old new  
  ```

  没有使用ssh 配置老是不成功 先这样凑合用吧  

* 出现fatal: unable to access url： OpenSSL SSL_read: Connection was aborted, errno 10053

  ```bash
  git config --global http.sslVerify "false"
  ```

