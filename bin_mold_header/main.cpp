#include <cstring>
#include <endian.h>
#include <unistd.h>
#include <sys/socket.h>
#include "itch.h"
#include "mold_udp64.h"
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>

int send_message(int socket_fd, sockaddr_in &addr, const char name1[8], const char name2[8]){
        char buf[1500];
    int offset = 0;
    // 1. Write the MoldUDP64 Header
    MoldUDP64 header;
    strncpy(header.session, "TEST      ", 10);
    header.sequence_number = htobe64(3);
    header.message_count = htobe16(3); // We are writing 2 messages
    memcpy(buf, (const char*)&header, sizeof(MoldUDP64));

    offset += sizeof(MoldUDP64);
    
    // 2. Prepare the 2-byte message length prefix (36 bytes for AddOrder)
    uint16_t message_length = htobe16(sizeof(AddOrder));

    // ==========================================
    // MESSAGE 1: Buy 100 shares of AAPL @ $150.25
    // ==========================================
    AddOrder order1;
    order1.message_type = 'A'; // ASCII, no byte-swap needed
    order1.stock_locate = htobe16(1);
    
    // Pack the 16-bit tracking number and 48-bit timestamp together!
    uint64_t tracking1 = 1234;
    uint64_t timestamp1 = 45000000000ULL; // e.g., nanoseconds since midnight
    uint64_t combined1 = (tracking1 << 48) | timestamp1;
    order1.tracking_number_and_timestamp= htobe64(combined1);
    
    order1.order_reference_number = htobe64(10001);
    order1.buy_sell_indicator = 'B';
    order1.shares = htobe32(100);
    strncpy(order1.symbol, name1, 8); // 8 bytes, space padded
    order1.price = htobe32(1502500); // 150.25 * 10,000

    // Write Length, then Message
    memcpy(buf + offset, (const char*)&message_length, sizeof(uint16_t));
    memcpy(buf + offset + sizeof(uint16_t), (const char*)&order1, sizeof(AddOrder));

    offset += (sizeof(uint16_t) + sizeof(AddOrder));
    AddOrder order2;
    order2.message_type = 'A';
    order2.stock_locate = htobe16(2);
    
    uint64_t tracking2 = 1235;
    uint64_t timestamp2 = 45000000100ULL; // 100 nanoseconds later
    uint64_t combined2 = (tracking2 << 48) | timestamp2;
    order2.tracking_number_and_timestamp= htobe64(combined2);
    
    order2.order_reference_number = htobe64(10002);
    order2.buy_sell_indicator = 'S';
    order2.shares = htobe32(50);
    strncpy(order2.symbol, name2, 8); 
    order2.price = htobe32(3000000); // 300.00 * 10,000

    // Write Length, then Message
    memcpy(buf + offset, (const char*)&message_length, sizeof(uint16_t));
    memcpy(buf + offset + sizeof(uint16_t), (const char*)&order2, sizeof(AddOrder));
    offset += (sizeof(uint16_t) + sizeof(AddOrder));

    // Cancel order
    CancelOrder order3;
    order3.order_number = htobe64(10001);
    order3.msgType = 'X';
    order3.stock_locate = htobe16(1);

    order3.tracking_number_and_timestamp = htobe64(1234ULL << 48 | (timestamp1 + 3));

    size_t len = sizeof(CancelOrder);

    memcpy(buf + offset, (const char*)&len, sizeof(uint16_t));
    memcpy(buf + offset + sizeof(uint16_t), (const char*)&order3, sizeof(CancelOrder));
    offset += (sizeof(uint16_t) + sizeof(CancelOrder));

    int bytes = sendto(socket_fd, buf, offset, 0, (struct sockaddr*)&addr, sizeof(sockaddr_in));

    if ( bytes < 0){
        perror("Send failed!\n");
        return -1;
    }else{
        std::cout<< "Sending bytes!\n";
    }

    return 0;
}

int main(){

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));

    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);
    addr.sin_family = AF_INET;

    
    send_message(socket_fd, addr, "APPL    ", "MSFT    ");
    send_message(socket_fd, addr, "CSP1    ", "NTF     ");

    close(socket_fd);
    return 0;
}
