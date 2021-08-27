#include <iostream>
#include <cstring>
#include "RIFFChunk.h"

RIFFChunk::RIFFChunk(u_int8_t *buf, RIFFChunk *parent){
    this->parent = parent;

    //Parse RIFF Chunk
    //ID
    u_int8_t *id_ptr = buf;
    u_int8_t id_s[5];
    strncpy((char*) id_s, (char*) id_ptr, 4);
    id_s[4] = '\0';
    id = (char*) id_s;
    buf += 4;
    //Size
    size = *(u_int32_t*) (buf);
    buf += 4;

}

void RIFFChunk::getByteStream(u_int8_t *&buf){
    throw "Not implemented";
}

std::ostream& operator<<(std::ostream & os, const RIFFChunk & lhs){
    lhs.print(os);
    return os;
}

void RIFFChunk::print(std::ostream & os) const{
    os << "Unknown Chunk ID " << id << std::endl;
}