cmake_minimum_required(VERSION 3.28)

project("shared-memory")

add_library(
  shared-memory STATIC
  "src/duplex_shared_memory_file.cpp"
  $<$<PLATFORM_ID:Windows>:src/windows_shared_memory_file.cpp>
  $<$<PLATFORM_ID:Linux>:src/posix_shared_memory_file.cpp>)

target_include_directories(shared-memory PUBLIC include)
target_include_directories(shared-memory PRIVATE src)

set_property(TARGET shared-memory PROPERTY CXX_STANDARD 23)

# add_subdirectory(test)
