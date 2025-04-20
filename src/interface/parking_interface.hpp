#pragma once

#include <chrono>
#include <expected>
#include <string>

using TimePoint = std::chrono::steady_clock::time_point;

struct GoodbyeMessage {};

// Типы въезжающих машин
enum class CarType {
    MOTORCYCLE, // Мотоциклы, скутеры
    COMPACT,    // Легковые (седаны, хэтчбеки)
    OVERSIZED,  // Грузовики, автобусы
};

// Структура, содержащая информацию об автомобиле для управления парковкой.
struct CarInfo {
    std::string id;      // Уникальный идентификатор автомобиля (например, госномер "А123БВ").
    CarType type;        // Тип транспортного средства из перечисления CarType
    std::string ownerId; // Идентификатор владельца (например, ID клиента в базе данных).
};

// Интерфейс ITicket представляет абстракцию билета, который выдается при успешном въезде автомобиля на парковку. Это
// ключевой объект для идентификации парковочной сессии и управления выездом.
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
    // - ticket: билет, полученный при въезде.
    virtual std::expected<GoodbyeMessage, std::string /*error text*/> tryToExit(const CarInfo& carInfo,
        std::size_t outGateIdx, TimePoint tp, ITicket* ticket) = 0;

    // Возвращает количество свободных мест
    virtual std::size_t getAvailableSpaces() const = 0;
};

extern "C" {
// Необходимо реализовать эти функции, что бы можно было создать кастомный класс с IParking интерфейсом
IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);
void destroyParking(IParking* ptr);
}
