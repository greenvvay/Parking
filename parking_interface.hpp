#pragma once

#include <chrono>
#include <expected>
#include <string>

using TimePoint = std::chrono::steady_clock::time_point;

struct GoodbyeMessage {};

enum class CarType {
    MOTORCYCLE, // Мотоциклы, скутеры
    COMPACT,    // Легковые (седаны, хэтчбеки)
    OVERSIZED,  // Грузовики, автобусы
};

struct CarInfo {
    std::string id;
    CarType type;
    std::string ownerId;
};

class ITicket {
public:
    virtual ~ITicket() = default;

    virtual const std::string& id() const = 0;
};

// Интерфейс для управления парковкой.
// Позволяет имитировать въезд и выезд автомобилей, а также проверять доступность мест.
class IParking {
public:
    virtual ~IParking() = default;

    // Пытается зарегистрировать въезд автомобиля на парковку.
    // Возвращает билет (ITicket) при успехе или текст ошибки.
    // Параметры:
    // - carInfo: информация об автомобиле.
    // - inGateIdx: индекс въездного шлагбаума.
    // - tp: временная точка въезда.
    virtual std::expected<std::unique_ptr<ITicket>, std::string /*error text*/> tryToEnter(const CarInfo& carInfo,
        std::size_t inGateIdx, TimePoint tp) = 0;

    // Пытается обработать выезд автомобиля с парковки через указанный шлагбаум.
    // Параметры:
    // - carInfo: информация об автомобиле, который пытается выехать. Должен совпадать с данными, переданными при
    // въезде.
    // - outGateIdx: индекс выездного шлагбаума (должен быть корректным для данной парковки).
    // - tp: временная точка выезда (используется для расчета времени пребывания).
    // - ticket: билет, полученный при въезде. Должен быть валидным и неиспользованным.
    virtual std::expected<GoodbyeMessage, std::string /*error text*/> tryToExit(const CarInfo& carInfo,
        std::size_t outGateIdx, TimePoint tp, const ITicket& ticket) = 0;

    // Возвращает количество свободных мест
    virtual std::size_t getAvailableSpaces() const = 0;
};

extern "C" {
// Необходимо реализовать эти функции, что бы можно было создать кастомный класс с IParking интерфейсом
IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);
void destroyParking(IParking* ptr);
}
