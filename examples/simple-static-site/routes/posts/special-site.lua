require "luatml"
html_registertags()

return html_tag"article" {
	h2 "special site",
	p [[
	meta-information about a file can be stored in its filename.

	for example filenames beginning with a date are posts. these are then displayed on the "recent posts" page.
	
	all other files are not listed there, but can be linked to as they are generated as usual.
	]],
}

