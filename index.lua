#!/usr/bin/env luajit

require 'luatml'
html_registerdefaulttags()
local req = html_request()

local res_body = html_router(req, {
	_ = el_page,
	login = el_loginform,
	upload = do_upload,
	api = {
		workouts = {
			_ = el_allworkouts,
			["<id>"] = el_workout, -- TODO: pass id through request?
		},
	}
	--[[
	-- QoL: shortcuts for request methods
	shortcut = { GET=el_page, POST=do_something() }
	]]
})

-- helper

function is_loggedin()
	return false
end

-- api

function do_upload()
	return p {
		"uploaded: ",
		tostring(request.body),
	}
end

-- frontend

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
				style="display: flex; justify-content: between;",
				a {
					href="/cgi-bin/demo.lua",
					"→ see demo",
				},
				html_ifelse(is_loggedin()) {
					b"Hi User!"
				} {
					a {
						href="/cgi-bin/auth.lua",
						"Login"
					}
				}
			},
			form {
				style="display: flex; flex-direction: column;",
				["hx-encoding"] = "multipart/form-data",
				["hx-post"] = "/cgi-bin/upload.lua",
				input {
					type="file",
					name="file",
				},
				button {
					"Upload",
				},
				html_tag"progress" {
					value="0",
					max="100",
					id="progress",
				},
				script [[
					htmx.on('#form', 'htmx:xhr:progress', function(evt) {
						htmx.find('#progress').setAttribute('value', evt.detail.loaded/evt.detail.total * 100)
					});
				]],
			},
			{ res_body },
			p {
				"cookie: ", req.cookie
			},
		}
	}
end

if req.method == 'PUT' then
	header = html_header_html()
	print(html_response(header, b {req.body} ))
elseif req.method == 'POST' and req.uri.path[#req.uri.path] == "upload.lua" then
	header = html_header_html()
	print(html_response(header, b { req.body } ))
else
	print(html_response(html_header_html(), el_page()))
end

