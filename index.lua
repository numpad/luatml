#!/usr/bin/env luajit

require 'luatml'
math.randomseed(os.time())

html_registertags({
	"html", "head", "title", "body",
	"div", "p", "span", "h1", "ul",
	"li", "a", "form", "input",
	"meta", "style",
})

function form_login()
	return form {
		method="POST",
		--action="/auth/login",
		style="display: flex; flex-direction: column; row-gap: 4px;",
		input {
			type="text",
			placeholder="Username",
			name="username",
		},
		input {
			type="password",
			placeholder="Password",
			name="password",
		},
		input {
			type="submit",
			name="submit",
			value="Login",
		},
		html_tag"button" {
			type="button", -- stupid html. without this it would default as a submit button.
			onclick="onValidateInputs();",
			"Validate Input",
		},
	}
end

local var = "foo bar baz"
local var2 = 41
function counter()
	local ret = var2
	var2 = var2 + 1
	return a { ret, href="/post" }
end

function html_if(exp, html)
	if exp then return html end

	return ''
end
function html_if2(exp)
	return function(html)
		-- TODO: lists not working, [1] is quickfix
		if exp then return html[1] end

		return ''
	end
end

page = html {
	head {
		title "Page",
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1" },
	},
	body {
		div {
			class="my-class",
			p {
				"User data: ", var, ". Wow... nice!"
			},
			html_tag"h2" "Types:",
			321,
			"text",
			{7, "ate", 9}, -- TODO: lists not working
			div {
				"Functions?",
				ul {
					li { "41..", counter },
					li { "42..", counter },
					html_if(math.random() < 0.5,
						li { "43..", counter } ),
					html_if2(math.random() < 0.5) {
						li { "44..", counter },
					},
					li { "45..", counter },
				}
			},
			div {
				html_tag"script" [[
					document.write('Current Time:' + new Date())
				]],
				form_login,
				ul {
					li { "uri: " .. html_request().uri },
					li { "method: " .. html_request().method },
					li { io.read() },
				},
			},
		},
		html_tag"script" [[
			function onValidateInputs() {
				let name = 'Chris';
				alert('Hi, my name is ' + name);
			}
		]]
	}
}
var = 18

if os.getenv('REQUEST_METHOD') == 'POST' then
	print(html_response(html_header_redirect("/test")))
else
	print(html_response(html_header_html(), page))
end

