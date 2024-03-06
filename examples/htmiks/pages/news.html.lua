require 'luatml'
html_registertags()

if POST then
	return div {
		h2 { POST.title },
		p { POST.content },
		span {
			"~ ",
			os.date("%Y-%m-%d %H:%M:%S"),
		},
	}
end

return ul {
	li "hello world",
	li "no cool news",
	li "more at 11:30",
}

