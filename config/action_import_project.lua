newoption {
	trigger = "import-path",
	value = "PATH",
	description = "Defines the import path for the new project"
}

local function add_import(path)
	local file = io.open("premake5_imports", "a+")
	
	file:write(path, "\n")
	
	file:close()
end

return function(path)
	path = path or _OPTIONS["import_path"]
	
	if not path then
		error("expected path to import")
	end
	
	local premake5_script_path = path .. "/premake5.lua"
	
	-- Only include the project if it has a 'premake5.lua' file at the root
	if os.isfile(premake5_script_path) then
		add_import(premake5_script_path)
	end
end
