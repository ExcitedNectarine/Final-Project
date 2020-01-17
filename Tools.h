#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "Vertex.h"

namespace ENG
{
	std::vector<std::string> splitText(const std::string& text, const char delimeter);
	std::string readTextFile(const std::string& filename);

	/**
	* Simple .obj parser. Takes in an .obj file, and returns a vector of vertices.
	*/
	std::vector<Vertex> loadOBJ(const std::string& model);
}