local function read_file(file)
	local f, e = io.open(file)

	if f then
		local s = f:read("*a")
		f:close()

		return s
	else
		return false
	end
end

local function create_interfaces()
	return {
		modules = {},
		objects = {},
		categories = {}
	}
end

local function parsing_error(...)
	error(string.format(...), 3)
end

local function parse(root, local_path, deps)
	deps = deps or {}

	local interfaces = create_interfaces()
	local pending_attrs = {}

	local _I = {}
	function _I.module(name)
		if interfaces.module ~= nil then
			parsing_error("already declared current module '%s'", interfaces.module.name)
		end

		interfaces.module = { name = name, attributes = pending_attrs }
		pending_attrs = {}
	end

	function _I.import(module)
		if not deps[module] then
			-- Mark the module as preliminarily loaded to prevent recursive imports.
			deps[module] = true

			local module_path = module:gsub(":", "/") .. ".hint"
			local s, r = parse(root, module_path, deps)

			if not s then
				parsing_error(r)
			else
				-- Complete the module.
				deps[module] = r
			end
		end
	end

	function _I.attribute(attr, arg)
		table.insert(pending_attrs, { name = attr, argument = arg })
	end

	function _I.var(field_type, field)
		if type(field_type) ~= "string" then
			parsing_error("invalid type")
		end

		if type(field) ~= "string" then
			parsing_error("invalid field name")
		end

		return { type = field_type, field = field }
	end

	function _I.category()
		-- Nop.
	end

	local function object_index(self, category)
		return function(members)
			if type(members) ~= "table" then
				parsing_error("expected member list")
			end

			local fields = {}
			for i = 1, #members do
				if fields[members[i].field] then
					parsing_error("field '%s' is already defined", members[i].field))
				end

				fields[members[i].field] = true
			end

			local t = {
				category = category,
				members = members,
				attributes = pending_attrs
			}

			pending_attrs = {}

			return setmetatable(t, { is_object = true })
		end
	end

	_I.object = setmetatable({}, { __index = object_index })

	local function global_index(self, key)
		if _I[key] then
			return _I[key]
		else
			return tostring(key)
		end
	end

	local function global_newindex(self, key, value)
		if value == _I.category then
			local category = {
				type = "category",
				attributes = pending_attrs
			}

			pending_attrs = {}
			interfaces.categories[key] = category
		else
			if type(value) ~= "table" or not getmetatable(value).is_object then
				parsing_error("expected category or object")
			end

			if interfaces.objects[key] then
				parsing_error("type '%s' already defined", key))
			end

			interfaces.objects[key] = value
		end
	end

	local _M = setmetatable({}, { __index = global_index, __newindex = global_newindex })

	local s = read_file(root .. local_path)
	if not s then
		error("could not open file '%s'", local_path))
	end

	local c, e = loadstring(s, local_path)
	if not c then
		error(e)
	end

	setfenv(c, _M)

	local r
	r, e = pcall(c)

	if not r then
		error(e)
	end

	return interfaces, deps
end

return parse
