#pragma once

#include <map>
#include <future>
#include "Mesh.h"
#include "Texture.h"
#include "Tools.h"

namespace ENG
{
	class Resources
	{
	public:
		void loadMeshes(const std::vector<std::string>& files);
		void loadTextures(const std::vector<std::string>& files);
		Mesh& mesh(const std::string& file);
		Texture& texture(const std::string& file);

	private:
		std::map<std::string, Mesh> meshes;
		std::map<std::string, Texture> textures;
	};
}