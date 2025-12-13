#ifndef SORTING_HPP
#define SORTING_HPP

#include <vector>
#include <functional>
#include <type_traits>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <iterator>

namespace lab {

    class InsertionSort {
    public:
        template<typename Iterator, typename Compare = std::less<>>
        static void sort(Iterator first, Iterator last, Compare comp = Compare{}) {
            using value_type = typename std::iterator_traits<Iterator>::value_type;
            if (first == last) return;
            for (Iterator it = std::next(first); it != last; ++it) {
                value_type key = *it;
                Iterator j = it;
                while (j != first && comp(key, *std::prev(j))) {
                    *j = *std::prev(j);
                    --j;
                }
                *j = std::move(key);
            }
        }

        template<typename Container, typename Compare = std::less<>>
        void operator()(Container& container, Compare comp = Compare{}) const {
            sort(container.begin(), container.end(), comp);
        }

        template<typename T, std::size_t N, typename Compare = std::less<>>
        void operator()(T(&arr)[N], Compare comp = Compare{}) const {
            sort(std::begin(arr), std::end(arr), comp);
        }
    };

    class CountingSort {
    public:
        template<typename Container, typename KeyFunc, typename KeyType = int>
        static void sort(Container& container, KeyFunc key_func, KeyType min_key, KeyType max_key) {
            static_assert(std::is_integral_v<KeyType>, "KeyType must be integral");
            if (min_key > max_key) throw std::invalid_argument("min_key > max_key");

            using value_type = typename Container::value_type;
            KeyType range = max_key - min_key + 1;
            if (range <= 0) throw std::invalid_argument("invalid key range");

            std::vector<std::size_t> counts(static_cast<std::size_t>(range), 0);

            for (const auto& item : container) {
                KeyType k = key_func(item);
                if (k < min_key || k > max_key) throw std::out_of_range("key out of expected range");
                ++counts[static_cast<std::size_t>(k - min_key)];
            }

            std::vector<std::size_t> positions(counts.size(), 0);
            std::size_t sum = 0;
            for (std::size_t i = 0; i < counts.size(); ++i) {
                positions[i] = sum;
                sum += counts[i];
            }

            std::vector<value_type> output;
            output.resize(container.size());

            for (const auto& item : container) {
                KeyType k = key_func(item);
                std::size_t idx = static_cast<std::size_t>(k - min_key);
                output[positions[idx]++] = item;
            }

            std::copy(output.begin(), output.end(), container.begin());
        }

        template<typename T>
        static typename std::enable_if_t<std::is_integral_v<T>, void>
        sort(std::vector<T>& v, T min_key, T max_key) {
            sort(v, [](const T& x) { return x; }, min_key, max_key);
        }

        template<typename T, std::size_t N>
        static typename std::enable_if_t<std::is_integral_v<T>, void>
        sort(T(&arr)[N], T min_key, T max_key) {
            std::vector<T> tmp(arr, arr + N);
            sort(tmp, [](const T& x) { return x; }, min_key, max_key);
            std::copy(tmp.begin(), tmp.end(), arr);
        }

        template<typename T, std::size_t N, typename KeyFunc, typename KeyType = int>
        static void sort(T(&arr)[N], KeyFunc key_func, KeyType min_key, KeyType max_key) {
            std::vector<T> tmp(arr, arr + N);
            sort(tmp, key_func, min_key, max_key);
            std::copy(tmp.begin(), tmp.end(), arr);
        }

        template<typename Container, typename KeyFunc, typename KeyType = int>
        void operator()(Container& container, KeyFunc key_func, KeyType min_key, KeyType max_key) const {
            sort(container, key_func, min_key, max_key);
        }

        template<typename T>
        typename std::enable_if_t<std::is_integral_v<T>, void>
        operator()(std::vector<T>& v, T min_key, T max_key) const {
            sort(v, min_key, max_key);
        }

        template<typename T, std::size_t N>
        typename std::enable_if_t<std::is_integral_v<T>, void>
        operator()(T(&arr)[N], T min_key, T max_key) const {
            sort(arr, min_key, max_key);
        }

        template<typename T, std::size_t N, typename KeyFunc, typename KeyType = int>
        void operator()(T(&arr)[N], KeyFunc key_func, KeyType min_key, KeyType max_key) const {
            sort(arr, key_func, min_key, max_key);
        }
    };

} // namespace lab

#endif // SORTING_HPP
