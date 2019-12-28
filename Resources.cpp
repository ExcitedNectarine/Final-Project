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

		// A threaded for loop, each mesh is loaded asynchronously, reducing loading times.
		for (const std::string& mesh : files)
		{
			OUTPUT("Loading mesh '" << mesh << "'");
			std::string file = splitText(mesh, '/').back();
			futures[file] = std::async(std::launch::async, [mesh]() { return loadOBJ(mesh); });
		}

		// Must retrieve the loaded mesh afterwards.
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

		// Same as the loop for loading meshes. Asynchronously loads textures.
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