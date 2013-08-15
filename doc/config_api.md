+ [配置API介绍](#almrunapi)
	+ 常用基本配置API
		* [addCommand] 添加命令 
		* [shellExecute] 运行程序
		* [message] 显示提示 
		* [MessageBox] 提示对话框
		* [InputBox] 输入对话框
		* [enterKey] 模拟按键
		* [enableCommandKey] 激活对应命令热键 
		* [disableCommandKey] 禁用对应命令热键 
		* [setTimer] 添加定时器
		* [clearTimer] 清除定时器
		* [config] 程序配置
		* [ReConfig] 刷新配置 
	+ 系统界面相关API
		* [GetForegroundWindow]
		* [SetForegroundWindow]
		* [ShowWindow]
		* [CloseWindow]
		* [IsWindowMax]	
		* [IsWindowMin]
		* [IsWindowShown]
		* [GetWindowText]
		* [SetWindowText]
		* [GetWindowSize]
		* [SetWindowSize]
		* [GetWindowPosition]
		* [SetWindowPosition]
		* [FindWindow]
		* [FindWindowEx]
		* [GetMousePosition]
		* [SetMousePosition]
		* [SetWindowPos]
	+ 操作系统相关
		* [GetEnv] 读取环境变量 
		* [SetEnv] 设置环境变量 
		* [ListDir] 获取文件列表 
		* [SetClipboardData] 设置剪贴板内容
		* [GetClipboardData] 读取剪贴板内容 
		* [GetShellFolder] 获取系统文件夹路径 
		* [FileExists] 判断文件是否存在
		* [DirExists] 判断目录是否存在
		* [EmptyRecycleBin] 清空回收站
+ [触发事件机制](#almrunevent)
	* [onClose]
	* [onUndefinedCommand]

ALMRunAPI
=================
只要稍学习一下 lua 编程语言，就可以灵活的配置 ALMRun。配置文件位于 config 目录中，common.lua 为 ALMRun 提供了默认的配置。
这里介绍一下 ALMRun 配置文件的 API：
[addCommand]: #0-addcommand "addCommand{name=命令名,desc=备注,key=[热键][HOT_KEY],cmd=执行命令,func=函数}"
### 0. addCommand
	addCommand{name=命令名,desc=备注,key=[热键][HOT_KEY],cmd=执行命令,func=函数}

    添加一个命令
	其中,cmd和func二选一,优先使用cmd参数.desc和key都是可选的.
	desc用于在使用func时显示备注信息.使用cmd参数时desc没有效果.
> 
#### key 热键说明:

快捷键使用 "X-Y" 这样的字符串（不区分大小写）描述，例如：
  
	A-l 表示 Alt + l
	A-C-c 表示 Alt + Control + c
  在快捷键描述字符串 "X-Y" 中 X 为可选的（也就是快捷键可以为一个按键，例如 F9），可以为：
  
	A 表示 Alt 键
	C 表示 Control 键
	W 或者 M 表示 Win 键
	S 表示 Shift 键

  可用的表示按键的字符串请参考[key_string.txt][HOT_KEY]（部分按键字符串在 Windows 下无效）。
 
[shellExecute]: #1-shellexecute "shellExecute(commandName, commandArg, workingDir, show)"
### 1. shellExecute

```lua
shellExecute(commandName, commandArg, workingDir, show)
```
	用于执行一个 shell 命令
	commandName 为 shell 命令名
	commandArg 为 shell 命令需要的参数
	workingDir 为 shell 命令的工作路径
	show 表示显示的方式，包括 'normal'、'max'、'min'、'hide'（常规显示、最大化、最小化、隐藏）
	函数返回 true / false 表示是否执行是否成功
	
	范例，打开 cmd 窗口并且最大化
```lua
	shellExecute('cmd', '', '', 'max')
```

[GetForegroundWindow]: #2-getforegroundwindow "GetForegroundWindow()"
### 2. GetForegroundWindow
	 GetForegroundWindow()  
	获取前台窗口
	函数返回前台窗口

[SetForegroundWindow]: #3-setforegroundwindow "SetForegroundWindow(window)"
### 3. SetForegroundWindow
	SetForegroundWindow(window)
	设置窗口 window 为前台窗口
	SetForegroundWindow(window)

[ShowWindow]: #4-showwindow "ShowWindow(window,show)"
### 4. ShowWindow
	ShowWindow(window,show)

	显示窗口 window
	show 表示显示的方式，包括 'normal'、'max'、'min'、'hide'、'restore'（常规显示、最大化、最小化、隐藏、还原）

	范例，最大化或还原窗口
```lua
	if IsWindowMax(window) then
		  ShowWindow(window, 'restore')
	else
		  ShowWindow(window, 'max')
	end
	CloseWindow
```

[CloseWindow]: #5-closewindow "CloseWindow(window)"
### 5. CloseWindow
	CloseWindow(window)

	关闭窗口 window
	范例，关闭前台窗口
	CloseWindow(getForegroundWindow())

[IsWindowMax]: #6-iswindowmax "IsWindowMax(window)"
### 6. IsWindowMax
	IsWindowMax(window)

	窗口 window 是否为最大化
	函数返回 true / false 表示 window 是否最大化

[IsWindowMin]: #7-iswindowmin "IsWindowMin(window)"
### 7. IsWindowMin
	IsWindowMin(window)

	窗口 window 是否为最小化
	函数返回 true / false 表示 window 是否最小化


[IsWindowShown]: #8-iswindowshown "IsWindowShown(window)"
### 8. IsWindowShown
	IsWindowShown(window)

	窗口 window 是否在显示
	函数返回 true / false 表示 window 是否在显示


[GetWindowText]: #9-getwindowtext "GetWindowText(window)"
### 9. GetWindowText
	GetWindowText(window)

	获取窗口 window 的标题栏文字
	函数返回窗口 window 的标题栏文字


[SetWindowText]: #10-setwindowtext "SetWindowText(window, text)"
### 10. SetWindowText
	SetWindowText(window, text)

	设置窗口 window 的标题栏文字


[GetWindowSize]: #11-getwindowsize "GetWindowSize(window)"
### 11. GetWindowSize
	GetWindowSize(window)
	获取窗口 window 的大小
	函数返回两个值 width 和 height
```lua
	--范例，获取前台窗口的大小
	local width, height = GetWindowSize(GetForegroundWindow())
```

[SetWindowSize]: #12-setwindowsize "SetWindowSize(window, width, height)"
### 12. SetWindowSize
	SetWindowSize(window, width, height)

	设置窗口 window 的大小

[GetWindowPosition]: #13-getwindowposition "GetWindowPosition(window)"
### 13. GetWindowPosition
	GetWindowPosition(window)

	获取窗口 window 的位置
	函数返回两个值 x 和 y 为窗口左上角的位置

```LUA
	--获取前台窗口的位置
	local x, y = GetWindowPosition(getForegroundWindow())
```

[SetWindowPosition]: #14-setwindowposition "setWindowPosition(window, x, y)"
### 14. SetWindowPosition
	setWindowPosition(window, x, y)

	设置窗口 window 的位置
	设置的位置为窗口左上角的位置


[FindWindow]: #15-findwindow "FindWindow(name, parentWindow)"
### 15. FindWindow

	FindWindow(name, parentWindow)

	通过窗口名称查找窗口
	参数 parentWindow 为可选参数，用于表示被查找窗口的父窗口
	注:
	新版本的FindWindow使用了和Windows API一样的语法,即
	FindWindow(className,windowName)
	当然了为了保持兼容性,使用旧的方法也同样支持,程序内部自动会判断.

>参考资料:  
	1. <http://baike.baidu.com/view/373605.htm>   
	2. <http://msdn.microsoft.com/en-us/library/windows/desktop/ms633499(v=vs.85).aspx> 

[FindWindowEx]: #findwindowex "FindWindowEx(parent,child,classname,windowname)"
#### FindWindowEx

    FindWindowEx(HWND hwndParent, // handle to parent window
		    HWND hwndChildAfter, // handle to child window
		    LPCTSTR lpszClass, // class name
		    LPCTSTR lpszWindow // window name
		)

    在窗口列表中寻找与指定条件相符的第一个子窗口。
    该函数获得一个窗口的句柄，该窗口的类名和窗口名与给定的字符串相匹配。这个函数查找子窗口，从排在给定的子窗口后面的下一个子窗口开始。在查找时不区分大小写。

>参考资料:  
	1. <http://baike.baidu.com/view/1080313.htm>   
	2. <http://msdn.microsoft.com/en-us/library/windows/desktop/ms633500(v=vs.85).aspx> 

[GetMousePosition]: #16-getmouseposition "GetMousePosition()"
### 16. GetMousePosition
	GetMousePosition()

	获取当前鼠标的位置
	函数返回两个值 x 和 y 为鼠标的位置


[SetMousePosition]: #17-setmouseposition "SetMousePosition(x, y)"
### 17. SetMousePosition
	SetMousePosition(x, y)

	设置当前鼠标的位置

[enterKey]: #18-enterkey "enterKey(keyStr)"
### 18. enterKey
	enterKey(keyStr)
	模拟敲击一个按键

	范例，模拟快捷键 Shift + F10 来打开右键菜单
	enterKey('S-F10')

[message]: #19-message "message(msg[,title])"
### 19. message

	将用对话框输出消息
	message(str)

	范例，输出 Hello ALMRun
	message('Hello ALMRun')

[enableCommandKey]: #20-enablecommandkey "enableCommandKey(keyID)"
### 20. enableCommandKey
	enableCommandKey(keyID)
	激活快捷键
	参数 keyID 由 addCommand 函数返回
	创建一个命令后，其快捷键是默认激活的
	若执行了 disableCommandKey 禁用了某个命令的快捷键，可调用此函数激活此命令的快捷键


[disableCommandKey]: #21-disablecommandkey "disableCommandKey(keyID)"
### 21. disableCommandKey
	disableCommandKey(keyID)
	禁用快捷键
	参数 keyID 由 addCommand 函数返回
	创建一个命令后，其快捷键是默认激活的
	调用此函数可以禁用某个命令的快捷键


```lua
	--范例，按 HOME 键禁用某快捷键
	local keyID = addCommand{ key = 'C-1', func = function() enterKey('S-F10', 'V', '', 'Return') end }
	local enabled = true
	addCommand{ key = 'HOME', func = function()
			  if enabled then
						 disableCommandKey(keyID)
						 enabled = false
			  else
						 enableCommandKey(keyID)
						 enabled = true
			  end
	end }
```

[setTimer]: #22-settimer "setTimer(milliseconds, oneShot, callback)"
### 22. setTimer
	setTimer(milliseconds, oneShot, callback)

	开启一个定时器
	参数 milliseconds 表示定时器每隔 milliseconds 触发一次
	参数 oneShot 表示定时器是否只触发一次
	参数 callback 为一个 Lua 函数，定时器每次触发时都会被调用
	函数返回成功创建的定时器


	范例，实现功能：使用 Windows 照片查看器时，定时翻看下一张图片
	--
	使用 F7 启动一个定时器，每 5 秒模拟按下右方向键
	使用 Ctrl + F7 关闭定时器
```lua
	local timer
	addCommand{ key = 'F7', func = function()
			  timer = setTimer(5000, false, function()
						 enterKey('Right')
			  end)
	end }
	addCommand{ key = 'C-F7', func = function()
			  clearTimer(timer)
	end }
	clearTimer
```

[clearTimer]: #23-cleartimer "clearTimer(timer)"
### 23. clearTimer
	clearTimer(timer)
	清理定时器

[GetEnv]: #24-getenv "GetEnv(NAME) "
### 24. GetEnv
	GetEnv(NAME) 
	获取一个环境变量`NAME`的值
	比如:
	GetEnv('WinDir') 结果 "C:\WINDOWS"

[SetEnv]: #25-setenv "SetEnv(var,value)"
### 25. SetEnv
	SetEnv(var,value)

	设置一个环境变量
	例:
	SetEnv('test','mytest')
	设置一个变量test值mytest
	如果value为空,则删作该变量.

[config]: #26-config "config{CONFIG1=value1,CONFIG2=value2...}"
### 26. config
	config{CONFIG1=value1,CONFIG2=value2...}

  程序序设置,目前支持以下设置
 
	* CompareMode  命令匹配模式
   		0 默认,任意位置匹配
		1 匹配开头
		2 使用lua函数HookCompre自定义匹配
		例:config{CompareMode=0}
	* ROOT
		设置默认相对根目录.
	* Explorer	设置默认的文件管理器(按Ctrl+D定位文件时)
		例:使用Tc config( Explorer = [[C:\totalcmd\totalcmd.exe]] }
	* NumberKey 是否允许数字键快捷键(类似ALTRUN)
		0,禁用(默认);1,启用
		启用之后可以按数字0-9和空格快速启动条目.
		注意: 如果按下的数字刚好可以匹配列表条目时则不会启动.
	* ShowTrayIcon 是否显示托盘图标
		0,不显示;1,显示(默认)
	* ShowTopTen 仅显示前10项
	* ExecuteIfOnlyOne 当仅剩一项匹配项时立即执行

[ListDir]: #27-listdir "ListDir(path,spec,sub)"
### 27. ListDir
	ListDir(path,spec,sub)

	返回指定目录下的文件列表
	path  路径,
	spec  文件名匹配
	sub	子目录限制
	例: ListDir([[C:\WINDOWS]],".exe",0)

[SetClipboardData]: #28-setclipboarddata "SetClipboardData(TEXT)"
[GetClipboardData]: #28-getclipboarddata "GetClipboardData()"
### 28. SetClipboardData
	SetClipboardData(TEXT)
	设剪贴板内容为TEXT.
	GetClipboardData()
	获取剪贴板TEXT格式.获取失败时返回nil

[GetShellFolder]: #29-getshellfolder "GetShellFolder([CSIDL])"
### 29. GetShellFolder
	GetShellFolder([CSIDL])

  根据`CSIDL`获取系统文件夹路径,例子: 
 
```lua  
   --获取桌面文件夹位置 
   GetShellFolder(0)
```
[InputBox]: #30-inputbox "InputBox(MESSAGE,CAPTION,DEFVALUE)"
### 30. InputBox
	InputBox(MESSAGE,CAPTION,DEFVALUE)

    显示一个文本输入框
    MESSAGE 提示信息,CAPTION 标题,DEFVALUE 默认值
    注: 所有参数都是可选的,其中DEFVALUE如果以"*"开头,将作为密码输入框,输入的密码显示为'*'

[MessageBox]: #31-messagebox "MessageBox(MESSAGE[,CAPTION,STYLE])"
### 31. MessageBox
	MessageBox(MESSAGE,CAPTION,STYLE)

    显示一个信息提示和message不一样的是这个会暂停脚本/程序的运行,并返回一个数值,在脚本中可以使用这个的控制程序的运行.
    MESSAGE 提示信息,CAPTION 标题,STYLE messagebox样式值
    样式(style)值参考,例: 显示是/否按钮和警告的图标 就是0x108 
    按钮:
    是/否         0x00000008
    确定/取消     0x00000030
    OK            0x00000004(默认)
    HELP	  0x1000(可以和上面组合)

    图标:
    警告(WARNING)       0x00000100
    错误(ERROR)         0x00000200
    提示(INFORMATION)  0x00000800(默认)

    对应返回值:
    是/确定   0x4
    否        0x8
    取沙      0x10
    HELP      0x1000

[ReConfig]: #32-reconfig "ReConfig()"
### 32. ReConfig
	ReConfig()
    刷新配置,重新加载配置文件

[DirExists]: #33-direxists "DirExists(path)"
### 33. DirExists
	DirExists(path) 判断目录path是否存在.

[FileExists]: #34-fileexists "FileExists(file)"
### 34. FileExists
	FileExists(file) 判断文件file是否存在.

[EmptyRecycleBin]: #35-emptyrecyclebin "EmptyRecycleBin(Bool Force)"
### 35. EmptyRecycleBin
	EmptyRecycleBin(Force)
	用法:
	EmptyRecycleBin() 或 EmptyRecycleBin(1)
	Force参数为1时直接清空没有任何提示,否则会有提示.
	其它说明:
	若有特殊需要还可以使用SHEmptyRecycleBin,具体请参考Windows API的介绍.
	EmptyRecycleBin()相当于SHEmptyRecycleBin(nil,nil,nil)
[SetWindowPos]: #36-setwindowpos "BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int x, int y,int cx, int cy, UINT nFlags)"
### 36. SetWindowPos
    SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int x, int y,int cx, int cy, UINT nFlags)
    
	该函数改变一个子窗口，弹出式窗口或顶层窗口的尺寸，位置和Z序。****
	子窗口，弹出式窗口，及顶层窗口根据它们在屏幕上出现的顺序排序、顶层窗口设置的级别最高，并且被设置为Z序的第一个窗口。

	说明: 这个调用WINDOWS API SetWindowPos,但修改了一下,除了第一个参数之个,其它的都是可选的(使用默认值).
		hWndInsertAfter  窗口排序
		x,y 窗口位置.
		cx,cy 窗口大小.
	例子:
	   1.SetWindowpos(GetForegroundWindow(),-1) 当前窗口置顶
	   2.SetWindowpos(GetForegroundWindow(),0,100,200) 把当前窗口移到屏幕100,200处

>参考资料:  
	1. <http://baike.baidu.com/view/1080349.htm>   
	2. <http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545(v=vs.85).aspx> 
    
    
ALMRunEvent
===============
事件发生时，ALMRun 会去调用定义的事件处理函数。例如：ALMRun 关闭（或重新载入配置时）时会调用一个名为 onClose 的函数。addEventHandler 用于为某个事件添加事件处理函数。
我们可以在 ALMRun 的基本配置中找到如下代码：
```lua
  addEventHandler('onClose', function()
      ...
  end)

  addEventHandler('onUndefinedCommand', function(commandName, commandArg)
      ...
  end)
```
ALMRun 已有的事件如下:
[onClose]: #1-onclose
### 1. onClose

	ALMRun 关闭或者重新载入配置时触发
	onClose()

[onUndefinedCommand]: #2-onundefinedcommand
### 2. onUndefinedCommand

	执行 ALMRun 命令时，出现未定义命令时触发
	onUndefinedCommand(commandName, commandArg)

Other 其它说明
==========
1. 内置环境变量
    * ALMRUN_HOME	(ALMRUN程序所在目录)
    * ALMRUN_DRIVE 	(ALMRUN程序所在磁盘比如C:)
    * ALMRUN_ROOT	(ALMRUN默认根目录,可用config和ROOT参数修改)
    * ALMRUN_EXPLORER	(默认文件管理器,使用了config的Explorer参数之后有效)
    * Desktop		当前用户桌面文件夹路径
    * Programs		当前用户程序文件夹路径
    * CommonDesktop	所有用户(All Users)桌面文件夹路径
    * CommonPrograms 	所有用户(All Users)程序文件夹路径
2. 关于文件路径说明
    以下表达的都是同一个意思,:

		"C:\\WINDOWS"
		[[c:\WINDOWS]]
		[=[C:\WINDOWS]=]
		[==[c:\windows]==]
		第一个比较常见,由于Windows的文件路径是以"\"分隔的,但"\"以LUA脚本中是一个转义符,所以在使用中需要用两个"\"即"\\"
		第2,3,4个不会转义里面的字符内容.建议使用.
3.以下窗口操作的window参数都可以为0或空值,这时自动使用前台窗口
>
[CloseWindow],[GetWindowText],[SetWindowText],[GetWindowSize], [SetWindowSize],[GetWindowPosition],[SetWindowPosition]

[CSIDL]: CSIDL.TXT "Windows 系统特殊文件夹CSIDL参考"
[HOT_KEY]: key_string.txt "部份热键字符表,像A-Z之类的直接使用字符本身表示"
