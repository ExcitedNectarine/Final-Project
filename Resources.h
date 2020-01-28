#pragma once

#include <map>
#include <future>
#include "Mesh.h"
#include "Texture.h"
#include "Audio.h"
#include "Tools.h"
#include "Shader.h"
#include "Output.h"
#include "CubeMap.h"

namespace ENG
{
	/**
	* Stores all resources, and loads resources in using multithreading.
	*/
	class Resources
	{
	public:
		~Resources();
		void cleanup();
		
		void loadMeshes(const std::vector<std::string>& files);
		void loadTextures(const std::vector<std::string>& files);
		void loadSounds(const std::vector<std::string>& files);
		void loadShaders(const std::vector<std::string>& files);
		//void loadCubeMaps(const std::vector<std::vector<std::string>> files);

		Mesh& mesh(const std::string& file);
		Texture& texture(const std::string& file);
		SoundBuffer& sound(const std::string& file);
		Shader& shader(const std::string& file);

	private:
		std::map<std::string, Mesh> meshes;
		std::map<std::string, Texture> textures;
		std::map<std::string, SoundBuffer> sounds;
		std::map<std::string, Shader> shaders;
		//std::map<std::string, CubeMap> cubemaps;
	};
}