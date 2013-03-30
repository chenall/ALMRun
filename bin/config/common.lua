addCommand{ key = 'A-R', func = toggleMerry }

addCommand{ key = 'A-M', func = function()
	local window = getForegroundWindow()
	if isWindowMax(window) then
		showWindow(window, 'restore')
	else
		showWindow(window, 'max')
	end
end }

addCommand{ key = 'A-Q', func = function()
	closeWindow(getForegroundWindow())
end }

local curHideWindow
addCommand{ key = 'A-H', func = function()
    if not curHideWindow then
        curHideWindow = getForegroundWindow()
        showWindow(curHideWindow, 'hide')
    else
        showWindow(curHideWindow, 'normal')
        curHideWindow = nil
    end
end }

if WINDOWS then
	addCommand{ name = 'c', func = function() shellExecute('c:', '', '', 'max') end }
	addCommand{ name = 'd', func = function() shellExecute('d:', '', '', 'max') end }
	addCommand{ name = 'e', func = function() shellExecute('e:', '', '', 'max') end }
	addCommand{ name = 'f', func = function() shellExecute('f:', '', '', 'max') end }

	addCommand{ name = 'cmd', key = 'A-T', func = function(arg)
		local window = getForegroundWindow()
		local dir = getWindowText(window)
		shellExecute('cmd', '', dir)
	end }

	addCommand{ key = 'A-Up', func = function() shellExecute('nircmdc', 'changesysvolume 65535', '', 'hide') end }
	addCommand{ key = 'A-Down', func = function() shellExecute('nircmdc', 'changesysvolume -65535', '', 'hide') end }
	addCommand{ key = 'A-Left', func = function() shellExecute('nircmdc', 'changesysvolume -4000', '', 'hide') end }
	addCommand{ key = 'A-Right', func = function() shellExecute('nircmdc', 'changesysvolume 4000', '', 'hide') end }
elseif MAC then
	addCommand{ name = 'terminal', key = 'A-T', func = function() shellExecute('/Applications/Utilities/terminal.app') end }
end

addCommand{ name = 'google',desc = "ÓÃGoogleËÑË÷", func = function(str) shellExecute('http://www.google.com.hk/search?q=' .. str) end }
addCommand{ name = 'googlecode',func = function(str) shellExecute('http://code.google.com/query/#q=' .. str) end }
addCommand{ name = 'baidu', func = function(str) shellExecute('http://www.baidu.com/s?wd=' .. str) end }
addCommand{ name = 'youku', func = function(str) shellExecute('http://www.soku.com/search_video/q_' .. str) end }
addCommand{ name = 'doubanbook', func = function(str) shellExecute('http://book.douban.com/subject_search?search_text=' .. str) end }
addCommand{ name = 'doubanmovie', func = function(str) shellExecute('http://movie.douban.com/subject_search?search_text=' .. str) end }
addCommand{ name = 'name5566', func = function(str) shellExecute('http://name5566.com/?s=' .. str) end }

