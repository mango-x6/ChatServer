# 定义包含该目录所有文件的变量
aux_source_directory(. SRC_LIST)
aux_source_directory(./model MODEL_LIST)

add_executable(ChatServer ${SRC_LIST} ${MODEL_LIST})


# 4. 链接muduo库及依赖项
target_link_libraries(ChatServer
    muduo_net       # muduo网络库
    muduo_base      # muduo基础库
    mysqlclient     # mysql开发包
    pthread         # 线程库
)