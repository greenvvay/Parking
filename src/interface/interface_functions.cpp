#include "parking_interface.hpp"
#include "parking/parking.hpp"

IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount) {
    return new Parking(inGatesCount, outGatesCount, parkingSpacesCount);
}

void destroyParking(IParking* ptr) {
    delete ptr;
    ptr = nullptr;
}