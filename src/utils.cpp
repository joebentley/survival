#include "utils.h"
#include <cstdlib>
#include <ctime>

double randDouble() {
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

std::vector<std::string> wordWrap(const std::string &toBeWrapped, size_t columns) {
    std::vector<std::string> lines;
    size_t index = 0;
    std::string currentSubstring = toBeWrapped.substr(0, columns);
    lines.push_back(currentSubstring);

    while (currentSubstring.size() == columns) {
        index += columns;
        currentSubstring = toBeWrapped.substr(index, columns);
        lines.push_back(currentSubstring);
    }

    return lines;
}

clock_t gStartTime;

void beginTime() {
    gStartTime = clock();
}

float endTime() {
    return (float)(clock() - gStartTime) / CLOCKS_PER_SEC;
}

