#include "Tools.h"

namespace ENG
{
	std::vector<std::string> splitText(const std::string& text, const char delimeter)
	{
		std::vector<std::string> split;
		std::stringstream ss(text);
		std::string item;

		while (std::getline(ss, item, delimeter))
			split.push_back(item);

		return split;
	}

	std::string readTextFile(const std::string& filename)
	{
		std::ifstream file(filename.c_str());

		if (!file.is_open())
			throw std::exception(std::string("ENG::readTextFile failed to open file '" + filename + "'.").c_str());

		std::string contents;
		std::string line;

		while (!file.eof())
		{
			std::getline(file, line);
			contents.append(line + '\n');
		}

		return contents;
	}

	std::vector<Vertex> loadOBJ(const std::string& filename)
	{
		std::stringstream stream(readTextFile(filename));
		std::string line;

		// Used to store vertices for indexing.
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		std::vector<Vertex> vertices;

		while (std::getline(stream, line))
		{
			if (line.size() == 0) break;

			std::vector<std::string> split = splitText(line, ' ');
			if (split.at(0) == "v") // Vertex pos
			{
				positions.emplace_back(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str()),
					atof(split.at(3).c_str())
				);
			}
			else if (split.at(0) == "vt") // Tex coords
			{
				uvs.emplace_back(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str())
				);
			}
			else if (split.at(0) == "vn") // Normals
			{
				normals.emplace_back(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str()),
					atof(split.at(3).c_str())
				);
			}
			else if (split.at(0) == "f") // Faces
			{
				split.erase(split.begin());
				for (std::string& face : split)
				{
					std::vector<std::string> indices = splitText(face, '/');

					glm::vec3 position = positions.at(atoi(indices.at(0).c_str()) - 1);
					glm::vec2 uv = uvs.at(atoi(indices.at(1).c_str()) - 1);
					glm::vec3 normal = normals.at(atoi(indices.at(2).c_str()) - 1);

					vertices.emplace_back(position, uv, normal);
				}
			}
		}

		return vertices;
	}

	float randomFloat(const float from, const float to)
	{
		static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		std::uniform_real_distribution<float> dist(from, to);
		return dist(engine);
	}

	int randomInt(const int from, const int to)
	{
		static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
		std::uniform_int_distribution<int> dist(from, to);
		return dist(engine);
	}
}