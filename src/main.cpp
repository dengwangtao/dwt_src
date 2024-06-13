#include <iostream>

#include "tcp/TcpServer.h"
#include "SRCServer.h"


int main() {
    
    dwt::EventLoop loop;

    dwt::SRCServer server(
        &loop,
        dwt::InetAddress("127.0.0.1", 8888),
        "SRCServer"
    );

    server.start();
    loop.loop();

    return 0;
}