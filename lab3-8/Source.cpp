#include <mpi.h>
#include <iostream>
#include <time.h>

using namespace std;

const int letterNumber = 13;

struct Letter {
	char ch;
	int number;
};

int main(int argc, char* argv[]) {
	srand(time(0));
	setlocale(LC_ALL, "Russian");
	MPI_Init(&argc, &argv);
	int proc_count, received_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &received_rank);

	if (proc_count == letterNumber) {
		//Регистрируем структуру
		Letter lt;
		int blocklens[2] = { 1, 1 };
		MPI_Aint indices[2] = { 0, &lt.number - (int*)& lt };
		MPI_Datatype old_types[2] = { MPI_CHAR, MPI_INT };
		MPI_Datatype userType;
		MPI_Type_struct(2, blocklens, indices, old_types, &userType);
		MPI_Type_commit(&userType);

		Letter answer[letterNumber];
		MPI_Request request[letterNumber];
		int flag[letterNumber];
		bool messageReceived = false;

		for (int i = 0; i < letterNumber; i++) {
			MPI_Irecv(answer + i, 1, userType, i, MPI_ANY_TAG, MPI_COMM_WORLD, request + i);
		};

		while (!messageReceived) {
			for (int i = 0; i < letterNumber; i++) {
				if (answer[i].ch == 's') {
					cout << "Процесс " << received_rank << " получил сообщение об остановке." << endl;
					MPI_Type_free(&userType);
					MPI_Finalize();
					return 0;
				}
			};

			messageReceived = true;
			for (int i = 0; i < letterNumber; i++) {
				MPI_Test(request + i, flag + i, MPI_STATUSES_IGNORE);
				messageReceived *= flag[i];
			};

			if (!messageReceived) {
				MPI_Request req;
				int receiver = rand() % letterNumber;
				lt.number = received_rank;
				switch (received_rank) {
				case 0:
					lt.ch = 'н';
					break;
				case 1:
					lt.ch = 'а';
					break;
				case 2:
					lt.ch = 'д';
					break;
				case 3:
					lt.ch = 'о';
					break;
				case 4:
					lt.ch = 'м';
					break;
				case 5:
					lt.ch = 'е';
					break;
				case 6:
					lt.ch = 'ч';
					break;
				case 7:
					lt.ch = 'е';
					break;
				case 8:
					lt.ch = 'м';
					break;
				case 9:
					lt.ch = 'о';
					break;
				case 10:
					lt.ch = 'д';
					break;
				case 11:
					lt.ch = 'а';
					break;
				case 12:
					lt.ch = 'н';
					break;
				};
				MPI_Isend(&lt, 1, userType, receiver, 0, MPI_COMM_WORLD, &req);
				cout << "Процесс " << received_rank << " отправил процессу " << receiver << " букву " << lt.ch << " с номером " << lt.number << endl;
			}
			else {
				cout << "Процесс " << received_rank << " получил все буквы из фразы." << endl;
				for (int i = 0; i < letterNumber; i++) {
					if (i == received_rank) { continue; };
					MPI_Request req;
					lt.ch = 's';
					lt.number = 0;
					MPI_Isend(&lt, 1, userType, i, 0, MPI_COMM_WORLD, &req);
				};

				for (int i = 0; i < letterNumber; i++) {
					if (answer[i].ch == 's') {
						cout << "Процесс " << received_rank << " получил сообщение об остановке." << endl;
						MPI_Type_free(&userType);
						MPI_Finalize();
						return 0;
					}
				};

				cout << "Процесс " << received_rank << " фраза в обратном порядке: ";
				for (int i = letterNumber - 1; i >= 0; i--) {
					cout << answer[i].ch;
				};
				cout << endl;
			}
		};

		MPI_Type_free(&userType);
	}
	else {
		cout << "Ошибка! Число процессов должно быть равно числу букв во фразе." << endl;
	};

	MPI_Finalize();
	return 0;
};