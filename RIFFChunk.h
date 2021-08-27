#pragma once

#include <string>
#include <vector>
#include <iostream>

#define DEFAULT_PATH "/mnt/d/dtmf.wav";

class RIFFChunk{
	public:
		RIFFChunk(u_int8_t *buf, RIFFChunk *parent);

		RIFFChunk *parent;
		std::vector<RIFFChunk*> children;
		u_int32_t size;
		std::string id;
		virtual void getByteStream(u_int8_t *&buf);
		
		friend std::ostream& operator<<(std::ostream & os, const RIFFChunk & lhs);
	private:
		virtual void print(std::ostream & os) const;
};
