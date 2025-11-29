#pragma once

class Rule {
public:
    std::string pattern;
    std::string replace;
    bool moveRight = true;
};
