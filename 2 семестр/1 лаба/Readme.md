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
    class FishType {
        <<enumeration>>
        SALMON
        COD
        TUNA
        HERRING
    }

    class FishStatus {
        <<enumeration>>
        FRESH
        PROCESSED
    }

    class Fish {
        +String species
        +Float weight
        +String status
        +Float price_per_kg
        +to_dict() Dict
    }

    class Net {
        +Int id
        +String name
        +Float capacity
        +Int mesh_size
        +to_dict() Dict
    }

    class Fisherman {
        +Int id
        +String name
        +String role
        +to_dict() Dict
    }

    class Vessel {
        +Int id
        +String name
        +Float capacity
        +List cargo
        +Net net
        +List crew
        +equip_net(Net)
        +add_crew_member(Fisherman)
        +catch_fish() List
        +to_dict() Dict
        +from_dict(Dict) Vessel$
    }

    class ColdStorage {
        +Float capacity
        +List storage
        +store_fish(List)
        +process_all_fish() Int
        +to_dict() Dict
        +from_dict(Dict) ColdStorage$
    }

    class FishMarket {
        +Float balance
        +sell_products(ColdStorage) Float
        +to_dict() Dict
        +from_dict(Dict) FishMarket$
    }

    class FishingProductionSystem {
        +List vessels
        +List fishermen
        +List nets
        +ColdStorage cold_storage
        +FishMarket market
        +create_vessel(String, Float)
        +assign_crew(Int, Int)
        +perform_fishing(Int)
        +unload_vessel(Int)
        +process_production()
        +sell_production()
        +save_state(String)
        +load_state(String)
    }

    FishingProductionSystem *-- Vessel : Управляет
    FishingProductionSystem *-- ColdStorage : Включает
    FishingProductionSystem *-- FishMarket : Включает
    Vessel o-- Fisherman : Команда
    Vessel o-- Net : Оснащение
    Vessel *-- Fish : Груз
    ColdStorage *-- Fish : Хранит
```
### 2.Диаграмма состояний
Описывает жизненный цикл производственного процесса и переходы между состояниями системы.
```mermaid
stateDiagram-v2
    [*] --> Ожидание : Запуск системы

    state Ожидание {
        [*] --> Подготовка
        Подготовка --> Готовность : Назначены сеть и команда
    }

    Ожидание --> Вылов : Команда fish
    
    state Вылов {
        [*] --> Заброс_Сети
        Заброс_Сети --> Ошибка : Нет сети или команды
        Заброс_Сети --> Загрузка_Трюма : Успешный улов
        Загрузка_Трюма --> Перегруз_Судна : Превышена вместимость
    }

    Вылов --> Ожидание : Возврат в порт
    Вылов --> Разгрузка : Команда unload

    Разгрузка --> Хранение : Перемещение на склад
    
    state Хранение {
        [*] --> Свежая_Рыба
        Свежая_Рыба --> Переработанная_Рыба : Команда process
    }

    Хранение --> Продажа : Команда sell

    Продажа --> Ожидание : Прибыль получена
