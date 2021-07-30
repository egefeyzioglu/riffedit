#include <string>
#include <algorithm>
#include "RIFFFile.h"


RIFFFile::RIFFFile(){}
RIFFFile::RIFFFile(u_int8_t & buf){
	
}
RIFFFile::RIFFFile(u_int8_t * buf, size_t len){
	if(len < RIFF_HEADER_SIZE){
		throw "RIFF header size incorrect";
	}
	
	//Check format
	if(std::string((char*) buf, 4) != "RIFF"){
		throw "Input file is not a RIFF file (magic number missing or incorrect.)";
	}
	if(std::string((char*) (buf+8), 4) != "WAVE"){
		throw "Input file is not a WAVE file (file format is incorrect.)";
	}
	
	//Chunk size
	int64_t total_size = 8; //Initialised to 8 since we already processed 8 bytes
	u_int32_t *chunk_size = (u_int32_t*) (buf+4);
	total_size+= *chunk_size;
	std::cout << "Chunk size is " << *chunk_size << ". That means the file is " <<  total_size <<  "B large." << std::endl;
	
	//Check if the size is correct
	if((u_int64_t) total_size != len){ //This is technically bad form, but total_size can't possibly exceed (2^32 - 1) + 8, so I'm not concerned about a conversion problem here
		char exception[100];
		sprintf(exception, "Header says the file is %ld long, but we were provided %ldB.", total_size, len);
		throw exception;
	}
	
	//Advance pointer past the header
	buf += 12;
	
	u_int8_t *ptr = buf;
	int i = 1;
	while(ptr - buf < *chunk_size - 8){ //-8 because we need to skip over stuff not included in the count
		std::string id((char*) ptr, 4);
		
		u_int32_t *size_ = (u_int32_t*) (ptr + 4);
		u_int32_t size = *size_; //So that we aren't mutating the buffer
		size+= 8; //Read size doesn't include the first 8 bytes
		
		
		std::cout << std::endl;
		this->handle_subchunk((u_int8_t*) ptr, size, id, ptr - buf + 12L, i); //Add 12 to the offset because of the RIFF header	
		std::cout << std::endl;
		
		//Advance the pointer
		ptr += size;
		i++;
	}
}


void RIFFFile::handle_subchunk(uint8_t *baseptr, u_int32_t len, std::string id, size_t offset, int subchunk_no){
	RIFFSubChunk new_subchunk;
	subchunks.push_back(new_subchunk);
	std::cout << "\033[1m======SubChunk" << subchunk_no << "======\033[m" << std::endl;
	std::cout << "Base pointer offset is " << offset << std::endl;
	std::cout << "SubChunk" << subchunk_no << " ID: \"" << id << "\""  << std::endl;
	std::cout << "SubChunk" << subchunk_no << " Size: "<< len << std::endl;
	if(id == "data"){
		if(false){
			std::cout << "hexdump of the data contained" << std::endl << std::endl;
			//hexdump(path, offset, len);
		}else{
			std::cout << "Suppressed data output." << std::endl;
		}
	}else if(id == "fmt "){
		u_int8_t *extra_params = NULL;
		u_int16_t *extra_params_len = NULL;
		u_int8_t *ptr = baseptr;
		//Read all the fields by advancing the pointer one by one
		ptr += 8;
		u_int16_t *format = (u_int16_t*) ptr;
		ptr += 2;
		u_int16_t *num_channels = (u_int16_t*) ptr;
		ptr += 2;
		u_int32_t *sample_rate = (u_int32_t*) ptr;
		ptr += 4;
		u_int32_t *byterate = (u_int32_t*) ptr;
		ptr += 4;
		u_int16_t *block_align = (u_int16_t*) ptr;
		ptr += 2;
		u_int16_t *bits_per_sample = (u_int16_t*) ptr;
		ptr += 2;
		if(baseptr + len != ptr){ //We have extra data
			extra_params_len = (u_int16_t*) ptr;
			ptr += 2;
			extra_params = ptr;
		}
		
		std::string format_str = std::to_string(*format);		

		//Now print everything
		std::cout << "Audio Format: " << (*format == 1 ? "PCM" : format_str) << std::endl;
		std::cout << "Number of Channels: " <<  *num_channels << std::endl;
		std::cout << "Sample Rate: " << *sample_rate << std::endl;
		std::cout << "Bitrate: " << *byterate * 8 << "bps" << " (" <<  *byterate << "Bps)" << std::endl;
		std::cout << "Block Allign: " << *block_align << std::endl;
		std::cout << "Bits per Sample: " << *bits_per_sample << std::endl;
		
		if(extra_params != NULL){
			std::cout << "Extra parameters:" << std::endl;
			//hexdump(path, offset, len);
		}
	}else if(id == "LIST"){
		//Make sure the list type is "INFO", that's the only one we know
		std::string list_type_id((char*) baseptr+8, 4);
		if(list_type_id != "INFO"){
			std::cerr << "Unknown list type id: \"" << list_type_id << "\"" << std::endl;
			return;
		}
		uint8_t *ptr = baseptr;
		ptr+=12;//Skip header
		while(ptr != baseptr + len){
			std::string info_id((char*) ptr, 4);
			ptr += 4;
			u_int32_t *text_size = (u_int32_t*) ptr;
			ptr += 4;
			std::string text((char*)ptr, *text_size);
			
			//Remove null padding if it exists
			text.erase(std::find(text.begin(), text.end(), '\0'), text.end());
			
			std::cout << info_id << ": " << text << std::endl;
			ptr += *text_size;
		}
		
	}else{
		std::cout << "Unknown SubChunk id \"" << id << "\"." << std::endl;
		std::cout << "Data contained:" << std::endl;
//		hexdump(path, offset, len);
	}
}