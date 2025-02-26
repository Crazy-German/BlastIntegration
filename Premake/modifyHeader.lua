local function modifyHeader(headerFilePath, searchString, replaceString)
	local input = io.open(headerFilePath, "r")
	if not input then
		print("Could not open input")
		return
	end
	local content = input.read("*all")
	input.close()

	content = content:gsub(searchString, replaceString)

	local outputFile = io.open(headerFilePath, "w")
    if not outputFile then
        print("Could not open the file for writing: " .. headerFilePath)
        return
    end

    -- Write the modified content back to the header file
    outputFile:write(content)
    outputFile:close()

    print("Modified header file: " .. headerFilePath)
end

if #arg ~= 3 then
    print("Usage: lua ModifyHeader.lua <header_file_path> <search_string> <replace_string>")
else
    modifyHeader(arg[1], arg[2], arg[3])
end