// ShuttingDown.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ShuttingDown.h"
#include "httplib.h"
#pragma warning(disable:4996)


// 函数用于获取本地计算机的非回环 IPv4 地址
std::string getLocalIPv4() {
    std::string ipv4Address;  // 用于存储最终获取到的 IPv4 地址

    char hostName[256];  // 用于存储主机名
    // 获取本地主机名
    if (gethostname(hostName, sizeof(hostName)) == 0) {
        struct hostent* host = gethostbyname(hostName);  // 通过主机名获取主机信息
        if (host && host->h_addrtype == AF_INET) {  // 检查是否为 IPv4 地址类型
            struct in_addr** addr_list = (struct in_addr**)host->h_addr_list;
            // 遍历地址列表
            for (int i = 0; addr_list[i] != NULL; i++) {
                std::string ip = inet_ntoa(*addr_list[i]);  // 将地址转换为字符串
                if (ip != "127.0.0.1") {  // 检查是否为非回环地址
                    ipv4Address = ip;
                    break;  // 找到第一个非回环地址后退出循环
                }
            }
        }
    }

    // 如果仍然没有获取到有效的非回环 IPv4 地址，输出错误信息
    if (ipv4Address.empty()) {
        std::cerr << "No valid non-loopback IPv4 address found." << std::endl;
    }

    return ipv4Address;  // 返回获取到的 IPv4 地址
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    httplib::Server svr;  // 创建 httplib 服务器对象

    std::string localIPv4 = getLocalIPv4();  // 获取本地 IPv4 地址
    std::cerr << localIPv4 << std::endl;
    // 如果获取地址失败
    if (localIPv4.empty()) {
        std::cerr << "Failed to obtain a local IPv4 address. Exiting." << std::endl;
        return -1;  // 程序退出并返回 -1
    }

    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        svr.stop();  // 停止 httplib 服务器
        std::system("shutdown -s -t 0");  // 执行关机命令
        res.set_content("<html><body>Shutting down...</body></html>", "text/html");
        return 0;  // 程序正常结束返回 0
        });

    // 尝试在获取到的地址和指定端口上启动服务器监听
    if (!svr.listen(localIPv4, 6060)) {
        std::cerr << "Error starting server on IP: " << localIPv4 << std::endl;
        return -1;  // 启动监听失败时程序退出并返回 -1
    }

    return 0;  // 程序正常结束返回 0
}