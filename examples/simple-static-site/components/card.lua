require 'luatml'
html_registertags()

return function(inner) return div {
	style="background-color: tomato; border-radius: 1em; border: 1px solid #ccc; flex-grow: 1; padding: 1em 0.5em 1em 0.5em;",
	inner or span { "-- placeholder --", style="font-style: italic; opacity: 50%;" },
} end

