# luatml

a simple lua static html generator.


for a quick introduction, head over to the [quickstart](#quickstart) section!


## rationale

for most of my use-cases the web development tools i encountered are not pleasant to work with
and turn web development into a surprisingly burdensome task that requires me to spend
more time learning and fighting the tools rather than focussing on the content itself.
that i perceive as time wasted.


## what is `luatml`

[luatml](https://github.com/numpad/luatml) is...
 - a *lua library* and domain-specific language to write html, with handy utility functions.
 - a *cli tool* to translate this "lua-html" to static html.
 - and a small set of *guidelines, recommendations and examples* 


## quickstart



```lua
require 'luatml'

luatml_registertags()

return html {
    head {
        title "demo - luatml",
        meta { charset="utf-8", content="width=device-width, initial-scale=1" },
        luatml_tag"style" [[
            .text-gray {
                color: gray;
            }
        ]]
    },
    body {
        h1 "Hello, World!",
        p {
            "Today's a beautiful day.",
            class="subtitle text-gray",
        }
    }
}
```

