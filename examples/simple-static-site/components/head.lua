require 'luatml'
html_registertags()

return function (extra)
	return head {
		meta { charset="utf-8" },
		meta { name="viewport", content="width=device-width, initial-scale=1.0" },
		title "My Website",
		style [[
			.cols-2 {
				display: flex;
				justify-content: space-evenly;
			}
		]],
		extra or nil,
	}
end

