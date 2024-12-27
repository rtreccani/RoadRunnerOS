add_library(tasks tasks.c tasks.s)
target_include_directories(tasks PUBLIC .)
target_link_libraries(tasks basic_framebuffer memory)