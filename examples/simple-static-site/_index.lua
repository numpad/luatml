require 'luatml'
html_registertags()

links = {
	"/contact",
	"/about",
	"/about/team",
}

return html {
	head {
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1.0" },
		title "My Website",
	},
	body {
		h1 "Simple Static Site",
		p "This demonstrates how to use `luatml generate`",
		ul {
			function(link)
				local v = {}
				for i, link in ipairs(links) do
					v[#v+1] = li { "link: ", link }
				end
				return v
			end,
			li "end",
		},
		ul {
			html_foreach(links) {
				function (item)
					return li {item}
				end
			},
			li "end",
		},
	},
}

