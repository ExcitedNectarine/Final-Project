#pragma once

#include <iostream>

#define OUTPUT_ENABLED 1

#if OUTPUT_ENABLED == 1
#define OUTPUT(output) (std::cout << output << std::endl)
#define OUTPUT_ERROR(output) (std::cerr << output << std::endl)
#define PAUSE_CONSOLE std::cin.get()
#else
#define OUTPUT(output)
#define OUTPUT_ERROR(output)
#define PAUSE_CONSOLE
#endif