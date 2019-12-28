#include "Settings.h"

namespace ENG
{
	Settings::Settings() {}
	Settings::Settings(const std::string& filename) { load(filename); }
	
	void Settings::load(const std::string& filename)
	{
		OUTPUT("Loading settings file '" + filename + "'.");

		std::stringstream stream(readTextFile(filename));
		std::string line;

		while (std::getline(stream, line))
		{
			if (line.size() == 0) break;

			std::vector<std::string> split = splitText(line, '=');
			settings.insert({ split[0], split[1] });
		}
	}
	
	void Settings::write(const std::string& filename)
	{
		OUTPUT("Writing settings file '" + filename + "'.");

		std::ofstream file;
		file.open(filename);

		if (!file.is_open())
			throw std::exception(std::string("ENG::Settings::write failed to open file '" + filename + "'.").c_str());

		file.clear();
		for (const auto& p : settings)
			file << p.first << "=" << p.second << std::endl;
		file.close();
	}
	
	std::string Settings::get(const std::string& key) { return settings.at(key); }
	int Settings::geti(const std::string& key) { return atoi(settings.at(key).c_str()); }
	void Settings::set(const std::string& key, const std::string& value) { settings[key] = value; }
}