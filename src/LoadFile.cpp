// this file is used to load an audio file and return the audio data
// the audio data is stored in a vector of floats

#include "LoadFile.h"
#include <iostream>
#include <fstream>
#include <vector>

std::vector<float> loadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Determine file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read file data into vector
    std::vector<float> audioData(fileSize / sizeof(float));
    file.read(reinterpret_cast<char*>(audioData.data()), fileSize);

    return audioData;
}