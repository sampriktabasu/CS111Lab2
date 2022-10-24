#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process {
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 time_remaining;
  u32 time_elapsed;
  bool response;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end) {
  u32 current = 0;
  bool started = false;
  while (*data != data_end) {
    char c = **data;

    if (c < 0x30 || c > 0x39) {
      if (started) {
	return current;
      }
    }
    else {
      if (!started) {
	current = (c - 0x30);
	started = true;
      }
      else {
	current *= 10;
	current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data) {
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++])) {
    if (c < 0x30 || c > 0x39) {
      exit(EINVAL);
    }
    if (!started) {
      current = (c - 0x30);
      started = true;
    }
    else {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED) {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;
  

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL) {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i) {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }
  
  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  struct process *temp_p;
  // initialize all processes from the data array from processes.txt
  for(u32 i = 0; i < size; i++) {
    temp_p = &data[i];
    // set the time remaining for each process equal to its burst time as no time has been completed for each process yet
    temp_p->time_remaining = temp_p->burst_time;
    temp_p->response = false;
  }

  struct process *paused_process = NULL;
  u32 quantum_remaining = quantum_length;
  
  while(!finished) {
    // loop through all processes still in the array of processes from the txt, check arrival time and add newly arrived processes to the queue
    for(u32 i = 0; i < size; i++) {
      temp_p = &data[i];
      if(temp_p->arrival_time == t) {
	TAILQ_INSERT_TAIL(&list, temp_p, pointers);
      }
    }
    // now that the active processes have been added to the TAILQ list, reset to be null
    temp_p = NULL;

    // check if there are is a process that had just been run but is paused/still has time remaining, place it at the end of the queue as well
    if(paused != NULL) {
      TAILQ_INSERT_TAIL(&list, paused, pointers);
      // reset the paused process holder to be null as it is now accounted for 
      paused = NULL;
    }

    // check if the queue is empty, if not set current process to the first process in the queue
    if(!TAILQ_EMPTY(&list)) {
      temp_p = TAILQ_FIRST(&list);
    }

    // now check if there is any active process in the temp process holder variable
    if(temp_p != NULL && p->time_remaining > 0 ) {
      // decrement the time remaining for the process to run
      p->time_remaining -= 1;
      // update the time that has passed in the quantum
      quantum_remaining -= 1;
      //
    }
    

    
  }
  //check the arrival time 

  time_elapsed = 0;
  
  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float) total_waiting_time / (float) size);
  printf("Average response time: %.2f\n", (float) total_response_time / (float) size);

  free(data);
  return 0;
}
