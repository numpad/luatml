require 'luatml'
html_registertags()

return div {
	style="display: flex; justify-content: space-between; align-items: center; padding: 0.25em 0.5em 0.25em 0.5em; background-color: #eee;",
	a {
		"☾ luatml",
		href = "/",
		style = "font-size: 1.2em; text-decoration: none;",
	},
	div {
		style="display: flex; justify-content: space-evenly; gap: 0.75em;",
		a {
			"Products",
			href="/products"
		},
		a {
			"About",
			href="/about"
		},
		a {
			"About: Team",
			href="/about/team"
		},
	},
}

