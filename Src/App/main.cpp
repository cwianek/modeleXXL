//
// Created by mateusz on 01.08.17.
//


#include <MPIWrapper.h>

int main(int argc, char** argv ) {
    MPIWrapper* mpiWrapper = new MPIWrapper();

    mpiWrapper->hello();

    delete mpiWrapper;
    return 0;
}