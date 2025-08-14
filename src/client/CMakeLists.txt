# 定义包含该目录所有文件的变量
aux_source_directory(. SRC_LIST)

add_executable(ChatClient ${SRC_LIST})


# 4. 链接muduo库及依赖项
target_link_libraries(ChatClient
    pthread         # 线程库
)