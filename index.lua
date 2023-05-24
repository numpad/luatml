#!/usr/bin/env lua

require 'luatml'

html_registertags({"html", "head", "title", "body", "div", "p", "span", "h1", "ul", "li", "a", "form", "input" })

function form_login()
	return form {
		method="POST",
		action="/auth/login",
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

page = html {
	head {
		title "Page",
		html_tag"meta" { charset="utf-8" }
	},
	body {
		div {
			class="my-class",
			p {
				"User data: ", var, ". Wow... nice!"
			},
			div {
				"Functions?",
				ul {
					li { counter },
					li { counter },
				}
			},
			div {
				html_tag"script" [[
					document.write('Current Time:' + new Date())
				]],
				form_login,
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


print(html_tostring(page))

