add_subdirectory(tcp)


aux_source_directory(. SRC_LIST)

add_library(dwt_src ${SRC_LIST})
target_link_libraries(dwt_src dwt_tcp pthread)



add_executable(main ${SRC_LIST})
target_link_libraries(main dwt_tcp pthread)


