#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <SFML/Graphics.hpp>
#include <string>

// Downloads an image from a URL and loads it into an sf::Texture
// Returns true on success, false on failure
bool loadTextureFromURL(sf::Texture& texture, const std::string& url);

#endif
