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
#include "RIFFDataSubChunk.h"


std::ostream & operator<<(std::ostream& os, const RIFFDataSubChunk & lhs){
	lhs.print(os);
	return os;
}


void RIFFDataSubChunk::print(std::ostream & os) const {
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
