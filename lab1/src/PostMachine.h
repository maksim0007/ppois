#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <Rule.h>
class Rule {
public:
    std::string pattern;
    std::string replace;
    bool moveRight = true;
};

class PostMachine {
    std::string m_tape = "_";
    size_t m_pos = 0;
    std::vector<Rule> m_rules;

public:
    PostMachine() = default;
    explicit PostMachine(const std::string& tape);

    void addRule(const std::string& pattern, const std::string& replace, bool moveRight = true);
    void removeRule(size_t index);
    size_t ruleCount() const { return m_rules.size(); }

    bool step();
    PostMachine& operator++() { step(); return *this; }
    void run(int maxSteps = -1);

    const std::string& tape() const { return m_tape; }
    size_t pos() const { return m_pos; }

    friend std::ostream& operator<<(std::ostream& os, const PostMachine& pm);
    friend std::istream& operator>>(std::istream& is, PostMachine& pm);
};


