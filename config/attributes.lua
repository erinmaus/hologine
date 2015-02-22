-- Attributes used to build projects.
local Attributes = {}

-- Imports the platform headers and implementation into the project.
-- Expects that the platform headers and implementation are located in a directory
-- above 'root_path':
--
-- root_path == code/hologine_library/hologine
--
-- So when building on Windows, the path to the Windows headers and implementation
-- would be...
--
-- impl_path == code/hologine_library/windows
function Attributes.import_platform_impl(platform)
	platform = platform or os.get()
	
	return function(project_name, project_target_name, root_path)
		local root_path_platform = root_path .. "/../" .. platform
		
		includedirs { root_path_platform }
		files { root_path_platform .. "/**.cpp", root_path_platform .. "/**.hpp" }
	end
end

-- Makes a project a static library if 'value' is true.
function Attributes.is_library(value)
	return function()
		if value then
			kind "StaticLib"
		end
	end
end

-- Makes a project a console app if 'value' is true.
function Attributes.is_console_app(value)
	return function()
		if value then
			kind "ConsoleApp"
		end
	end
end

-- Makes a project a windowed app if 'value' is true.
function Attributes.is_windowed_app(value)
	return function()
		if value then
			kind "WindowedApp"
		end
	end
end

return Attributes
