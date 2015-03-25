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