#pragma once

#include <string>
#include <map>
#include "Tools.h"
#include "Output.h"

namespace ENG
{
	class Settings
	{
	public:
		Settings();
		Settings(const std::string& filename);
		void load(const std::string& filename);
		void write(const std::string& filename);
		std::string get(const std::string& key);
		int geti(const std::string& key);
		float getf(const std::string& key);
		void set(const std::string& key, const std::string& value);

	private:
		std::map<std::string, std::string> settings;
	};
}