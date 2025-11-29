#include "Rect2D.h"
#include <algorithm>

Rect2D::Rect2D() : x1(0), y1(0), x2(0), y2(0) {}

Rect2D::Rect2D(int x1, int y1, int x2, int y2)
    : x1(x1), y1(y1), x2(x2), y2(y2) {
    normalize();
}

Rect2D::Rect2D(const Rect2D& other)
    : x1(other.x1), y1(other.y1), x2(other.x2), y2(other.y2) {}

Rect2D& Rect2D::operator=(const Rect2D& other) {
    if (this != &other) {
        x1 = other.x1; y1 = other.y1;
        x2 = other.x2; y2 = other.y2;
    }
    return *this;
}

void Rect2D::normalize() {
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
}

void Rect2D::move(int dx, int dy) {
    x1 += dx; y1 += dy;
    x2 += dx; y2 += dy;
}

void Rect2D::resize(int w, int h) {
    if (w < 0 || h < 0) return;
    x2 = x1 + w;
    y2 = y1 + h;
}

Rect2D& Rect2D::operator++() {
    x2++; y2++;
    return *this;
}
Rect2D Rect2D::operator++(int) {
    Rect2D old = *this;
    ++(*this);
    return old;
}

Rect2D& Rect2D::operator--() {
    if (width() > 0) x2--;
    if (height() > 0) y2--;
    return *this;
}
Rect2D Rect2D::operator--(int) {
    Rect2D old = *this;
    --(*this);
    return old;
}

Rect2D Rect2D::operator+(const Rect2D& r) const {
    int l = std::min(left(), r.left());
    int t = std::min(top(), r.top());
    int R = std::max(right(), r.right());
    int B = std::max(bottom(), r.bottom());
    return Rect2D(l, t, R, B);
}
Rect2D& Rect2D::operator+=(const Rect2D& r) {
    *this = *this + r;
    return *this;
}

Rect2D Rect2D::operator-(const Rect2D& r) const {
    int l = std::max(left(), r.left());
    int t = std::max(top(), r.top());
    int R = std::min(right(), r.right());
    int B = std::min(bottom(), r.bottom());
    if (l >= R || t >= B) return Rect2D(0, 0, 0, 0);
    return Rect2D(l, t, R, B);
}
Rect2D& Rect2D::operator-=(const Rect2D& r) {
    *this = *this - r;
    return *this;
}

bool Rect2D::operator==(const Rect2D& r) const {
    return left() == r.left() && top() == r.top() &&
           right() == r.right() && bottom() == r.bottom();
}

std::ostream& operator<<(std::ostream& os, const Rect2D& r) {
    return os << r.x1 << " " << r.y1 << " " << r.x2 << " " << r.y2;
}
std::istream& operator>>(std::istream& is, Rect2D& r) {
    return is >> r.x1 >> r.y1 >> r.x2 >> r.y2, r.normalize(), is;
}
