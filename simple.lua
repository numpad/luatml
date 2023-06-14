
require 'luatml'
html_registertags()

return html_tostring(html {
	head {
		title "simple page",
	},
	body {
		h1 "hello, world!",
		p "what a beautiful day it is!",
	}
})

