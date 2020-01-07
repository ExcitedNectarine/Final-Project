#include "Audio.h"

#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.c>

namespace ENG
{
	void audioInit()
	{
		ALCdevice* device = alcOpenDevice(NULL);
		if (!device) throw std::exception();

		ALCcontext* context = alcCreateContext(device, NULL);
		if (!context)
		{
			alcCloseDevice(device);
			throw std::exception();
		}

		if (!alcMakeContextCurrent(context))
		{
			alcDestroyContext(context);
			alcCloseDevice(device);
			throw std::exception();
		}
	}

	SoundBuffer::SoundBuffer(const std::string& filename)
	{
		int channels;
		int sample_rate;
		short* data;
		std::size_t samples = stb_vorbis_decode_filename(filename.c_str(), &channels, &sample_rate, &data);

		ALenum format;
		ALsizei frequency = sample_rate;
		std::vector<char> buffer;
		buffer.resize(sizeof(*data) * channels * samples);
		memcpy(&buffer.at(0), data, buffer.size());

		if (channels == 1) format = AL_FORMAT_MONO16;
		else format = AL_FORMAT_STEREO16;

		alGenBuffers(1, &id);
		alBufferData(id, format, &buffer.at(0), static_cast<ALsizei>(buffer.size()), frequency);

		free(data);
	}

	SoundSource::SoundSource(SoundBuffer* buffer) : buffer(buffer)
	{
		alGenSources(1, &id);
	}

	void SoundSource::play()
	{
		alSource3f(id, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSourcei(id, AL_BUFFER, buffer->getID());
		alSourcePlay(id);
	}
}