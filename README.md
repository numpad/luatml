# luatml

a simple lua static html generator.


for a quick introduction, head over to the [quickstart](#quickstart) section!


## rationale

for most of my use-cases the web development tools i encountered are not pleasant to work with
and turn web development into a surprisingly burdensome task that requires me to spend
more time learning and fighting the tools rather than focussing on the content itself.
that i perceive as time wasted.


## what is `luatml`

 - a *lua library* and domain-specific language to write html, with handy utility functions.
 - a *cli tool* to translate this "lua-html" to static html.
 - and a small set of *guidelines, recommendations and examples* 


## quickstart

there are [a few ways](#features) to use `luatml`, all of them boil down to the core feature:
*converting lua to html*. this example does focus only on that part.

 1. download and compile[1] the luatml cli program

```bash
$ git clone https://github.com/numpad/luatml.git
$ cd luatml/
$ make
```

 3. create a new lua file `helloworld.lua` and copy the code below.
 4. translate the file to html and save it: `./luatml build helloworld.lua > index.html`.
 5. view the `index.html` in a browser.

    [1]: compiling luatml requires a C99 compliant compiler and lua.

```lua
-- helloworld.lua

require 'luatml'
luatml_registertags()

return html {
    head {
        meta { charset="utf-8" },
        title "a very simple website",
        style [[
            .subtitle {
                color: #444;
            }
        ]]
    },
    body {
        h1 "hello, world!",
        p {
            "there's not much more to luatml :)",
            class = "subtitle"
        },
    }
}

```


## syntax

`luatml` is plain-old lua, mostly just making use of the languages optional parenthesis.

| HTML                              | luatml translation                   |
|-----------------------------------|--------------------------------------|
| `<p>Hello World</p>`      | `p "Hello World"`, `p("Hello World")` or `p { "Hello World" }` |
| `<p style="color: red;">Hello World</p>` | `p { "Hello World", style="color: red;" }` |
| `<button hx-post="/clicked">Click me</button>` | `button { "Click me", ["hx-post"]="/clicked" }` |
| `<ul><li>first</li><li>second</li></ul>` | `ul { li "first", li "second" }` |
| `<MyCustomTag></MyCustomTag>` | `luatml_tag"MyCustomTag" {}` |

