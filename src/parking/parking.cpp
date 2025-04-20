#include "parking/parking.hpp"


Parking::Parking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount)
    : _parkingSpacesCount(parkingSpacesCount){
}

std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> Parking::tryToEnter(const CarInfo& carInfo,
    std::size_t inGateIdx, TimePoint tp) {

    std::lock_guard lock(_parkingSpacesCountMtx);

    if (_parkingSpacesCount == 0) {
        return std::unexpected("No slots available");
    }

    _parkedCars.emplace(carInfo.id);

    return std::unique_ptr<ITicket>(new Ticket(carInfo));
}

std::expected<GoodbyeMessage, std::string /*error text*/> Parking::tryToExit(const CarInfo& carInfo,
    std::size_t outGateIdx, TimePoint tp, ITicket* ticket) {

    std::lock_guard lock(_parkingSpacesCountMtx);

    if (ticket->id() != carInfo.id) {
        return std::unexpected("Ticket is not yours");
    }

    if (not _parkedCars.contains(ticket->id())) {
        return std::unexpected("Ticket already used");
    }

    _parkedCars.erase(ticket->id());

    return {};
};

std::size_t Parking::getAvailableSpaces() const {
    std::lock_guard lock(_parkingSpacesCountMtx);
    return _parkingSpacesCount;
}
