#include "parking_interface.hpp"

#include <mutex>
#include <iostream>
#include <unordered_set>


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

class Parking : public IParking {
public:
    Parking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount)
       : _parkingSpacesCount(parkingSpacesCount){
    }

    std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> tryToEnter(const CarInfo& carInfo,
        std::size_t inGateIdx, TimePoint tp) override {
    
        std::lock_guard lock(_parkingSpacesCountMtx);

        if (_parkingSpacesCount == 0) {
            return std::unexpected("No slots available");
        }

        _parkedCars.emplace(carInfo.id);

        return std::unique_ptr<ITicket>(new Ticket(carInfo));
    }

    std::expected<GoodbyeMessage, std::string /*error text*/> tryToExit(const CarInfo& carInfo,
        std::size_t outGateIdx, TimePoint tp, ITicket* ticket) override {

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

    std::size_t getAvailableSpaces() const override {
        std::lock_guard lock(_parkingSpacesCountMtx);
        return _parkingSpacesCount;
    }

private:
    std::size_t _parkingSpacesCount;
    mutable std::mutex _parkingSpacesCountMtx;
     
    std::unordered_set<std::string> _parkedCars;
};

IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount) {

    return new Parking(inGatesCount, outGatesCount, parkingSpacesCount);
}

void destroyParking(IParking* ptr) {
    delete ptr;
    ptr = nullptr;
}

int main(int argc, char* argv[]) {

    auto* parkingPtr = createParking(10, 10, 10);

    const auto firstCar = CarInfo{ "Q123QQ", CarType::COMPACT, "Q" };

    const auto ticket = parkingPtr->tryToEnter(firstCar, 0, std::chrono::steady_clock::now());

    if (not ticket) {
        std::cout << ticket.error() << std::endl;
    }

    const auto goodbyeMsg = parkingPtr->tryToExit(firstCar, 9, std::chrono::steady_clock::now(), ticket.value().get());

    if (not goodbyeMsg) {
        std::cout << goodbyeMsg.error() << std::endl;
    }

    destroyParking(parkingPtr);
}