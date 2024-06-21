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



```shell
命令: 
- create	/userService/userinfo data_xxxxxx [node_type 1:永久; 2:会话]
- get 		/userService/userinfo 
- set 		/userService/userinfo data_xxxxxx
- delete 	/userService/userinfo
- ls 		/userService
- stat 		/userService/userinfo
- exists 	/userService/userinfo
```



目录树

```
.
|-- CMakeLists.txt
|-- README.md
|-- bin				# 可执行文件目录
|   |-- client
|   |-- main
|   `-- test
|-- client			# 客户端代码
|   |-- CMakeLists.txt
|   |-- client.cpp
|   |-- messages.pb.cc
|   `-- messages.pb.h
|-- lib				# 库文件
|   |-- libdwt_src.a
|   `-- libdwt_tcp.a
|-- resources		# 其他资源
|   `-- protobuf_message.vsdx
|-- src				# 源码
|   |-- CMakeLists.txt
|   |-- Callbacks.h
|   |-- DNode.h
|   |-- HeapHeartbeatCounter.cpp
|   |-- HeapHeartbeatCounter.h
|   |-- HeapTimer.cpp
|   |-- HeapTimer.h
|   |-- HeartbeatCounter.cpp
|   |-- HeartbeatCounter.h
|   |-- SRCServer.cpp
|   |-- SRCServer.h
|   |-- Services.cpp
|   |-- Services.h
|   |-- Session.cpp
|   |-- Session.h
|   |-- SessionManager.cpp
|   |-- SessionManager.h
|   |-- main.cpp
|   |-- messages.pb.cc
|   |-- messages.pb.h
|   |-- proto
|   |-- tcp
|   |-- uuid4.cpp
|   `-- uuid4.h
`-- test		# 测试目录
    |-- CMakeLists.txt
    |-- test.cpp
    |-- test_HeapHeartbeatCounter.cpp
    |-- test_HeapTimer.cpp
    `-- test_Logger.cpp
```
