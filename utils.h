#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <string>

float randFloat();
std::vector<std::string> wordWrap(const std::string &toBeWrapped, size_t columns);

void beginTime();
float endTime();

#endif // UTILS_H_
