-- Constructs a project 'project_name' (with target name of 'project_target_name')
-- located at 'root_path'. The project can optionally make use various 'platforms'
-- and may depend on 'deps'. Finally, various 'attributes' can be applied to
-- the project.
--
-- 'platforms' is a table of platforms this project targets. For a full range
-- of values, refer to the main Premake5 script.
--
-- 'deps' is a table of previously constructed projects or libraries
-- in order of priority (last being most important).
--
-- 'attributes' is a list of attributes to apply before configurations. Examples
-- include is_library; for a full list, refer to config/attributes.lua
--
-- The platform independent portions of the project are expected to be located
-- at 'root_path' (e.g., code/hologine_library/hologine contains all
-- platform-agnostic portions of hologine). Platform-specific implementations
-- used by the platform-agnostic portions are expected to located in their own
-- directories alongside 'root_path' (e.g., code/hologine_library/windows
-- contains all Windows-specific code).
--
-- When dependencies are used, the proper include directories are propagated.
-- This means that if Project A has a 'windows' platform, then Project B will
-- inherit the 'windows' platform as an additional include directory.
--
-- Following this projecy layout order ensures everything goes smoothly.
return function(project_name, project_target_name, root_path, platforms, deps, attributes)
	project(project_name)
		targetname(project_target_name)
		language "C++"
		files { root_path .. "/**.cpp", root_path .. "/**.hpp" }
		includedirs { root_path }
		warnings "Extra"
		
		local prefix_path = _OPTIONS["prefix"]
		if prefix_path then
			includedirs { prefix_path .. "/include" }
			libdirs { prefix_path .. "/lib" }
		end
		
		if platforms then
			for i = 1, #platforms do
				local root_platform_path = root_path .. "/../" .. platforms[i]
				includedirs { root_platform_path }
				files { root_platform_path .. "/**.cpp", root_platform_path .. "/**.hpp" }
			end
		end
		
		if deps then
			for i = 1, #deps do
				if deps[i].type == "library" then
					if type(deps[i].name) == "table" then
						links { deps[i].name[os.get()] }
					else
						links { deps[i].name }
					end
				elseif deps[i].type == "project" then
					links { deps[i].name }
					includedirs { deps[i].path }
					
					if deps[i].platforms then
						for j = 1, #deps[i].platforms do
							includedirs { deps[i].path .. "/../" .. deps[i].platforms[j] }
						end
					end
				end
			end
		end
		
		if attributes then
			for i = 1, #attributes do
				attributes[i](project_name, project_target_name, root_path)
			end
		end
		
		filter "action:gmake"
			buildoptions { "-std=c++11" }

		filter "options:enable-tests"
			defines { "HOLOGINE_TESTING_ENABLED" }
		
		filter "configurations:Debug"
			objdir("build/obj/" .. project_target_name .. "/debug")
			defines { "HOLOGINE_DEBUG", "_DEBUG" }
			flags { "Symbols" }
			optimize "Off"
		
		filter "configurations:Release"
			objdir("build/obj/" .. project_target_name .. "/release")
			defines { "HOLOGINE_RELEASE", "NDEBUG" }
			flags { "LinkTimeOptimization" }
			optimize "Speed"
	
		-- Workaround for a current (last checked as of 2015/01/19) Premake5 bug
		filter "action:gmake"
			implibextension ".a"
		
		return {
			type = "project",
			name = project_name,
			path = root_path,
			platforms = platforms
		}
end
