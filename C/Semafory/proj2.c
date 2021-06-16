/*
 * IOS project 2
 * Author: Richard Seipel, xseipe00
 * Date: 18.4.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>

//constants
int PI, IG, JG, IT, JT;

//future semaphores
sem_t *turniket = NULL;
sem_t *mutex = NULL;
sem_t *confirmed = NULL;
sem_t *all_signed_in = NULL;
sem_t *output = NULL;

//future shared variables
int *NE = NULL;
int *NC = NULL;
int *NB = NULL;
int *judge_in = NULL;
int *output_num = NULL;
FILE **out_file = NULL;

/*
 * Process immigrant
 */
void imm(int imm_num){

	sem_wait(output);
	(*output_num)++;
	fprintf(*out_file, "%-8d: IMM %-10d: starts\n", *output_num, imm_num);
	sem_post(output);
	
	sem_wait(turniket);
	sem_wait(output);
	(*output_num)++;
	(*NE)++;
	(*NB)++;
	fprintf(*out_file, "%-8d: IMM %-10d: %-22s: %-6d: %-6d: %d\n", *output_num, imm_num, "enters", *NE, *NC, *NB);
	sem_post(output);
	sem_post(turniket);

	sem_wait(mutex);

	sem_wait(output);
	(*output_num)++;
	(*NC)++;
	fprintf(*out_file, "%-8d: IMM %-10d: %-22s: %-6d: %-6d: %d\n", *output_num, imm_num, "checks", *NE, *NC, *NB);
	sem_post(output);

	if (*judge_in==1 && *NE==*NC){ 
		sem_post(all_signed_in);
	}
	else{
		sem_post(mutex);
	}
	
	sem_wait(confirmed);

	sem_wait(output);
	(*output_num)++;
	fprintf(*out_file, "%-8d: IMM %-10d: %-22s: %-6d: %-6d: %d\n", *output_num, imm_num, "wants certificate",*NE, *NC, *NB);
	sem_post(output);

	usleep(rand()%(IT+1)); // sleep for random time form <0, IT>

	sem_wait(output);
	(*output_num)++;
	fprintf(*out_file, "%-8d: IMM %-10d: %-22s: %-6d: %-6d: %d\n", *output_num, imm_num, "got certificate", *NE, *NC, *NB);
	sem_post(output);

	sem_wait(turniket);
	sem_wait(output);
	(*output_num)++;
	(*NB)--;
	fprintf(*out_file, "%-8d: IMM %-10d: %-22s: %-6d: %-6d: %d\n", *output_num, imm_num, "leaves", *NE, *NC, *NB);
	sem_post(output);
	sem_post(turniket);
}

/*
 * Process judge
 */
void judge(){
	int imm_done = 0;
	int backup_NC;
	while (imm_done < PI){

		usleep(rand()%(JG+1)); // sleep for random time form <0, JG>

		sem_wait(output);
		(*output_num)++;
		fprintf(*out_file, "%-8d: %-14s: %s\n", *output_num, "JUDGE", "wants to enter");
		sem_post(output);

		sem_wait(turniket);
		sem_wait(mutex);
		
		sem_wait(output);
		(*output_num)++;
		fprintf(*out_file, "%-8d: %-14s: %-22s: %-6d: %-6d: %d\n", *output_num, "JUDGE", "enters", *NE, *NC, *NB);
		sem_post(output);

		*judge_in = 1;

		if (*NE > *NC){
			sem_wait(output);
			(*output_num)++;
			fprintf(*out_file, "%-8d: %-14s: %-22s: %-6d: %-6d: %d\n", *output_num, "JUDGE", "waits for imm", *NE, *NC, *NB);
			sem_post(output);

			sem_post(mutex);
			sem_wait(all_signed_in);
		}

		sem_wait(output);
		(*output_num)++;
		fprintf(*out_file, "%-8d: %-14s: %-22s: %-6d: %-6d: %d\n", *output_num, "JUDGE", "starts confirmation", *NE, *NC, *NB);
		sem_post(output);
		
		usleep(rand()%(JT+1)); // sleep for random time form <0, JT>
		
		sem_wait(output);
		(*output_num)++;
		backup_NC = *NC;
		*NE = 0;
		*NC = 0;
		fprintf(*out_file, "%-8d: %-14s: %-22s: %-6d: %-6d: %d\n", *output_num, "JUDGE", "ends confirmation", *NE, *NC, *NB);
		sem_post(output);
		
		imm_done += backup_NC; // counts how many immigrants have confirmation
		for (int i=0; i<backup_NC; i++)
			sem_post(confirmed); // let all confirmed immigrants continue

		usleep(rand()%(JT+1)); // sleep for random time form <0, JT>
		
		sem_wait(output);
		(*output_num)++;
		fprintf(*out_file, "%-8d: %-14s: %-22s: %-6d: %-6d: %d\n", *output_num, "JUDGE", "leaves", *NE, *NC, *NB);
		sem_post(output);
		
		*judge_in = 0;

		sem_post(mutex);
		sem_post(turniket);
	}

	sem_wait(output);
	(*output_num)++;
	fprintf(*out_file, "%-8d: %-14s: %s\n", *output_num, "JUDGE", "finishes");
	sem_post(output);
}

/*
 * Generates one judge
 */
void judge_generator(){
	pid_t pid=fork();
		if (pid==0) { // judge starts
			judge();
			exit(0); // when all immigrants have confirmation, judge ends
		} else if (pid==-1) {
			fprintf(stderr, "judge fork() error\n");
			exit(-1);
		}
}

/*
 * Generates PI immigrants
 */
void imm_generator(){
	for (int i=1; i<=PI; i++){ // parent process loops here

		usleep(rand()%(IG+1)); // sleep for random time form <0, IG>

		pid_t pid=fork();
		if (pid==0) { // new immigrant starts
			imm(i);
			exit(0); // when immigrant leave the building he returns here
		} else if (pid==-1) {
			fprintf(stderr, "immigrant fork() error\n");
			exit(-1);
		}
	}

	while (wait(NULL) > 0); // waits for all immigrants
}

/*
 * Destroys semaphores, dealocates shared memory, closes file
 */
void destroy_everything(){
	sem_destroy(turniket);
	sem_destroy(mutex);
	sem_destroy(confirmed);
	sem_destroy(all_signed_in);
	sem_destroy(output);

	munmap(turniket, sizeof(sem_t));
	munmap(mutex, sizeof(sem_t));
	munmap(confirmed, sizeof(sem_t));
	munmap(all_signed_in, sizeof(sem_t));
	munmap(output, sizeof(sem_t));

	munmap(NE, sizeof(int));
	munmap(NC, sizeof(int));
	munmap(NB, sizeof(int));
	munmap(judge_in, sizeof(int));
	munmap(output_num, sizeof(int));

	fclose(*out_file);
	munmap(out_file, sizeof(FILE *));
}

/*
 * Controls right type, right intervals, number of arguments
 * and converts them to integers
 */
bool args_control(int argc, char *argv[]){
	if (argc == 6){ // 5 arguments + 1 path
		char *endptr;

		PI = strtol(argv[1], &endptr, 0); // first argumnet
		if (*endptr != '\0' || PI<1){
			fprintf(stderr, "Argument PI must be integer bigger than 0\n");
			destroy_everything();
			return false;
		}

		int values[4];
		for (int i=0; i<4; i++){
			values[i] = strtol(argv[i+2], &endptr, 0); // other arguments
			if (*endptr != '\0' || values[i]<0 || values[i]>2000){
				fprintf(stderr, "Arguments IG, JG, IT, JT must be integers in interval <0, 2000>\n");
				destroy_everything();
				return false;
			}
		}

		// usleep measures time in microseconds and arguments are in miliseconds
		IG = values[0]*1000; JG = values[1]*1000; IT = values[2]*1000; JT = values[3]*1000;
	}
	else{
		fprintf(stderr, "Usage: ./proj2 PI IG JG IT JT\n");
		destroy_everything();
		return false;
	}
	return true;
}


int main(int argc, char *argv[]){

	// shared memory allocation
	turniket = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	mutex = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	confirmed = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	all_signed_in = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	output = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);

	NE = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	NC = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	NB = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	judge_in = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	output_num = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
	out_file = mmap(NULL, sizeof(FILE *), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);

	// semaphores initialization
	sem_init(turniket, 1, 1);
	sem_init(mutex, 1, 1);
	sem_init(confirmed, 1, 0);
	sem_init(all_signed_in, 1, 0);
	sem_init(output, 1, 1);

	// shared variables initialization
	*NE = 0; *NC = 0; *NB = 0;
	*judge_in = 0;
	*output_num = 0;
	*out_file = fopen("proj2.out", "w");

	setlinebuf(*out_file); // writes every line to file immediately

	if (!args_control(argc, argv)) return 1; 

	// start
	judge_generator();
	pid_t pid=fork();
		if (pid==0) { // generator starts
			imm_generator();
			exit(0); // when all immigrants leave, generator ends
		} else if (pid==-1) {
			fprintf(stderr, "imm_generator fork() error\n");
			exit(-1);
		}
	
	while (wait(NULL) > 0); // main process waits here for all the other
	
	destroy_everything();
	return 0;
}