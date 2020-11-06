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
		stbi_set_flip_vertically_on_load(0);
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
			textures.at(f.first).createFromImage(image);
			image.cleanup();
		}
	}

	void Resources::loadSounds(const std::vector<std::string>& files)
	{
		std::map<std::string, std::future<Audio>> futures;

		// Same as the loop for loading meshes. Asynchronously loads sounds.
		for (const std::string& sound : files)
		{
			OUTPUT("Loading sound '" << sound << "'");
			std::string file = splitText(sound, '/').back();
			futures[file] = std::async(std::launch::async, loadAudio, sound);
		}

		for (auto& f : futures)
		{
			sounds.insert({ f.first, SoundBuffer() });

			Audio audio = f.second.get();
			sounds.at(f.first).createFromAudio(audio);
			audio.cleanup();
		}
	}

	void Resources::loadShaders(const std::vector<std::string>& files)
	{
		for (const std::string& shader : files)
		{
			OUTPUT("Loading and compiling shader '" << shader << "'");

			std::string file = splitText(shader, '/').back();
			std::string shader_text = ENG::readTextFile(shader);
			std::string vertex;
			std::string fragment;
			std::string line;
			std::string* write_to = &vertex;

			std::stringstream ss(shader_text);
			while (std::getline(ss, line))
			{
				if (line == "[VERTEX]") // read vertex code
				{
					write_to = &vertex;
					continue;
				}
				else if (line == "[FRAGMENT]") // read frag code
				{
					write_to = &fragment;
					continue;
				}

				*write_to += line + '\n';
			}

			shaders.insert({ file, Shader(vertex, fragment) });
		}
	}

	Mesh& Resources::mesh(const std::string& file) { return meshes.at(file); }
	Texture& Resources::texture(const std::string& file) { return textures.at(file); }
	SoundBuffer& Resources::sound(const std::string& file) { return sounds.at(file); }
	Shader& Resources::shader(const std::string& file) { return shaders.at(file); }
}