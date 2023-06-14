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

function loggedin_message()
	local user = "Chris" -- TODO: get from post data
	return p {
		style="color: green;",
		"Welcome, ", user, "!"
	}
end

local var = "foo bar baz"
local var2 = 41
function counter()
	local ret = var2
	var2 = var2 + 1
	return a { ret, href="/post" }
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
			div {
				style=[[
					background-color: tomato; margin: 5px;
					display: flex; flex-direction: column;
					]],
				321,
				"text",
				{
					7,
					html_tag"i" "ate",
					9,
				},
			},
			div {
				"Functions?",
				ul {
					li { "41..", counter },
					li { "42..", counter },
					html_if(math.random() < 0.5) {
						li { "43..", counter },
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
					li { "uri: " .. tostring(html_request().path) },
					li { "query: " .. tostring(html_request().query) },
					li {
						"parsed: ",
						function()
							local p = html_request().uri.path
							local r = ""
							for i, v in ipairs(p) do
								r = r .. v .. (i ~= #p and " / " or "")
							end
							return r
						end
					},
					li { "method: " .. tostring(html_request().method) },
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
	return (html_response(html_header_redirect("/test")))
else
	return (html_response(html_header_html(), page))
end

