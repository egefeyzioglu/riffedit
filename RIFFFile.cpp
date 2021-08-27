#include "RIFFFile.h"
#include <sys/stat.h>
#include <fstream>
#include <stdexcept>


RIFFFile::RIFFFile(std::string path){
    if(path.empty()) path = DEFAULT_PATH;
	
	//TODO: Check if file exists
	std::ifstream fp(path, std::ios::in | std::ios::binary | std::ios::ate);
	if(!fp.is_open()){
		throw std::runtime_error("File can't be opened");
	}
	
	//Read header
	char *header = (char*) calloc(RIFF_HEADER_SIZE, 0);
	fp.seekg(0, std::ios::beg);
	fp.read(header, RIFF_HEADER_SIZE);	

	if(fp.gcount() != RIFF_HEADER_SIZE){
		throw std::invalid_argument("RIFF header size incorrect!");
	}
	
	//Check format
	if(std::string(header, 4) != "RIFF"){
		throw std::invalid_argument("Input file is not a RIFF file (magic number missing or incorrect.)");
	}
	if(std::string(header+8, 4) != "WAVE"){
		throw std::invalid_argument("Input file is not a WAVE file (file format is incorrect.)");
	}
	
	//Chunk size
	int size_left = -4; //Initialised to -4 since we will read 4 bytes less than the chunk size
	u_int32_t *chunk_size = (u_int32_t*) (header+4);
	size_left+= *chunk_size;
	std::cout << "Chunk size is " << *chunk_size << ". That means the file is " <<  *chunk_size + 8 <<  "B large, and we have " << size_left << "B left to read" << std::endl;
	
	//Read rest of the file
	char buf[size_left];
	fp.read((char*)buf, size_left);
	
	//Check if we could read the whole file
	if(fp.gcount() != size_left){
		std::string err = "Only read ";
		err += fp.gcount();
		err += " bytes when the RIFF header said we would read ";
		err += size_left;
		err += ". File corrupted or read was interrupted.";
		throw std::runtime_error(err);
	}
	
	char *ptr = buf;
	int i = 1;
	while(ptr - buf < size_left){
		std::string id(ptr, 4);
		
		u_int32_t *size_ = (u_int32_t*) (ptr + 4);
		u_int32_t size = *size_; //So that we aren't mutating the buffer
		size+= 8; //Read size doesn't include the first 8 bytes
		
		
		std::cout << std::endl;
		//handle_subchunk((u_int8_t*) ptr, size, id, ptr - buf + 12L, i); //Add 12 to the offset because of the RIFF header	
		std::cout << std::endl;
		
		//Advance the pointer
		ptr += size;
		i++;
	}
}

RIFFFile::RIFFFile(){
    is_rifx = false; //Default to RIFF
}

RIFFFile::RIFFFile(std::vector<RIFFChunk> chunks){
    RIFFFile(chunks, false); //Default to RIFF
}

RIFFFile::RIFFFile(std::vector<RIFFChunk> chunks, bool is_rifx) :
    chunks(chunks),
    is_rifx(is_rifx)
	{}

RIFFFile::~RIFFFile(){} //Nothing to do

bool RIFFFile::isRIFX() const {return is_rifx;}

const std::vector<RIFFChunk> & RIFFFile::getChunks() const{return chunks;}