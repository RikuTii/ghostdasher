#pragma once
#include "global-includes.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	void Loadresources();

	sf::Font& GetPrimaryFont()
	{
		return *m_fonts.at("Tahoma").get();
	}
private:
	std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
};

extern std::unique_ptr<ResourceManager> m_resourceManager;