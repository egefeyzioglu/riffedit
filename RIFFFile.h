#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "RIFFSubChunk.h"


#define RIFF_HEADER_SIZE 12

class RIFFFile{
	public:
		RIFFFile();
		RIFFFile(u_int8_t & buf);
		RIFFFile(u_int8_t * buf, size_t len);
		RIFFFile(std::vector<u_int8_t> buf);
		
		void getByteStream(u_int8_t & buf);
		std::vector<u_int8_t> getByteStream();
		std::vector<RIFFSubChunk> subchunks;
		
		friend std::ostream& operator<<(std::ostream & os, const RIFFFile & lhs);
	private:
		// Parses the subchunk pointed by baseptr, returns the subchunk object and sets size to how many bytes the pointer should be advanced by to reach the beginning of the next subchunk
		RIFFSubChunk parseSubchunk(uint8_t *baseptr, u_int32_t *size);
};
