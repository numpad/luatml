require 'luatml'
html_registertags()

local menu_entries = {
	{ name="Home", href="/" },
	{ name="Control Flow", href="/controlflow" },
}

return div {
	style="display: flex; justify-content: space-between; align-items: center; padding: 0.25em 0.5em 0.25em 0.5em; background-color: #eee;",
	a {
		"☾ luatml",
		href="/",
		style="font-size: 1.2em; text-decoration: none;",
	},
	div {
		style="display: flex; justify-content: space-evenly; gap: 0.75em;",
		html_foreach(menu_entries) {
			function (item, i, list)
				return a { item.name, href=item.href }
			end,
		},
	},
}

