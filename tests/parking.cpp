#include <gtest/gtest.h>

#include "ticket/ticket.hpp"
#include "parking/parking.hpp"
#include "interface/parking_interface.hpp"

TEST(firstTest, test) {
    IParking* parking = createParking(10, 10, 10);
    
    const auto carInfo = CarInfo {"A001AA", CarType::COMPACT, "1"};

    const auto ticket = parking->tryToEnter(carInfo, 1, std::chrono::steady_clock::now());
    size_t availableSpaces = parking->getAvailableSpaces();
    EXPECT_EQ(availableSpaces, 9);

    parking->tryToExit(carInfo, 1, std::chrono::steady_clock::now(), ticket.value().get());
    availableSpaces = parking->getAvailableSpaces();
    EXPECT_EQ(availableSpaces, 10);

    destroyParking(parking);
}