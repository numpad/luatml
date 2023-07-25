require 'luatml'
html_registertags()

return html {
	head {
		meta { charset="utf-8" },
		title "01 Introduction - luatml",
		link { rel="stylesheet", href="https://s3-us-west-2.amazonaws.com/colors-css/2.2.0/colors.min.css" },
		style [[
			@keyframes rainbow-animation {
				from { color: red; }
				to   { color: green; }
			}
			.rainbow {
				animation: rainbow-animation 2s infinite alternate;
			}
		]],
	},
	body {
		h1 "01 Introduction",
		p {
			class="rainbow",
			"A very basic example on how to use luatml.",
		},
	},
}

