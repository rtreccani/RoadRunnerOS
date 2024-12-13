add_library(memory memory_simple_allocator.c pages.s pages.c)
target_include_directories(memory PUBLIC .)
target_include_directories(memory PRIVATE private)
target_link_libraries(memory basic_framebuffer)