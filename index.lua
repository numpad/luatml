#!/usr/bin/env luajit

require 'luatml'
html_registerdefaulttags()
req = html_request()

html_router(request, {
	_ = el_page,
	login = { GET=el_loginform, POST=do_login },
	api = {
		workouts = {
			_ = el_allworkouts,
			["<id>"] = el_workout, -- TODO: pass id through request?
		},
	},
})

function el_head()
	return head {
		title "text",
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1" },
		html_tag"script" { src="https://unpkg.com/htmx.org@1.9.2" },
		style [[
			body {
				background-color: lightsteelblue;
				display: flex;
				justify-content: center;
				align-items: center;
			}
		]],
	}
end

function el_loginform()
	return form {
		style="display: flex; flex-direction: column;",
		input {
			name="username",
			type"text",
			placeholder="Username",
		},
		input {
			name="password",
			type"password",
			placeholder="Password",
		},
		button {
			["hx-post"] = "/auth.lua",
			"Login",
		},
	}
end

function el_page()
	return html {
		el_head,
		body {
			style="display: flex; flex-direction: column;",
			div {
				a {
					href="/cgi-bin/demo.lua",
					"→ see demo",
				},
			},
			form {
				["hx-target"] = "this",
				["hx-swap"] = "outerHTML",
				style="display: flex; flex-direction: column;",
				span "new text...",
				html_tag"textarea" { name="text", placeholder="enter something...", rows=8 },
				button {
					["hx-put"] = "/cgi-bin/index.lua",
					"Save",
				},
			},
		}
	}
end

if req.method == 'POST' then
	print(html_response(html_header_redirect("/cgi-bin/demo.lua")))
elseif req.method == 'PUT' then
	print(html_response(html_header_html(), b {req.body} ))
else
	print(html_response(html_header_html(), el_page()))
end

