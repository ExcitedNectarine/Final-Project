#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "Vertex.h"

namespace ENG
{
	std::vector<std::string> splitText(const std::string& text, const char delimeter);
	std::string readTextFile(const std::string& filename);
	std::vector<Vertex> loadOBJ(const std::string& model);
}