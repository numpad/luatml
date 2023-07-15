require 'luatml'
html_registertags()

function is_loggedin()
	return false
end

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
		}
	}
end

return el_page()

