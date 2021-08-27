#pragma once

#include <vector>
#include "RIFFChunk.h"

#define RIFF_HEADER_SIZE 12

class RIFFFile{
    public:
        RIFFFile();
        RIFFFile(std::string path);
        RIFFFile(std::vector<RIFFChunk> chunks);
        RIFFFile(std::vector<RIFFChunk> chunks, bool is_rifx);
        ~RIFFFile();

        const std::vector<RIFFChunk> & getChunks() const;
        bool isRIFX() const;
    private:

        std::vector<RIFFChunk> chunks;
        bool is_rifx; //Changes the endianness. Little endian if false

};