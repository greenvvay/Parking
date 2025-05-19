#include "parking/parking.hpp"

#include <algorithm>

#include "utils/utils.hpp"

Parking::Parking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount)
    : _inGatesCount(inGatesCount), _outGatesCount(outGatesCount), _parkingSpacesCount(parkingSpacesCount){
}

Parking::~Parking() = default;

std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> Parking::tryToEnter(const CarInfo& carInfo,
    std::size_t inGateIdx, TimePoint tp) {

    std::lock_guard lock(_parkingSpacesCountMtx);

    if (_parkingSpacesCount == 0) {
        return std::unexpected("No slots available");
    }

    if (inGateIdx > _inGatesCount) {
        return std::unexpected("Wrong gate");
    }

    _parkedCars.emplace(carInfo.id);
    _parkingSpacesCount--;

    {
        std::lock_guard lock(_logsMtx);
        _logs.push_back({EventType::ENTER, std::chrono::steady_clock::now(), carInfo});
    }
    return std::unique_ptr<ITicket>(new Ticket(carInfo));
}

std::expected<GoodbyeMessage, std::string /*error text*/> Parking::tryToExit(const CarInfo& carInfo,
    std::size_t outGateIdx, TimePoint tp, ITicket* ticket) {

    std::lock_guard lock(_parkingSpacesCountMtx);

    if (ticket->id() != carInfo.id) {
        return std::unexpected("Ticket is not yours");
    }

    if (outGateIdx > _outGatesCount) {
        return std::unexpected("Wrong gate");
    }

    if (not _parkedCars.contains(ticket->id())) {
        return std::unexpected("Ticket already used");
    }

    _parkedCars.erase(ticket->id());
    _parkingSpacesCount++;

    {
        std::lock_guard lock(_logsMtx);
        _logs.push_back({EventType::EXIT, std::chrono::steady_clock::now(), carInfo});
    }

    return {};
};

std::size_t Parking::getAvailableSpaces() const {
    std::lock_guard lock(_parkingSpacesCountMtx);
    return _parkingSpacesCount;
}

void Parking::setupTariff(const Tariff& tariff) {
    std::lock_guard lock(_tariffMtx);
    _tariff = tariff;
}

Price Parking::getPayment(const ITicket& ticket) {
    try {
        const auto& ticketImpl = dynamic_cast<const Ticket&>(ticket);

        TariffDuringTheDay tariffThisDay;
        {
            std::lock_guard lock(_tariffMtx);
            tariffThisDay = _tariff[getDayOfWeekCount()];
        }

        const auto entryTime = ticketImpl.getEntryTime();
        const auto [exitHour, exitMinute] = getCurrentTime();

        Price totalPrice(0.0);

        for(const auto& tariff : tariffThisDay) {

            if(exitHour < tariff.interval.from.hour or entryTime.hour > tariff.interval.to.hour) {
                continue;
            }

            const auto fromHours = std::max(tariff.interval.from.hour, entryTime.hour);
            const auto tillHours = std::min(tariff.interval.to.hour, exitHour);

            const auto fromMinute = std::max(tariff.interval.from.minute, entryTime.minute);
            const auto tillMinute = std::min(tariff.interval.to.minute, exitMinute);

            totalPrice += ((tillHours - fromHours) * 60 + (tillMinute - fromMinute)) * tariff.price;
        }

        return totalPrice;
    }
    catch(const std::bad_cast& e) {
        return 0.0;
    }
}

bool Parking::processPayment(ITicket& ticket, Price amount) {

    const Price recipt = getPayment(ticket);
    if(amount >= recipt) {
        // TODO: билет помечается как оплаченный
        return true;
    }
    return false;
}

Logs Parking::getLogs(TimePoint from, TimePoint to) {

    std::lock_guard lock(_logsMtx);
    std::vector<LogEntry> logs;

    for(const auto event : _logs) {
        
        if(event.tp > to) {
            return logs;
        }

        if(event.tp >= from and event.tp <= to) {
            logs.push_back(event);
        }

        return logs;
    }
}
