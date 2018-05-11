#include "utils.h"

float randFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}