add_library(glad "src/glad.c")
target_include_directories(glad PUBLIC "include")
set_property(TARGET glad PROPERTY POSITION_INDEPENDENT_CODE ON)