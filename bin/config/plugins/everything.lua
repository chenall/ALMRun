--everything插件查询文件功能
RegisterPlugin('command',{name="EV",func=function(commandName,...)
    local t_command = {}
    if commandName:len() > 0 then
        local files = Everything(commandName);
        for k,v in pairs(files) do
            local pre = "*"
            if v["TYPE"]=="FILE" then
                pre = ""
            end
            table.insert(t_command,{name=pre..v["NAME"],desc=v["PATH"],order=1090,cmd = v["FULL_PATH"]})
        end
    else
        table.insert(t_command,{name="用Everything查找文件",desc="用Everything查找文件",order=1099});
    end
    return t_command
end})
