#pragma once

#include "interface/parking_interface.hpp"

#include <mutex>
#include <iostream>
#include <unordered_set>

#include "ticket/ticket.hpp"

class Parking : public IParking {
public:
    Parking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);

    std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> tryToEnter(const CarInfo& carInfo,
        std::size_t inGateIdx, TimePoint tp) override;

    std::expected<GoodbyeMessage, std::string /*error text*/> tryToExit(const CarInfo& carInfo,
        std::size_t outGateIdx, TimePoint tp, ITicket* ticket) override;

    std::size_t getAvailableSpaces() const override;

private:
    std::size_t _parkingSpacesCount;
    mutable std::mutex _parkingSpacesCountMtx;
        
    std::unordered_set<std::string> _parkedCars;
};