#pragma once
#include <cstdint>
#include <endian.h>
#include <string_view>

struct [[gnu::packed]] AddOrder{
    char message_type;
    uint16_t stock_locate;
    // combined tracking number and timestamp as timestamp is 6 bytes and the first field is the tracking number
    uint64_t tracking_number_and_timestamp;
    uint64_t order_reference_number;
    // 'B' represents a buy indicator and 'S' represents a sell indicator
    char buy_sell_indicator;
    uint32_t shares;
    char symbol[8];
    uint32_t price;
};

class AddOrderView{
private:
    const AddOrder* order;
public:

    AddOrderView(const char* ptr){
        this->order = reinterpret_cast<const AddOrder*>(ptr);
    }

    int16_t get_stock_locate() const{
        return be16toh(order->stock_locate);
    }

    uint16_t get_tracking_number() const{
        uint64_t target = be64toh(order->tracking_number_and_timestamp);
        return target>>48;
    }

    uint64_t get_timestammp() const{
        uint64_t target = be64toh(order->tracking_number_and_timestamp);
        return target & 0x0000FFFFFFFFFFFF;
    }

    uint64_t get_order_reference_number() const{
        return be64toh(order->order_reference_number);
    }
    
    char get_indicator() const{
        return order->buy_sell_indicator;
    }

    uint32_t get_shares() const{
        return be32toh(order->shares);
    }

    std::string_view get_symbol() const{
        return std::string_view(order->symbol, 8);
    }

    double get_price() const{
        return (double)(be32toh(order->price)) / 10000.0;
    }
};

struct [[gnu::packed]] CancelOrder{
    char msgType;
    uint16_t stock_locate;
    uint16_t tracking_number_and_timestamp;
    uint64_t order_number;
};

class CancelOrderView{
private:
    const CancelOrder* order;
public:
    CancelOrderView(const char* ptr) {
        this->order = reinterpret_cast<const CancelOrder*>(ptr);
    }

    uint16_t get_stock_locate() const{
        return be16toh(order->stock_locate);
    }

    uint16_t get_tracking_number() const{
        return be64toh(order->tracking_number_and_timestamp) >> 48;
    }

    uint16_t get_timestamp() const{
        return be64toh(order->tracking_number_and_timestamp) & 0x0000FFFFFFFFFFFF;
    }

    uint64_t get_order_number() const{
        return be64toh(order->order_number);
    }
};
