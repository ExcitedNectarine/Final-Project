#include "Resources.h"

namespace ENG
{
	Resources::~Resources() { cleanup(); }

	void Resources::cleanup()
	{
		for (auto& p : meshes)
			p.second.cleanup();
		for (auto& p : textures)
			p.second.cleanup();
	}

	void Resources::loadMeshes(const std::vector<std::string>& files)
	{
		std::map<std::string, std::future<std::vector<Vertex>>> futures;
		for (const std::string& mesh : files) // TODO: Run each iteration as a seperate task using std::async
		{
			OUTPUT("Loading mesh '" << mesh << "'");
			std::string file = splitText(mesh, '/').back();
			futures[file] = std::async(std::launch::async, [mesh]() { return loadOBJ(readTextFile(mesh)); });
		}

		for (auto& f : futures)
		{
			meshes.insert({ f.first, Mesh() });
			meshes.at(f.first).setVertices(f.second.get());
		}
	}

	void Resources::loadTextures(const std::vector<std::string>& files)
	{
		stbi_set_flip_vertically_on_load(1);
		std::map<std::string, std::future<Image>> futures;
		for (const std::string& texture : files)
		{
			OUTPUT("Loading texture '" << texture << "'");

			std::string file = splitText(texture, '/').back();
			futures[file] = std::async(std::launch::async, loadImage, texture);
		}

		for (auto& f : futures)
		{
			textures.insert({ f.first, Texture() });

			Image image = f.second.get();
			textures.at(f.first).load(image);
			image.cleanup();
		}
	}

	Mesh& Resources::mesh(const std::string& file) { return meshes.at(file); }
	Texture& Resources::texture(const std::string& file) { return textures.at(file); }
}