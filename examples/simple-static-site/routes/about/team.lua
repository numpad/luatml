require 'luatml'
html_registertags()


return html {
	require 'examples/simple-static-site/components/head',
	body {
		require 'examples/simple-static-site/components/navbar',
		h1 "About our Team",
		p "This is just another dummy webpage.",
	},
}

