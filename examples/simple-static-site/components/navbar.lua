require 'luatml'
html_registertags()

return div {
	style="display: flex; justify-content: space-between; background-color: #eee;",
	a {
		"lautml",
		href = "/",
		style = "font-size: 1.2em; text-decoration: none;",
	},
	div {
		style="display: flex; justify-content: space-evenly;",
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


