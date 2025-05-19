#include <gtest/gtest.h>

#include "ticket/ticket.hpp"
#include "parking/parking.hpp"
#include "interface/parking_interface.hpp"

TimeInterval createInterval(uint8_t from_h, uint8_t from_m, uint8_t to_h, uint8_t to_m) {
    return TimeInterval{
        .from = {.hour = from_h, .minute = from_m},
        .to = {.hour = to_h, .minute = to_m}
    };
}

TEST(testPayment, test) {
    IParking* parking = createParking(10, 10, 10);

    Tariff tariff;
    for (int i = 0; i < 7; i++) {
        tariff[i] = {
            {createInterval(0, 0, 24, 0), 10.0}
        };
    }

    parking->setupTariff(tariff);

    const auto carInfo = CarInfo {"A001AA", CarType::COMPACT, "1"};

    const auto ticket = parking->tryToEnter(carInfo, 1, std::chrono::steady_clock::now());
    Ticket* t = static_cast<Ticket*>(ticket.value().get());
    const auto price = parking->getPayment(*t);

    parking->tryToExit(carInfo, 1, std::chrono::steady_clock::now(), ticket.value().get());
    destroyParking(parking);
}