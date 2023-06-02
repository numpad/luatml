#!/usr/bin/env luajit

require 'luatml'
html_registerdefaulttags()
req = html_request()

function el_head()
	return head {
		title "text",
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1" },
		html_tag"script" { src="https://unpkg.com/htmx.org@1.9.2" },
		style [[
			body {
				background-color: white;
				display: flex;
				justify-content: center;
				align-items: center;
			}
		]],
	}
end

function el_page()
	return html {
		el_head,
		body {
			style="display: flex; flex-direction: column;",
			div {
				a {
					href="/cgi-bin/index.lua",
					"→ see demo",
				},
			},
			div {
				["hx-target"] = "this",
				["hx-swap"] = "outerHTML",
				style="display: flex; flex-direction: column;",
				span "new text...",
				html_tag"textarea" { placeholder="..." },
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

