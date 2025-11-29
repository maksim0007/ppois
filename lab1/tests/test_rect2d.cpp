#include <gtest/gtest.h>
#include <sstream>
#include "Rect2D.h"

TEST(Rect2D, DefaultConstruct) {
    Rect2D r;
    EXPECT_EQ(r.left(), 0);
    EXPECT_EQ(r.top(), 0);
    EXPECT_EQ(r.right(), 0);
    EXPECT_EQ(r.bottom(), 0);
}

TEST(Rect2D, ConstructAndNormalize) {
    Rect2D r(5, 5, 1, 1);
    EXPECT_EQ(r.left(), 1);
    EXPECT_EQ(r.top(), 1);
    EXPECT_EQ(r.right(), 5);
    EXPECT_EQ(r.bottom(), 5);
}

TEST(Rect2D, CopyConstruct) {
    Rect2D r1(1,2,3,4);
    Rect2D r2(r1);
    EXPECT_TRUE(r1 == r2);
}

TEST(Rect2D, Assign) {
    Rect2D r1(1,1,2,2), r2(0,0,0,0);
    r2 = r1;
    EXPECT_TRUE(r1 == r2);
}

TEST(Rect2D, Move) {
    Rect2D r(0,0,2,2);
    r.move(3,4);
    EXPECT_EQ(r.left(), 3);
    EXPECT_EQ(r.top(), 4);
    EXPECT_EQ(r.right(), 5);
    EXPECT_EQ(r.bottom(), 6);
}

TEST(Rect2D, Resize) {
    Rect2D r(0,0,2,2);
    r.resize(5,3);
    EXPECT_EQ(r.width(), 5);
    EXPECT_EQ(r.height(), 3);
}

TEST(Rect2D, PreInc) {
    Rect2D r(0,0,2,2);
    ++r;
    EXPECT_EQ(r.width(), 3);
    EXPECT_EQ(r.height(), 3);
}

TEST(Rect2D, PostInc) {
    Rect2D r(0,0,2,2);
    Rect2D old = r++;
    EXPECT_EQ(old.width(), 2);
    EXPECT_EQ(r.width(), 3);
}

TEST(Rect2D, PreDec) {
    Rect2D r(0,0,2,2);
    --r;
    EXPECT_EQ(r.width(), 1);
    EXPECT_EQ(r.height(), 1);
}

TEST(Rect2D, PostDec) {
    Rect2D r(0,0,2,2);
    Rect2D old = r--;
    EXPECT_EQ(old.width(), 2);
    EXPECT_EQ(r.width(), 1);
}

TEST(Rect2D, Union) {
    Rect2D a(0,0,2,2), b(1,1,3,3);
    Rect2D u = a + b;
    EXPECT_EQ(u.left(), 0);
    EXPECT_EQ(u.top(), 0);
    EXPECT_EQ(u.right(), 3);
    EXPECT_EQ(u.bottom(), 3);
}

TEST(Rect2D, UnionAssign) {
    Rect2D a(0,0,2,2);
    a += Rect2D(1,1,3,3);
    EXPECT_EQ(a.right(), 3);
}

TEST(Rect2D, Intersection) {
    Rect2D a(0,0,3,3), b(1,1,4,4);
    Rect2D i = a - b;
    EXPECT_EQ(i.left(), 1);
    EXPECT_EQ(i.top(), 1);
    EXPECT_EQ(i.right(), 3);
    EXPECT_EQ(i.bottom(), 3);
}

TEST(Rect2D, IntersectionEmpty) {
    Rect2D a(0,0,1,1), b(2,2,3,3);
    Rect2D i = a - b;
    EXPECT_EQ(i.width(), 0);
    EXPECT_EQ(i.height(), 0);
}

TEST(Rect2D, Equality) {
    Rect2D a(0,0,2,2), b(0,0,2,2), c(0,0,3,3);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
}

TEST(Rect2D, StreamIO) {
    Rect2D r(1,2,3,4);
    std::stringstream ss;
    ss << r;
    Rect2D r2;
    ss >> r2;
    EXPECT_TRUE(r == r2);
}
