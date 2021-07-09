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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <math.h>

#define RIFF_HEADER_SIZE 12
#define DEFAULT_PATH "/mnt/d/dtmf.wav";

char header[RIFF_HEADER_SIZE];

bool do_output_data = false;
bool only_data = false;

char *path;

void handle_subchunk(char *baseptr, u_int32_t len, char* id, size_t offset, int subchunk_no){
	if(only_data){
		if(strcmp(id, "data")) return;
	}else{
		printf("\033[1m======SubChunk%d======\033[m\nBase pointer offset is %ld\n", subchunk_no, offset);
		printf("SubChunk%d ID: \"%s\"\n", subchunk_no, id);
		printf("SubChunk%d Size: %u\n\n", subchunk_no, len);
	}
	if(!strcmp(id, "data")){
		if(do_output_data){
			printf("hexdump of the data contained\n");
			fflush(stdout);
			int pid = fork();
			if(pid == -1){
				fprintf(stderr, "Can't fork to hexdump!\n");
			}else if(pid != 0){
				wait(0);
			}else{
				//hexdump /path/to/file.wav -C -s offset -n length
				char offset_s[(int) ceil(log10(offset + 8))];
				char len_s[(int) ceil(log10(len - 8))];
				
				sprintf(offset_s, "%ld", offset + 8);
				sprintf(len_s, "%d", len - 8);

				char *const args[] = {"hexdump", path, "-C", "-s", offset_s, "-n", len_s, NULL};
				char **env = NULL;
				
				execve("/usr/bin/hexdump", args, env);
			}
		}else{
			printf("Suppressed data output.\n");
		}
	}else if(!strcmp(id, "fmt ")){
		char *extra_params = NULL;
		u_int16_t *extra_params_len = NULL;
		char *ptr = baseptr;
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
		
		char format_str[6]; //Max value is 65535
		sprintf(format_str, "%d", *format);		

		//Now print everything
		printf("Audio Format: %s\n", *format == 1 ? "PCM" : format_str);
		printf("Number of Channels: %u\n", *num_channels);
		printf("Sample Rate: %d\n", *sample_rate);
		printf("Bitrate: %ubps (%uBps)\n", *byterate * 8, *byterate);
		printf("Block Allign: %u\n", *block_align);
		printf("Bits per Sample: %u\n", *bits_per_sample);
		
		if(extra_params != NULL){
			printf("Extra parameters:\n");
			fflush(stdout);
			int pid = fork();
			if(pid == -1){
				fprintf(stderr, "Can't fork to hexdump!\n");
			}else if(pid != 0){
				wait(0);
			}else{
				offset += 24;//Get to the extra parameters
				//hexdump /path/to/file.wav -C -s offset -n length
				char offset_s[(int) ceil(log10(offset))];
				char extra_params_len_s[(int) ceil(log10(*extra_params_len))];
				
				sprintf(offset_s, "%ld", offset);
				sprintf(extra_params_len_s, "%d", *extra_params_len);

				char *const args[] = {"hexdump", path, "-C", "-s", offset_s, "-n", extra_params_len_s, NULL};
				char **env = NULL;
				
				execve("/usr/bin/hexdump", args, env);
			}
		}
	}else if(!strcmp(id, "LIST")){
		//Make sure the list type is "INFO", that's the only one we know
		if(strncmp(baseptr+8, "INFO", 4)){
			char list_type_id[5];
			list_type_id[4] = 0;
			strncpy(list_type_id, baseptr+8, 4);
			fprintf(stderr, "Unknown list type id: \"%s\"\n", list_type_id);
			return;
		}
		char *ptr = baseptr;
		ptr+=12;//Skip header
		while(ptr != baseptr + len){
			char info_id[5];
			info_id[4] = 0;
			strncpy(info_id, ptr, 4);
			ptr += 4;
			u_int32_t *text_size = (u_int32_t*) ptr;
			ptr += 4;
			char text[*text_size + 1];
			text[*text_size] = 0;
			strncpy(text, ptr, *text_size);
			
			printf("%s: %s\n", info_id, text);
			ptr += *text_size;
		}
		
	}else{
		printf("Unknown SubChunk id \"%s\".\nData contained:\n", id);
		fflush(stdout);
		int pid = fork();
		if(pid == -1){
			fprintf(stderr, "Can't fork to hexdump!\n");
		}else if(pid != 0){
			wait(0);
		}else{
			//hexdump /path/to/file.wav -C -s offset -n length
			char offset_s[(int) ceil(log10(offset))];
			char len_s[(int) ceil(log10(len))];
			
			sprintf(offset_s, "%ld", offset);
			sprintf(len_s, "%d", len);

			char *const args[] = {"hexdump", path, "-C", "-s", offset_s, "-n", len_s, NULL};
			char **env = NULL;
			
			execve("/usr/bin/hexdump", args, env);
		}
	}
}


int main(int argc, char** argv){
	FILE *fp;
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			if(!strcmp(argv[i], "-d")) do_output_data = true;
			if(!strcmp(argv[i], "-do")){do_output_data = true; only_data = true;}
		}else{
			path = argv[i];
		}
	}
	if(!path) path = DEFAULT_PATH;
	fp = fopen(path, "rb");
	
	//Read header
	char *header = calloc(RIFF_HEADER_SIZE, 0);
	fseek(fp, 0, SEEK_SET);
	int num = fread(header, 1, RIFF_HEADER_SIZE, fp);	

	if(num != RIFF_HEADER_SIZE){
		fprintf(stderr, "RIFF header size incorrect!\n");
		return EXIT_FAILURE;
	}
	
	//Check format
	if(strncmp(header, "RIFF", 4)){
		fprintf(stderr, "Input file is not a RIFF file (magic number missing or incorrect.)\n");
		return EXIT_FAILURE;
	}
	if(strncmp(header+8, "WAVE", 5)){
		fprintf(stderr, "Input file is not a WAVE file (file format is incorrect.)\n");
		return EXIT_FAILURE;
	}
	
	//Chunk size
	int size_left = -4; //Initialised to -4 since we will read 4 bytes less than the chunk size
	u_int32_t *chunk_size = (u_int32_t*) (header+4);
	size_left+= *chunk_size;
	printf("Chunk size is %d. That means the file is %dB large, and we have %dB left to read\n", *chunk_size, *chunk_size + 8, size_left);
	
	//Read rest of the file
	char buf[size_left];
	num = fread(buf, 1, size_left, fp);
	
	//Check if we could read the whole file
	if(num != size_left){
		fprintf(stderr, "Only read %d bytes when the RIFF header said we would read %d. File corrupted or read was interrupted.\n", num, size_left);
		return EXIT_FAILURE;
	}
	
	char *ptr = buf;
	int i = 1;
	while(ptr - buf < size_left){
		char id[5];
		id[4] = 0;
		strncpy(id, ptr, 4);
		
		u_int32_t *size = (u_int32_t*) (ptr + 4);
		*size+= 8; //Read size doesn't include the first 8 bytes
		
		
		printf("\n");
		handle_subchunk(ptr, *size, id, ptr - buf + 12L, i); //Add 12 to the offset because of the RIFF header	
		printf("\n");
		
		//Advance the pointer
		ptr += *size;
		i++;
	}

	//if(isatty(fileno(stdout))) printf("\n");
	return EXIT_SUCCESS;
	printf("Read %d characters.", num);
/*
	if(num < BUFFER_SIZE) printf(" Printing %d anyway.\n", BUFFER_SIZE);
	else printf("\n");

	for(int i = 0; i < BUFFER_SIZE; i++){
		printf("%c", buf[i]);
	}
	

	//fclose(fp);
	return 0;*/
}

