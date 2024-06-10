aux_source_directory(. SRC_LIST)

add_executable(test ${SRC_LIST})

target_link_libraries(test dwt_src)