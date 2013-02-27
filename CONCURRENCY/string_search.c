#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>

#define NTHREADS 100
#define MAXSIZE     6000000

static char keyword[80];
static char file[MAXSIZE];
static int length_of_keyword = 0;
static int size_per_thread = 0;
static int file_size = 0;

struct thread_info{
    int count;
    char *start_of_text;
};

void * search_string(void *thread_data){
    char *curr_ptr;
    struct thread_info *info = (struct thread_info *) thread_data;

    curr_ptr = strstr(info->start_of_text, keyword);
    while( (curr_ptr != NULL) && (curr_ptr < (info->start_of_text + size_per_thread)) ){
        info->count++;
        curr_ptr = curr_ptr+length_of_keyword;
        curr_ptr = strstr(curr_ptr, keyword);
    };
}

int main(int argc, char * argv[]){

    int i;
    pthread_t thread_id[NTHREADS];
    int total_count = 0;
    struct thread_info t_info[NTHREADS];
    struct timeval start, end;
    long runtime;

    if(argc != 2){
        printf("Usage: %s \"search_string\" < filename\n", argv[0]);
        return 1;
    }
    length_of_keyword = strlen(argv[1]);
    strncpy(keyword, argv[1], length_of_keyword);
    /* calculate file size */
    while( (file[file_size] = getc(stdin)) != EOF){
        file_size++;
    }

    gettimeofday(&start, NULL);
    size_per_thread = (int) (file_size/NTHREADS);
    for(i =0; i<NTHREADS; i++){
        t_info[i].start_of_text = &file[i*size_per_thread];
        t_info[i].count = 0;
        pthread_create(&thread_id[i], NULL, search_string, (void *)&t_info[i]);
    }
    for(i=0;i<NTHREADS;i++){
        pthread_join(thread_id[i], NULL);
        total_count = total_count + t_info[i].count;
    }

//    printf("count:%d\n", total_count);
    gettimeofday(&end, NULL);
    runtime = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;

    printf("%d\t%ld\n", NTHREADS,runtime);
    return 0;
}
