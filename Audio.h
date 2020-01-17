#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "al.h" 
#include "alc.h"

namespace ENG
{
	void audioInit();

	struct Audio
	{
		void cleanup();

		int channels;
		int sample_rate;
		short* data;
		std::size_t samples;
	};

	Audio loadAudio(const std::string& filename);

	class SoundBuffer
	{
	public:
		void createFromAudio(const Audio& audio);
		ALuint getID() { return id; }

	private:
		ALuint id;
	};

	class SoundSource
	{
	public:
		SoundSource();
		void setBuffer(SoundBuffer* buffer);
		void play();

	private:
		ALuint id;
		SoundBuffer* buffer;
	};
}