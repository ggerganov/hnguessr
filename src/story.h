#pragma once

#include <string>
#include <vector>

struct Token {
    bool toGuess = false;
    std::string text;
};

struct Story {
    std::string by;
    std::string descendants;
    std::string id;
    std::string score;
    std::string time;
    std::string title;
    std::string url;

    std::string out;
    std::vector<Token> tokens;

    void randomize();
};

