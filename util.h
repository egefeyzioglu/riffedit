#pragma once

#include <string>

#define TMP_SUBDIRECTORY "riffedit"
#define MAX_TMP_TRIES 1000000000

// Returns the string representation of a random number `len` bytes long
// Max length is 64, `len` will be capped at 264
std::string rand_num_string(int len);
