#include <gtest/gtest.h>
#include "PostMachine.h"

TEST(PostMachine, DefaultTape) {
    PostMachine pm;
    EXPECT_EQ(pm.tape(), "_");
}

TEST(PostMachine, CustomTape) {
    PostMachine pm("abc");
    EXPECT_EQ(pm.tape(), "abc");
    EXPECT_EQ(pm.pos(), 0);
}

TEST(PostMachine, EmptyTapeHandled) {
    PostMachine pm("");
    EXPECT_EQ(pm.tape(), "_");
}

TEST(PostMachine, AddRule) {
    PostMachine pm;
    pm.addRule("a", "X");
    EXPECT_EQ(pm.ruleCount(), 1);
}

TEST(PostMachine, RemoveRule) {
    PostMachine pm;
    pm.addRule("a", "X");
    pm.removeRule(0);
    EXPECT_EQ(pm.ruleCount(), 0);
}

TEST(PostMachine, StepSuccess) {
    PostMachine pm("ab");
    pm.addRule("a", "X");
    ASSERT_TRUE(pm.step());
    EXPECT_EQ(pm.tape(), "Xb");
    EXPECT_EQ(pm.pos(), 1);
}

TEST(PostMachine, StepNoMatch) {
    PostMachine pm("xyz");
    pm.addRule("a", "X");
    EXPECT_FALSE(pm.step());
}

TEST(PostMachine, StepFirstMatch) {
    PostMachine pm("aa");
    pm.addRule("aa", "b");
    pm.addRule("a", "z");
    ASSERT_TRUE(pm.step());
    EXPECT_EQ(pm.tape(), "b");
}

TEST(PostMachine, MultiStep) {
    PostMachine pm("aaaa");
    pm.addRule("aa", "b");
    pm.step();  // "baa"
    pm.step();  // "bb"
    EXPECT_EQ(pm.tape(), "bb");
}

TEST(PostMachine, RunLimit) {
    PostMachine pm("aaa");
    pm.addRule("aa", "b");
    pm.run(5);
    EXPECT_EQ(pm.tape(), "ba");
}

TEST(PostMachine, RunStops) {
    PostMachine pm("xyz");
    pm.addRule("a", "X");
    pm.run();  // ничего не делает
    EXPECT_EQ(pm.tape(), "xyz");
}

TEST(PostMachine, PostIncOperator) {
    PostMachine pm("ab");
    pm.addRule("a", "X");
    ++pm;
    EXPECT_EQ(pm.tape(), "Xb");
}

TEST(PostMachine, StreamIO) {
    std::stringstream ss("hello");
    PostMachine pm;
    ss >> pm;
    EXPECT_EQ(pm.tape(), "hello");
}

TEST(PostMachine, Insertion) {
    PostMachine pm("a");
    pm.addRule("a", "xyz");
    pm.step();
    EXPECT_EQ(pm.tape(), "xyz");
}

TEST(PostMachine, Deletion) {
    PostMachine pm("hello");
    pm.addRule("ll", "");
    pm.step();
    EXPECT_EQ(pm.tape(), "heo");
}
