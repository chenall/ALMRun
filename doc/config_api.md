Merry 的配置和 API
=================
只要稍学习一下 lua 编程语言，就可以灵活的配置 Merry。配置文件位于 config 目录中，common.lua 为 Merry 提供了默认的配置。
这里介绍一下 Merry 配置文件的 API：

### 0. addCommand{name=命令名,desc=备注,key=热键,cmd=执行命令,func=函数}
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

  可用的表示按键的字符串请参考[key_string.txt](key_string.txt)（部分按键字符串在 Windows 下无效）。

### 1. shellExecute
	用于执行一个 shell 命令
	commandName 为 shell 命令名
	commandArg 为 shell 命令需要的参数
	workingDir 为 shell 命令的工作路径
	show 表示显示的方式，包括 'normal'、'max'、'min'、'hide'（常规显示、最大化、最小化、隐藏）
	函数返回 true / false 表示是否执行是否成功
```lua
	shellExecute(commandName, commandArg, workingDir, show)
```
	
	范例，打开 cmd 窗口并且最大化
```lua
	shellExecute('cmd', '', '', 'max')
```

### 2. getForegroundWindow()
	获取前台窗口
	函数返回前台窗口

### 3. setForegroundWindow
	设置窗口 window 为前台窗口
	setForegroundWindow(window)

### 4. showWindow
	显示窗口 window
	show 表示显示的方式，包括 'normal'、'max'、'min'、'hide'、'restore'（常规显示、最大化、最小化、隐藏、还原）
	showWindow(window, show)

	范例，最大化或还原窗口
```lua
	if isWindowMax(window) then
		  showWindow(window, 'restore')
	else
		  showWindow(window, 'max')
	end
	closeWindow
```
### 5. closeWindow(window)
	关闭窗口 window
	范例，关闭前台窗口
	closeWindow(getForegroundWindow())

### 6. isWindowMax

	窗口 window 是否为最大化
	函数返回 true / false 表示 window 是否最大化
	isWindowMax(window)

### 7. isWindowMin

	窗口 window 是否为最小化
	函数返回 true / false 表示 window 是否最小化
	isWindowMin(window)

### 8. isWindowShown

	窗口 window 是否在显示
	函数返回 true / false 表示 window 是否在显示
	isWindowShown(window)

### 9. getWindowText

	获取窗口 window 的标题栏文字
	函数返回窗口 window 的标题栏文字
	getWindowText(window)

### 10. setWindowText

	设置窗口 window 的标题栏文字
	setWindowText(window, text)

### 11. getWindowSize

	获取窗口 window 的大小
	函数返回两个值 width 和 height
	getWindowSize(window)

	范例，获取前台窗口的大小
	local width, height = getWindowSize(getForegroundWindow())
	setWindowSize

### 12. setWindowSize(window, width, height)
	设置窗口 window 的大小

### 13. getWindowPosition

	获取窗口 window 的位置
	函数返回两个值 x 和 y 为窗口左上角的位置
	getWindowPosition(window)

	范例，获取前台窗口的位置
	local x, y = getWindowPosition(getForegroundWindow())

### 14. setWindowPosition

	设置窗口 window 的位置
	设置的位置为窗口左上角的位置
	setWindowPosition(window, x, y)

### 15. findWindow

	通过窗口名称查找窗口
	参数 parentWindow 为可选参数，用于表示被查找窗口的父窗口
	findWindow(name, parentWindow)

### 16. getMousePosition

	获取当前鼠标的位置
	函数返回两个值 x 和 y 为鼠标的位置
	getMousePosition()

### 17. setMousePosition

	设置当前鼠标的位置
	setMousePosition(x, y)

### 18. enterKey

	模拟敲击一个按键
	enterKey(keyStr)

	范例，模拟快捷键 Shift + F10 来打开右键菜单
	enterKey('S-F10')

### 19. message

	将用对话框输出消息
	message(str)

	范例，输出 Hello Merry
	message('Hello Merry')

### 20. enableCommandKey

	激活快捷键
	参数 keyID 由 addCommand 函数返回
	创建一个命令后，其快捷键是默认激活的
	若执行了 disableCommandKey 禁用了某个命令的快捷键，可调用此函数激活此命令的快捷键
	enableCommandKey(keyID)

### 21. disableCommandKey

	禁用快捷键
	参数 keyID 由 addCommand 函数返回
	创建一个命令后，其快捷键是默认激活的
	调用此函数可以禁用某个命令的快捷键
	disableCommandKey(keyID)

	范例，按 HOME 键禁用某快捷键
```lua
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
### 22. setTimer

	开启一个定时器
	参数 milliseconds 表示定时器每隔 milliseconds 触发一次
	参数 oneShot 表示定时器是否只触发一次
	参数 callback 为一个 Lua 函数，定时器每次触发时都会被调用
	函数返回成功创建的定时器
	setTimer(milliseconds, oneShot, callback)

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
### 23. clearTimer(timer)
	清理定时器

	如果有更多 API 需求，请给我发邮件。

### 24. GetEnv
	获取一个环境变量
	比如:
	GetEnv('WinDir') 结果 "C:\WINDOWS"

### 25. SetEnv
	设置一个环境变量
	例:
	SetEnv('test','mytest')
	设置一个变量test

### 26. config
  程序序设置
  目前支持以下设置
	1.CompareMode
	  命令匹配模式
		1 匹配开头
		2 使用lua函数HookCompre自定义匹配
		0 默认,任意位置匹配
	例:config{CompareMode=0}
	2.ROOT
		设置相对根目录.
	3.Explorer	设置默认的文件管理器(按Ctrl+D定位文件时)
	例:使用Tc config( Explorer = [[C:\totalcmd\totalcmd.exe]] }
### 27. ListDir(path,ext,sub)
	返回指定目录下的文件列表
	path  路径,
	ext	扩展名限制
	sub	子目录限制
	例: ListDir([[C:\WINDOWS]],".exe",0)
### 28. SetClipboardData(TEXT)
	设剪贴板内容为TEXT.
	GetClipboardData()
	获取剪贴板TEXT格式.获取失败时返回nil
    
Event Merry 事件
=========
我们可以在 Merry 的基本配置中找到如下代码：
```lua
  addEventHandler('onClose', function()
      ...
  end)

  addEventHandler('onUndefinedCommand', function(commandName, commandArg)
      ...
  end)
```
事件发生时，Merry 会去调用定义的事件处理函数。例如：Merry 关闭（或重新载入配置时）时会调用一个名为 onClose 的函数。addEventHandler 用于为某个事件添加事件处理函数。Merry 已有的事件如下：

### 1. onClose

	Merry 关闭或者重新载入配置时触发
	onClose()

### 2. onUndefinedCommand

	执行 Merry 命令时，出现未定义命令时触发
	onUndefinedCommand(commandName, commandArg)
Other 其它说明
==========
1. 内置变量
    * ALMRUN_HOME	(ALMRUN程序所在目录)
    * ALMRUN_DRIVE 	(ALMRUN程序所在磁盘比如C:)
    * ALMRUN_ROOT	(ALMRUN默认根目录,可用config和ROOT参数修改)
    * ALMRUN_EXPLORER	(默认文件管理器,使用了config的Explorer参数之后有效)
2. 关于文件路径说明
    以下表达的都是同一个意思,
   "C:\\WINDOWS"
   [[c:\WINDOWS]]
   [=[C:\WINDOWS]=]
   [==[c:\windows]==]
   第一个比较常见,由于Windows的文件路径是以"\"分隔的,但"\"以LUA脚本中是一个转义符,所以在使用中需要用丙个"\"即"\\"
   第2,3,4个不会转义里面的字符内容.建议使用.