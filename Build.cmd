@echo off
title ALMRun 自动编译脚本 by chenall http://almrun.chenall.net

REM 
REM 如果你的wxWidgets库没有安装在以下位置请删除下一行的REM注释，并修改为正确的位置，否则无法编译
REM set WXWIN=D:\dev\wxWidgets-3.0.0
REM

pushd %~dp0build
if not DEFINED VS100COMNTOOLS goto :END
if not exist ALMRun.sln cmake .. -DRTL=MT || goto :END
if not exist ALMRun.sln goto :END
"%VS100COMNTOOLS%\..\IDE\devenv.com" ALMRun.sln /build Release
if errorlevel 1 goto :END
copy /y Release\ALMRun.exe ..\bin\ > nul
popd
if not exist bin\doc md bin\doc
copy /y doc\*.* bin\doc\ >nul
copy /y README.md bin\doc\ > nul
copy /y README.md bin\ > nul
echo.
echo.
echo 编译成功完成，生成的文件在BIN目录下。
echo.
goto :END
:NEED_VS100
echo.
echo 系统未安装VS2010，无启启动编译。
echo.
:END
pause
