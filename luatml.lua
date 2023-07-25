
function html_tag(name)
	return function(data)
		return { tag = name, data = data}
	end
end

-- helpers

-- luatml_if(is_debug_enabled) { button "Show secrets" }
function html_if(exp)
	return function(html)
		if exp then return html end

		return ''
	end
end

-- luatml_ifelse(is_debug_enabled) { p "Using dev db" } { p "Using prod db" }
function html_ifelse(exp)
	return function(html)
		return function(htmlelse)
			if exp then return html end

			return htmlelse
		end
	end
end

-- luatml_foreach{"Foo", "Bar", "Baz"} { p "Found:", function(item, i, list) return p { item .. " at " .. i } end }
function html_foreach(list)
	return function(html)
		local result = {}
		for i, listitem in ipairs(list) do
			for j, tag in ipairs(html) do
				if type(tag) == "function" then
					-- TODO: deep search, or write "context"
					--       to html tag. and during _tostring()
					--       search for "nearest" context for functions.
					table.insert(result, function ()
						return tag(listitem, i, list)
						end)
				else
					table.insert(result, tag)
				end
			end
		end

		return result
	end
end

-- router

function html_route_parse(uri)
	if type(uri) ~= 'string' then
		return nil
	end

    local parsedUri = {}
    local path, query = uri:match("([^?]*)%??(.*)")

    -- Parse the path
    parsedUri.path = {}
	if path ~= nil then
		for segment in path:gmatch("[^/]+") do
			table.insert(parsedUri.path, segment)
		end
	end

    -- Parse the query parameters
	parsedUri.query = {}
	if query ~= nil then
		for key, value in query:gmatch("([^=&]+)=([^=&]*)") do
			parsedUri.query[key] = tonumber(value) or value
		end
	end

    return parsedUri
end

function html_route_match(path, path_index, routes)
	local dir = path[path_index]
	if dir == nil then
		dir = "_"
	end

	local route = routes[dir]
	
	if type(route) == "table" then
		return html_route_match(path, path_index + 1, route)
	end

	return route
end

function html_router(request, routes)
	if routes == nil then
		return nil
	elseif type(routes) ~= 'table' then
		return nil
	end

	return html_route_match(request.uri.path, 1, routes)
end

-- api

function html_registertags(tags)
	if tags == nil then
		return html_registerdefaulttags()
	end

	for i, name in ipairs(tags) do
		_G[name] = html_tag(name)
	end
end

function html_registerdefaulttags()
	html_registertags({
		"html",
		"head", "title", "meta", "style", "script", "link",
		"body",
		"div", "img",
		"p", "span", "b", "i", "h1", "h2", "h3", "h4", "h5",
		"ul", "ol", "li", "a",
		"form", "input", "button",
	})
end

-- converts a `html_tag`-tree into plaintext HTML.
-- example:
-- 
-- local variable = 42
-- local page = html {
--   h1 "Hello World",
--   div {
--     p "Here is some data: ", variable, "!",
--     p {
--       "Danger",
--       class = "text-warning",
--     }
--   },
--   html_tag"script" [[
--     let name = "Chris";
--     alert(`Hi, i'm ${name}!`);
--   ]]
-- }
--
function html_tostring(html)
	if type(html) == 'string' then
		return html
	elseif type(html) == 'number' then
		return string.format("%g", html)
	elseif type(html) == 'function' then
		return html_tostring(html())
	elseif type(html) == 'table' and html.tag == nil then
		-- this is a normal array
		result = ''
		for index, v in ipairs(html) do
			result = result .. html_tostring(v)
		end
		return result
	elseif type(html) == 'table' and type(html.tag) == 'string' then
		-- this is a html tag

		-- attributes
		local attrs = ''
		if type(html.data) == 'table' then
			for name, value in pairs(html.data) do
				if type(name) ~= 'number' then
					attrs = attrs .. ' '
					local val = ''
					if type(value) == 'string' then
						val = value
					elseif type(value) == 'number' then
						val = tostring(value)
					end
					attrs = attrs .. string.format('%s="%s"', name, val)
				end
			end
		end

		-- tag itself
		local result = string.format('<%s%s>', html.tag, attrs)
		
		if type(html.data) == 'table' then
			for tag, value in ipairs(html.data) do
				result = result .. html_tostring(value)
			end
		elseif type(html.data) == 'string' then
			result = result .. html.data
		elseif type(html.data) == 'number' then
			result = result .. tostring(html.data)
		end
		
		return result .. string.format('</%s>', html.tag)
	end

	return nil
end

