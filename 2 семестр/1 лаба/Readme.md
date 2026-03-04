# Лабораторная работа №1 по курсу “Проектирование ПО интеллектуальных систем”

**Выполнил:** Змитревич Михаил Юрьевич, группа 4104

## Цель работы
1. Изучить основные возможности языка Python для разработки программных систем с интерфейсом командной строки (CLI).
2. Разработать программную систему на языке Python согласно описанию предметной области.

---

## Вариант 19: Модель рыболовного производства

**Предметная область:** Процесс вылова и переработки рыбы на рыбопромыслах.

**Ключевые сущности системы:** Рыболовное производство, рыбак, рыба, судно, сеть, хладокомбинат, рынок рыбной продукции.

---

## Архитектура системы (UML)

Система спроектирована с применением объектно-ориентированного анализа, паттернов проектирования и с учетом принципов SOLID.

### 1. Диаграмма классов
Отражает структуру сущностей, их атрибуты, методы и взаимосвязи (агрегацию, композицию).

```mermaid
classDiagram
    direction TB

    class GameException {
        <<Exception>>
    }
    class StorageFullError {
        <<Exception>>
    }
    class ResourceNotFoundError {
        <<Exception>>
    }
    GameException <|-- StorageFullError
    GameException <|-- ResourceNotFoundError

    class ВидРыбы {
        <<enumeration>>
        ЛОСОСЬ
        ТРЕСКА
        ТУНЕЦ
        СЕЛЬДЬ
    }

    class СтатусРыбы {
        <<enumeration>>
        СВЕЖАЯ
        ПЕРЕРАБОТАННАЯ
    }

    class Рыба {
        +вид: str
        +вес: float
        +статус: str
        +цена_за_кг: float
        +to_dict() Dict
    }

    class Сеть {
        +id: int
        +название: str
        +вместимость: float
        +размер_ячейки: int
        +to_dict() Dict
    }

    class Рыбак {
        +id: int
        +имя: str
        +роль: str
        +to_dict() Dict
    }

    class Судно {
        +id: int
        +название: str
        +вместимость: float
        +груз: List~Рыба~
        +сеть: Optional~Сеть~
        +команда: List~Рыбак~
        +equip_net(net: Сеть)
        +add_crew_member(fisherman: Рыбак)
        +catch_fish() List~Рыба~
        +to_dict() Dict
        +from_dict(data: Dict)$ Судно
    }

    class Хладокомбинат {
        +вместимость: float
        +склад: List~Рыба~
        +store_fish(fish_batch: List~Рыба~)
        +process_all_fish() int
        +to_dict() Dict
        +from_dict(data: Dict)$ Хладокомбинат
    }

    class РыбныйРынок {
        +баланс: float
        +sell_products(storage: Хладокомбинат) float
        +to_dict() Dict
        +from_dict(data: Dict)$ РыбныйРынок
    }

    class СистемаУправления {
        +суда: List~Судно~
        +рыбаки: List~Рыбак~
        +сети: List~Сеть~
        +хладокомбинат: Хладокомбинат
        +рынок: РыбныйРынок
        +create_vessel(name, capacity)
        +create_fisherman(name)
        +create_net(name, capacity)
        +assign_crew(vessel_id, fisherman_id)
        +equip_vessel(vessel_id, net_id)
        +perform_fishing(vessel_id)
        +unload_vessel(vessel_id)
        +process_production()
        +sell_production()
        +show_status()
        +save_state(filename)
        +load_state(filename)
    }

    Судно "1" *-- "*" Рыба : содержит (груз)
    Судно "1" o-- "0..1" Сеть : оснащено
    Судно "1" o-- "*" Рыбак : команда
    Хладокомбинат "1" *-- "*" Рыба : хранит (склад)
    СистемаУправления "1" *-- "*" Судно
    СистемаУправления "1" *-- "*" Рыбак
    СистемаУправления "1" *-- "*" Сеть
    СистемаУправления "1" *-- "1" Хладокомбинат
    СистемаУправления "1" *-- "1" РыбныйРынок
```
### 2.Диаграмма состояний
Описывает жизненный цикл производственного процесса и переходы между состояниями системы.
```mermaid
flowchart TD
    Start([Запуск программы]) --> Init[Инициализация Системы Управления]
    Init --> Load[Автозагрузка данных: load_state]
    
    Load --> Loop{Главный цикл: while True}

    Loop --> Input[/Ожидание ввода команды/]
    Input --> CheckCmd{Проверка команды}

    CheckCmd -->|help| CmdHelp[Вывод справки]
    CheckCmd -->|status| CmdStatus[Вывод статуса]
    CheckCmd -->|add_vessel / add_fisher / add_net| CmdAdd[Создание сущности]
    CheckCmd -->|assign / equip| CmdEquip[Назначение персонала / Экипировка]
    CheckCmd -->|fish| CmdFish[Ловля рыбы: catch_fish]
    CheckCmd -->|unload| CmdUnload[Выгрузка на склад: store_fish]
    CheckCmd -->|process| CmdProcess[Переработка: process_all_fish]
    CheckCmd -->|sell| CmdSell[Продажа: sell_products]
    CheckCmd -->|save / load| CmdState[Сохранение / Загрузка состояния]
    
    CheckCmd -->|Неизвестная команда или ошибка| CmdError[Сообщение об ошибке]
    
    %% Явно выделенный блок выхода
    CheckCmd -->|exit| CmdExit[Завершение работы цикла]

    %% Возврат в цикл
    CmdHelp --> Loop
    CmdStatus --> Loop
    CmdAdd --> Loop
    CmdEquip --> Loop
    CmdFish --> Loop
    CmdUnload --> Loop
    CmdProcess --> Loop
    CmdSell --> Loop
    CmdState --> Loop
    CmdError --> Loop

    %% Финал
    CmdExit --> End([Выход из программы])
    
    style End fill:#f99,stroke:#333,stroke-width:2px
    style CmdExit fill:#ffcccb,stroke:#f00
