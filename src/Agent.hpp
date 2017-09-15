#pragma once
#include "headers.hpp"

struct processState{
	int clock;
	int rank;
    int size;
    int actualState;
    int currentCompetitionId;
    int city;
    int room;
    int roomRequestClock;
    int numOfAgreesToTakeRoom;
    int numOfHotelRooms;
    std::vector<int> currentRoomQueue; //list of id-s that wait for this room
};

class Agent{
public:
	Agent();
	void run();

private:
	processState state;
	void inviteToCompetitions();
	static void *handleInvitationToCompetition(void * p);
	static void *roomRequest(void *p);
	void randomOrganizator();
	static void sendRoomAgree(MPI_Status status, processState * state);
	static bool checkPriority(int recvTab[], MPI_Status status, processState * state);
	void sendMessagesAskingRoom();
	void receiveRoomAnswer();
	void initThreads();
	void chooseRoomAndCity();
	void organizeCompetitions();
	void checkIfAllAgrees();
	
};



