#pragma once

#include "interface/parking_interface.hpp"

#include <string>

class Ticket : public ITicket {
public:

    Ticket(const CarInfo& carInfo) : _id(carInfo.id) {
    }

    const std::string& id() const override {
        return _id;
    };

private:
    std::string _id;
};