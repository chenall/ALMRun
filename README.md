alrun
=====
Fork from http://code.google.com/p/name5566-merry

Merry 的特点和功能
==================
	跨平台的支持，让你在所有平台下保持一致的操作方式（目前仅仅实现了 Windows 版本，相信不久之后 Linux 和 MacOS 版就会完成）
	可以定义 Merry 命令（快捷键）来完成一些常用的操作，例如：打开目录、启动程序、打开网页等
	通过 Merry 命令可以模拟用户的输入，例如：模拟键盘和鼠标输入
	通过 Merry 可以轻松的管理窗口，例如：关闭窗口、最大化和还原窗口、移动窗口、隐藏窗口等
	通过 Merry 命令可以来进行自动化的操作，例如：打开某应用程序，自动输入用户密码进行登录等
	Merry 采用完全开放的体系，可以使用 Lua 的扩展库或者外部程序来扩展 Merry 的功能

增加或修改功能
=============
1.添加命令方法修改
 command参数改为name,增加desc(备注)和cmd(命令)参数.
 新的命令参数
 addCommand{name=命令名,desc=备注,key=热键,cmd=执行命令,func=函数}
 其中,cmd和func二选一,优先使用cmd参数.desc和key都是可选的.
 desc用于在使用func时显示备注信息.使用cmd参数时desc没有效果.

2.支持自动扫描目录添加命令
  scan_dir("E:\\绿色软件","*.exe")

3.支持直接调用ALTRUN的配置文件(部份功能不可用)
  altrun_config('ShortCutList.txt')
  提示: 可以ALTRUN的配置文件复制到merry.exe目录下,使用上面的命令,或指定路径(同上)

4.支持任意位置匹配,原版是从头开始匹配
  若不需要任意位置匹配请打开base.lua
  删除HookCompare相关语句块就行了.

5.类似于altrun的功能
  1.按Ctrl+D打开对应命令的文件夹(使用cmd参数时可用,否则直接运行命令)
  2.可以使用Alt+N 快速执行匹配列表中的命令

6.命令参数修改
  选定命令之后按Tab进入参数模式,输入完成按回车

7.message支持标题
  message(title,message)
  或兼容原版
  message(message)

8.界面修改
  1).命令列表底部新增显示对应命令的命令行或备注(优先显示命令行)
  2).托盘菜单中文化.


相关资源: 
========
原版: http://code.google.com/p/name5566-merry/
修改: http://chenall.net/post/alrun-merry/

Build Merry(编译)
=================
cd Build
cmake ..
merry.sln

Requirement(环境)
======================
vs2010 sp1
cmake >= 2.8

