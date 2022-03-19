#pragma once

#include "story.h"

constexpr int  kFirstDay    = 19070;
constexpr char kTokenSymbol = '$';
constexpr auto kName        = "HNGuessr";
constexpr auto kTitle       = "HNGuessr";
constexpr auto kDescription = "Guess the Hacker News titles";
constexpr auto kURL         = "https://hnguessr.ggerganov.com";
constexpr auto kDomain      = "ggerganov.com";

struct State {
    int64_t timestamp_s = 0;
    int32_t curDay = 0;
    int32_t curIssue = 0;
    std::vector<Story> stories;

    void clear();
    bool reload(const char *filename);
    bool loadConfig();
    bool saveConfig();
    bool generateIndex(const char * filename, int nStories);
    bool generateHTML();

    std::string getConfigFilename() const {
        return "config-" + std::to_string(curDay) + ".txt";
    }
};

