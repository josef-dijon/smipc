# CMakeList.txt : CMake project for shared_memory_experiments, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("smipc")

# Add source to this project's executable.
add_library(smipc STATIC
	"src/abstract_shared_memory_file.cpp"
	"src/duplex_shared_memory_file.cpp"
	"src/windows_shared_memory_file.cpp"
	"src/server.cpp"
	"src/client.cpp"
)

target_include_directories(smipc PRIVATE include)

set_property(TARGET smipc PROPERTY CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
