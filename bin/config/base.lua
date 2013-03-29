local eventTable = {}
function addEventHandler(eventName, eventHandler)
	if not eventTable[eventName] then
		eventTable[eventName] = {}
	end

	local events = eventTable[eventName] 
	table.insert(events, eventHandler)

	if not _G[eventName] then
		_G[eventName] = function(...)
			for _, _eventHandler in ipairs(events) do
				_eventHandler(...)
			end
		end
	end
end

function enterString(str)
	local t = {}
	str:gsub('.', function(c)
		table.insert(t, c)
	end)
	enterKey(table.unpack(t))
end
