# luatml

a simple & minimalistic lua web framework.

## example

the following example demonstrates responding with
a very basic web page.

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

