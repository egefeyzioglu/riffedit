/******************************************************************************
 * This is a proof of concept. That is why it compiles with many warnings and *
 * why the file is not fully parsed. I can read the RIFF chunk, including     *
 * parsing its header/subchunks, and I can parse the individual subchunks.    *
 * Achieving that was my goal with this so that is why I have left it in this *
 * state. Maybe I'll come back and make it a fully features RIFF editor, who  *
 * knows?                                                                     *
 *                                                                            *
 * Ege Feyzioglu 2021                                                         *
 *****************************************************************************/

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "RIFFFile.h"

#define RIFF_HEADER_SIZE 12
#define DEFAULT_PATH "/mnt/d/dtmf.wav";

uint8_t header[RIFF_HEADER_SIZE];

bool do_output_data = false;
bool only_data = false;

std::string path;

void hexdump(std::string file_path, size_t offset, u_int32_t len){
	std::cout << std::flush;
	int pid = fork();
	if(pid == -1){
		std::cerr << "Can't fork to hexdump!" << std::endl;
	}else if(pid != 0){
		wait(0);
	}else{
		//hexdump /path/to/file.wav -C -s offset -n length
		char offset_s[(int) ceil(log10(offset + 8))];
		char len_s[(int) ceil(log10(len - 8))];
		char path_s[path.length() + 1];
		
		sprintf(offset_s, "%ld", offset + 8);
		sprintf(len_s, "%d", len - 8);
		strcpy(path_s, path.c_str());
		
		//args
		char args_hexdump[] = "hexdump";
		char args_c_flag[] = "-C";
		char args_s_flag[] = "-s";
		char args_n_flag[] = "-n";
		char *args[] = {args_hexdump, path_s, args_c_flag, args_s_flag, offset_s, args_n_flag, len_s, NULL};
		char *env[] = {NULL};
		
		execve("/usr/bin/hexdump", args, env);
	}
}

void handle_subchunk(uint8_t *baseptr, u_int32_t len, std::string id, size_t offset, int subchunk_no){
	if(only_data){
		if(id != "data") return;
	}else{
		std::cout << "\033[1m======SubChunk" << subchunk_no << "======\033[m" << std::endl;
		std::cout << "Base pointer offset is " << offset << std::endl;
		std::cout << "SubChunk" << subchunk_no << " ID: \"" << id << "\""  << std::endl;
		std::cout << "SubChunk" << subchunk_no << " Size: "<< len << std::endl;
	}
	if(id == "data"){
		if(do_output_data){
			std::cout << "hexdump of the data contained" << std::endl << std::endl;
			hexdump(path, offset, len);
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
			hexdump(path, offset, len);
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
		hexdump(path, offset, len);
	}
}


int main(int argc, char** argv){
	//Parse arguments. TODO: Make this use the standard function	
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			if(std::string(argv[i]) == "-d") do_output_data = true;
			if(std::string(argv[i]) == "-do"){do_output_data = true; only_data = true;}
		}else{
			path = argv[i];
		}
	}
	
	if(path.empty()) path = DEFAULT_PATH;
	
	//TODO: Check if file exists
	std::ifstream fp(path, std::ios::in | std::ios::binary | std::ios::ate);
	if(!fp.is_open()){
		std::cerr << "File can't be opened" << std::endl;
		return EXIT_FAILURE;
	}
	struct stat stat_buf;
	if(stat(path.c_str(), &stat_buf)){
		std::cerr << "Cannot stat " << path << std::endl;
		perror("stat");
		return EXIT_FAILURE;
	}
	
	std::cout << "stat says the file is " << stat_buf.st_size << " long." << std::endl;
	
	char *buffer = (char*) calloc((int) stat_buf.st_size, 'A');
	fp.seekg(0, std::ios::beg);
	fp.read(buffer, (int) stat_buf.st_size);
	RIFFFile((u_int8_t*) buffer, stat_buf.st_size);
	fp.close();
	return EXIT_SUCCESS;
	//Parse arguments. TODO: Make this use the standard function	
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			if(std::string(argv[i]) == "-d") do_output_data = true;
			if(std::string(argv[i]) == "-do"){do_output_data = true; only_data = true;}
		}else{
			path = argv[i];
		}
	}
	
	if(path.empty()) path = DEFAULT_PATH;
	
	//TODO: Check if file exists
	//std::ifstream fp(path, std::ios::in | std::ios::binary | std::ios::ate);
	if(!fp.is_open()){
		std::cerr << "File can't be opened" << std::endl;
		return EXIT_FAILURE;
	}
	
	//Read header
	char *header = (char*) calloc(RIFF_HEADER_SIZE, 0);
	fp.seekg(0, std::ios::beg);
	fp.read(header, RIFF_HEADER_SIZE);	

	if(fp.gcount() != RIFF_HEADER_SIZE){
		std::cerr << "RIFF header size incorrect!" << std::endl;
		return EXIT_FAILURE;
	}
	
	//Check format
	if(std::string(header, 4) != "RIFF"){
		std::cerr << "Input file is not a RIFF file (magic number missing or incorrect.)" << std::endl;
		return EXIT_FAILURE;
	}
	if(std::string(header+8, 4) != "WAVE"){
		std::cerr << "Input file is not a WAVE file (file format is incorrect.)" << std::endl;
		return EXIT_FAILURE;
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
		std::cerr << "Only read " << fp.gcount() << " bytes when the RIFF header said we would read " << size_left << ". File corrupted or read was interrupted." << std::endl;
		return EXIT_FAILURE;
	}
	
	char *ptr = buf;
	int i = 1;
	while(ptr - buf < size_left){
		std::string id(ptr, 4);
		
		u_int32_t *size_ = (u_int32_t*) (ptr + 4);
		u_int32_t size = *size_; //So that we aren't mutating the buffer
		size+= 8; //Read size doesn't include the first 8 bytes
		
		
		std::cout << std::endl;
		handle_subchunk((u_int8_t*) ptr, size, id, ptr - buf + 12L, i); //Add 12 to the offset because of the RIFF header	
		std::cout << std::endl;
		
		//Advance the pointer
		ptr += size;
		i++;
	}

	//if(isatty(fileno(stdout))) printf("\n");
	fp.close();
	return EXIT_SUCCESS;
/*
	printf("Read %d characters.", num);
	if(num < BUFFER_SIZE) printf(" Printing %d anyway.\n", BUFFER_SIZE);
	else printf("\n");

	for(int i = 0; i < BUFFER_SIZE; i++){
		printf("%c", buf[i]);
	}
	

	//fclose(fp);
	return 0;*/
}

