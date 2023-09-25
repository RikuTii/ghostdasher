#pragma once
#include "global-includes.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	void Loadresources();


	std::optional<sf::Texture*> GetTexture(const std::string &key)
	{
		auto it = m_textures.find(key);
		if (it->second.get())
		{
			return it->second.get();
		}

		return nullptr;
	}

	sf::Font& GetPrimaryFont()
	{
		return *m_fonts.at("Tahoma").get();
	}
private:
	std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
	std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;

};

extern std::unique_ptr<ResourceManager> resourceManager;