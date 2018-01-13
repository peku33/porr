#include <iostream>

#include <mpi.h>

#include <time.h>
#include <thread>
#include <random>

#include "Graph.hpp"
#include "Task.hpp"
#include "ParticleGroup.hpp"

int main(int ArgC, char ** ArgV)
{
	MPI_Init(&ArgC, &ArgV);

	int MpiRankMy, MpiRankTotal;
	MPI_Comm_rank(MPI_COMM_WORLD, &MpiRankMy);
	MPI_Comm_size(MPI_COMM_WORLD, &MpiRankTotal);

	std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Rank start" << std::endl;

	std::unique_ptr<const Graph> GPtr;
	std::unique_ptr<const Task> TPtr;

	if(MpiRankMy == 0)
	{
		// Root

		// Generator liczb losowych dla grafu.
		// Można go zainicjować stałą liczbą żeby uzyskać ten sam graf w  kolejnych uruchomieniach programu
		// Inicjalizacja bezparametrowo losuje seed na podstawie czasu

		// std::mt19937 GraphRandomGenerator(1);
		std::mt19937 GraphRandomGenerator;

		//=====================================================================
		// Graph
		//=====================================================================
		// Generowanie losowego grafu
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph generation start" << std::endl;
		Graph G = Graph::GenerateWaxmanRandom(GraphRandomGenerator, 50, 1.0, 0.20, 10, 100);
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph generation end" << std::endl;
		std::cout << std::endl;

		// Propagacja losowego grafu
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph propagation start" << std::endl;
		for(int Destination = 1; Destination < MpiRankTotal; Destination++)
			G.MpiSend(Destination, MPI_COMM_WORLD);
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph propagation end" << std::endl;

		// Po wysłaniu przenosimy graf do globalnej struktury
		GPtr.reset(
			new Graph(
				std::move(G)
			)
		);

		//=====================================================================
		// Task
		//=====================================================================
		Task T(*GPtr, 0, G.GetSize() - 1);

		// Wyślij zadanie
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Task propagation start" << std::endl;
		for(int Destination = 1; Destination < MpiRankTotal; Destination++)
			T.MpiSend(Destination, MPI_COMM_WORLD);
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Task propagation end" << std::endl;

		// Po wysłaniu przenosimy
		TPtr.reset(
			new Task(
				std::move(T)
			)
		);

	}
	else
	{
		// Non-Root

		//=====================================================================
		// Graph
		//=====================================================================

		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph reception start" << std::endl;
		Graph G = Graph::MpiReceive(0, MPI_COMM_WORLD);
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Graph reception end" << std::endl;

		// Po odebraniu przenosimy graf do globalnej struktury
		GPtr.reset(
			new Graph(
				std::move(G)
			)
		);

		//=====================================================================
		// Task
		//=====================================================================
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Task reception start" << std::endl;
		Task T = Task::MpiReceive(*GPtr, 0, MPI_COMM_WORLD);
		std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Task reception end" << std::endl;

		// Po odebraniu przenosimy do globalnej struktury
		TPtr.reset(
			new Task(
				std::move(T)
			)
		);
	}

	// Graf został rozpropagowany
	// Uruchamiamy runnery
	// Jazda
	std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Algorithm start" << std::endl;
	const std::chrono::steady_clock::time_point TimePointStart = std::chrono::steady_clock::now();

	// Generator liczb losowych dla cząstek
	// Pełna inicjalizacja pozwoli uzyskać powtarzalne wyniki

	// std::mt19937 ParticleRandomGenerator(1);
	std::mt19937 ParticleRandomGenerator;

	// Modyfikujemy generator dla każdego wątku
	ParticleRandomGenerator.seed(ParticleRandomGenerator() + MpiRankMy);

	ParticleGroup PG(*TPtr, 8, 64, 4, 4.0, 0.75, ParticleRandomGenerator, MpiRankMy, MpiRankTotal, MPI_COMM_WORLD);
	PG.Run();

	const std::chrono::steady_clock::time_point TimePointEnd = std::chrono::steady_clock::now();
	std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Algorithm end: " << std::chrono::duration_cast<std::chrono::milliseconds>(TimePointEnd - TimePointStart).count() << "ms" << std::endl;
	std::cout << std::endl;

	// Bariera synchronizacji
	MPI_Barrier(MPI_COMM_WORLD);

	// Ultraprymitywny sposób na pokazanie wyników w kolejności
	std::this_thread::sleep_for(std::chrono::milliseconds((1 + MpiRankMy) * 250));

	std::cout << "MpiRankMy = " << MpiRankMy << std::endl;
	const ParticleGroup::HistoryEntries_t & HistoryEntries = PG.GetHistoryEntries();
	for(const ParticleGroup::HistoryEntry_t & HistoryEntry : HistoryEntries)
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(HistoryEntry.TimePoint - TimePointStart).count() << "ms" << "\t" << HistoryEntry.PathWeight << "\t" << HistoryEntry.ParticleIteration << std::endl;
	std::cout << std::endl;

	MPI_Finalize();

	return 0;
}

