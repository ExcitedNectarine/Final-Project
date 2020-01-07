#pragma once

#include <string>
#include <vector>
#include "al.h" 
#include "alc.h"

namespace ENG
{
	void audioInit();

	class SoundBuffer
	{
	public:
		SoundBuffer(const std::string& filename);
		ALuint getID() { return id; }

	private:
		ALuint id;
	};

	class SoundSource
	{
	public:
		SoundSource() {}
		SoundSource(SoundBuffer* buffer);
		void play();

	private:
		ALuint id;
		SoundBuffer* buffer;
	};
}