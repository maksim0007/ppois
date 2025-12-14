#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <utility>
#include <ostream>
#include <type_traits>
#include <set>

namespace lab {

    template<typename T>
   template<typename T>
   class DefaultGraphTraits {
   public:
     using value_type = T;
     static constexpr bool is_directed = false;
    };

    template<typename T, typename Traits = DefaultGraphTraits<T>>
    class Graph {
    public:
        using value_type = T;
        using traits_type = Traits;
        using adjacency_list_type = std::unordered_map<T, std::vector<T>>;
        using size_type = std::size_t;

        class Edge {
        private:
            T u_;
            T v_;

        public:
            Edge(const T& first, const T& second) : u_(first), v_(second) {}

            const T& first() const noexcept { return u_; }
            const T& second() const noexcept { return v_; }

            bool operator==(const Edge& other) const {
                if constexpr (traits_type::is_directed) {
                    return u_ == other.u_ && v_ == other.v_;
                } else {
                    return (u_ == other.u_ && v_ == other.v_) || (u_ == other.v_ && v_ == other.u_);
                }
            }
        };

        Graph() = default;
        Graph(const Graph& other) : adj_(other.adj_), edge_count_(other.edge_count_) {}
        Graph(Graph&&) noexcept = default;

        Graph& operator=(const Graph& other) {
            if (this != &other) {
                adj_ = other.adj_;
                edge_count_ = other.edge_count_;
            }
            return *this;
        }

        Graph& operator=(Graph&&) noexcept = default;
        ~Graph() = default;

        bool empty() const noexcept { return adj_.empty(); }
        void clear() noexcept {
            adj_.clear();
            edge_count_ = 0;
        }

        void add_vertex(const T& v) {
            if (contains_vertex(v)) throw std::invalid_argument("vertex already exists");
            adj_.emplace(v, std::vector<T>{});
        }

        void remove_vertex(const T& v) {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");

            // Óäàëÿåì âñå ðåáðà, ñâÿçàííûå ñ ýòîé âåðøèíîé
            for (const auto& neighbor : it->second) {
                if (neighbor == v) continue; // Ïðîïóñêàåì ïåòëþ

                auto& neighbors_list = adj_.at(neighbor);
                auto pos = std::find(neighbors_list.begin(), neighbors_list.end(), v);
                if (pos != neighbors_list.end()) {
                    neighbors_list.erase(pos);
                    if (edge_count_ > 0) edge_count_--;
                }
            }

            // Óäàëÿåì ïåòëè
            size_t loop_count = 0;
            for (const auto& neighbor : it->second) {
                if (neighbor == v) {
                    loop_count++;
                }
            }
            edge_count_ -= loop_count;

            adj_.erase(it);
        }

        bool contains_vertex(const T& v) const noexcept {
            return adj_.find(v) != adj_.end();
        }

        void add_edge(const T& a, const T& b) {
            if (!contains_vertex(a) || !contains_vertex(b))
                throw std::out_of_range("vertex not found");

            if (contains_edge(a, b))
                throw std::invalid_argument("edge already exists");

            adj_[a].push_back(b);
            if constexpr (!traits_type::is_directed) {
                if (a != b) {
                    adj_[b].push_back(a);
                    edge_count_++;
                } else {
                    edge_count_++;
                }
            } else {
                edge_count_++;
            }
        }

        void remove_edge(const T& a, const T& b) {
            if (!contains_vertex(a) || !contains_vertex(b))
                throw std::out_of_range("vertex not found");

            if (!contains_edge(a, b))
                throw std::out_of_range("edge not found");

            auto& neighbors_a = adj_.at(a);
            auto pos_a = std::find(neighbors_a.begin(), neighbors_a.end(), b);
            if (pos_a != neighbors_a.end()) {
                neighbors_a.erase(pos_a);
            }

            if constexpr (!traits_type::is_directed) {
                if (a != b) {
                    auto& neighbors_b = adj_.at(b);
                    auto pos_b = std::find(neighbors_b.begin(), neighbors_b.end(), a);
                    if (pos_b != neighbors_b.end()) {
                        neighbors_b.erase(pos_b);
                    }
                }
            }

            if (edge_count_ > 0) edge_count_--;
        }

        bool contains_edge(const T& a, const T& b) const {
            auto it = adj_.find(a);
            if (it == adj_.end()) return false;
            const auto& vec = it->second;
            return std::find(vec.begin(), vec.end(), b) != vec.end();
        }

        size_type vertex_count() const noexcept { return adj_.size(); }
        size_type edge_count() const noexcept { return edge_count_; }

        size_type degree(const T& v) const {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");
            return it->second.size();
        }

        class vertex_iterator {
        public:
            using map_iter = typename adjacency_list_type::iterator;
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using reference = T&;
            using pointer = T*;
            using difference_type = std::ptrdiff_t;

            vertex_iterator() = default;
            explicit vertex_iterator(map_iter it) : it_(it) {}

            T& operator*() const { return const_cast<T&>(it_->first); }
            T* operator->() const { return const_cast<T*>(&(it_->first)); }

            vertex_iterator& operator++() { ++it_; return *this; }
            vertex_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
            vertex_iterator& operator--() { --it_; return *this; }
            vertex_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }

            bool operator==(const vertex_iterator& o) const { return it_ == o.it_; }
            bool operator!=(const vertex_iterator& o) const { return !(*this == o); }

            map_iter base() const { return it_; }

        private:
            map_iter it_;
        };

        class const_vertex_iterator {
        public:
            using map_iter = typename adjacency_list_type::const_iterator;
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using reference = const T&;
            using pointer = const T*;
            using difference_type = std::ptrdiff_t;

            const_vertex_iterator() = default;
            explicit const_vertex_iterator(map_iter it) : it_(it) {}

            const T& operator*() const { return it_->first; }
            const T* operator->() const { return &(it_->first); }

            const_vertex_iterator& operator++() { ++it_; return *this; }
            const_vertex_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
            const_vertex_iterator& operator--() { --it_; return *this; }
            const_vertex_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }

            bool operator==(const const_vertex_iterator& o) const { return it_ == o.it_; }
            bool operator!=(const const_vertex_iterator& o) const { return !(*this == o); }

            map_iter base() const { return it_; }

        private:
            map_iter it_;
        };

        class edge_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = Edge;
            using reference = const Edge&;
            using pointer = const Edge*;
            using difference_type = std::ptrdiff_t;

            edge_iterator() = default;
            edge_iterator(typename std::vector<Edge>::const_iterator it) : it_(it) {}

            const Edge& operator*() const { return *it_; }
            const Edge* operator->() const { return &(*it_); }

            edge_iterator& operator++() { ++it_; return *this; }
            edge_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }

            bool operator==(const edge_iterator& other) const { return it_ == other.it_; }
            bool operator!=(const edge_iterator& other) const { return !(*this == other); }

        private:
            typename std::vector<Edge>::const_iterator it_;
        };

        class adjacency_iterator {
        public:
            using vec_iter = typename std::vector<T>::iterator;
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using reference = T&;
            using pointer = T*;
            using difference_type = std::ptrdiff_t;

            adjacency_iterator() = default;
            adjacency_iterator(vec_iter it) : it_(it) {}

            T& operator*() const { return *it_; }
            T* operator->() const { return &(*it_); }

            adjacency_iterator& operator++() { ++it_; return *this; }
            adjacency_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
            adjacency_iterator& operator--() { --it_; return *this; }
            adjacency_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }

            bool operator==(const adjacency_iterator& o) const { return it_ == o.it_; }
            bool operator!=(const adjacency_iterator& o) const { return !(*this == o); }

            vec_iter base() const { return it_; }
        private:
            vec_iter it_;
        };

        class const_adjacency_iterator {
        public:
            using vec_iter = typename std::vector<T>::const_iterator;
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = T;
            using reference = const T&;
            using pointer = const T*;
            using difference_type = std::ptrdiff_t;

            const_adjacency_iterator() = default;
            const_adjacency_iterator(vec_iter it) : it_(it) {}

            const T& operator*() const { return *it_; }
            const T* operator->() const { return &(*it_); }

            const_adjacency_iterator& operator++() { ++it_; return *this; }
            const_adjacency_iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
            const_adjacency_iterator& operator--() { --it_; return *this; }
            const_adjacency_iterator operator--(int) { auto tmp = *this; --(*this); return tmp; }

            bool operator==(const const_adjacency_iterator& o) const { return it_ == o.it_; }
            bool operator!=(const const_adjacency_iterator& o) const { return !(*this == o); }

            vec_iter base() const { return it_; }
        private:
            vec_iter it_;
        };

        vertex_iterator vertices_begin() {
            return vertex_iterator(adj_.begin());
        }
        vertex_iterator vertices_end() {
            return vertex_iterator(adj_.end());
        }
        const_vertex_iterator vertices_cbegin() const {
            return const_vertex_iterator(adj_.cbegin());
        }
        const_vertex_iterator vertices_cend() const {
            return const_vertex_iterator(adj_.cend());
        }

        adjacency_iterator neighbors_begin(const T& v) {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");
            return adjacency_iterator(it->second.begin());
        }
        adjacency_iterator neighbors_end(const T& v) {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");
            return adjacency_iterator(it->second.end());
        }

        const_adjacency_iterator neighbors_cbegin(const T& v) const {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");
            return const_adjacency_iterator(it->second.cbegin());
        }
        const_adjacency_iterator neighbors_cend(const T& v) const {
            auto it = adj_.find(v);
            if (it == adj_.end()) throw std::out_of_range("vertex not found");
            return const_adjacency_iterator(it->second.cend());
        }

        edge_iterator edges_begin() const {
            auto edges = get_all_edges();
            return edge_iterator(edges.begin());
        }

        edge_iterator edges_end() const {
            auto edges = get_all_edges();
            return edge_iterator(edges.end());
        }

        std::vector<Edge> get_all_edges() const {
            std::vector<Edge> edges;
            std::set<std::pair<T, T>> seen_edges;

            for (const auto& vertex_pair : adj_) {
                const T& vertex = vertex_pair.first;
                const auto& neighbors = vertex_pair.second;

                for (const T& neighbor : neighbors) {
                    if constexpr (traits_type::is_directed) {
                        edges.emplace_back(vertex, neighbor);
                    } else {
                        // Äëÿ íåîðèåíòèðîâàííîãî ãðàôà äîáàâëÿåì êàæäîå ðåáðî òîëüêî îäèí ðàç
                        T first = std::min(vertex, neighbor);
                        T second = std::max(vertex, neighbor);

                        if (seen_edges.insert({first, second}).second) {
                            edges.emplace_back(first, second);
                        }
                    }
                }
            }

            return edges;
        }

        friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
            os << "Graph (vertices: " << g.vertex_count() << ", edges: " << g.edge_count() << ")\n";

            for (auto it = g.vertices_cbegin(); it != g.vertices_cend(); ++it) {
                const T& vertex = *it;
                os << "  " << vertex << " -> ";

                for (auto nit = g.neighbors_cbegin(vertex); nit != g.neighbors_cend(vertex); ++nit) {
                    os << *nit << " ";
                }
                os << "\n";
            }

            return os;
        }

    private:
        adjacency_list_type adj_;
        size_type edge_count_ = 0;
    };

} // namespace lab

#endif // GRAPH_HPP


