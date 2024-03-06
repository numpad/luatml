require 'luatml'
html_registertags()

return html {
	head {
		meta { charset='utf-8' },
		meta { name='viewport', content='width=device-width,initial-scale=1.0' },
		title "luatml+htmx test",
		script { src='https://unpkg.com/htmx.org@1.9.10' },
	},
	body {
		h1 "hello htmx",
		div {
			h2 "news:",
			div { id='news-feed' },
			button {
				"refresh",
				['hx-get']='/news.html',
				['hx-target']='#news-feed',
				['hx-swap']='innerHTML',
			},
		},
		div {
			h2 "create post",
			form {
				['hx-post']='/news.html',
				['hx-target']='this',
				['hx-swap']='outerHTML',
				input { type='text', placeholder="title...", name='title' },
				html_tag"textarea" { name='content', rows='4' },
				button {
					"create post",
				},
			},
		},
	},
}

