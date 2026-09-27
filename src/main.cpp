#include <iostream>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include "mold_udp64.h"
#include "itch.h"
#include "network.h"
#include <memory.h>
#include <unistd.h>

#define PORT 9000
#define MAX_BUFFER 1500

uint16_t get_message_length(const char* ptr){
    const uint16_t* new_ptr = reinterpret_cast<const uint16_t*>(ptr);
    return be16toh(new_ptr[0]);
}

int main(){

    int socket_fd = connect_to_socket("127.0.0.1", PORT);

    std::cout << "Listenting to: 127.0.0.1:"<< PORT << " ..\n";

    char buf[MAX_BUFFER];

    while (true){
        int bytes_read = read_from_socket(buf, socket_fd, MAX_BUFFER);
        if (bytes_read == 0){
            perror("Could not read from socket!\n");
            break;
        }

        const char* ptr = buf;
        auto mold_view = MoldUDPView(ptr);

        std::cout << mold_view.get_session() << '\n';
        
        std::cout << mold_view.get_message_count() << " ---------- " << mold_view.get_sequence_number() << '\n';

        const uintmax_t number_of_messages = mold_view.get_message_count();

        ptr += sizeof(MoldUDP64);
        for ( size_t i{0}; i < number_of_messages; i++ ){
            auto message_length = get_message_length(ptr);
            ptr += sizeof(uint16_t);

            auto message_type = *ptr;
            
            switch (message_type){
                case 'A': {

                    auto add_order_view = AddOrderView(ptr);

                    std::cout << "Stock: " << add_order_view.get_symbol() << " -- Shares: " << add_order_view.get_shares() << " -- Price: " << add_order_view.get_price() << " -- Indicator: " << add_order_view.get_indicator() << '\n';
                    break;
                }
                case 'X': {

                    auto cancel_order_view = CancelOrderView(ptr);

                    std::cout << "Order Reference number: " << cancel_order_view.get_order_number() << "----- Stock Locate: " << cancel_order_view.get_stock_locate() << " ---- Tracking number: " << cancel_order_view.get_tracking_number() << '\n';
                    break;
                }
                default:{
                    std::cout << "I dont know this message type!" << '\n';
                }
            }

            ptr += message_length;
        }
        std::cout.flush();
    }

    close(socket_fd);
    return 0;
}
