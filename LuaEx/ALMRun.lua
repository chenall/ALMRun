ALMRUN_CONFIG_PATH = ALMRUN_HOME .. "config\\"

--LUA错误捕获函数
function error_hook(msg)
    message(msg:gsub("^(.+):(%d+):(.+)$","文件：%1\n错误行 [%2] 以后的内容将被忽略\n%3"),"Configure failed")
end

--使用luajit的xpcall来调用可以防此由于意外错误导致程序崩溃
function DoFile(file)
    xpcall(dofile,error_hook,file)
end

if FileExists('LuaEx/base.lua') then --基本库
    DoFile('LuaEx/base.lua')

    --读取用户扩展库
    local Files = ListDir(ALMRUN_CONFIG_PATH,"*.lua",-1)
    if Files == nil then 
	return
    end
    for key,value in pairs(Files) do  
	DoFile(value)
    end
else
    MessageBox("LUA基本库LuaEx\\base.lua不存在,将禁用LUA扩展功能","LUA扩展")
end

if FileExists('LuaEx/update.lua') then
    DoFile('LuaEx/update.lua')
end