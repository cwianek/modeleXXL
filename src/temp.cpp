
#define TAG 100

#define DOES_ANYBODY_ORGANIZE_COMPETITIONS 300

struct structToSend{
	int clock;
	int state;
	int size;
	int rank;
};

structToSend str;
pthread_mutex_t strMutex;

void run();

void sendMessagesAskingIfCompetitionIsHeld() {
    int buf[2] = {0, 1};
    str.clock++;
    buf[0] = str.clock;
    for (int i = 0; i < str.size; i++) {
        if (i != str.rank) {
            MPI_Send(buf, 2, MPI_INT, i, DOES_ANYBODY_ORGANIZE_COMPETITIONS, MPI_COMM_WORLD);
        }
    }
    printf("Wyslalem pytanie czy ktos organizuje konkurs? %d \n",str.rank);
}



//COMPETITION_QUESTION RESPONDER
void *doYouOrganizeResponder(void *ptr) {
    structToSend *processState = (structToSend *) ptr;
    int decision[2];
    MPI_Status status;

    while (true) {
        MPI_Recv(decision, 2, MPI_INT, MPI_ANY_SOURCE, DOES_ANYBODY_ORGANIZE_COMPETITIONS, MPI_COMM_WORLD, &status);
        pthread_mutex_lock(&strMutex);
        sharedData->clock = std::max(sharedData->clock, decision[0]) + 1;
        if(processState->state == INVITE_COMPETITORS && FREE_SLOTS){
			decision[1] = processState.city; //send him city id
			processState->competitors.push(status.MPI_SOURCE)
		} else{
			decision[1] = -1
		}
        processState->clock++;
        
        MPI_Send(decision, 2, MPI_INT, status.MPI_SOURCE, COMPETITION_ANSWER, MPI_COMM_WORLD);
        
       if (decision[1] == -1)
            printf("Wyslalem procesowi informacje, ze nie organizuje konkursu");
        else
             printf("Wyslalem procesowi informacje, ze organizuje konkurs");
        pthread_mutex_unlock(&strMutex);
    }
}


int main(int argc, char **argv)
{
    int provided=0;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided!=MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "Brak wystarczajacego wsparcia dla watkow - wychodze!\n");
        MPI_Finalize();
        exit(-1);
    }

    int size;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

	run();
    MPI_Finalize();
}

void run(){
	
	str.clock = 0;
	str.state = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &str.rank);
	MPI_Comm_size(MPI_COMM_WORLD, &str.size);
	
    
    pthread_mutex_lock(&strMutex);
    printf("Zmieniam stan na ASK_ORGANIZATION\n");
    str.clock++;
    str.state = 111;
    sendMessagesAskingIfCompetitionIsHeld();
    pthread_mutex_unlock(&strMutex);
	
}
