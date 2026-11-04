#include <chrono>
#include <filesystem>  // For std::filesystem
#include <iostream>
#include <vector>

#include "basic_transform.h"
#include "texture_atlas.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main() {
    // OpenGL settings we request for:
    sf::ContextSettings settings;
    settings.depthBits = 24;   // 24-bit depth buffer
    settings.stencilBits = 8;  // 8-bit stencil buffer
    // Request OpenGL 2.1 for compatibility with both SFML/Graphics AND macOS:
    //  - SFML's Graphics module implementation relies on legacy fixed-function pipeline features
    //     of OpenGL, which means only up to OpenGL 2.1. Any newer version requires Compatibility
    //     profile (i.e., maintaining legacy features).
    //  - macOS has deprecated OpenGL support, meaning for anything newer than 2.1 (i.e., 3.0 and
    //      newer), only Core profile is supported, i.e., REMOVING legacy fixed-function features.
    // As a result, if you want to both support macOS AND use SFML's Graphics features (e.g.,
    // sf::RenderWindow), you can use at latest OpenGL 2.1, which is a nightmare.
    settings.majorVersion = 2;
    settings.minorVersion = 1;
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Shaders", sf::State::Windowed, settings);
    (void)window.setActive();  // set window as OpenGL target

    // Query the settings we actually received (may be different):
    const auto& actualSettings = window.getSettings();
    std::cout << "OpenGL version " << actualSettings.majorVersion << "."
              << actualSettings.minorVersion << " flags: " << actualSettings.attributeFlags << '\n';
    std::cout << "Maximum texture width (or height): " << sf::Texture::getMaximumSize() << '\n';

    TextureAtlas atlas({2048, 2048}, 2);
    std::vector<std::filesystem::path> paths;
    for (const auto& f : std::filesystem::directory_iterator(std::filesystem::path{"assets/"})) {
        if (std::filesystem::is_regular_file(f) && f.path().extension().string() == ".png") {
            paths.push_back(f.path());
        }
    }
    std::sort(paths.begin(), paths.end());  // lexicographic order
    for (const auto& path : paths) {
        atlas.registerTexture(path);
    }
    sf::Image atlasImg = atlas.getTexture().copyToImage();
    if (!atlasImg.saveToFile("atlas.png")) {
        std::cerr << "Failed to save atlas to file\n";
    } else {
        std::cout << "Successfully saved atlas.png file (inside build folder)!\n";
    }
}