import unittest
import os
import random
from unittest.mock import patch
from io import StringIO
from fishing_system import (
    FishingProductionSystem, Fish, Net, Fisherman, Vessel, ColdStorage, FishMarket,
    GameException, StorageFullError, ResourceNotFoundError, FishStatus, FishType, main
)

# Фиксируем генератор случайных чисел, чтобы улов в тестах всегда был одинаковым и не вызывал ошибок
random.seed(42)


class TestModels(unittest.TestCase):
    def test_fish(self):
        f = Fish(species=FishType.SALMON.value, weight=2.5, price_per_kg=10.0)
        self.assertEqual(f.status, FishStatus.FRESH.value)
        d = f.to_dict()
        self.assertEqual(d['species'], FishType.SALMON.value)

    def test_net_and_fisherman(self):
        n = Net(id=1, name="Трал", capacity=100.0, mesh_size=10)
        f = Fisherman(id=1, name="Иван", role="Капитан")
        self.assertEqual(n.to_dict()['name'], "Трал")
        self.assertEqual(f.to_dict()['name'], "Иван")

    def test_vessel(self):
        v = Vessel(id=1, name="Санта-Мария", capacity=50.0)
        n = Net(id=1, name="Сеть", capacity=20.0, mesh_size=5)
        f = Fisherman(id=1, name="Петр")

        with self.assertRaises(GameException):
            v.catch_fish()

        v.equip_net(n)
        with self.assertRaises(GameException):
            v.catch_fish()

        v.add_crew_member(f)

        # Благодаря random.seed(42) этот улов теперь гарантированно > 0
        catch = v.catch_fish()
        self.assertTrue(len(v.cargo) > 0)

        d = v.to_dict()
        v2 = Vessel.from_dict(d)
        self.assertEqual(v2.name, v.name)
        self.assertEqual(len(v2.crew), 1)

    def test_vessel_overload(self):
        v = Vessel(1, "Малыш", 1.0)
        v.equip_net(Net(1, "Огромная сеть", 100.0, 10))
        v.add_crew_member(Fisherman(1, "Иван"))
        v.cargo.append(Fish(FishType.COD.value, weight=1.0))
        with self.assertRaises(StorageFullError):
            v.catch_fish()

    def test_cold_storage_and_market(self):
        cs = ColdStorage(capacity=20.0)
        market = FishMarket()

        fish1 = Fish(FishType.TUNA.value, 10.0)
        fish2 = Fish(FishType.HERRING.value, 15.0)

        with self.assertRaises(StorageFullError):
            cs.store_fish([fish1, fish2])

        cs.store_fish([fish1])
        self.assertEqual(len(cs.storage), 1)

        rev = market.sell_products(cs)
        self.assertEqual(rev, 0.0)

        processed_count = cs.process_all_fish()
        self.assertEqual(processed_count, 1)
        self.assertEqual(cs.storage[0].status, FishStatus.PROCESSED.value)

        rev = market.sell_products(cs)
        self.assertTrue(rev > 0)
        self.assertEqual(len(cs.storage), 0)

        cs_dict = cs.to_dict()
        cs2 = ColdStorage.from_dict(cs_dict)
        self.assertEqual(cs2.capacity, cs.capacity)

        m_dict = market.to_dict()
        m2 = FishMarket.from_dict(m_dict)
        self.assertEqual(m2.balance, market.balance)


class TestFishingProductionSystem(unittest.TestCase):
    def setUp(self):
        self.system = FishingProductionSystem()
        self.test_filename = "test_data.json"

    def tearDown(self):
        if os.path.exists(self.test_filename):
            os.remove(self.test_filename)

    def test_system_creation_and_assignment(self):
        self.system.create_vessel("Лодочка", 100.0)
        self.system.create_fisherman("Рыбак")
        self.system.create_net("Сеточка", 50.0)

        with self.assertRaises(ResourceNotFoundError):
            self.system.assign_crew(99, 99)

        with self.assertRaises(ResourceNotFoundError):
            self.system.equip_vessel(99, 99)

        self.system.assign_crew(1, 1)
        self.system.equip_vessel(1, 1)

        self.assertEqual(len(self.system.vessels[0].crew), 1)

    def test_full_process_and_status(self):
        self.system.create_vessel("Титаник", 500.0)
        self.system.create_fisherman("Джек")
        self.system.create_net("Трал", 100.0)
        self.system.assign_crew(1, 1)
        self.system.equip_vessel(1, 1)

        with self.assertRaises(ResourceNotFoundError):
            self.system.perform_fishing(99)

        self.system.perform_fishing(1)

        with self.assertRaises(ResourceNotFoundError):
            self.system.unload_vessel(99)

        self.system.unload_vessel(1)
        self.system.unload_vessel(1)

        self.system.process_production()
        self.system.sell_production()
        self.system.show_status()
        self.assertTrue(self.system.market.balance >= 0)

    def test_save_and_load(self):
        self.system.create_vessel("Бриг", 300.0)
        self.system.save_state(self.test_filename)
        self.assertTrue(os.path.exists(self.test_filename))

        new_system = FishingProductionSystem()
        new_system.load_state(self.test_filename)
        self.assertEqual(len(new_system.vessels), 1)
        self.assertEqual(new_system.vessels[0].name, "Бриг")

    def test_load_non_existent(self):
        self.system.load_state("fake_file.json")
        self.assertEqual(len(self.system.vessels), 0)


class TestCLI(unittest.TestCase):
    @patch('builtins.input', side_effect=[
        'help',
        'add_vessel Виктория 500',
        'add_fisher Борис',
        'add_net Трал 200',
        'assign 1 1',
        'equip 1 1',
        'fish 1',
        'unload 1',
        'process',
        'sell',
        'status',
        'save',
        'load',
        'abracadabra',
        'add_vessel',
        'exit'
    ])
    @patch('sys.stdout', new_callable=StringIO)
    def test_cli_execution(self, mock_stdout, mock_input):
        """Тест проверяет ввод команд пользователем (через mock)."""
        main()
        output = mock_stdout.getvalue()
        self.assertIn("Выход из системы...", output)


if __name__ == '__main__':
    unittest.main()