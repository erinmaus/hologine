-- Creates a library dependency for use with make_project.
--
-- 'library_name' is the library name, or names.
--
-- If this value is a string, then the library is considered to have the same
-- name across platforms, other than specific mangling dealt with by Premake
-- (i.e., .lib vs .a, lib prefix with GCC and similar).
--
-- If this value is a table, then it is considered a key-value of the target
-- operating system and the library name, such as (for OpenGL):
--
-- { linux = "GL", macosx = "OpenGL.framework", windows = "opengl32" }
return function(library_name)
	return {
		type = "library",
		name = library_name
	}
end