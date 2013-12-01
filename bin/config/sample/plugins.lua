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
RegisterPlugin('command',{name="CALC",func=function(commandName,...)
    local t_command = {}
    table.insert(t_command,{name=string.format("计算: %s",commandName),desc="ALMRun 计算器插件",order=1099});
    local _,result = pcall(loadstring("return "..commandName))
    result = tonumber(result)
    if result ~= nil then
	table.insert(t_command,{name=string.format("    = %d(0x%X)",result,result),desc="ALMRun 计算器插件",order=1090})
    end
    return t_command
end})
