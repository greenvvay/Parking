# Интерфейс управления парковки

## Описание
Парковка содержит:
 - заданное количетво парковочных мест
 - заданое количетво входных и выходных шлагбаумов

Заехать на парковку можно только через шлагбаум.
При въезде шлагбаум выдает билет.
Для выезда необходимо предъявить билет

## Интерфейс

Содержит класс интерфейс ```IParking``` и фабриные методы для его создания и уничтожения. 

## Использование

Необходимо наследоваться от ```IParking``` и реализовать методы:
```c++
IParking* createParking(std::size_t inGatesCount, std::size_t outGatesCount, std::size_t parkingSpacesCount);
void destroyParking(IParking* ptr);
```

## Базовое задание

Необходимо реализовать интейрфейс и фабричные методы, таким образом что бы удовлетворить следующие условия:
- На праковку может заехать любой автомобиль если на ней есть места
- Выехать с парковки может автомобиль только с билетом, который был получен на въезде
- Нельзя выехать дважды по одному и тому же билету

Необходимо скомпилироать динамическую библиотеку.
