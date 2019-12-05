#pragma once

#include <iostream>

#define OUTPUT_ENABLED 1

#if OUTPUT_ENABLED == 1
#define OUTPUT(output) (std::cout << output << std::endl)
#define OUTPUT_ERROR(output) (std::cerr << output << std::endl)
#else
#define OUTPUT(output)
#define OUTPUT_ERROR(output)
#endif