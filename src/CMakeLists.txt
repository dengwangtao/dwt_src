add_subdirectory(tcp)


aux_source_directory(. SRC_LIST)



add_executable(main ${SRC_LIST})
target_link_libraries(main dwt_tcp protobuf pthread)


