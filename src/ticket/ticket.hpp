#pragma once

#include "interface/parking_interface.hpp"

#include <string>

#include "utils/utils.hpp"

class Ticket : public ITicket {
public:

    Ticket(const CarInfo& carInfo) : _id(carInfo.id) {
        const auto [h, m] = getCurrentTime();
        _timeEntry = TimeEntry {h, m};
    }

    const std::string& id() const override {
        return _id;
    };

    TimeEntry getEntryTime() const {
        return _timeEntry;
    };

private:
    std::string _id;
    TimeEntry _timeEntry;
};