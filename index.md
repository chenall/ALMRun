---
layout: page
title: ALMRun 快速启动软件
tags: ALMRun 启动 ALTRUN Merry 热键
tagline: 绿色实用，热键管理，程序管理，LUA扩展
---


ALMRun
=======
*******
   [ALMRun]是一个快速启动软件，小巧绿色，简单易用，随用随忘，只要开启了它，完全不用在意到它的存在，你需要的时候随叫随到。

   基本功能：　快速启动，热键管理．  
   扩展功能：　使用LUA脚本来进行功能扩展，程序内置一些常用的API可直接调用（比如模拟键盘或鼠标输入、窗口管理、自动登录等），使用[luajit]作为LUA引擎，功能强大。  
   出生日期：　2013-03-31  

   源自[Merry]，结合了[ALTRun]的优点，经过多次改进增强，现在基本上[ALTRun]上面可以实现的功能，使用`ALMRun`都可以实现了，并且`ALMRun`直接支持`ALTRun`的配置文件，并保持和`ALTRun`一样的使用使用习惯，很容易就可以直接上手。

* [基本功能](#ALMRUN_BASE)
* [扩展功能](#ALMRun_Advanced)
* [相关资源](#ALMRun_resource)
* [更新记录]
* [API 参考](https://github.com/chenall/ALMRun/blob/master/doc/config_api.md)
* [程序下载](https://github.com/chenall/ALMRun/archive/Build.zip)

<h2 id="ALMRUN_BASE"></h2>
ALMRun Base 基本功能
====================
********************
*	程序相关快捷键
    * `ALT+R`(全局)　　　　隐藏激活程序（可在参数配置中修改）
    * `ALT+L`(全局)　　　　运行上一个命令(可改)
    * `CTRL+D`　　 　　　　自动定位所选条目的文件位置，方便查找
    * `Alt+C`　　　　　　　打开参数配置界面
    * `Alt+S`　　　　　　　打开命令管理器
    * `Alt+X`　　　　　　　退出程序
    * `Tab`　　　　　　　　选定条目，并且进入输入参数模式。
    * `Ctrl+N/Alt+N`　　　N=0-9,启动列表中对应的条目
    * `Enter`　　　　　　　启动程序
    * `F1`　　　　　　　　显示关于窗口
    * `F2`　　　　　　　　编辑当前项目
*   基本操作

    输入要启动的程序名（支持任意位置匹配和中文首字母识别），筛选过虑列表清单。然后根据列表框选择要启动的程序，如果需要输入参数可按`Tab`键，否则直接启动程序。如果在配置中勾选了“允许数字快捷键“，还支持直接按数字键0-9来启动，或按空格键来启动当前选择条目。
*	其它操作
    * 程序主窗口（图标）：　右键可以进行参数配置、命令管理、退出，刷新命令等操作。
    * 列表框：右键可以对命令进行管添加、修改、删除、定位。
    * 命令管理窗口：列表框右键，编辑、删除指定命令,按`Insert`键添加命令，按`Delete`键删除当前选择命令，双击某个条目可以修改，支持拖放方式。暂不支持管理自动扫描目录。
*   支持拖放或右键发送到  
    如果选择了多个文件或目录，则不弹出窗口，自动批量添加。

*   在命令中可以使用的一些特殊参数: 这些执行时会替换为相应的内容
	* **{&#37;p}/{&#37;p+}**　　用户输入的参数*{&#37;p+}*是强制型参数，有*{&#37;p+}*的命令执行时一定要输入参数。
	* **{&#37;c}**　　　　　　剪贴板内容
	* **{&#37;wt}**　　　　　当前窗口的标题
	* **'@'/'>'/'+'**　　　　在命令中前置时有效
	>前置'@' 隐藏执行,   
	前置'>' 请求管理员权限有(NT6或以上有效)  
	前置`+` 必需输入参数
    也可以组合使用  
    例子(以管理员权限隐藏执行notepad.exe): ">@notepad.exe“

*   内置环境变量
    * `%ALMRUN_HOME%`	(ALMRUN程序所在目录)
    * `%ALMRUN_DRIVE%` 	(ALMRUN程序所在磁盘比如C:)
    * `%ALMRUN_ROOT%`	(ALMRUN默认根目录,可用config和ROOT参数修改)
    * `%ALMRUN_EXPLORER%`	(默认文件管理器,使用了config的Explorer参数之后有效)
    * `%Desktop%`		当前用户桌面文件夹路径
    * `%Programs%`		当前用户程序文件夹路径
    * `%CommonDesktop%`	所有用户(All Users)桌面文件夹路径
    * `%CommonPrograms%` 	所有用户(All Users)程序文件夹路径

*  常用配置直接使用参数配置功能即可，需要一些高级配置打开[bin/ALMRun.ini](https://github.com/chenall/ALMRun/blob/master/bin/ALMRun.ini)里面有更详细的配置介绍.

<h2 id="ALMRun_Advanced"></h2>
ALMRun Advanced 扩展功能
========================
************************
  ALMRUN使用[luajit]引擎支持使用LUA脚本进行功能扩展，只需要把你的扩展脚本放在`config`目录下即可，不限文件名，不限子目录。
  扩展能请参考[API 介绍](https://github.com/chenall/ALMRun/blob/master/doc/config_api.md)

  * 使用LUA扩展时需注意：
    * Lua脚本有区分大小写，调用API时需要注意大小写
    * 对于文件路径的正确写法如下: 详细请考config_api尾部的内容
      1. `"c:/windows/notepad.exe"`
      2. `“c:\\windows\\notepad.exe"`
      3. `[[c:\windows\notepad.exe]]`
    * 调用API时建议使用xpcall调用，可以防止由于出现错误导致后续的脚本无法运行 
      例子：添加命令，使用了xpcall如果失败时不会中止脚本运行，否则后面的脚本不会执行

```lua
	--语法xpcall(API,error_hook,API参数)
	--其中API就是你要调用的函数，error_hook，是固定的错误提示函数在ALMRun.lua中
	xpcall(addCommand,error_hook,{ name = "test",cmd = "cmd.exe /k echo test" })
```
   
<h2 id="ALMRun_resource"></h2>
Resource 相关资源
================
****************
* 原版: <http://code.google.com/p/name5566-merry/>  
* 源码: <https://github.com/chenall/ALMRun>  
* 搏客: <http://chenall.net/post/alrun-merry/>  
* 主页: <http://almrun.chenall.net>  
* altrun介绍：<http://xbeta.info/altrun.htm> 可以参考一下，使用方法基本上都是差不多的。

[Merry]:http://code.google.com/p/name5566-merry/
[ALTRUN]:https://code.google.com/p/altrun/
[ALMRUN]:http://almrun.chenall.net/
[luajit]:http://luajit.org/
[更新记录]:update_log.html
Requirement(编译环境)
======================
	vs2010 sp1
	cmake >= 2.8
	wxWidgets >=2.9.5

Build 编译方法
===================
	cd Build
	cmake ..
	ALMRun.sln

Download 程序下载
===================

ALMRUN最新编译版： <https://github.com/chenall/ALMRun/archive/Build.zip>

其它版本请从 [更新记录] 下载