#include <iostream>

#include <time.h>
#include <thread>

#include "Graph.hpp"
#include "Task.hpp"
#include "ParticleGroup.hpp"
#include "ParticleGroupRunner.hpp"

int main()
{
	// Generowanie losowego grafu
	std::cout << "Graph generation start" << std::endl;
	const Graph G = Graph::GenerateWaxmanRandom(50, 1.0, 0.20, 10, 100);
	std::cout << "Graph generation end" << std::endl;
	std::cout << std::endl;

	// Układ grafu na ekran. Nie radzę odpalać dla grafu o boku > 5, nic tego nie narysuje
	// G1.DumpAdjascencyMatrix(std::cout);
	// G1.GraphViz(std::cout);

	// Formułowanie zadania
	// Zadanie polega na dotarciu z punku 0 (lewy dolny róg) do punktu ostaniego (prawy górny róg)
	const Task T(G, 0, G.GetSize() - 1);

	// Rój cząstek
	std::cout << "ParticleGroupRunner generation start" << std::endl;
	ParticleGroupRunner PGR(T, 0, 40, 25, 5, 4.0, 2.0);
	std::cout << "ParticleGroupRunner generation end" << std::endl;
	std::cout << std::endl;

	// Jazda
	std::cout << "Algorithm start" << std::endl;
	const std::chrono::steady_clock::time_point TimePointStart = std::chrono::steady_clock::now();
	ParticleGroupRunner::RunResult_t RunResult = PGR.Run();
	const std::chrono::steady_clock::time_point TimePointEnd = std::chrono::steady_clock::now();
	std::cout << "Algorithm end" << std::endl;
	std::cout << "Algorithm time: " << std::chrono::duration_cast<std::chrono::milliseconds>(TimePointEnd - TimePointStart).count() << "ms" << std::endl;
	std::cout << std::endl;

	// Wynik
	if(RunResult.ParticleBest)
	{
		std::cout << "Best path found: ";
		RunResult.ParticleBest.value().GetBestGraphPath().value().DumpPath(std::cout);
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Path not found" << std::endl;
	}
	std::cout << std::endl;

	// Historia postępu
	std::cout << "Progress history: " << std::endl;
	for(size_t ParticleGroupId = 0; ParticleGroupId < RunResult.HistoryEntries.size(); ParticleGroupId++)
	{
		std::cout << "ParticleGroupId = " << ParticleGroupId << std::endl;
		const ParticleGroup::HistoryEntries_t & HistoryEntries = RunResult.HistoryEntries.at(ParticleGroupId);
		for(const ParticleGroup::HistoryEntry_t & HistoryEntry : HistoryEntries)
			std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(HistoryEntry.TimePoint - TimePointStart).count() << "ms" << "\t" << HistoryEntry.PathWeight << "\t" << HistoryEntry.ParticleIteration << std::endl;
		std::cout << std::endl;
	}
	//
	std::cout << std::endl;

	return 0;
}

