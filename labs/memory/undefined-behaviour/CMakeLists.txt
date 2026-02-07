add_library(vs_memory_common INTERFACE)
target_include_directories(vs_memory_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/common)

add_executable(vs_mem_allocation allocation/allocation_bench.cpp)
target_link_libraries(vs_mem_allocation PRIVATE vs_memory_common)

add_executable(vs_mem_layout layout/aos_soa_bench.cpp)
target_link_libraries(vs_mem_layout PRIVATE vs_memory_common)

add_executable(vs_mem_access access/access_patterns_bench.cpp)
target_link_libraries(vs_mem_access PRIVATE vs_memory_common)

if (VS_ENABLE_UB_DEMOS)
  add_executable(vs_mem_ub undefined_behavior/ub_demo.cpp)
  target_link_libraries(vs_mem_ub PRIVATE vs_memory_common)
  # Intentionally do NOT add sanitizers here. Let the user choose.
endif()
