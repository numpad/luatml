require 'luatml'
html_registertags()

local links = {
	"/controlflow",
	"/about",
	"/about/team",
}

return html {
	require 'components/head', -- require path is relative to the built directory
	body {
		require 'components/navbar',
		h1 "Simple Static Site",
		p "This demonstrates how to use `luatml build`",

		-- list of links, testing for-each variants
		div {
			class="cols-2",
			ul {
				function()
					local v = {}
					for i, link in ipairs(links) do
						v[#v+1] = li { a { link, href=link } }
					end
					return v
				end,
				li "end",
			},
			ul {
				html_foreach(links) {
					function (link, i, links)
						return li { a { "basic: ", link, href=link } }
					end,
					li {
						function (link, i, links)
							return a { "and nested: ", link, href=link }
						end,
					}
				},
				li "end",
			},
		},

		-- test components
		div {
			style="display: flex; gap: 1em;",
			-- component with no argument
			require 'components/card',
			-- component with simple argument:
			require 'components/card' "simple text",
			-- component with complex content
			require 'components/card' {
				style="color: blue;",
				a {
					"some link",
					href="#",
				},
				span {
					"(visit now)",
				},
			},
		},

		-- test static file
		div {
			style="display: flex; justify-content: center;",
			img {
				style="width: 100%;",
				src="/static/lenna.webp",
				alt="Any kind of image or static file works."
			},
		},

		-- test file list
		html_foreach(luatml_listfiles("src")) {function(item, i, arr)
			if item:sub(-2) ~= ".c" and item:sub(-2) ~= ".h" then
				return ""
			end

			return p {
				i, ". ", item
			}
		end}
	},
}

