-- Constants relevant to building Hologine.
HOLOGINE_SOLUTION_NAME = "hologine"
HOLOGINE_PLATFORM_LIBRARY_PROJECT_NAME = "hologine_platform"
HOLOGINE_TEST_FRAMEWORK_PROJECT_NAME = "hologine_test_framework"

-- Global Hologine build configuration state.
hologine_config = {}

-- Imported modules.
hologine_config.attributes = require "config.attributes"
hologine_config.make_solution = require "config.solution"
hologine_config.make_project = require "config.project"
hologine_config.make_library = require "config.library"

-- Imported actions.
hologine_config.actions = {}
hologine_config.actions.clear_imports = require "config.action_clear_imports"
hologine_config.actions.import_project = require "config.action_import_project"

newoption {
	trigger = "enable-tests",
	description = "Enable unit testing."
}

newoption {
	trigger = "prefix",
	value = "PATH",
	description = "Additional include and library path for dependencies"
}

hologine_config.solution = hologine_config.make_solution(HOLOGINE_SOLUTION_NAME)
hologine_config.solution.platform_lib = hologine_config.make_project(
	HOLOGINE_PLATFORM_LIBRARY_PROJECT_NAME, "hologine_platform", "code/hologine_platform/hologine",
	
	-- Platforms
	{ os.get() },
	
	-- Dependencies
	nil,
	
	-- Attributes
	{ hologine_config.attributes.is_library(true) }
)

if _OPTIONS["enable-tests"] then
	-- Utility method to add a test suite; the test suite must be located in
	-- directory 'name' inside the code directory.
	local function add_test_suite(name, deps)
		-- Insert reference to Boost unit testing framework.
		table.insert(deps, hologine_config.make_library("boost_unit_test_framework"))

		hologine_config.solution[name] = hologine_config.make_project(
			name, name, "code/" .. name, nil, deps, { hologine_config.attributes.is_console_app(true) }
		)
	end

	add_test_suite("hologine_platform_tests",
			{ hologine_config.solution.platform_lib })
end

newaction {
	trigger = "import-project",
	description = "Imports a project that depends on Hologine.",
	execute = hologine_config.actions.import_project
}

newaction {
	trigger = "clear-imports",
	description = "Clears imported projects.",
	execute = hologine_config.actions.clear_imports
}

-- Import extra projects.
local imports = io.open("premake5_imports")

if imports then
	for project in file:lines() do
		dofile(project)
	end
	imports:close()
end
