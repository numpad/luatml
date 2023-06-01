#!/usr/bin/env luajit
require 'luatml'

html_registertags({"html", "head", "meta", "body", "div", "p"})

function compare_routes(a, b)
	-- compare path
	if #a.path ~= #b.path then return false end
	for i = 1, #a.path do
		if a.path[i] ~= b.path[i] then
			return false
		end
	end

	-- compare query
	for key, value in pairs(a.query) do
		if a.query[key] ~= b.query[key] then
			return false
		end
	end
	
	return true
end

function testcase(name)
	_ALL_TESTCASES = _ALL_TESTCASES or {}

	local test
	test = {
		name = name,
		expect = function(value)
			return {
				from = function(data)
					local wspace = "%s+"
					local expected = string.gsub(value, wspace, "")
					local actual = string.gsub(html_tostring(data), wspace, "")
					local ok = (expected == actual)
					if not ok then
						print("expected")
						print("--------")
						print(expected)
						print("actual")
						print("------")
						print(actual)
					end
					
					table.insert(_ALL_TESTCASES, {
						testcase_name = name,
						testcase_passed = ok,
					})
				end,
				from_routes = function(routes)
					local ok = true
					for i, route in ipairs(routes) do
						local expected = value[i]

						if compare_routes(route, expected) == false then
							ok = false
						end
					end

					table.insert(_ALL_TESTCASES, {
						testcase_name = name,
						testcase_passed = ok,
					})
				end
			}
		end,
	}

	return test
end

function testcase_results()
	for index, testresult in ipairs(_ALL_TESTCASES) do
		local result
		if testresult.testcase_passed then
			result = "[" .. string.char(27) .. "[32mPASS"
			  .. string.char(27) .. "[0m]"
		else
			result = "[" .. string.char(27) .. "[31mFAIL"
			  .. string.char(27) .. "[0m]"
		end

		print(result .. '  ' .. testresult.testcase_name)
	end

	_ALL_TESTCASES = {}
end

-- Actual Tests --

--- Test a very basic website.
--- TODO: implement void tags like <meta />
testcase "rendering a simple page"
  .expect [[
  	<html>
		<head>
			<meta charset="utf-8"></meta>
		</head>
		<body>
			<p>Hello, World!</p>
		</body>
	</html>
  ]]
  .from(
  	html {
		head {
			meta { charset="utf-8" }
		},
		body {
			p "Hello, World!"
		},
	})

--- Test rendering strings, numbers, lists, functions
testcase "rendering lua types"
  .expect [[
	<p style="color: red; margin: 1px;">
		foo12.3bar42baz
	</p>
  ]]
  .from(
	p {
		style="color: red; margin: 1px;",
		"foo",
		12.3,
		{ "bar", 42 },
		function() return "baz" end,

	}
  )

-- functions returning html
testcase "function rendering"
	.expect [[
		<div>
			<p>foo</p>
			<p>bar</p>
		</div>
	]]
	.from(div {
		function() return { p "foo", p "bar" } end,
	})

-- router
testcase "basic route parsing"
	.expect {
		-- index
		{ path = {}, query = {} },
		{ path = {}, query = {} },
		{ path = {}, query = { query = "param" } },
		{ path = {}, query = { query = "param" } },
		-- simple paths
		{ path = {"foo"}, query = {} },
		{ path = {"foo"}, query = {} },
		{ path = {"foo", "bar"}, query = {} },
		{ path = {"foo", "bar"}, query = {} },
	}
	.from_routes {
		-- index
		html_route_parse(""),
		html_route_parse("/"),
		html_route_parse("?query=param"),
		html_route_parse("/?query=param"),
		-- simple paths
		html_route_parse("/foo"),
		html_route_parse("/foo/"),
		html_route_parse("/foo/bar"),
		html_route_parse("/foo/bar/"),
	}

testcase_results()

