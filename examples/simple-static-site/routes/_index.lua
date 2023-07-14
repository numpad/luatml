require 'luatml'
html_registertags()


local links = {
	"/contact",
	"/about",
	"/about/team",
}

return html {
	head {
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1.0" },
		title "My Website",
		style [[
			.cols-2 {
				display: flex;
				justify-content: space-evenly;
			}
		]],
	},
	body {
		require 'examples/simple-static-site/components/navbar',
		h1 "Simple Static Site",
		p "This demonstrates how to use `luatml generate`",

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
			require 'examples/simple-static-site/components/card',
			-- component with simple argument:
			require 'examples/simple-static-site/components/card' "simple text",
			-- component with complex content
			require 'examples/simple-static-site/components/card' {
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
	},
}

