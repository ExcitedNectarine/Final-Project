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

		std::string contents;
		std::string line;

		while (!file.eof())
		{
			std::getline(file, line);
			contents.append(line + '\n');
		}

		return contents;
	}

	std::vector<Vertex> loadOBJ(const std::string& model)
	{
		std::stringstream stream(model);
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
				positions.push_back(glm::vec3(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str()),
					atof(split.at(3).c_str()))
				);
			}
			else if (split.at(0) == "vt") // Tex coords
			{
				uvs.push_back(glm::vec2(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str()))
				);
			}
			else if (split.at(0) == "vn") // Normals
			{
				normals.push_back(glm::vec3(
					atof(split.at(1).c_str()),
					atof(split.at(2).c_str()),
					atof(split.at(3).c_str()))
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

					vertices.push_back({ position, uv, normal });
				}
			}
		}

		return vertices;
	}
}