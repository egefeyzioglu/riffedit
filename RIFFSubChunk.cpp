#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <cstring>
#include <array>
#include "RIFFSubChunk.h"
#include "util.h"

RIFFSubChunk::RIFFSubChunk(){} //Nothing to do
RIFFSubChunk::RIFFSubChunk(const RIFFSubChunk & o){
	id = o.id;
	data = o.data;
}
RIFFSubChunk::~RIFFSubChunk(){} //Nothing to do

std::ostream & operator<<(std::ostream& os, const RIFFSubChunk & lhs){
	lhs.print(os);
	return os;
}


void RIFFSubChunk::print(std::ostream & os) const {
	os << "Unknown SubChunk id " << id << std::endl;
	os << "Hexdump of the data contained:" << std::endl;
	// Write subchunk contents to a file in /tmp
	//	Create the file in /tmp and get a handle to it
	int offset = 0; // Maybe I'll make it start at the correct offset at some point
	char tmp_template[] = "/tmp/riffedit.XXXXXX";
	int tmp_fd = mkstemp(tmp_template);
	char tmp_fd_path[4096];
	sprintf(tmp_fd_path, "/proc/%d/fd/%d", getpid(), tmp_fd);
	//	Write to file
	write(tmp_fd, "test", 4);
	// Call hexdump -C on that file and capture its output. Send the captured output to os
	std::cout << std::flush;
	std::array<char, 128> buffer;
	std::string result;
	char cmd[300];
	sprintf(cmd, "/usr/bin/hexdump %s -C -s %d -n %d", tmp_fd_path, offset, 4);
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	os << result;
}

void RIFFSubChunk::getByteStream(u_int8_t & buf){}
std::vector<u_int8_t> RIFFSubChunk::getByteStream(){return data;}

// Sets the SubChunk ID. Returns true if successful, false if not.
// Format spec requires an id string 4B long. Nothing is changed on faliure.
bool RIFFSubChunk::setId(std::string id){
	if(id.size() != 4) return false;
	this->id = id;
	return true;
}
std::string RIFFSubChunk::getId(){return id;}

// Sets the data byte vector. Returns true if successful, false if not.
// Format spec requires overall length less than 2^32 + 8 B. Nothing is changed on faliure.
bool RIFFSubChunk::setData(std::vector<u_int8_t> data){
	if(data.size() > 4294967304) return false;
	this->data = data;
	return true;
}