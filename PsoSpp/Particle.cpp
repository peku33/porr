#include "Particle.hpp"

#include <algorithm>

#include "ParticleGroup.hpp"
#include "Task.hpp"
#include "Graph.hpp"
#include "GraphPath.hpp"

Particle::Particle(const ParticleGroup & PG, std::mt19937 & RandomGenerator):
	PG(PG),
	RandomGenerator(RandomGenerator),
	Priorities(new GraphPath::Priority_t[PG.GetTask().GetGraph().GetSize()]),
	Velocities(new Velocity_t[PG.GetTask().GetGraph().GetSize()])
{
	// Inicjalizacja losowego stanu
	RandomInitialize();
}

const std::optional<const GraphPath> & Particle::GetGraphPathBest() const
{
	return GraphPathBest;
}

void Particle::RandomInitialize()
{
	std::uniform_real_distribution<GraphPath::Priority_t> PriorityDistribution(0.0, 1.0);
	std::uniform_real_distribution<Velocity_t> VelocityDistribution(0.0, 1.0);

	// Losowe Priorities, Velocities, [0.0; 1.0]
	std::generate(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSize(), [&] { return PriorityDistribution(RandomGenerator); });
	std::generate(Velocities.get(), Velocities.get() + PG.GetTask().GetGraph().GetSize(), [&] { return VelocityDistribution(RandomGenerator); });
}

bool Particle::Run()
{
	// Próbujemy skonstruować ścieżkę
	std::optional<GraphPath::VertexIndexes_t> VertexIndexes = GraphPath::BuildVertexIndexes(PG.GetTask(), Priorities);

	// Jeśli nie udało się zbudować ścieżki - kończymy
	if(!VertexIndexes)
		return false;

	// Udało się zbudować ścieżkę - sprawdzamy czy nastąpiła poprawa
	GraphPath::PathWeight_t PathWeight = GraphPath::CalculatePathWeight(PG.GetTask().GetGraph(), VertexIndexes.value());

	// Jeśli nie nastąpiła poprawa - kończymy
	if(GraphPathBest && GraphPathBest.value().GetPathWeight() <= PathWeight)
		return false;

	// Poprawa nastąpiła - zapisujemy nową ścieżkę
	GraphPath::Priorities_t PrioritiesNew(new GraphPath::Priority_t[PG.GetTask().GetGraph().GetSize()]);
	std::copy(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSize(), PrioritiesNew.get());

	GraphPathBest.emplace(
		PG.GetTask(),
		std::move(PrioritiesNew),
		std::move(VertexIndexes.value()),
		PathWeight
	);

	return true;
}

void Particle::Update(const double & Fi1, const double & Fi2, const std::optional<const GraphPath> & GraphPathBestGlobal)
{
	// Jeśli nie ma maksimum globalnego - idziemy w stronę bez zmian
	const GraphPath::Priorities_t & PriorietiesBestGlobal = GraphPathBestGlobal ? GraphPathBestGlobal.value().GetPriorities() : Priorities;
	
	// Idziemy w stronę maksimum lokalnego, jeśli nie ma - globalnego
	const GraphPath::Priorities_t & PriorietiesBestLocal = GraphPathBest ? GraphPathBest.value().GetPriorities() : PriorietiesBestGlobal;

	// Generator liczb losowych
	std::uniform_real_distribution<double> RDistribution(0.0, 1.0);

	// 2.4
	const double Phi = Fi1 + Fi2;
	const double Chi = 1.0 / (2 * abs(2 - Phi - sqrt(pow(Phi, 2) - 4.0 * Phi)));

	for(Graph::VertexIndex_t VertexIndex = 0; VertexIndex < PG.GetTask().GetGraph().GetSize(); VertexIndex++)
	{
		// 2.3
		// Zaktualizuj trajektorię na podstawie pozycji swojej i najlepszego z otoczenia
		const double R1 = RDistribution(RandomGenerator);
		const double R2 = RDistribution(RandomGenerator);
		Velocities[VertexIndex] = Chi * (Velocities[VertexIndex] + Fi1 * R1 * PriorietiesBestLocal[VertexIndex] - Priorities[VertexIndex]) + Fi2 * R2 * (PriorietiesBestGlobal[VertexIndex] - Priorities[VertexIndex]);

		// 2.2
		// Zaktualizuj położenie na podstawie trajektorii
		Priorities[VertexIndex] += Velocities[VertexIndex];
	}
}
