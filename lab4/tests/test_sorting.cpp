#include <gtest/gtest.h>
#include "../src/include/sorting.hpp"
#include "../src/include/person.hpp"
#include <vector>
#include <algorithm>

using namespace lab;

TEST(SortingTest, InsertionSortVector) {
    std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6 };
    insertion_sort(v);

    std::vector<int> expected = { 1, 1, 2, 3, 4, 5, 6, 9 };
    EXPECT_EQ(v, expected);
}

TEST(SortingTest, InsertionSortArray) {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    insertion_sort(arr);

    std::vector<int> expected = { 1, 1, 2, 3, 4, 5, 6, 9 };
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(arr[i], expected[i]);
    }
}

TEST(SortingTest, CountingSortVectorInt) {
    std::vector<int> v = { 3, 1, 4, 1, 5, 2, 6 };
    counting_sort(v, 1, 6);

    std::vector<int> expected = { 1, 1, 2, 3, 4, 5, 6 };
    EXPECT_EQ(v, expected);
}

TEST(SortingTest, CountingSortArrayInt) {
    int arr[] = { 3, 1, 4, 1, 5, 2, 6 };
    counting_sort(arr, 1, 6);

    std::vector<int> expected = { 1, 1, 2, 3, 4, 5, 6 };
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(arr[i], expected[i]);
    }
}

TEST(SortingTest, CountingSortVectorPerson) {
    std::vector<Person> people = {
        Person("Alice", 30),
        Person("Bob", 22),
        Person("Charlie", 22),
        Person("Dave", 45),
        Person("Eve", 30)
    };

    counting_sort(people, [](const Person& p) { return p.age(); }, 22, 45);

    EXPECT_EQ(people[0].name(), "Bob");
    EXPECT_EQ(people[1].name(), "Charlie");
    EXPECT_EQ(people[2].name(), "Alice");
    EXPECT_EQ(people[3].name(), "Eve");
    EXPECT_EQ(people[4].name(), "Dave");
}

TEST(SortingTest, InsertionSortVectorPerson) {
    std::vector<Person> people = {
        Person("Alice", 30),
        Person("Bob", 22),
        Person("Charlie", 22),
        Person("Dave", 45),
        Person("Eve", 30)
    };

    insertion_sort(people);

    EXPECT_EQ(people[0].name(), "Bob");
    EXPECT_EQ(people[1].name(), "Charlie");
    EXPECT_EQ(people[2].name(), "Alice");
    EXPECT_EQ(people[3].name(), "Eve");
    EXPECT_EQ(people[4].name(), "Dave");
}

TEST(SortingTest, CountingSortWithOutOfRange) {
    std::vector<int> v = { 3, 1, 7, 2 }; // 7 is out of range

    EXPECT_THROW(counting_sort(v, 1, 6), std::out_of_range);
}

TEST(SortingTest, InsertionSortDescending) {
    std::vector<int> v = { 3, 1, 4, 1, 5, 9, 2, 6 };
    insertion_sort(v, std::greater<int>());

    std::vector<int> expected = { 9, 6, 5, 4, 3, 2, 1, 1 };
    EXPECT_EQ(v, expected);
}