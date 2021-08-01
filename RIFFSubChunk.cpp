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
	char tmp_path[] = "/tmp/riffedit.XXXXXX"; //Path to the tmp file
	int tmp_fd = mkstemp(tmp_path);
	char tmp_fd_path[4096]; //Path to the tmp file descriptor in the procfs
	sprintf(tmp_fd_path, "/proc/%d/fd/%d", getpid(), tmp_fd);
	//	Write to file
	write(tmp_fd, data.data() + SUBCHUNK_HEADER_SIZE, data.size() - SUBCHUNK_HEADER_SIZE);
	// Call hexdump -C on that file and capture its output. Send the captured output to os
	std::cout << std::flush;
	std::array<char, 128> buffer;
	std::string result;
	char cmd[300];
	sprintf(cmd, "/usr/bin/hexdump %s -C -s %d -n %u", tmp_fd_path, offset, (u_int32_t) data.size());
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	os << result;
	unlink(tmp_path);
	close(tmp_fd);
}

void RIFFSubChunk::getByteStream(std::vector<u_int8_t> & buf){
	//Truncate buffer vector and allocate with the new length
	buf.resize(0);
	buf.resize(data.size() + SUBCHUNK_HEADER_SIZE);
	//Copy in the new data
	std::copy(data.begin(), data.end(), buf.begin() + SUBCHUNK_HEADER_SIZE);
	//Copy the header
	for(int i = 0; i < 4; i++) buf[i] = id.c_str()[i];
	*((u_int32_t *)(buf.data()) + 1) = (u_int32_t) data.size();
}

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
	//Check if size fits into a 32 bit unsigned int (+8 because the first 8 bits aren't included)
	if(data.size() > 4294967304) return false;
	this->data = data;
	return true;
}
