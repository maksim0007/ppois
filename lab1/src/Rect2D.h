#pragma once
#include <iostream>

class Rect2D {
    int x1, y1, x2, y2;

    void normalize();

public:
    Rect2D();
    Rect2D(int x1, int y1, int x2, int y2);
    Rect2D(const Rect2D& other);
    ~Rect2D() = default;
    Rect2D& operator=(const Rect2D& other);

    int left() const { return std::min(x1, x2); }
    int top() const { return std::min(y1, y2); }
    int right() const { return std::max(x1, x2); }
    int bottom() const { return std::max(y1, y2); }
    int width() const { return right() - left(); }
    int height() const { return bottom() - top(); }

    void move(int dx, int dy);
    void resize(int w, int h);

    Rect2D& operator++();
    Rect2D operator++(int);
    Rect2D& operator--();
    Rect2D operator--(int);

    Rect2D operator+(const Rect2D& r) const;
    Rect2D& operator+=(const Rect2D& r);
    Rect2D operator-(const Rect2D& r) const;
    Rect2D& operator-=(const Rect2D& r);

    bool operator==(const Rect2D& r) const;
    bool operator!=(const Rect2D& r) const { return !(*this == r); }

    friend std::ostream& operator<<(std::ostream& os, const Rect2D& r);
    friend std::istream& operator>>(std::istream& is, Rect2D& r);
};
