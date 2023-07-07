#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Shared variables and global constants.
 *
 */
#define MAX_FILES 10

char* CITIES[] = {

	"Charlottetown.dat",
	"Edmonton.dat",
	"Halifax.dat",
	"Montreal.dat",
	"Ottawa.dat",
	"Quebec.dat",
	"Toronto.dat",
	"Vancouver.dat",
	"Victoria.dat",
	"Winnipeg.dat"
};

pthread_mutex_t mutex;
 
double globalMax;
double globalMin;
int maxCity = -1;
int minCity = -1;
int totalValues = 0;


/* Function: validValue
 * Argument: char* str
 *
 * Determines the validity of the value by,
 * comparing if the string is empty, a digit,
 * or a negative value.
 *
 * return value: 1 or 0
 */
int validValue(char *str){

	if(!str){
		return 0;
	}
	
	char digit = str[0];
	if(digit == '-' || isdigit(digit)){
		return 1;
	}
	
	return 0;
}

/* Function: formatCity
 * Argument: char* filename
 *
 * Allocates memory for a char pointer, 
 * and returns the city of the file.
 *
 * Returns: dynamically allocated char pointer.
 */
char *formatCity(char *fileName){

	char *city = (char*)malloc(sizeof(char) * strlen(fileName));
	int i = 0;
	for(;fileName[i] != '.'; i++){
		city[i] = fileName[i];
	}
	city[i] = '\0';
	return city;
}

/* Function: printData
 * Argument: cityIndex(Index of the city)
 * 	     min(Minimum temperature of the city)
 * 	     max(Maximum temperature of the city)
 * 	     avg(Average temperature of the city)
 * 	     count(Values processed for the city)
 * 
 * Prints the arguments in a certain format. 
 */
void printData(int cityIndex, double min, double max, double avg, int count){

	char * city = formatCity(CITIES[cityIndex]);
	printf("====================================================================\n");
	printf("Data for: %s city\n", city);
	printf("%s's lowest temperature is: %g degrees Celsius.\n", city, min);
	printf("%s's highest tempterature is: %g degrees Celsius.\n", city, max);
	printf("The average temperature for %s is: %g degrees Celsius.\n", city, avg);
	printf("Total values processed for %s are: %i.\n", city, count);
	free(city);	


}
/* Function: processFile
 * Argument: arg(index of the file to be processed)
 *
 * Reads the file and gets certain data and prints them
 * 
 * Returns: Null
 */ 
void* processFile(void *arg){

	int city = *(int*)arg;
	FILE *fp = fopen(CITIES[city], "r");
	char *line = NULL;
	long unsigned len = 0;
	getline(&line, &len, fp);
	
	double min = 0;
	double max = 0;
	double inMin;
	double inMax;
	double sum = 0;
	double count = 0;
	while(getline(&line, &len, fp) != EOF){
		if(sscanf(line, "%lf\t%lf", &inMax, &inMin) == 2){
			sum = sum + inMax + inMin;
			if(count == 0 || inMax > max){
				max = inMax;
			}
			count++;
			if(count == 1 || inMin < min){
				min = inMin;
			}
			count++;
		}
	}
		
	pthread_mutex_lock(&mutex);
	printData(city, min, max, sum/count, count);
	if(minCity == -1 || min <= globalMin){
		minCity = city;
		globalMin =  min;
	}
	if(maxCity == -1 || max >= globalMax){
		maxCity = city;
		globalMax = max;
	}
	totalValues += count;
	pthread_mutex_unlock(&mutex);
	
	fclose(fp);
	if(line){
		free(line);
	}
	free((int*)arg);
	return NULL;
}

/* Function: multiThread
 * Argument: void
 *
 * If the program was ran with a multithread flag
 * all of the files are processed in parallel
 *
 * Returns: void
 */
void multiThread(){

	pthread_t t[MAX_FILES];	
	pthread_mutex_init(&mutex, NULL);
	for(int i = 0; i < MAX_FILES; i++){

		int *arg = (int*)malloc(sizeof(int));
		*arg = i;
		if(pthread_create(&t[i], NULL, &processFile, arg)){
			printf("Failed to create thread.\n");	
			exit(1);
		}
	}
		
	for(int i = 0; i < MAX_FILES; i++){
		if(pthread_join(t[i], NULL)){
			printf("Failed to join thread.\n");
			exit(1);
		}
	}
	

	pthread_mutex_destroy(&mutex);
	
	
}

/* Function: singleThread
 * Argument: void
 *
 * If the program was started with no flags
 * then each file is processed sequantially
 *
 * Returns: void
 */
void singleThread(){
	
	for(int i = 0; i < MAX_FILES; i++){
		int *arg = (int*)malloc(sizeof(int));
		*arg = i;
		processFile(arg);
	}

}

/* Main function prints the execution time in clocks
 * and overall values from the files. 
 */
int main(int argc, char* argv[]){

	
	clock_t start,end;
	double exeTime;
	start = clock();
	if(argc == 1){
		printf("Running as single thread.\n");
		singleThread();
	}
	else if(!strcmp(argv[1], "-m")){
		multiThread();
	}
	else{
		printf("Error: flag not found. For multithread execution add the flag -m.\n");
		return 0;
	}
	end = clock();
	exeTime = (double)(end-start);

	char *min = formatCity(CITIES[minCity]);
	char *max = formatCity(CITIES[maxCity]);

		
	printf("====================================================================\n");
	printf("Total values evaluated is: %i.\n", totalValues);	
	printf("The lowest temperature overall is: %g reported in %s.\n", globalMin, min);
	printf("The highest temperature overall is: %g reported in %s.\n", globalMax, max);
	printf("Elapsed time: %g clocks.\n", exeTime);	
	free(min);
	free(max);
	return 1;
}
