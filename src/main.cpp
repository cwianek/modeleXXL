#include "headers.hpp"


void init(){
	int provided=0;
    MPI_Init_thread(0, 0, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "MPI_THREAD_MULTIPLE isn't supported\n");
        MPI_Finalize();
        exit(-1);
    }
    
}

int main(int argc, char **argv)
{
	srand(getpid());
    init();
    Agent * agent = new Agent();
    agent->run();
    

	
	
    MPI_Finalize();
}
