import json
import random
import os
from typing import List, Dict, Optional, Any
from enum import Enum
from dataclasses import dataclass, field, asdict


# --- 1. Вспомогательные конструкции и исключения ---

class GameException(Exception):
    """Базовый класс для исключений в нашей системе."""
    pass


class StorageFullError(GameException):
    """Ошибка: Хранилище переполнено."""
    pass


class ResourceNotFoundError(GameException):
    """Ошибка: Ресурс (судно, сеть и т.д.) не найден."""
    pass


class FishType(Enum):
    SALMON = "Лосось"
    COD = "Треска"
    TUNA = "Тунец"
    HERRING = "Сельдь"


class FishStatus(Enum):
    FRESH = "Свежая"
    PROCESSED = "Переработанная"


# --- 2. Сущности предметной области (Models) ---

@dataclass
class Fish:
    """Сущность: Рыба."""
    species: str
    weight: float
    status: str = FishStatus.FRESH.value
    price_per_kg: float = 10.0

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class Net:
    """Сущность: Сеть."""
    id: int
    name: str
    capacity: float  # Максимальный вес улова за раз
    mesh_size: int  # Размер ячейки (влияет на тип рыбы, условно)

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


@dataclass
class Fisherman:
    """Сущность: Рыбак."""
    id: int
    name: str
    role: str = "Матрос"

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class Vessel:
    """Сущность: Судно."""

    def __init__(self, id: int, name: str, capacity: float):
        self.id = id
        self.name = name
        self.capacity = capacity
        self.cargo: List[Fish] = []
        self.net: Optional[Net] = None
        self.crew: List[Fisherman] = []

    def equip_net(self, net: Net) -> None:
        self.net = net

    def add_crew_member(self, fisherman: Fisherman) -> None:
        self.crew.append(fisherman)

    def catch_fish(self) -> List[Fish]:
        """Операция вылова рыбы."""
        if not self.net:
            raise GameException(f"Судно {self.name} не оснащено сетью!")
        if not self.crew:
            raise GameException(f"На судне {self.name} нет команды!")

        # Логика улова: случайное количество рыбы, ограниченное вместимостью сети и трюма
        catch_weight = random.uniform(0, self.net.capacity)
        fish_count = int(catch_weight / 2.0)  # Допустим, средняя рыба 2 кг

        new_catch = []
        for _ in range(fish_count):
            f_type = random.choice(list(FishType))
            fish = Fish(
                species=f_type.value,
                weight=random.uniform(1.0, 5.0),
                price_per_kg=random.uniform(5.0, 15.0)
            )
            new_catch.append(fish)

        # Проверяем перегруз судна
        current_load = sum(f.weight for f in self.cargo)
        new_load = sum(f.weight for f in new_catch)

        if current_load + new_load > self.capacity:
            raise StorageFullError("Трюм судна переполнен! Невозможно поднять сеть.")

        self.cargo.extend(new_catch)
        return new_catch

    def to_dict(self) -> Dict[str, Any]:
        return {
            "id": self.id,
            "name": self.name,
            "capacity": self.capacity,
            "cargo": [f.to_dict() for f in self.cargo],
            "net": self.net.to_dict() if self.net else None,
            "crew": [c.to_dict() for c in self.crew]
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'Vessel':
        vessel = cls(data['id'], data['name'], data['capacity'])
        vessel.cargo = [Fish(**f) for f in data['cargo']]
        if data['net']:
            vessel.net = Net(**data['net'])
        if data['crew']:
            vessel.crew = [Fisherman(**c) for c in data['crew']]
        return vessel


class ColdStorage:
    """Сущность: Хладокомбинат."""

    def __init__(self, capacity: float):
        self.capacity = capacity
        self.storage: List[Fish] = []

    def store_fish(self, fish_batch: List[Fish]) -> None:
        """Операция хранения и транспортировки (прием с судна)."""
        current_weight = sum(f.weight for f in self.storage)
        batch_weight = sum(f.weight for f in fish_batch)

        if current_weight + batch_weight > self.capacity:
            raise StorageFullError("Хладокомбинат переполнен!")

        self.storage.extend(fish_batch)

    def process_all_fish(self) -> int:
        """Операция переработки рыбы."""
        count = 0
        for fish in self.storage:
            if fish.status == FishStatus.FRESH.value:
                fish.status = FishStatus.PROCESSED.value
                fish.price_per_kg *= 1.5  # Обработанная рыба дороже
                count += 1
        return count

    def to_dict(self) -> Dict[str, Any]:
        return {
            "capacity": self.capacity,
            "storage": [f.to_dict() for f in self.storage]
        }

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'ColdStorage':
        cs = cls(data['capacity'])
        cs.storage = [Fish(**f) for f in data['storage']]
        return cs


class FishMarket:
    """Сущность: Рынок рыбной продукции."""

    def __init__(self, balance: float = 0.0):
        self.balance = balance

    def sell_products(self, storage: ColdStorage) -> float:
        """Операция продажи рыбной продукции (продаем только переработанную)."""
        to_sell = [f for f in storage.storage if f.status == FishStatus.PROCESSED.value]
        revenue = 0.0

        if not to_sell:
            return 0.0

        for fish in to_sell:
            revenue += fish.weight * fish.price_per_kg
            storage.storage.remove(fish)  # Убираем со склада

        self.balance += revenue
        return revenue

    def to_dict(self) -> Dict[str, Any]:
        return {"balance": self.balance}

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'FishMarket':
        return cls(data['balance'])


# --- 3. Система управления (Controller) ---

class FishingProductionSystem:
    """Главный контроллер: Рыболовное производство."""

    def __init__(self):
        self.vessels: List[Vessel] = []
        self.fishermen: List[Fisherman] = []
        self.nets: List[Net] = []
        self.cold_storage = ColdStorage(capacity=1000.0)
        self.market = FishMarket()

    def create_vessel(self, name: str, capacity: float) -> None:
        vid = len(self.vessels) + 1
        self.vessels.append(Vessel(vid, name, capacity))
        print(f"Судно '{name}' создано.")

    def create_fisherman(self, name: str) -> None:
        fid = len(self.fishermen) + 1
        self.fishermen.append(Fisherman(fid, name))
        print(f"Рыбак '{name}' нанят.")

    def create_net(self, name: str, capacity: float) -> None:
        nid = len(self.nets) + 1
        self.nets.append(Net(nid, name, capacity, 10))
        print(f"Сеть '{name}' добавлена на склад.")

    def assign_crew(self, vessel_id: int, fisherman_id: int) -> None:
        vessel = next((v for v in self.vessels if v.id == vessel_id), None)
        fisherman = next((f for f in self.fishermen if f.id == fisherman_id), None)

        if not vessel or not fisherman:
            raise ResourceNotFoundError("Судно или рыбак не найдены.")

        vessel.add_crew_member(fisherman)
        print(f"{fisherman.name} назначен на {vessel.name}.")

    def equip_vessel(self, vessel_id: int, net_id: int) -> None:
        vessel = next((v for v in self.vessels if v.id == vessel_id), None)
        net = next((n for n in self.nets if n.id == net_id), None)

        if not vessel or not net:
            raise ResourceNotFoundError("Судно или сеть не найдены.")

        vessel.equip_net(net)
        print(f"{vessel.name} оборудовано сетью {net.name}.")

    def perform_fishing(self, vessel_id: int) -> None:
        vessel = next((v for v in self.vessels if v.id == vessel_id), None)
        if not vessel:
            raise ResourceNotFoundError("Судно не найдено.")

        caught = vessel.catch_fish()
        total_weight = sum(f.weight for f in caught)
        print(f"Успех! Поймано {len(caught)} рыб. Общий вес: {total_weight:.2f} кг.")

    def unload_vessel(self, vessel_id: int) -> None:
        vessel = next((v for v in self.vessels if v.id == vessel_id), None)
        if not vessel:
            raise ResourceNotFoundError("Судно не найдено.")

        count = len(vessel.cargo)
        if count == 0:
            print("Трюм пуст.")
            return

        self.cold_storage.store_fish(vessel.cargo)
        vessel.cargo.clear()
        print(f"Выгружено {count} рыб на хладокомбинат.")

    def process_production(self) -> None:
        count = self.cold_storage.process_all_fish()
        print(f"Переработано {count} единиц рыбы. Готово к продаже.")

    def sell_production(self) -> None:
        revenue = self.market.sell_products(self.cold_storage)
        print(f"Продана партия. Выручка: {revenue:.2f}$. Текущий баланс: {self.market.balance:.2f}$")

    def show_status(self) -> None:
        print("\n--- СТАТУС ПРОИЗВОДСТВА ---")
        print(f"Баланс: {self.market.balance:.2f}$")
        print(f"Рыба на складе: {len(self.cold_storage.storage)} шт. (Вместимость: {self.cold_storage.capacity})")
        print(f"Флот ({len(self.vessels)} судов):")
        for v in self.vessels:
            print(
                f"  - [{v.id}] {v.name}: Команда {len(v.crew)}, Сеть: {'Да' if v.net else 'Нет'}, Груз: {len(v.cargo)} шт.")
        print("---------------------------")

    # --- Сериализация (Сохранение/Загрузка) ---
    def save_state(self, filename: str = "fishing_data.json") -> None:
        data = {
            "vessels": [v.to_dict() for v in self.vessels],
            "fishermen": [f.to_dict() for f in self.fishermen],
            "nets": [n.to_dict() for n in self.nets],
            "cold_storage": self.cold_storage.to_dict(),
            "market": self.market.to_dict()
        }
        with open(filename, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=4)
        print("Данные успешно сохранены.")

    def load_state(self, filename: str = "fishing_data.json") -> None:
        if not os.path.exists(filename):
            print("Файл сохранения не найден. Начинаем новую игру.")
            return

        try:
            with open(filename, "r", encoding="utf-8") as f:
                data = json.load(f)

            # Восстановление объектов
            self.fishermen = [Fisherman(**f) for f in data.get("fishermen", [])]
            self.nets = [Net(**n) for n in data.get("nets", [])]
            self.vessels = [Vessel.from_dict(v) for v in data.get("vessels", [])]

            if "cold_storage" in data:
                self.cold_storage = ColdStorage.from_dict(data["cold_storage"])
            if "market" in data:
                self.market = FishMarket.from_dict(data["market"])

            print("Данные успешно загружены.")
        except json.JSONDecodeError:
            print("Ошибка чтения файла сохранения.")
        except Exception as e:
            print(f"Ошибка загрузки: {e}")


# --- 4. Интерфейс командной строки (CLI) ---

def print_help():
    print("""
    Доступные команды:
    1. add_vessel <имя> <вместимость> - Купить судно
    2. add_fisher <имя>               - Нанять рыбака
    3. add_net <имя> <вместимость>    - Купить сеть
    4. assign <id_судна> <id_рыбака>  - Назначить рыбака на судно
    5. equip <id_судна> <id_сети>     - Оборудовать судно сетью
    6. fish <id_судна>                - Отправить судно на рыбалку
    7. unload <id_судна>              - Разгрузить судно на склад
    8. process                        - Переработать рыбу на складе
    9. sell                           - Продать готовую продукцию
    10. status                        - Показать отчет
    11. save                          - Сохранить прогресс
    12. load                          - Загрузить прогресс
    13. exit                          - Выход
    """)


def main():
    system = FishingProductionSystem()
    print("=== Система управления рыболовным производством (Вариант 19) ===")

    # Автоматическая попытка загрузки при старте
    system.load_state()

    while True:
        try:
            command_line = input("\nВведите команду (help для справки): ").strip().split()
            if not command_line:
                continue

            cmd = command_line[0].lower()
            args = command_line[1:]

            if cmd == "exit":
                print("Выход из системы...")
                break

            elif cmd == "help":
                print_help()

            elif cmd == "status":
                system.show_status()

            elif cmd == "add_vessel":
                if len(args) < 2: raise ValueError("Нужно имя и вместимость")
                system.create_vessel(args[0], float(args[1]))

            elif cmd == "add_fisher":
                if len(args) < 1: raise ValueError("Нужно имя")
                system.create_fisherman(args[0])

            elif cmd == "add_net":
                if len(args) < 2: raise ValueError("Нужно имя и вместимость")
                system.create_net(args[0], float(args[1]))

            elif cmd == "assign":
                if len(args) < 2: raise ValueError("Нужны ID судна и ID рыбака")
                system.assign_crew(int(args[0]), int(args[1]))

            elif cmd == "equip":
                if len(args) < 2: raise ValueError("Нужны ID судна и ID сети")
                system.equip_vessel(int(args[0]), int(args[1]))

            elif cmd == "fish":
                if len(args) < 1: raise ValueError("Нужен ID судна")
                system.perform_fishing(int(args[0]))

            elif cmd == "unload":
                if len(args) < 1: raise ValueError("Нужен ID судна")
                system.unload_vessel(int(args[0]))

            elif cmd == "process":
                system.process_production()

            elif cmd == "sell":
                system.sell_production()

            elif cmd == "save":
                system.save_state()

            elif cmd == "load":
                system.load_state()

            else:
                print("Неизвестная команда.")

        except ValueError as ve:
            print(f"Ошибка ввода: {ve}")
        except GameException as ge:
            print(f"Ошибка логики: {ge}")
        except Exception as e:
            print(f"Критическая ошибка: {e}")


if __name__ == "__main__":
    main()