RegisterPlugin('command', function(commandName)
    return {name="执行LUA命令:"..commandName,desc="ALMRun 插件示范",cmd = function(arg)
	    MessageBox(loadstring("return "..commandName)(arg))
	end}
end)

RegisterPlugin('command', function(commandName)
    return {name="run "..commandName,desc="ALMRun 运行插件",cmd=function(arg)
	shellExecute(commandName,arg)
    end
    }
end)