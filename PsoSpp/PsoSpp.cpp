#include <iostream>

#include <time.h>
#include <thread>

#include "Graph.hpp"
#include "Task.hpp"
#include "ParticleGroup.hpp"

int main()
{
	// Incjalizacja generatora liczb losowych
	srand((unsigned int) time(NULL));

	const Graph G1 = Graph::GenerateWaxmanRandom(50, 1.0, 0.20, 10, 100);
	const Task T1(G1, 0, G1.GetSideSize() * G1.GetSideSize() - 1);

	// G1.DumpAdjascencyMatrix(std::cout);
	// G1.GraphViz(std::cout);

	std::vector<std::thread> ParticleGroupThreads;

	ParticleGroupThreads.emplace_back([&] {
		ParticleGroup PG(T1, 30, 30, 30, 6, 3);
		PG.Run();
	});
	ParticleGroupThreads.emplace_back([&] {
		ParticleGroup PG(T1, 30, 30, 5, 6, 3);
		PG.Run();
	});
	ParticleGroupThreads.emplace_back([&] {
		ParticleGroup PG(T1, 30, 30, 30, 3, 3);
		PG.Run();
	});
	ParticleGroupThreads.emplace_back([&] {
		ParticleGroup PG(T1, 30, 30, 5, 3, 3);
		PG.Run();
	});

	for(std::thread & ParticleGroupThread : ParticleGroupThreads)
		ParticleGroupThread.join();

    return 0;
}

