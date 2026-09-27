#include "network.h"
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int connect_to_socket(const std::string &ip, const uint16_t port){
    int listen_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (listen_socket < 0){
        perror("Socket creation failed!\n");
        return -1;
    }

    int reuse = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt failed");
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if ( bind(listen_socket, (struct sockaddr*)&addr, sizeof(sockaddr_in)) < 0) {
        perror("Bind failed!\n");
        return -2;
    };
    return listen_socket;
}

int read_from_socket(char *buf, const int socket_fd, const size_t size){
   return recvfrom(socket_fd, buf, size, 0, NULL, NULL);
}
