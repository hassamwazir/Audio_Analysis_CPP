// This is the header file for the LoadFile class.
// The LoadFile class is used to load an audio file and return the audio data.
// The audio data is stored in a vector of floats.
//

#ifndef LOADFILE_H
#define LOADFILE_H

#include <string>
#include <vector>

std::vector<float> loadFile(const std::string& filename);

#endif // LOADFILE_H