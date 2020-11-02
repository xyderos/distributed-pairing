
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAKEN 1

#define FREE 0

int find(const int arr[], int size){

    srand(time(0));

    int partner = rand()%(size);

    while(arr[partner] == TAKEN) partner = (partner + 1)%(size);

    return partner;
}

static int occupied(const int arr[], int size){

    for(int i = 0; i < size; i++) if(arr[i] == FREE) return 0;

    return 1;
}

void teacher(const int size, const int rank){

    int arr[size];

    //intialize the array
    for(int i = 0; i <= size; i++) arr[i] = FREE;

    srand(time(0));

    int index = rand()%(size-2);

    index++;

    //print take a random student
    arr[0] = TAKEN;

    MPI_Send(&arr, size, MPI_INT, index, 0, MPI_COMM_WORLD);

}

int student(MPI_Status *status,const int size, const int rank){

    int arr[size];

    MPI_Recv(&arr, size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);

    printf("student %d has woken up\n", rank);

    arr[rank] = TAKEN;

    if(occupied(arr, size)){

        printf("student %d works with himself\n",rank);

        arr[rank] = TAKEN;

        return 0;

    }

    int partner = find(arr, size);

    printf("student %d is working with student %d\n", rank, partner);

    arr[partner] = TAKEN;

    if(occupied(arr, size)) return 0;

    int nextStudent = find(arr, size);

    MPI_Send(&arr, size, MPI_INT, nextStudent, 0, MPI_COMM_WORLD);
}

int main(int argc,char ** argv){

    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(0));

    MPI_Status status;

    if (rank == 0) teacher(size,rank);

    else student(&status,rank,size);

    MPI_Finalize();

    return 0;
}
