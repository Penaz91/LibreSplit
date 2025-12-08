function print_table(t, visited, indent)
    indent = indent or 0
    visited = visited or {}
    if visited[t] then
        print("(Circular reference)")
        return
    end
    visited[t] = true
    for key, value in pairs(t) do
        if type(value) == "table" then
            print(string.rep(" ", indent) .. key .. ":")
            print_table(value, visited, indent + 4)
        else
            print(string.rep(" ", indent) .. key .. ": " .. tostring(value))
        end
    end
end

function shallow_copy_table(t)
    local t2 = {}
    for k,v in pairs(t) do
        t2[k] = v
    end
    return t2
end

return {
    print_table = print_table,
    shallow_copy_table = shallow_copy_table
}
