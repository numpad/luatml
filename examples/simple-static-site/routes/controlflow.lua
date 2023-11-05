require 'luatml'
html_registertags()

return html {
	require 'components/head' {
		style [[
			h2::before {
				content: '#';
				margin-right: 0.5em;
				color: #888;
			}
			h2 {
				margin-top: 1.75em;
			}
			h3 {
				color: #555;
			}
			pre: {
				white-space: pre-line;
			}
		]],
	},
	body {
		require 'components/navbar',
		h1 "Control Flow",
		p "In this document, we'll go through simple control flow examples using `luatml`.",

		-- if / else
		div {
			h2 "if / else",
			p {
				style="display: flex; flex-direction: column;",
				html_tag"code" "luatml_if(condition) { ... }",
				html_tag"code" "luatml_ifelse(condition) { ... } { ... }",
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
		},

		-- foreach
		div {
			h2 "for each",
			p {
				style="display: flex; flex-direction: column;",
				html_tag"code" "luatml_foreach(items) { ... }",
			},

			h3 "code:",
			html_tag"pre" [[
ul {
    luatml_foreach{'first', 2, b"3rd" } {
        li "got an item:",
        function (item, index, list)
            return li { index .. ". " .. item }
        end
    },
}
			]],

			h3 "output:",
			div {
				html_foreach{'first', 2, b"3rd" } {
					li "got an item:",
					function (item, index, list)
						return li { index, ". ", item }
					end,
				},
			},
		},
	},
}

