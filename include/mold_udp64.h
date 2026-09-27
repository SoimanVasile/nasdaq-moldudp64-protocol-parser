#pragma once

#include <cstdint>
#include <endian.h>
#include <string_view>
struct [[gnu::packed]] MoldUDP64{
    char session[10];
    uint64_t sequence_number;
    uint16_t message_count;
};

class MoldUDPView{
private:
    const MoldUDP64* udp;
public:

    MoldUDPView(const char* ptr) {
        this->udp = reinterpret_cast<const MoldUDP64*>(ptr);
    }
    std::string_view get_session() const{
        return std::string_view(this->udp->session, 10);
    }
    uint64_t get_message_count() const{
        return be16toh(this->udp->message_count);
    }
    uint64_t get_sequence_number() const{
        return be64toh(this->udp->sequence_number);
    }
};
