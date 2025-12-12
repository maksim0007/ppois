#include <iostream>
#include <vector>
#include <algorithm>
#include "include/person.hpp"
#include "include/sorting.hpp"
#include "include/graph.hpp"

int main() {
    using namespace lab;

    std::cout << "Demo: Insertion sort and Counting sort with Person\n";

    std::vector<Person> people{
        Person("Alice", 30),
        Person("Bob", 22),
        Person("Charlie", 22),
        Person("Dave", 45),
        Person("Eve", 30)
    };

    std::cout << "Before:\n";
    for (const auto& p : people) std::cout << p << "\n";

    insertion_sort(people, [](const Person& a, const Person& b) { return a < b; });

    std::cout << "\nAfter insertion_sort:\n";
    for (const auto& p : people) std::cout << p << "\n";

    std::vector<Person> people2 = {
        Person("Fay", 25), Person("Gus", 35), Person("Hank", 25), Person("Ivy", 28)
    };
    int min_age = 25, max_age = 35;
    counting_sort(people2, [](const Person& p) { return p.age(); }, min_age, max_age);

    std::cout << "\nAfter counting_sort by age:\n";
    for (const auto& p : people2) std::cout << p << "\n";

    std::cout << "\nDemo: Graph<std::string>\n";
    Graph<std::string> g;
    g.add_vertex("A"); g.add_vertex("B"); g.add_vertex("C");
    g.add_edge("A", "B");
    g.add_edge("B", "C");
    g.add_edge("A", "C");

    std::cout << "Graph contents:\n" << g << "\n";

    std::cout << "Iterate vertices:\n";
    for (auto it = g.vertices_begin(); it != g.vertices_end(); ++it) {
        std::cout << *it << " (deg=" << g.degree(*it) << ")\n";
        std::cout << " neighbors: ";
        for (auto nit = g.neighbors_begin(*it); nit != g.neighbors_end(*it); ++nit) {
            std::cout << *nit << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nIterate edges:\n";
    auto edges = g.get_all_edges();
    for (const auto& edge : edges) {
        std::cout << edge.u << " - " << edge.v << "\n";
    }

    return 0;
}