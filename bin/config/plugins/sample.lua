--[[
插件基本语法格式:

RegisterPlugin('command',{name=名称,func=function(参数,...)
--目前只支持一个参数就是输入的参数内容
--插件内容
--插件内容
--为了在ALMRun中显示一个你通过插件增加的命令需要返回一个 命令或命令组
--例子
--1. 返回单个命令:
-- return {name=名称1,desc="ALMRun 运行插件",cmd = commandName,order=-1}
--2. 返回多个命令:
-- local t_command = {}
-- table.insert(t_command,{name=名称1,desc=注释,cmd=命令行,order=1099});
-- table.insert(t_command,{name=名称2,desc=注释,cmd=命令行2,order=1099});
-- return t_command
end})

注: 关于命令可以参考ALMRun的API函数addCommand(doc\config_api.md文件)
--]]

 RegisterPlugin('command',{name='LUA',func = function(commandName,...)
     local t_command = {name="执行LUA命令:"..commandName,desc="ALMRun 插件示范",order=1000}
     if commandName:len() > 1 then
	 t_command.cmd = function(arg)
	     MessageBox(loadstring("return "..commandName)(arg))
	 end
     end
     return t_command
 end})

--order值为负数,使得这个命令的优先级比较低,也就是保持这个命令在列表尾部
RegisterPlugin('command',{func=function(commandName,...)
    return {name="Run "..commandName,desc="ALMRun 运行插件",cmd = commandName,order=-1}
end})

--计算器插件示范，返回多个命令，通过order值对命令进行排序，值高的排前面
RegisterPlugin('command',{name="CAL",func=function(commandName,...)
    local t_command = {}
    table.insert(t_command,{name=string.format("计算: %s",commandName),desc="计算器插件",order=1099});
    local _,result = pcall(loadstring("return "..commandName))
    local nresult = tonumber(result)
    --result = tonumber(result)
    if result ~= nil then
        if math.ceil(result) == result then
            table.insert(t_command,{name=string.format("    = %d(0x%X)",result,result),desc="计算结果",order=1090})
        else
            local strresult = string.format("%f",result)
            local rightstr = string.sub(strresult,string.len(strresult),string.len(strresult)+1)
            while rightstr =="0" do
                strresult = string.sub(strresult,0,string.len(strresult)-1)
                rightstr = string.sub(strresult,string.len(strresult),string.len(strresult)+1)
            end
            table.insert(t_command,{name=string.format("    = %s",strresult),desc="计算结果",order=1090})
        end
    end
    return t_command
end})
