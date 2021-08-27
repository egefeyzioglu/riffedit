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

#include "RIFFChunk.h"
#include "RIFFFile.h"

#define RIFF_HEADER_SIZE 12
#define DEFAULT_PATH "/mnt/d/dtmf.wav";

bool do_output_data = false;
bool only_data = false;

std::string path;

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
	RIFFFile file(path);
	return EXIT_SUCCESS;
}
