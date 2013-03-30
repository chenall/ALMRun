-- 高级模式,使用LUA函数自定义命令
addCommand{ name = "test_func", desc = "命令函数测试",func = function(arg,id,flags)
	message('测试',"运行test命令,参数:"..arg..',命令ID:'..id,"aa")
end}

-- 普通模式,直接调用cmd参数指定的程序
addCommand{ name = "test_cmd", desc = "单独命令测试",cmd = "cmd.exe /c pause >nul|echo 参数:"}

-- 扫描指定目录自动添加
scan_dir("E:\\绿色软件","*.exe")

-- 调用altrun的配置
altrun_config("ShortCutList.txt")

