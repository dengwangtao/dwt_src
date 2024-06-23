# dwt_src

## 服务注册中心(Service Registration Center)

业务服务使用TCP通信, 使用多Reactor模型+one loop per thread并发处理多个tcp连接

当conntion请求到达后, 会生成sessionId, 用来进行心跳检测, 每次心跳包会带着该sessionId

心跳使用UDP进行通信, 客户端需要每隔一段时间发送UDP心跳包来维护session, 服务端维护一个小根堆超时计时器和一个大根堆心跳计数器, 当连续(*max_heartbeat*=5)次接收不到心跳包, 执行回调, 删除会话节点

```shell
# 构建
mkdir build
cd build
cmake ..
make
```



提供客户端CLI(/bin/client_cli)和客户端静态库API(/lib/src_client.a /client/src_client.h)供第三方软件使用
```shell
# client_cli 命令: 

create	/userService/userinfo [node_data] [node_type 1:永久节点; 2:会话节点]
get 	/userService/userinfo 
set 	/userService/userinfo [node_data]
delete 	/userService/userinfo
ls 		/userService
stat 	/userService/userinfo
exists 	/userService/userinfo

# []为可选字段
```





case: 

> main为src服务端
>
> client为客户端cli
>
> (左)创建三个节点分别为
>
> - /node01
>   - /node01/func01
> - /node02
>
> node02为会话节点, 当会话结束后(心跳包停止), 该会话对应的会话节点被删除
>
> .
>
> (右) 查询, 使用ls查看子节点, 使用stat查看节点状态...
>
> 当/node02被删除后, `ls /`命令不再显示node02

![image-20240621162138021](https://github.com/dengwangtao/dwt_src/blob/main/resources/image-20240621162138021.png?raw=true)



目录树

```
.
├── bin						# 可执行文件目录
├── client					# 客户端目录
│   ├── client_cli.cpp
│   ├── CMakeLists.txt
│   ├── messages.pb.cc
│   ├── messages.pb.h
│   ├── proto
│   ├── src_client.cpp
│   └── src_client.h
├── CMakeLists.txt
├── lib						# 静态库生成目录
│   ├── libdwt_tcp.a
│   └── libsrc_client.a
├── README.md
├── resources				# 其他资源文件
├── src						# 核心源码
│   ├── Callbacks.h
│   ├── CMakeLists.txt
│   ├── DNode.h
│   ├── HeapHeartbeatCounter.cpp
│   ├── HeapHeartbeatCounter.h
│   ├── HeapTimer.cpp
│   ├── HeapTimer.h
│   ├── HeartbeatCounter.cpp
│   ├── HeartbeatCounter.h
│   ├── main.cpp
│   ├── messages.pb.cc
│   ├── messages.pb.h
│   ├── proto				# 协议文件夹
│   ├── Services.cpp
│   ├── Services.h
│   ├── Session.cpp
│   ├── Session.h
│   ├── SessionManager.cpp
│   ├── SessionManager.h
│   ├── SRCServer.cpp
│   ├── SRCServer.h
│   ├── tcp					# tcp网络模块
│   ├── uuid4.cpp
│   └── uuid4.h
└── test
    ├── CMakeLists.txt
    ├── test.cpp
    ├── test_HeapHeartbeatCounter.cpp
    ├── test_HeapTimer.cpp
    └── test_Logger.cpp
```
