#include "JspConfigFile.h"
#include "JspLog.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Jsp
{
	ConfigFile::ConfigFile()
	{
		m_values.clear();
	}

	ConfigFile::~ConfigFile()
	{
	}

	bool ConfigFile::load(const char *configFile)
	{
		if (!configFile) return false;

		m_values.clear();

		std::ifstream ifs(configFile);
		if (ifs.fail())
		{
			Jsp::printLog(LOG_FATAL, "file not found : (%s)\n", configFile);
			return false;
		}

		std::string line;

		getline(ifs, line);
		while (true)
		{
			if (line.size() > 0)
			{
				line = ConfigFile::stripNLAtLineEnd(line);

//				printf("(%s)\n", line.c_str());

				std::string s = line.substr(0, 1);
				if (strcmp(s.c_str(), "#") != 0)	// skip comment
				{
					int i = line.find("=");
					if (i>=0)
					{
						std::string name = ConfigFile::trim(line.substr(0, i));
						std::string value = ConfigFile::trim(line.substr(i+1));
						if (name.length() > 0)
						{
//							printf("name=(%s), value=(%s)\n", name.c_str(), value.c_str());
							m_values[name] = value;
						}
					}
				}
			}

			// reached to end of file. exit loop
			if (ifs.eof()) break;

			// read next line
			getline(ifs, line);
		}

		ifs.close();

		return true;
	}

	std::string ConfigFile::trim(std::string str)
	{
		int i, j;

		i = str.find_first_not_of(" ");
		if (i==-1) return "";	// all chars are space

		if (i>0) str = str.substr(i);	// cut top spaces

		j = str.find_last_not_of(" ");
		if (j>0) str = str.substr(0, j+1);	// cut bottom spaces

		return str;
	}

	std::string ConfigFile::stripNLAtLineEnd(std::string str)
	{
		int n = str.size();
		if (n>0)
		{
			const char *p = str.c_str();
			if (p[n-1] == '\n' || (p[n-1] == '\r'))
			{
				return str.substr(0, n-1);
			}
		}

		return str;
	}

	const char *ConfigFile::getStringValue(const char *name)
	{
		if (!name) return NULL;

		const char *value = m_values[name].c_str();

		return (value && *value) ? value : NULL;
	}
};
