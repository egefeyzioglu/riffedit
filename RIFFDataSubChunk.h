#pragma once
#include "RIFFSubChunk.h"


class RIFFDataSubChunk: public RIFFSubChunk{
	public:
		using RIFFSubChunk::RIFFSubChunk;

		//virtual bool operator==(RIFFSubChunk lhs) const;
		// Out stream operator overload, can't be virtual. Should call print(os, lhs).
		friend std::ostream& operator<<(std::ostream & os, const RIFFDataSubChunk & lhs);
	private:
		// Actual operator<<
		void print(std::ostream & os) const;
};
