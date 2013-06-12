 ALMRun
=======
*******
基于 Merry svn r98.
Fork from <http://code.google.com/p/name5566-merry>


* [Merry 的特点和功能](#merry-)
* [增加或修改功能](#modify-)
* [使用方法](#usage-)
* [相关资源](#resource-)
* [编译方法](#build-)
* [更新记录](doc/update.log)
* [API 参考](doc/config_api.md)

Merry 的特点和功能
==================
******************
*	跨平台的支持，让你在所有平台下保持一致的操作方式（目前仅仅实现了 `Windows` 版本，相信不久之后 `Linux` 和 `MacOS` 版就会完成）
*	可以定义 `Merry` 命令（快捷键）来完成一些常用的操作，例如：打开目录、启动程序、打开网页等
*	通过 Merry 命令可以模拟用户的输入，例如：模拟键盘和鼠标输入
*	通过 Merry 可以轻松的管理窗口，例如：关闭窗口、最大化和还原窗口、移动窗口、隐藏窗口等
*	通过 Merry 命令可以来进行自动化的操作，例如：打开某应用程序，自动输入用户密码进行登录等
*	Merry 采用完全开放的体系，可以使用 Lua 的扩展库或者外部程序来扩展 Merry 的功能

Modify 增加或修改功能
==============
**************
#### 1. 添加命令方法修改
    command 参数改为name,增加desc(备注)和cmd(命令)参数.     
    新的命令参数:
  ```lua
	addCommand{name=命令名,desc=备注,key=热键,cmd=执行命令,func=函数}
  ```
 	其中,cmd和func二选一,优先使用cmd参数.desc和key都是可选的.
	desc用于在使用func时显示备注信息.使用cmd参数时desc没有效果.

#### 1. 支持自动扫描目录添加命令
    scan_dir(PATH,SPEC,SUB)
>   	PATH  要扫描的目录
    SPEC   过滤
          例: *.exe(所有.exe的文件),k*.exe(所有以k开头的exe文件),多个条件使用"|"分隔,"*.exe|*.txt"
    SUB   子目录级别,默认(-1)
          -1 不限.
          0  不管子目录
          N  N级子目录
```lua
  	scan_dir("E:\\绿色软件","*.exe")
```

#### 3.  支持直接调用`ALTRUN`的配置文件(部份功能不可用)
> 
```lua
	altrun_config('ShortCutList.txt')
```
	提示: 可以ALTRUN的配置文件复制到ALMRun.exe目录下,使用上面的命令,或指定路径(同上)

#### 4. 可以进行各种配置([config](doc/config_api.md#26-config)命令)

#### 5.类似于altrun的功能
	1). 按Ctrl+D打开对应命令的文件夹
	2). 可以使用Alt+N 快速执行匹配列表中的命令

#### 6. 命令参数修改
	选定命令之后按Tab进入参数模式,输入完成按回车

#### 7. message支持标题
```lua
	message(message,title)
	--或兼容原版
	message(message)
```
#### 8. 界面修改
	1).命令列表底部新增显示对应命令的命令行或备注(优先显示备注信息)
	2).托盘菜单中文化.
	3).支持鼠标点击执行(原版不支持)
	4).其它小修改.

#### 9. 支持命令排序
	1).默认排序算法过程
	   * 排序值越大排在越前面(一般是指运行次数)
	   * 前缀匹配靠前
	   * 命令名称正向排序

	2).提供两个接口GetCmdOrder和SetCmdOrder允许自定义排序.

#### 10. 支持系统环境变量扩展

#### 11. 支持使用拼音检索(使用中文区位表查询法,只支持GB2312,可能会有一些错误,欢迎报错).
	像比如命令列表中有"宽带连接",则可以输入"KDLJ".

#### 12. 为方便使用,增加以管理员权限运行符号和隐藏执行符号
	前置'@' 隐藏执行,
	前置'>' 请求管理员权限有(NT6或以上有效)
	也可以组合使用
	例子(以管理员权限隐藏执行): addCommand{ name="test", cmd=[[>@notepad.exe]]}

#### 13. 新增支持INI文件格式配置,添加删除命令更加简单方便

#### 14. 更多功能请参考[API 介绍](doc/config_api.md)

Usage 使用方法:
=========
*******************************
#### 1. 直接运行ALMrun.exe就可以开始使用了,默认已经配置了自动扫描开始菜单和桌面程序
    默认热键说明:
	1.Alt+R 显隐本程序热键(在ALMRUN.INI中可修改)
	2.Alt+H 显示或隐藏当前前台窗口(在common.lua中,不用可删除)
	3.Alt+M 最大化/恢复当前窗口
	4.Alt+T 打开CMD窗口
    文件说明:
	ALMRun.exe 主程序
	ALMRun.ini 新版配置文件(优先使用Config目录下的ALMRun.ini文件)
	LuaEx\ALMRun.lua Lua扩展,基本文件不可删除
	LuaEx\base.lua 不可删除,基本功能扩展
	Config	    用户配置文件目录,这个目录里面的lua文件(包括子目录)会被自动调用,属于高级扩展
	Config\ALMRun.ini 用户INI配置
	config\order.txt  命令排序自动产生的
	config\histroy.lua 不存在的命令记录也会自动生成
	config\sample	LUA扩展使用例子


    配置说明:
	打开ALMRun.ini里面有详细的介绍.
	

#### 2. 添加命令(高级方法)(用记事本打开my_conf.lua)
```lua
  addCommand{ name=命令名, desc=备注, key=热键, cmd=执行命令, func=函数}
```
   *  基本命令添加方法:

  ```lua
	addCommand{ name = "test_cmd", desc = "命令测试",cmd = "cmd.exe /c pause >nul|echo 参数:"}
  ```
   * 使用函数添加命令:

  ```lua
	addCommand{ name = "test_func", desc = "命令函数测试",func = function(arg,id,flags)
		message('测试',"运行test命令,参数:"..arg..',命令ID:'..id)
	end}
  ```
   * 自动扫描指定目录下的程序

     以下就可以自动扫描`e:\绿色软件`目录下(包括子目)所有EXE文件到命令列表中:
```lua
	scan_dir("E:\\绿色软件","*.exe")	
	--不包括子目录的情况
	scan_dir("E:\\绿色软件","*.exe",0)
```
   * 如果您有用过ALTRUN,则可以直接使用ALTRUN的配置文件.例:
```lua
	altrun_config("E:\\ALTRun\\ShortCutList.txt")
```

#### 3. 更多功能请查看[原版说明](http://code.google.com/p/name5566-merry/)或[API命令介绍](doc/config_api.md)
		
*******************************
Resource 相关资源:
=========
* 原版: <http://code.google.com/p/name5566-merry/>  
* 修改: <https://github.com/chenall/ALMRun>  
* 介绍: <http://chenall.net/post/alrun-merry/>  

[Merry]:http://code.google.com/p/name5566-merry/

Build 编译方法
===================
	cd Build
	cmake ..
	ALMRun.sln

Requirement(环境)
======================
	vs2010 sp1
	cmake >= 2.8

