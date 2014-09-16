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
            table.insert(t_command,{name=pre..v["NAME"],desc=v["PATH"],order=1090,cmd = 'Explorer /select, "'..v["FULL_PATH"]..'"'})
        end
    else
        table.insert(t_command,{name="用Everything查找文件",desc="用Everything查找文件",order=1099});
    end
    return t_command
end})
--查询Docset插件，功能参考自Wox插件
RegisterPlugin('command',{name="DOC",func=function(commandName,...)
    local t_command = {}
    if commandName:len() > 0 then
        local sql = require "ljsqlite3"
        local path = ALMRUN_HOME.."Docset"
        if DirExists(path) then
            local listdir = ListDir(path,"docSet.dsidx",3)
            --table.insert(t_command,{name="查询文档",desc="查询文档",order=1099});
            for k,v in pairs(listdir) do
                local htmldir = string.gsub(v,"\\","/")
                local docdir = string.gsub(htmldir,"docSet.dsidx","")
                local lan = string.sub(v,string.len(path)+2,string.find(v,"\\Contents\\Resources")-1)
                lan = string.gsub(lan,".docset","")
                local dbfile = v
                local conn = sql.open(dbfile)
                local checktb = conn:exec("select * from sqlite_master where name = 'searchIndex'")
                local sql = "select * from searchIndex where name like '%"..commandName.."%' order by lower(name) asc, path asc limit 30"
                if checktb ==nil then
                    sql = "select ztokenname as name, zpath as path from ztoken join ztokenmetainformation on ztoken.zmetainformation = ztokenmetainformation.z_pk join zfilepath on ztokenmetainformation.zfile = zfilepath.z_pk where (ztokenname like '%"..commandName.."%') order by lower(ztokenname) asc, zpath asc limit 30"
                end
                local arr = conn:exec(sql)
                if arr~=nil then
                    for i=1,#arr["name"] do
                        table.insert(t_command,{name=arr["name"][i],desc=lan,order=1090,cmd = function(arg) shellExecute('"'..docdir..'Documents/'..arr["path"][i]..'"', '', '')
                        end
                        })
                    end
                end
            end
        end
    else
        table.insert(t_command,{name="DOC",desc="查询文档",order=1099});
    end
    return t_command
end})