#include <string>
#include <fstream>
#include "util.h"

std::string rand_num_string(int len){
	len = len > 64 ? 64 : len;
        std::ifstream in("/dev/urandom");
        u_int8_t buf[64];
        in.read((char*) buf, 64);
	*buf = ((*((u_int64_t *) buf)) >> (64 - len));
        std::string a = std::to_string(*(u_int64_t *) buf);
        return a;	
}
