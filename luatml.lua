
function html_tag(name)
	return function(data)
		return { tag = name, data = data}
	end
end

function html_registertags(tags)
	for i, name in ipairs(tags) do
		_G[name] = html_tag(name)
	end
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
	elseif type(html) == 'table' then
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

