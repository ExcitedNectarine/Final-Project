#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include "Vertex.h"

namespace ENG
{
	std::vector<std::string> splitText(const std::string& text, const char delimeter);
	std::string readTextFile(const std::string& filename);

	/**
	* Simple .obj parser. Takes in an .obj file, and returns a vector of vertices.
	*/
	std::vector<Vertex> loadOBJ(const std::string& model);

	// simple random float between two values.
	float randomFloat(const float from, const float to);
	int randomInt(const int from, const int to);
}