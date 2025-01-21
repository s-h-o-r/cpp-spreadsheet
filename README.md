# Реалзация базовых функций таблицы по типу Excel
Репозиторий сделан с целью изучения языка С++. В этом репозитории реализован стандартный функционал таблицы (spreadsheet). Проект ориентирован на функциональность работы с данными и формулами в электронных таблицах на уровне кода, но не включает визуальное представление данных. Таблица поддерживает отображение текста и числовых данных, умеет считать формулы и ссылаться на другие ячейки. Реализована обработка таких ошибок как деление на ноли и циклические зависимости. Для разбора формулы используется парсер [ANTLR](https://github.com/antlr/antlr4).

## Технологии
- [С++17](https://en.cppreference.com/w/cpp/17)
- [ANTLR](https://github.com/antlr/antlr4)
- Сборка [Cmake](https://cmake.org/)

## Тестирование
В данном проекте не были использованы фреймворки тестирования. Все модульные тесты функционала содержаться в файле [main.cpp](https://github.com/s-h-o-r/cpp-spreadsheet/blob/main/spreadsheet/main.cpp). 

## Запуск
Проект не предполагается для полноценного запуска. Цель проекта - изучение языка, поэтому возможность запуска, полноценного тестирования и графический интерфейс не реализованы в данном проекте. Для запуска тестов можно клонировать репозиторий с помощью `git clone`, собрать проект с помощью Cmake (версии не ниже 3.8) и запустить прорамму без аргументов. Если программа завершилась успешно, то все предполагаемые тесты пройдены успешно.

## Будущее проекта
На данный момент данный проект завершен. Дальнейшее развитие проекта не предполагается.
