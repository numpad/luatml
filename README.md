# luatml

a simple lua static html generator.


## rationale

for most of my use-cases the web development tools i encountered are not pleasant to use
and turn web development into a surprisingly burdensome task that requires me to spend
more time learning and fighting the tools rather than focussing on the content itself.
that i perceive as time wasted.


## what is this

[luatml](https://github.com/numpad/luatml) is:
 - *a lua library*, to write 
 - a cli tool
 - and a small set of guidelines to write 


## example

the 

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

