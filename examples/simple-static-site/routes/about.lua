require 'luatml'
html_registertags()


return html {
	require 'examples/simple-static-site/components/head',
	body {
		require 'examples/simple-static-site/components/navbar',
		h1 "About Site",
		p "Here we showcase how we handle index files (in short, we don't!).",

		p {
			"To learn more about us, visit the ",
			a { "About - Team", href="/about/team/", },
			" subsite!",
		},
	},
}

