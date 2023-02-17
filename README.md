# SoftMechKeyboard
使用软件手段模拟机械键盘的声音(A program imitate mechanical keboard sounds)

## 2.17 12：33
应该是这个项目最后一次写日志 因为已经差不多了

现在已经返校，整个装置都可以正常运行，目前采用的是mos管作为开关实现迅速开关，成本上也还可以接受

唯一一个问题就是好像这软件每次开启的时候都需要用xcom类似激活单片机一样的操作

应该是软件的问题，也懒得优化了，说实话这个状态还不是很实用，就是说太复杂了，要是能更集成一点就好了

代码方面，现在是在while（1）这个循环里实现控制电磁阀的操作

唉，开学前1个月几乎啥都没看，基本就在喝酒玩乐，备考复习，这第一周过完考试也结束了，可以再捡起来了，希望又有有意思的项目给我做一做哈哈

## 12.30 14：43
holyshit!!我居然以为上一次更新是昨天晚上，精神状态堪忧...

28号那次根本没有完全解决问题，是这样的：

32读取数据要看是否以\r\n结尾（当然 可以不看，但是在usart函数里写了就根据这个规则），也就是最后两位是0d 0a

（\0d就是ascii 13也就是\r \0a就是ascii 10也就是\n）

我原以为windows这边输出会自动加上\r\n（或者说我根本没考虑到这个）

就导致单片机不将STA设定为收到——他以为我发的都是错误信息！！而XCOM自动开启发送新行，自然就可以被单片机读到

我真傻...

要不是今天再测试了一下，28号这个bug可能一直不会出来

我用的是vpsd软件模拟com1和com3，然后开一个XCOM 查看com1，用我的软件试着写了一下，com1显示接收到信息在16进制下是"61 62 "

再开XCOM查看com3，同样发送ab，com1接收到"61 62 0d 0a"

这不就破案了，回到cpp代码里，在data后面加上\r\n就是

---

其实今天的进展还有更进一步的电路实现，我用三极管搭了一个开关电路，接上了电磁阀，也确实可以用

但是电磁阀根本动不起来...原因就是单片机输出最多25mA电流，三极管增益又不够大，我不敢跑满，就20mA往下，

然后整个电路设计充满了约等于，不精确，

我也不敢随便加电源电压，怕板子报废，所以最终呈现结果是电磁阀动不了...

接下来准备上继电器，虽然pdd买了光耦，但是没细看手册，内最大电流才50mA，电磁阀最低得600mA，我测了一下要是通24V可以到1.2A

光耦必定直接噶了，继电器会有声音，我本来不打算用的，不过看来还是得上的！

## 12.28 23：15
沃趣 连更3次 git都怀疑我在水了

其实就是编译器的问题 之前一直是32位模式使用编译器的

现在调整为64位 可以直接运行了 到此就完成了初步的硬件开发！！

睡觉！！

## 12.28 22：54
确实是漏掉东西了 就是在dcb初始化完了得设置串口状态 不然发送的信号当然不对了

但是现在又有新bug

就是在DevC里面开起来就可以正常使用

但是编译后从文件目录直接开启程序就没办法正常运行 

很疑惑...

## 12.28 21:47
子弹飞了很久也没飞到，但是最终还是有一点点小成果：实现了最基本的功能！

用的确实是stm32，确实觉得大材小用，因为这本质应该是一个继电器干的事情...

32上的串口通信其实比较简洁明了的

- Led的初始化函数
	- 定义结构体
	- 使能gpio时钟
	- 结构体赋值
	- 初始化led
- USART1初始化函数
	- 定义USART、GPIO、NVIC初始化结构体
	- 使能gpio和USART时钟
	- 设置GPIOA的9和10引脚复用为USART1
	- GPIO结构体赋值 初始化GPIO
	- USART结构体赋值 初始化USART
	- NVIC结构体赋值 初始化NVIC
- 串口中断处理函数
	- 检测是否是接受寄存器中断
	- 读数据
	- 闪烁LED（对就是在这里莫名奇妙插进来）
	- 重复接收 》是否接收完毕 是否接受错误重新接收
	- 接收完毕将STA寄存器写入完毕标识符
- 主函数
	- 执行各种初始化函数
	- while（1）卡住 （不是）
		- 检测STA是否为写入完毕 
			- STA置零

win端的原理就是用win32的api实现串口通信，然后把串口发信息的函数在按键按下的处理函数里面调用一下

也就是替换了MyBeep（）函数的位置，

串口重要的几个地方就是

设置串口（初始化？）：
- HANDLE com CreateFile();
- DCB dcb;dcb. ;DCB for Device Control Block
- COMMTIMEOUTS cto;cto. ;串口超时
- SetCommTimeouts(com3,&cto);设定串口超时
- SetupComm(com3,200,200);开启端口
- SetCommState(com3，&dcb);这里先前漏掉了 是设置串口状态的句子

写串口：
- char data[3]="ab";这里data类型应该char就好 反正后续有一个强制转型
- DWORD  numToWrite = strlen(data);要写入的长度
- DWORD  writtenNum;已写入的长度
- BOOL STA;是否写完
- STA=WriteFile(com3,(LPVOID)data,numToWrite,&writtenNum,NULL);

读串口：（尚未探究）

代码大致如下：
```cpp
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<thread>
using namespace std;//头文件

/*
...重复代码不展示了
*/

HANDLE com3;//这里这个com3为句柄类型，后面都要用到的

void com3Init(){
	com3 = CreateFile(...);
	if(com3==INVALID_HANDLE_VALUE){
		printf("shit!");
	}
	//DCB setting
	DCB dcb;
	//...dcb相关参数设定，dcb和通信的参数约定有很大关系，可能就是在这里出错
	
	//timeout setting
	COMMTIMEOUTS cto;
	//...cto相关参数设定，超时什么的 没有大概问题也不很大
	SetupComm(com3,200,200);//开启com3 设定输入输出缓冲区都为200
}

char data[3]="ab";
DWORD  numToWrite = strlen(data);
DWORD  writtenNum;
BOOL STA;
//上面是准备发送的相关参数
void MCUBeep(){
	STA=WriteFile(com3,(LPVOID)data,numToWrite,&writtenNum,NULL);
	//就是所谓的从com3发送的句子，这里overlapped设置成NULL，本来也要传一个结构体指针的
}

LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam){
/*
不重要代码
*/
		switch (wParam){
			case WM_KEYDOWN://按键消息触发
				thread mcuB(MCUBeep);
				mcuB.detach();
			}
/*
不重要代码
*/
}

/*
省略原有代码
*/
```


是不是看起来很理想很ok？

其实！很折磨！！！在32的程序 串口中断处理函数那边的闪烁led就是和win端妥协的产品

这样就完全没有拓展性，但凡是涉及任何其他操作都做不了 没办法调整硬件部分！！纯纯继电器！！！

32是完全接受到信息了，但是估计是信息的某些地方有问题，32不认这信息，导致STA没办法被写入完毕标识符

电脑是可以发出这种信息的，用XCOM实验的时候就可以，那就一定是上位机代码的问题

> 22：30：我好像看到哪里不对劲了 发完这个就改改试试！
	
## 12.17 23:42
stm32学习好复杂的，关于硬件部分还得让子弹飞飞，我觉得确实不应该用stm32，今天小做了一个自定义发声的版本

就是用windows自带的PlaySound()函数！

```cpp
//以下是微软官方定义
BOOL PlaySound(
   LPCTSTR pszSound,
   HMODULE hmod,
   DWORD   fdwSound
);
```
其中pszSound就是文件路径参数，hmod好像具体用处不清楚，一般填NULL

fdwSound就是决定这个文件怎么播放，我在这里设定了```cpp 
SND_FILENAME | SND_NOWAIT | SND_ASYNC ```三个参数

就是用文件名播放 播放冲突直接跳过 播放后直接执行下一条（其实我感觉就最后一个有用 不过无所谓啦 做着玩）

还有要注意在devc里面的编译选项里 在连接器中加入 “ -lwinmm” 这一句 

不加我过不了编译 具体原理就不深究了吧

然后关于音效 随手找了一个鸡你太美的（露出鸡脚了）音效试了一下 整活确实是好手 

但是我其实没很喜欢这样搞 毕竟我希望软件使用简洁一点 所以目前版本里我是注释掉了

说不定以后可以搞苹果按键音？ 以后再说罢！！

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

