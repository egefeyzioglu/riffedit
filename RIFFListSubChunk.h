#pragma once

#include <vector>
#include <iostream>

#include "RIFFSubChunk"



class RIFFListSubChunk: public RIFFSubChunk{
	public:
		using RIFFSubChunk::RIFFSubChunk;
		//virtual bool operator==(RIFFSubChunk lhs) const;
		// Out stream operator overload, can't be virtual. Should call print(os, lhs).
		friend std::ostream& operator<<(std::ostream & os, const RIFFSubChunk & lhs);
	protected:
		// Actual operator<<
		virtual void print(std::ostream & os) const;
};
