#include <audio/SoundManager.hpp>
#include <audio/MADStream.hpp>

#include <iostream>

void checkALerror(const std::string& file, unsigned int line);

#if RW_DEBUG
	#define alCheck(stmt) do { stmt; checkALerror(__FILE__, __LINE__); } while(0)
#else
	#define alCheck(stmt) stmt
#endif

void SoundManager::SoundSource::loadFromFile(const std::string& filename)
{
	fileInfo.format = 0;
	file = sf_open(filename.c_str(), SFM_READ, &fileInfo);

	if (file) {
		size_t numRead = 0;
		std::array<int16_t, 4096> readBuffer;

		while ((numRead = sf_read_short(file, readBuffer.data(), readBuffer.size())) != 0) {
			data.insert(data.end(), readBuffer.begin(), readBuffer.begin() + numRead);
		}
	} else {
		std::cerr << "Error opening sound file \"" << filename << "\": " << sf_strerror(file) << std::endl;
	}
}

SoundManager::SoundManager()
: backgroundNoise(nullptr)
{

}

bool SoundManager::playBackground(const std::string& fileName)
{
	if( backgroundNoise )
	{
		delete backgroundNoise;
	}
	
	sf::Music* bg = new sf::Music;
	
	if( bg->openFromFile( fileName ) )
	{
		backgroundNoise = bg;
		backgroundNoise->setLoop(true);
		bg->play();
		return true;
	}
	
	delete bg;
	return false;
}

void SoundManager::pause(bool p)
{
	if( backgroundNoise )
	{
		if( p )
		{
			backgroundNoise->pause();
		}
		else
		{
			backgroundNoise->play();
		}
	}
}

void checkALerror(const std::string& file, unsigned int line)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		std::cerr << "OpenAL error at " << file << ":" << line << ": ";

		switch (err) {
		case AL_INVALID_NAME:
			std::cerr << "Invalid name!";
			break;
		case AL_INVALID_VALUE:
			std::cerr << "Invalid value!";
			break;
		case AL_INVALID_OPERATION:
			std::cerr << "Invalid operation!";
			break;
		default:
			std::cerr << err;
		}

		std::cerr << std::endl;
	}
}
