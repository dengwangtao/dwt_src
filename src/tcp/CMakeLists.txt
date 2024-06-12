

aux_source_directory(. SRC_LIST)

add_library(dwt_tcp ${SRC_LIST})

# target_link_libraries(dwt_tcp pthread)