#include "headers.hpp"

#define WAIT_FOR_INVITATION 1
#define WANTING_TO_PARTICIPATE 2
#define ORGANIZE_COMPETITIONS 3
#define ASK_ROOM 4
#define ROOM_QUESTION 5
#define ROOM_ANSWER 6


#define INVITATION_TO_COMPETITION 10
#define INVITATION_TO_COMPETITION_RESPONSE 11

using namespace std;

pthread_mutex_t stateMutex;

Agent::Agent(){
	MPI_Comm_rank(MPI_COMM_WORLD, &state.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &state.size);
}

void Agent::randomOrganizator(){
	int randomNumber = rand() % 100;
	pthread_mutex_lock(&stateMutex);
	if(randomNumber < 30){
		state.actualState = ORGANIZE_COMPETITIONS;
	}else{
		state.actualState = WAIT_FOR_INVITATION;
	}
	pthread_mutex_unlock(&stateMutex);
}

bool Agent::checkPriority(int recvTab[], MPI_Status status, processState * state){
	bool priority;
	if (recvTab[0] < state->roomRequestClock){ 
		priority = false;
	}
    else if (recvTab[0] == state->roomRequestClock) {
		 if (state->rank > status.MPI_SOURCE){
			 priority = false;
		 }
         else{
			   priority = true;
		 }
    } 
    else if (recvTab[0] > state->roomRequestClock){
		 priority = true;
	}
	return priority;
}


void Agent::sendRoomAgree(MPI_Status status, processState *state){
	 int tabToBeSent[2];
	 tabToBeSent[1] = 1;
     state->clock++;
     tabToBeSent[0] = state->clock;
     MPI_Send(tabToBeSent, 2, MPI_INT, status.MPI_SOURCE, ROOM_ANSWER, MPI_COMM_WORLD);
     printf("Wysylam zgode %d, jestem proces %d\n",status.MPI_SOURCE,state->rank);
}

void Agent::receiveRoomAnswer(){
	int message[2];
	MPI_Status status;
	MPI_Recv(message, 2, MPI_INT, MPI_ANY_SOURCE, ROOM_ANSWER, MPI_COMM_WORLD, &status);
    state.clock = max(state.clock, message[0]) + 1;
    printf("Odebralem zgode na zajecie sali od %d\n",status.MPI_SOURCE);
    state.numOfAgreesToTakeRoom++;
}

//~ void Agent::checkIfAllAgrees(){
	 //~ if (state.numOfAgreesToTakeRoom >= state.size - 1) {
		//~ printf("ZAJMUJE SALE %d W MIESCIE %d",state.hall, state.city);
        //~ printf("Zmieniam stan na ASK_INVITES");
        //~ state.clock++;
        //~ state.state = ASK_INVITES;
        //~ //send invites to other processes (== number of rooms) and add them on potential users list
        //~ processesToBeInvited = randomize(str);
        //~ state.clock++;
        //~ tabToBeSent[0] = state.clock;
        //~ for (int i = 0; i < state.numOfHotelRooms; i++) {
			//~ tabToBeSent[1] = (int) str.city;
            //~ MPI_Send(tabToBeSent, 2, MPI_INT, processesToBeInvited[i], COMPETITION_ANSWER, MPI_COMM_WORLD);
            //~ str.potentialUsers.push_back(processesToBeInvited[i]);
        //~ }
                    //~ printInfo("Wyslalem zaproszenie na konkurs do kilku procesow");
                    //~ //left critical section and left loop to wait for all potential responsed
                    //~ pthread_mutex_unlock(&strMutex);
                    //~ break;
	//~ }
//~ }

void Agent::chooseRoomAndCity(){
	pthread_mutex_lock(&stateMutex);
	state.city = rand() % 10;
    state.room = rand() % 5;
    state.clock++;
    state.actualState = ASK_ROOM;
    sendMessagesAskingRoom();
    pthread_mutex_unlock(&stateMutex);
}

void Agent::initThreads(){
	pthread_t waitForInviteToCompetitionThread;
	pthread_t roomRequestThread;
    pthread_create(&waitForInviteToCompetitionThread, NULL, Agent::handleInvitationToCompetition, &state);
    pthread_create(&roomRequestThread,NULL,Agent::roomRequest,&state);
}

void Agent::organizeCompetitions(){
	inviteToCompetitions();
	chooseRoomAndCity();

	while (true) {				
		pthread_mutex_lock(&stateMutex);
		receiveRoomAnswer();
        //checkIfAllAgrees();
        pthread_mutex_unlock(&stateMutex);
	}
}

void Agent::run(){
    initThreads();
    randomOrganizator();
    
    while(true){
		if(state.actualState == ORGANIZE_COMPETITIONS){
			
			organizeCompetitions();
			
		}else if(state.actualState == WANTING_TO_PARTICIPATE){
			//~ MPI_Status status;
			//~ int recv3Tab[3];
			//~ MPI_Recv(recv3Tab, 3, MPI_INT, state.currentCompetitionId, ROOM_ANSWER, MPI_COMM_WORLD, &status);
		}
		sleep(1);
	}
	
}

void Agent::sendMessagesAskingRoom() {
    int buf[3] = {0, state.city, state.room};
    state.clock++;
    buf[0] = state.clock;
    state.roomRequestClock = state.clock;
    for (int i = 0; i < state.size; i++) {
        if (i != state.rank) {
            MPI_Send(buf, 3, MPI_INT, i, ROOM_QUESTION, MPI_COMM_WORLD);
        }
    }
    printf("Wyslalem pytanie czy moge wziac sale %d w miescie %d, jestem proces %d\n", buf[2], buf[1], state.rank);
}

void Agent::inviteToCompetitions() {
	printf("Wysle zaproszenia %d\n", state.rank);
    int message[2]; //message[0] - clock, message[1] - answer
    int clock = state.clock++;
    message[0] = clock;
    for (int i = 0; i < state.size; i++) {
        if (i != state.rank) {
            MPI_Send(message, 2, MPI_INT, i, INVITATION_TO_COMPETITION, MPI_COMM_WORLD);
        }
    }
    printf("Wyslalem pytanie do wszystkich czy chcą uczestniczyć w konkursie, jestem %d\n", state.rank);
}

void* Agent::handleInvitationToCompetition(void * p){
	MPI_Status status;
	processState *state = (processState *) p;
	int message[2];
	while (true) {
        MPI_Recv(message, 2, MPI_INT, MPI_ANY_SOURCE, INVITATION_TO_COMPETITION, MPI_COMM_WORLD, &status);
  
        pthread_mutex_lock(&stateMutex);
        state->clock = max(state->clock, message[0]) + 1;
        state->clock++;
		message[0] = state->clock;
        //TODO: losowo chce brać udział
        if(state->actualState == WAIT_FOR_INVITATION){
			state->actualState = WANTING_TO_PARTICIPATE;
			state->currentCompetitionId = status.MPI_SOURCE;
			message[1] = 1;
			printf("Wyraze chęć do uczestnictwa od procesu %d, jestem %d\n", status.MPI_SOURCE, state->rank);
		}else{
			message[1] = 0;
			printf("Wyraze niechęć do uczestnictwa od procesu %d, jestem %d\n", status.MPI_SOURCE, state->rank);
		}
		pthread_mutex_unlock(&stateMutex);
		MPI_Send(message, 2, MPI_INT, status.MPI_SOURCE, INVITATION_TO_COMPETITION_RESPONSE, MPI_COMM_WORLD);
	}
	
}

void *Agent::roomRequest(void *p) {
	MPI_Status status;
    processState *state = (processState *) p;
    int recvTab[3];
    
    while (true) {
        MPI_Recv(recvTab, 3, MPI_INT, MPI_ANY_SOURCE, ROOM_QUESTION, MPI_COMM_WORLD, &status);
        //[0] clock, [1] city, [2] room

        pthread_mutex_lock(&stateMutex);
        state->clock = max(state->clock, recvTab[0]) + 1;
        printf("Odebralem pytanie o sale %d w miescie %d od procesu %d, jestem proces %d\n", recvTab[2], recvTab[1], status.MPI_SOURCE, state->rank); 

        if (state->actualState == ASK_ROOM) {
            if (recvTab[1] == state->city && recvTab[2] == state->room) { // he wants the same
                bool priority = checkPriority(recvTab,status, state);
                if (!priority) { // if his priority is higher
                   sendRoomAgree(status, state);
                } else { //if our priority is higher
                    state->clock++;
                    state->currentRoomQueue.push_back(status.MPI_SOURCE);
                }
            } else { // if not our room then agree
                sendRoomAgree(status, state);
            }
        } else { // other state - agree
            sendRoomAgree(status, state);
        }
        pthread_mutex_unlock(&stateMutex);
    }
}

