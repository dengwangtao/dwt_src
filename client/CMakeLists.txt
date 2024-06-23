
# aux_source_directory(. SRC_LIST)


add_library(src_client src_client.cpp messages.pb.cc)
target_link_libraries(src_client protobuf dwt_tcp)

add_executable(client_cli client_cli.cpp messages.pb.cc)
target_link_libraries(client_cli protobuf src_client)


