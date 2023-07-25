require 'luatml'
html_registertags()

return html {
	head {
		meta { charset='utf-8' },
		meta { name='viewport', content='width=device-width, initial-scale=1.0' },
		title "Christian Schäl – Softwareentwickler",
	},
	body {
		h1 "schäl.com",
		p "Softwareentwickler",
	},
}

