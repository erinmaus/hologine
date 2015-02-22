return function(solution_name)
	solution(solution_name)
		configurations { "Debug", "Release" }
		location "build"
		
		filter "configurations:Debug"
			targetdir "build/bin/debug"
		
		filter "configurations:Release"
			targetdir "build/bin/release"
	
	return {
		type = "solution",
		name = solution_name
	}
end