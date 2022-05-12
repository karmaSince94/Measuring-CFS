#include <ups/upscaledb.h>
#include <stdio.h> // for printf
#include <stdlib.h> // for exit
#include <string.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h>

long long BILLION = 1000000000;

// this helper function will handle unexpected errors.
static void check_error(ups_status_t st, const char *description) {
    printf("%s() returned error %d: %s\n", description, st, ups_strerror(st));
    exit(-1);
}	

static int key = 1;
static int value = 1;
static void insert(ups_db_t *db);


int main() {

	ups_status_t st;	//status variable
	ups_env_t *env;		// env object
	ups_db_t *db;		// db object
	ups_key_t key = {0};         /* the structure for a key */
  	ups_record_t record = {0};   /* the structure for a record */

	// create enviornment 
	st = ups_env_create(&env, "test.db", 0, 0664, 0);
	if(st != UPS_SUCCESS){
		check_error(st, "ups_env_create");
	}


	st = ups_env_create_db(env, &db, 1, 0, 0);
	if(st != UPS_SUCCESS){
		check_error(st, "ups_env_create_db");
	}


	insert(db);
	insert(db);
	// insert("two", "second record");
	// insert("three", "third record");
	// insert("four", "fourth record");
	// insert("five", "fifth record");

	
	// create cursor
	ups_cursor_t *cursor;
	printf("\nCreating cursor....\n", st);
	st = ups_cursor_create(&cursor, db, 0, 0);
	if(st != UPS_SUCCESS){
		check_error(st, "ups_cursor_create");
	}

	// move to the first key
	ups_key_t key1 = {0};
	ups_record_t record1 = {0};
	printf("\nMoving cursor for reading....\n", st);
	st = ups_cursor_move(cursor, &key1, &record1, UPS_CURSOR_FIRST);
	if(st != UPS_SUCCESS){
		printf("\nCursor move fail...\n");
		check_error(st, "ups_cursor_move first");
	}

	// print key, then move to the next and repeat
	do {
		printf("%s\t=> %s\n", (const char *)key1.data, (const char*)record1.data);
		st = ups_cursor_move(cursor, &key1, &record1, UPS_CURSOR_NEXT);
	} while (st != UPS_KEY_NOT_FOUND);


	st = ups_cursor_close(cursor);
	if(st != UPS_SUCCESS){
		printf("\nClosing cursor...");
		check_error(st, "ups_cursor_close");
	}

	st = ups_env_close(env, UPS_AUTO_CLEANUP);
	if(st != UPS_SUCCESS){
		printf("\nenv close failed!!\n");
		check_error(st, "ups_env_close");
	}else{
		printf("\nenv close success!!\n");
	}
	return 0;

}

static void insert(ups_db_t *db) {
	//clock_t start, end;
     	double cpu_time_used;

	struct timespec start, end;
	

	printf("\n***************************************");
	printf("\nProcessID executing the insert function : %ld", getppid());	
	printf("\nInserting record....%d %d", key, value);
	
	// convert int to string val (to store increament counter)
	char int_string_key[5] = "";
	char int_string_value[5] = "";
	sprintf(int_string_key, "%d", key);
	sprintf(int_string_value, "%d", value);
	char str_key[10] = "Key_";
	char str_value[20] = "Hellow_Upscale_";
	
	// append int to string 
	// like., 'Key_1', 'Hellow_Upscale_1'
	strcat(str_key, int_string_key);
	strcat(str_value, int_string_value);
	printf("\nstr_key -> %s", str_key);
	printf("\nstr_value -> %s", str_value);

	// upscale DB - creating key/record and inserting
	ups_key_t k = ups_make_key((void *)str_key, strlen(str_key) + 1);
	ups_record_t r = ups_make_record((void *)str_value, strlen(str_value) + 1);
	// start = clock();
	// end = clock();
	// cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	
	// printf("key->data : %p\n", (k.data));
	// printf("value->data : %p\n", (r.data));
	clock_gettime(CLOCK_MONOTONIC, &start);
	ups_status_t st = ups_db_insert(db, 0, &k, &r, 0);
	clock_gettime(CLOCK_MONOTONIC, &end);
	cpu_time_used = BILLION * ( end.tv_sec - start.tv_sec ) + 
			( end.tv_nsec - start.tv_nsec );
	printf("\nCPU time used : %lf", cpu_time_used);
	if(st != UPS_SUCCESS){
		printf("\nInsert record failed\n");
		check_error(st, "ups_db_insert");
	}else{
		printf("\nInsert record success\n");
	}
	key++;
	value++;
	printf("\n***************************************");

}



