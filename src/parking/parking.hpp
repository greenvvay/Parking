#pragma once

#include "interface/parking_interface.hpp"

#include <mutex>
#include <iostream>
#include <unordered_set>
#include <optional>
#include <array>

#include "ticket/ticket.hpp"

class Parking : public IParking {
public:
    Parking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);

    std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> tryToEnter(const CarInfo& carInfo,
        std::size_t inGateIdx, TimePoint tp) override;

    std::expected<GoodbyeMessage, std::string /*error text*/> tryToExit(const CarInfo& carInfo,
        std::size_t outGateIdx, TimePoint tp, ITicket* ticket) override;

    std::size_t getAvailableSpaces() const override;

    void setupTariff(const Tariff& tariff) override;

    Price getPayment(const ITicket& ticket) override;

    bool processPayment(ITicket& ticket, Price amount) override;

    Logs getLogs(TimePoint from, TimePoint to) override;

private:
    std::size_t _parkingSpacesCount;
    mutable std::mutex _parkingSpacesCountMtx;
        
    std::unordered_set<std::string> _parkedCars;

    mutable std::mutex _tariffMtx;
    Tariff _tariff;

    mutable std::mutex _logsMtx;
    std::vector<LogEntry> _logs;
};