#include "../include/LoadFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>  // For std::reverse
#include <chrono>     // For timing

// WAV file header format
struct WavHeader {
    char chunkID[4];        // "RIFF"
    uint32_t chunkSize;      // File size - 8 bytes
    char format[4];          // "WAVE"
    char subchunk1ID[4];     // "fmt "
    uint32_t subchunk1Size;  // Size of the fmt chunk (16 for PCM)
    uint16_t audioFormat;    // Audio format (1 for PCM)
    uint16_t numChannels;    // Number of channels
    uint32_t sampleRate;     // Sample rate
    uint32_t byteRate;       // byteRate = sampleRate * numChannels * bitsPerSample / 8
    uint16_t blockAlign;     // blockAlign = numChannels * bitsPerSample / 8
    uint16_t bitsPerSample;  // Bits per sample (8, 16, etc.)
    char subchunk2ID[4];     // "data"
    uint32_t subchunk2Size;  // Number of bytes in the data section
};

// Check if system is little-endian
bool isLittleEndian() {
    uint16_t test = 1;
    return *reinterpret_cast<char*>(&test) == 1;
}

// Convert little-endian to host-endian if necessary
template <typename T>
T littleEndianToHost(T value) {
    if (!isLittleEndian()) {
        char* start = reinterpret_cast<char*>(&value);
        std::reverse(start, start + sizeof(T));
    }
    return value;
}

// Function to load WAV file and return audio data as vector of floats
std::vector<float> loadWavFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    WavHeader header;

    // Read the WAV header
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    // Convert header fields from little-endian to host-endian (if necessary)
    header.chunkSize = littleEndianToHost(header.chunkSize);
    header.subchunk1Size = littleEndianToHost(header.subchunk1Size);
    header.audioFormat = littleEndianToHost(header.audioFormat);
    header.numChannels = littleEndianToHost(header.numChannels);
    header.sampleRate = littleEndianToHost(header.sampleRate);
    header.byteRate = littleEndianToHost(header.byteRate);
    header.blockAlign = littleEndianToHost(header.blockAlign);
    header.bitsPerSample = littleEndianToHost(header.bitsPerSample);
    header.subchunk2Size = littleEndianToHost(header.subchunk2Size);

    // Check if it's a valid WAV file
    if (std::string(header.chunkID, 4) != "RIFF" || std::string(header.format, 4) != "WAVE") {
        throw std::runtime_error("Invalid WAV file format: " + filename);
    }

    // Check if it's PCM (uncompressed) format
    if (header.audioFormat != 1) {
        throw std::runtime_error("Unsupported WAV format (not PCM): " + filename);
    }

    // Display the header information (optional, for debugging purposes)
    std::cout << "Audio Format: " << header.audioFormat << std::endl;
    std::cout << "Channels: " << header.numChannels << std::endl;
    std::cout << "Sample Rate: " << header.sampleRate << std::endl;
    std::cout << "Bits per Sample: " << header.bitsPerSample << std::endl;
    std::cout << "Data Size: " << header.subchunk2Size << " bytes" << std::endl;

    // Move to the start of the audio data
    std::vector<float> audioData;
    int bytesPerSample = header.bitsPerSample / 8;
    int numSamples = header.subchunk2Size / bytesPerSample;

    // Read the audio data
    if (header.bitsPerSample == 16) {
        std::vector<int16_t> rawAudioData(numSamples);
        file.read(reinterpret_cast<char*>(rawAudioData.data()), header.subchunk2Size);

        // Convert to host endian if necessary
        if (!isLittleEndian()) {
            for (auto& sample : rawAudioData) {
                sample = littleEndianToHost(sample);
            }
        }

        // Convert to float (normalize to range [-1.0, 1.0])
        audioData.resize(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            audioData[i] = rawAudioData[i] / 32768.0f;
        }
    } else if (header.bitsPerSample == 8) {
        std::vector<uint8_t> rawAudioData(numSamples);
        file.read(reinterpret_cast<char*>(rawAudioData.data()), header.subchunk2Size);

        // Convert to float (normalize to range [-1.0, 1.0])
        audioData.resize(numSamples);
        for (int i = 0; i < numSamples; ++i) {
            audioData[i] = (rawAudioData[i] - 128) / 128.0f;
        }
    } else {
        throw std::runtime_error("Unsupported bit depth: " + std::to_string(header.bitsPerSample));
    }

    return audioData;
}

// Function to save the audio data to a CSV file
void saveToCSV(const std::vector<float>& audioData, const std::string& outputFilename) {
    std::ofstream outFile(outputFilename);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file: " + outputFilename);
    }

    for (const auto& sample : audioData) {
        outFile << sample << std::endl;
    }

    outFile.close();
}

int main() {
    try {
        // Start measuring time
        auto start = std::chrono::high_resolution_clock::now();

        std::string filename = "../audio_files/input.wav";
        std::vector<float> audioData = loadWavFile(filename);

        std::cout << "Loaded " << audioData.size() << " samples from " << filename << std::endl;
        
        // Save audio data to a CSV file
        std::string outputFilename = "../audio_files/input.wav/audio_analysis/audio_files/output.csv"; // Output CSV file path
        saveToCSV(audioData, outputFilename);

        std::cout << "Audio data saved to " << outputFilename << std::endl;

        // End measuring time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate duration in milliseconds
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
