#include "story.h"

void Story::randomize() {
    for (auto & token : tokens) {
        token.toGuess = false;
    }

    int nLongTokens = 0;
    for (auto & token : tokens) {
        if (token.text.size() > 3) {
            ++nLongTokens;
        }
    }

    int nGuess = 0;
    if (nLongTokens > 1) {
        nGuess = 1;
    }
    if (nLongTokens > 3) {
        nGuess = rand() % 2 + 1;
    }

    while (nGuess > 0) {
        int tid = rand() % tokens.size();
        if (!tokens[tid].toGuess && tokens[tid].text.size() > 3) {
            tokens[tid].toGuess = true;
            nGuess--;
        }
    }
}
