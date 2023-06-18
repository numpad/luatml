
require 'luatml'
html_registertags()


if request.url == "/redirect" then
	return {
		status = 302,
		location = "/",
	}
elseif request.url == "/body" then
	return {
		status = 200,
		body = div { b"hello", "world" },
	}
end

return html {
		head {
			title "simple page",
			meta { name="viewport", content="width=device-width, initial-scale=1.0" },
		},
		body {
			h1 "hello, world!",
			p "what a beautiful day it is!",
			form {
				action="/post", method="POST",
				input {
					type="text",
					placeholder="some text",
					name="text",
				},
				input {
					type="submit",
					value="post",
				},
			},
			ul {
				li (request.version),
				li (request.method),
				li (request.url),
				li (request.body),
				html_if(request.method == "POST") {
					li { "post data:", request.body },
				},
			}
		}
	}

