#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <stdexcept>
#include <ostream>

namespace lab {

    class Person {
    public:
        Person() = default;
        Person(std::string name, int age)
            : name_(std::move(name)), age_(age)
        {
            if (age_ < 0) throw std::invalid_argument("age must be non-negative");
        }

        const std::string& name() const noexcept { return name_; }
        int age() const noexcept { return age_; }

        bool operator<(const Person& other) const noexcept {
            if (age_ != other.age_) return age_ < other.age_;
            return name_ < other.name_;
        }
        bool operator>(const Person& other) const noexcept { return other < *this; }
        bool operator==(const Person& other) const noexcept {
            return age_ == other.age_ && name_ == other.name_;
        }
        bool operator!=(const Person& other) const noexcept { return !(*this == other); }
        bool operator<=(const Person& other) const noexcept { return !(*this > other); }
        bool operator>=(const Person& other) const noexcept { return !(*this < other); }

    private:
        std::string name_;
        int age_ = 0;
    };

    inline std::ostream& operator<<(std::ostream& os, const Person& p) {
        os << "Person{name:\"" << p.name() << "\", age:" << p.age() << "}";
        return os;
    }

}

#endif
