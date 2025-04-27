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

// Временная отметка в пределах суток (без учета даты).
struct TimeEntry {
    std::uint8_t hour;
    std::uint8_t minute;
};
// Временной интервал в пределах суток.
struct TimeInterval {
    TimeEntry from;
    TimeEntry to;
};
// Тип для хранения денежного значения (базовая единица валюты)
using Price = double;
// Тарифное правило, связывающее временной интервал с ценой.
struct TariffEntry {
    TimeInterval interval; // Временное окно действия тарифа
    Price price;           // Стоимость за единицу времени (за час/минуту)
};
// Тарифная сетка в течение дня, представленная как упорядоченный список тарифных правил.
using TariffDuringTheDay = std::vector<TariffEntry>;

constexpr std::size_t DAYS_IN_A_WEEK = 7;
// Полная недельная тарифная сетка, представленная как массив дневных тарифов. Отсчет начинается с понедельника.
using Tariff = std::array<TariffDuringTheDay, DAYS_IN_A_WEEK>;

// Типы событий, фиксируемых в системе логирования парковки.
enum EventType {
    ENTER, // Автомобиль успешно въехал на парковку (соответствует вызову tryToEnter)
    EXIT,  // Автомобиль покинул парковку (соответствует успешному tryToExit)
};
// Запись лога, содержащая детали отдельного события.
struct LogEntry {
    EventType event; // Тип произошедшего события
    TimePoint tp;    // Момент времени с точностью до наносекунд (steady_clock)
    CarInfo ci;      // Копия CarInfo на момент события (гарантирует неизменность данных)
};
// Список событий от старых к новым.
using Logs = std::vector<LogEntry>;

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

    // Устанавливает тарифную политику для парковки.
    // Параметры:
    // - tariff: структура с тарифными правилами (ставки по типам авто, льготы и т.д.).
    // Примечание:
    // - Должен вызываться при инициализации системы и может обновляться в runtime.
    // - Реализация должна гарантировать thread-safety при изменении тарифов.
    virtual void setupTariff(const Tariff& tariff) = 0;

    // Рассчитывает сумму к оплате за парковку на основе времени стоянки.
    // Параметры:
    // - ticket: билет, содержащий информацию о въезде и типе автомобиля.
    // Возвращает:
    // - Структуру Price с детализацией стоимости (базовая ставка, доп. сборы и т.д.).
    // Исключения:
    // - Должен возвращать корректную нулевую стоимость, если билет недействителен.
    virtual Price getPayment(const ITicket& ticket) = 0;

    // Обрабатывает оплату парковки перед выездом.
    // Параметры:
    // - ticket: билет, по которому производится оплата.
    // - amount: внесенная сумма (должна быть >= суммы из getPayment()).
    // Возвращает:
    // - true, если оплата прошла успешно и шлагбаум может открыться.
    // - false, при ошибке (недостаточная сумма, проблемы с платежом и т.д.).
    // Гарантии:
    // - При успешной оплате билет помечается как "оплаченный".
    // - Метод должен быть идемпотентным при повторных вызовах.
    virtual bool processPayment(ITicket& ticket, Price amount) = 0;

    // Возвращает логи в интервале [from, to]
    virtual Logs getLogs(TimePoint from, TimePoint to) = 0;
};

extern "C" {
// Необходимо реализовать эти функции, что бы можно было создать кастомный класс с IParking интерфейсом
IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);
void destroyParking(IParking* ptr);
}