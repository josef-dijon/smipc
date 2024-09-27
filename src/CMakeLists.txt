cmake_minimum_required(VERSION 3.28)

project("smipc")

add_library(
  smipc STATIC
  "${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer/dekkar_lock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer/atomic_spin_lock.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/shared_memory/duplex_shared_memory_file.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/$<$<PLATFORM_ID:Windows>:shared_memory/windows_shared_memory_file.cpp>"
  "${CMAKE_CURRENT_SOURCE_DIR}/$<$<PLATFORM_ID:Linux>:shared_memory/posix_shared_memory_file.cpp>"
  # "${CMAKE_CURRENT_SOURCE_DIR}/shared_memory/client.cpp"
  # "${CMAKE_CURRENT_SOURCE_DIR}/shared_memory/server.cpp"
)

target_include_directories(smipc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_property(TARGET smipc PROPERTY CXX_STANDARD 23)
