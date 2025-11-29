#include "PostMachine.h"
#include <algorithm>

PostMachine::PostMachine(const std::string& tape)
    : m_tape(tape.empty() ? "_" : tape) {}

void PostMachine::addRule(const std::string& pattern, const std::string& replace, bool moveRight) {
    if (pattern.empty()) return;
    m_rules.push_back({pattern, replace, moveRight});
}

void PostMachine::removeRule(size_t index) {
    if (index < m_rules.size())
        m_rules.erase(m_rules.begin() + index);
}

bool PostMachine::step() {
    for (const auto& rule : m_rules) {
        size_t pos = m_tape.find(rule.pattern);
        if (pos != std::string::npos) {
            m_tape.replace(pos, rule.pattern.length(), rule.replace);
            m_pos = rule.moveRight ? (pos + rule.replace.length()) : pos;
            if (m_pos > m_tape.length()) m_pos = m_tape.length();
            return true;
        }
    }
    return false;
}

void PostMachine::run(int maxSteps) {
    int steps = 0;
    while ((maxSteps < 0 || steps < maxSteps) && step())
        ++steps;
}

std::ostream& operator<<(std::ostream& os, const PostMachine& pm) {
    os << pm.m_tape << " | " << pm.m_pos;
    return os;
}
std::istream& operator>>(std::istream& is, PostMachine& pm) {
    return is >> pm.m_tape, pm.m_pos = 0, is;
}
