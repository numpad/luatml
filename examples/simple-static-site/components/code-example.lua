require 'luatml'
html_registertags()

-- config.title
--       .apis
return function(config)
	return div {
		h2(config.title),
		p {
			style="display: flex; flex-direction: column;",
			html_foreach(config.apis) {
				function(item)
					return html_tag"code"(item)
				end
			}
		},


		h3 "code:",
		html_tag"pre" [[
		luatml_if(true) {
			a { "This will render!", href="#" },
		},
		luatml_if(false) {
			a { "This does not show!", href="#" },
		},]],

		h3 "output: ",
		div {
			html_if(true) {
				a { "This will render!", href="#" },
			},
			html_if(false) {
				a { "This does not show!", href="#" },
			},
		}
	}
end

