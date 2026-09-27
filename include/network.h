#pragma once
#include <string>
#include <cstdint>


// it will connect on the given ip with IPv4 and for udp
int connect_to_socket(const std::string& ip, const uint16_t port);

// It will read from the socket fd, it needs to connect first to be able to read
int read_from_socket(char* buf, const int socket_fd, const size_t size);
