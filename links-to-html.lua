-- Improved from from https://stackoverflow.com/questions/40993488

function Link(el)

    -- do not modify links that specify a protocol, as `http:`
    if string.match(el.target, "^%a+:") then
        return el
    end

    -- replace trailing `.md` with `.html`
    el.target = string.gsub(el.target, ".md$", ".html")

    return el
end
