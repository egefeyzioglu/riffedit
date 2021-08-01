#pragma once

#include <vector>
#include <iostream>

#define SUBCHUNK_HEADER_SIZE 12

class RIFFSubChunk{
	public:
		RIFFSubChunk();
		RIFFSubChunk(const RIFFSubChunk & o);
		~RIFFSubChunk();
		//RIFFSubChunk(RIFFSubChunk&& o);
		//RIFFSubChunk(u_int8_t & buf);
		//RIFFSubChunk(u_int8_t * buf, size_t len);
		//RIFFSubChunk(std::vector<u_int8_t> buf);

		void getByteStream(std::vector<u_int8_t> & buf);

		// Sets the SubChunk ID. Returns true if successful, false if not.
		// Format spec requires an id string 4B long. Nothing is changed on faliure.
		bool setId(std::string id);
		std::string getId();

		// Sets the data byte vector. Returns true if successful, false if not.
		// Format spec requires overall length less than 2^32 + 8 B. Nothing is changed on faliure.
		bool setData(std::vector<u_int8_t> data);
		std::vector<u_int8_t> getData();

		//virtual bool operator==(RIFFSubChunk lhs) const;
		// Out stream operator overload, can't be virtual. Should call print(os, lhs).
		friend std::ostream& operator<<(std::ostream & os, const RIFFSubChunk & lhs);
	protected:
		// Actual operator<<
		virtual void print(std::ostream & os) const;

		std::string id;
		std::vector<u_int8_t> data;
};
