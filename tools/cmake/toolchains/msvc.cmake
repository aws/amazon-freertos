if("${CMAKE_GENERATOR}" MATCHES "^Visual Studio.+")
    set(CMAKE_GENERATOR_PLATFORM Win32 CACHE INTERNAL "")
else()  # Support using Ninja generator with MSVC compiler. Needs to run with vcvarsall.bat
    set(CMAKE_C_COMPILER cl.exe CACHE FILEPATH "C compiler")
    set(CMAKE_CXX_COMPILER cl.exe CACHE FILEPATH "C++ compiler")

    # Disable compiler checks.
    set(CMAKE_C_COMPILER_FORCED TRUE)
    set(CMAKE_CXX_COMPILER_FORCED TRUE)
endif()
