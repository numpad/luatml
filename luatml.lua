
function html_tag(name)
	return function(data)
		return { tag = name, data = data}
	end
end

-- helpers

function html_if(exp)
	return function(html)
		if exp then return html end

		return ''
	end
end

-- router

function html_route_parse(uri)
	return {
	}
end

function html_router(request, routes)
	
end

-- api

function html_registertags(tags)
	for i, name in ipairs(tags) do
		_G[name] = html_tag(name)
	end
end

function html_response(header, body)
	response = ''

	for k, v in pairs(header) do
		response = response .. string.format('%s: %s\n', k, v)
	end
	response = response .. '\n'

	if type(body) == 'table' and body.tag ~= nil then
		response = response .. html_tostring(body)
	end

	return response
end

-- headers
function html_header_html()
	return {
		["Status"] = "200 Ok",
		["Content-Type"] = "text/html",
	}
end

function html_header_redirect(url)
	return {
		["Status"] = "302 Found",
		["Location"] = url,
	}
end

-- request data
function html_request()
	return {
		method      = os.getenv('REQUEST_METHOD'),
		contenttype = os.getenv('CONTENT_TYPE'),
		query       = os.getenv('QUERY_STRING'),
		uri         = os.getenv('REQUEST_URI'),
		cookies     = os.getenv('HTTP_COOKIE'),
		useragent   = os.getenv('HTTP_USER_AGENT'),
		remoteip    = os.getenv('REMOTE_ADDR'),
		remoteport  = os.getenv('REMOTE_PORT'),
		protocol    = os.getenv('SERVER_PROTOCOL'),
	}
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

