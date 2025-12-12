#include <algorithm>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "../src/include/graph.hpp"

using namespace lab;

TEST(GraphTest, BasicOperations) {
    Graph<int> g;
    EXPECT_TRUE(g.empty());
    EXPECT_EQ(g.vertex_count(), 0);
    EXPECT_EQ(g.edge_count(), 0);

    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);

    EXPECT_FALSE(g.empty());
    EXPECT_EQ(g.vertex_count(), 3);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_TRUE(g.contains_vertex(1));
    EXPECT_TRUE(g.contains_vertex(2));
    EXPECT_TRUE(g.contains_vertex(3));

    g.add_edge(1, 2);
    g.add_edge(2, 3);

    EXPECT_EQ(g.edge_count(), 2);
    EXPECT_TRUE(g.contains_edge(1, 2));
    EXPECT_TRUE(g.contains_edge(2, 3));
    EXPECT_FALSE(g.contains_edge(1, 3));

    EXPECT_EQ(g.degree(1), 1);
    EXPECT_EQ(g.degree(2), 2);
    EXPECT_EQ(g.degree(3), 1);
}

TEST(GraphTest, RemoveOperations) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_edge(1, 2);
    g.add_edge(2, 3);

    g.remove_edge(1, 2);
    EXPECT_EQ(g.edge_count(), 1);
    EXPECT_FALSE(g.contains_edge(1, 2));

    g.remove_vertex(3);
    EXPECT_EQ(g.vertex_count(), 2);
    EXPECT_EQ(g.edge_count(), 0);
    EXPECT_FALSE(g.contains_vertex(3));
    EXPECT_FALSE(g.contains_edge(2, 3));
}

TEST(GraphTest, StringGraph) {
    Graph<std::string> g;
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_edge("A", "B");
    g.add_edge("B", "C");

    EXPECT_TRUE(g.contains_edge("A", "B"));
    EXPECT_TRUE(g.contains_edge("B", "C"));
    EXPECT_FALSE(g.contains_edge("A", "C"));

    EXPECT_EQ(g.degree("A"), 1);
    EXPECT_EQ(g.degree("B"), 2);
    EXPECT_EQ(g.degree("C"), 1);
}

TEST(GraphTest, EdgeIterationBasic) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);

    // Проверяем с пустым графом
    auto edges = g.get_all_edges();
    EXPECT_EQ(edges.size(), 0);

    // Добавляем ребро
    g.add_edge(1, 2);
    edges = g.get_all_edges();
    EXPECT_EQ(edges.size(), 1);
    EXPECT_TRUE((edges[0].u == 1 && edges[0].v == 2) || (edges[0].u == 2 && edges[0].v == 1));

    // Добавляем еще одно ребро
    g.add_edge(2, 3);
    edges = g.get_all_edges();
    EXPECT_EQ(edges.size(), 2);
    bool has_edge_12 = false;
    bool has_edge_23 = false;
    for (const auto& e : edges) {
        if ((e.u == 1 && e.v == 2) || (e.u == 2 && e.v == 1)) has_edge_12 = true;
        if ((e.u == 2 && e.v == 3) || (e.u == 3 && e.v == 2)) has_edge_23 = true;
    }
    EXPECT_TRUE(has_edge_12);
    EXPECT_TRUE(has_edge_23);

    // Добавляем третье ребро
    g.add_edge(1, 3);
    edges = g.get_all_edges();
    EXPECT_EQ(edges.size(), 3);
    bool has_edge_13 = false;
    for (const auto& e : edges) {
        if ((e.u == 1 && e.v == 3) || (e.u == 3 && e.v == 1)) has_edge_13 = true;
    }
    EXPECT_TRUE(has_edge_13);
}

TEST(GraphTest, VertexIteration) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_edge(1, 2);
    g.add_edge(2, 3);

    std::vector<int> vertices;
    for (auto it = g.vertices_begin(); it != g.vertices_end(); ++it) {
        vertices.push_back(*it);
    }
    EXPECT_EQ(vertices.size(), 3);
    std::sort(vertices.begin(), vertices.end());
    EXPECT_EQ(vertices[0], 1);
    EXPECT_EQ(vertices[1], 2);
    EXPECT_EQ(vertices[2], 3);
}

TEST(GraphTest, AdjacencyIteration) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(2);
    g.add_vertex(3);
    g.add_edge(1, 2);
    g.add_edge(1, 3);

    std::vector<int> neighbors;
    for (auto it = g.neighbors_begin(1); it != g.neighbors_end(1); ++it) {
        neighbors.push_back(*it);
    }
    EXPECT_EQ(neighbors.size(), 2);

    // Не можем гарантировать порядок соседей в unordered_map
    bool has_2 = false;
    bool has_3 = false;
    for (int n : neighbors) {
        if (n == 2) has_2 = true;
        if (n == 3) has_3 = true;
    }
    EXPECT_TRUE(has_2);
    EXPECT_TRUE(has_3);
}

TEST(GraphTest, LoopEdge) {
    Graph<int> g;
    g.add_vertex(1);
    g.add_vertex(2);

    // Добавляем петлю
    g.add_edge(1, 1);
    EXPECT_TRUE(g.contains_edge(1, 1));
    EXPECT_EQ(g.degree(1), 1);
    EXPECT_EQ(g.edge_count(), 1);

    // Добавляем обычное ребро
    g.add_edge(1, 2);
    EXPECT_TRUE(g.contains_edge(1, 2));
    EXPECT_EQ(g.degree(1), 2);
    EXPECT_EQ(g.degree(2), 1);
    EXPECT_EQ(g.edge_count(), 2);

    // Проверяем итерацию по ребрам
    auto edges = g.get_all_edges();
    EXPECT_EQ(edges.size(), 2);
}

TEST(GraphTest, Exceptions) {
    Graph<int> g;

    // Проверка исключений при работе с вершинами
    EXPECT_FALSE(g.contains_vertex(1));
    EXPECT_THROW(g.degree(1), std::out_of_range);
    EXPECT_THROW(g.remove_vertex(1), std::out_of_range);

    // Добавляем вершину
    g.add_vertex(1);
    EXPECT_THROW(g.add_vertex(1), std::invalid_argument);

    // Проверка исключений при работе с ребрами
    EXPECT_THROW(g.add_edge(1, 2), std::out_of_range); // несуществующая вершина 2

    g.add_vertex(2);
    EXPECT_THROW(g.add_edge(1, 3), std::out_of_range); // несуществующая вершина 3
    EXPECT_THROW(g.remove_edge(1, 2), std::out_of_range); // несуществующее ребро

    g.add_edge(1, 2);
    EXPECT_THROW(g.add_edge(1, 2), std::invalid_argument); // дубликат ребра
    EXPECT_NO_THROW(g.remove_edge(1, 2));
    EXPECT_THROW(g.remove_edge(1, 2), std::out_of_range); // повторное удаление
}

TEST(GraphTest, OutputOperator) {
    Graph<std::string> g;
    g.add_vertex("A");
    g.add_vertex("B");
    g.add_vertex("C");
    g.add_edge("A", "B");
    g.add_edge("B", "C");

    std::stringstream ss;
    ss << g;
    std::string result = ss.str();

    EXPECT_NE(result.find("vertices: 3"), std::string::npos);
    EXPECT_NE(result.find("edges: 2"), std::string::npos);
    EXPECT_NE(result.find("A -> B"), std::string::npos);
    EXPECT_NE(result.find("B -> A C"), std::string::npos);
    EXPECT_NE(result.find("C -> B"), std::string::npos);
}