#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>

double randDouble();
std::vector<std::string> wordWrap(const std::string &toBeWrapped, size_t columns);
std::string repeat(int n, const std::string &str);

void beginTime();
float endTime();

#endif // UTILS_H_
