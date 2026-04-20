#include "SpriteLoader.h"
#include <curl/curl.h>
#include <vector>
#include <iostream>

// Callback function for libcurl to write downloaded data into a vector
static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::vector<unsigned char>* buffer = static_cast<std::vector<unsigned char>*>(userp);
    unsigned char* data = static_cast<unsigned char*>(contents);
    buffer->insert(buffer->end(), data, data + totalSize);
    return totalSize;
}

bool loadTextureFromURL(sf::Texture& texture, const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL\n";
        return false;
    }

    std::vector<unsigned char> imageData;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &imageData);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // Follow redirects
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);         // 15 second timeout
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "PokemonGame/1.0");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "CURL error downloading " << url << ": " << curl_easy_strerror(res) << "\n";
        return false;
    }

    if (imageData.empty()) {
        std::cerr << "Downloaded empty data from " << url << "\n";
        return false;
    }

    sf::Image image;
    if (!image.loadFromMemory(imageData.data(), imageData.size())) {
        std::cerr << "Failed to load image from downloaded data: " << url << "\n";
        return false;
    }

    if (!texture.loadFromImage(image)) {
        std::cerr << "Failed to create texture from image: " << url << "\n";
        return false;
    }

    std::cout << "Loaded sprite from: " << url << "\n";
    return true;
}
