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

	void Audio::cleanup() { free(data); }
	Audio loadAudio(const std::string& filename)
	{
		Audio audio;
		audio.samples = stb_vorbis_decode_filename(filename.c_str(), &audio.channels, &audio.sample_rate, &audio.data);
		return audio;
	}

	void SoundBuffer::createFromAudio(const Audio& audio)
	{
		ALenum format;
		ALsizei frequency = audio.sample_rate;
		std::vector<char> buffer;
		buffer.resize(sizeof(*audio.data) * audio.channels * audio.samples);
		memcpy(&buffer.at(0), audio.data, buffer.size());

		if (audio.channels == 1) format = AL_FORMAT_MONO16;
		else format = AL_FORMAT_STEREO16;

		alGenBuffers(1, &id);
		alBufferData(id, format, &buffer.at(0), static_cast<ALsizei>(buffer.size()), frequency);
	}

	SoundSource::SoundSource() { alGenSources(1, &id); }
	void SoundSource::setBuffer(SoundBuffer* buffer) { this->buffer = buffer; }
	void SoundSource::play()
	{
		alSource3f(id, AL_POSITION, 0.0f, 0.0f, 0.0f);
		alSourcei(id, AL_BUFFER, buffer->getID());
		alSourcePlay(id);
	}
}