#include "Particle.hpp"

#include "ParticleGroup.hpp"
#include "Task.hpp"
#include "Graph.hpp"
#include "GraphPath.hpp"

#include <algorithm>

Particle::Particle(const ParticleGroup & PG):
	PG(PG),
	Priorities(new Priority_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]),
	Velocities(new Velocity_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]),
	BestPriorities(new Priority_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()])
{
	// Inicjalizacja losowego stanu
	RandomInitialize();
}

Particle::Particle(const Particle & Other):
	PG(Other.PG),
	Priorities(new Priority_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]),
	Velocities(new Velocity_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]),
	BestPriorities(new Priority_t[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]),
	BestGraphPath(Other.BestGraphPath)
{
	std::copy(Other.Priorities.get(), Other.Priorities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), Priorities.get());
	std::copy(Other.Velocities.get(), Other.Velocities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), Velocities.get());
	std::copy(Other.BestPriorities.get(), Other.BestPriorities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), BestPriorities.get());
}



const std::optional<const GraphPath>& Particle::GetBestGraphPath() const
{
	return BestGraphPath;
}

void Particle::RandomInitialize()
{
	// Losowe Priorities, Velocities, [0.0; 1.0]
	std::generate(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), [] { return 1.0 * rand() / RAND_MAX; });
	std::generate(Velocities.get(), Velocities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), [] { return 1.0 * rand() / RAND_MAX; });

	// Jeœli wêze³ nie niesie za sob¹ ¿adnego najlepszego rozwi¹zania - równie¿ inicjujemy wêze³ najlepszego rozwi¹zania jako wêze³ lokalnego rozwi¹zania
	if(!BestGraphPath)
	{
		std::copy(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), BestPriorities.get());
	}
}

bool Particle::Run()
{
	// Lista odwiedzonych wêz³ów, aby nie tworzyæ cykli
	std::unique_ptr<bool[]> VisitedNodes(new bool[PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize()]);
	std::fill(VisitedNodes.get(), VisitedNodes.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), false);

	// Rozwi¹zanie czêœciowe - bufor na wierzcho³ki czêœciowo zbudowanej œcie¿ki
	GraphPath::VertexIndexes_t PartialSolution;

	// Zaczynamy od wierzcho³ka startowego
	Graph::VertexIndex_t CurrentVertexIndex = PG.GetTask().GetVertexIndexStart();

	while(true)
	{
		// Dodaj obecny wierzcho³ek do historii
		PartialSolution.push_back(CurrentVertexIndex);

		// Jeœli zadanie zosta³o spe³nione - koñczymy
		if(CurrentVertexIndex == PG.GetTask().GetVertexIndexEnd())
		{
			// Czy œcie¿ka uleg³a poprawie?
			GraphPath GP(
				PG.GetTask().GetGraph(),
				std::move(PartialSolution)
			);

			// Obecna œcie¿ka istnieje, kryterium nie uleg³o poprawie
			if(BestGraphPath && BestGraphPath.value().GetPathWeight() <= GP.GetPathWeight())
				return false;

			// Zapamiêtaj obecny stan jako najlepszy
			std::copy(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(), BestPriorities.get());

			// Zapisujemy œcie¿kê
			BestGraphPath.emplace(
				std::move(GP)
			);

			// Œcie¿ka uleg³a poprawie
			return true;
		}

		// Odznaczamy obecny wierzcho³ek jako odwiedzony
		VisitedNodes[CurrentVertexIndex] = true;

		// Wybierz nastêpny wierzcho³ek do przejœcia
		// SprawdŸ wszystkie wierzcho³ki, minmalizcuj¹c Weight * Priority
		bool AnyFound = false;
		Graph::VertexIndex_t BestNextVertexIndex;
		double BestWeight;

		for(Graph::VertexIndex_t NextVertexIndex = 0; NextVertexIndex < PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(); NextVertexIndex++)
		{
			// Pomijamy odwiedzone wêz³y
			if(VisitedNodes[NextVertexIndex])
				continue;

			// Pobieramy wartoœæ krawêdzi, sprawdzamy czy w ogóle istnieje
			const Graph::EdgeWeight_t EdgeWeight = PG.GetTask().GetGraph().GetEdgeWeight(CurrentVertexIndex, NextVertexIndex);
			if(EdgeWeight == std::numeric_limits<Graph::EdgeWeight_t>::max())
				continue;

			// Wêze³ nie zosta³ odwiedzony, krawêdŸ istnieje - obliczamy priotytet
			double Weight = Priorities[NextVertexIndex] * EdgeWeight;

			// Sprawdzamy czy to lepsze rozwi¹zanie
			if(!AnyFound || Weight < BestWeight)
			{
				AnyFound = true;
				BestNextVertexIndex = NextVertexIndex;
				BestWeight = Weight;
			}
		}

		// Nie ma dok¹d pójœæ
		if(!AnyFound)
			return false;

		// Idziemy do najlepszego wierzcho³ka
		CurrentVertexIndex = BestNextVertexIndex;
	}
}

void Particle::Update(const double & Fi1, const double & Fi2, const Particle & ParticleBest)
{
	// 2.4
	const double Phi = Fi1 + Fi2;
	const double Chi = 1.0 / (2 * abs(2 - Phi - sqrt(pow(Phi, 2) - 4.0 * Phi)));

	for(Graph::VertexIndex_t VertexIndex = 0; VertexIndex < PG.GetTask().GetGraph().GetSideSize() * PG.GetTask().GetGraph().GetSideSize(); VertexIndex++)
	{
		// 2.3
		// Zaktualizuj trajektoriê na podstawie pozycji swojej i najlepszego z otoczenia
		const double R1 = 1.0 * rand() / RAND_MAX;
		const double R2 = 1.0 * rand() / RAND_MAX;
		Velocities[VertexIndex] = Chi * (Velocities[VertexIndex] + Fi1 * R1 * (BestPriorities[VertexIndex] - Priorities[VertexIndex]) + Fi2 * R2 * (ParticleBest.BestPriorities[VertexIndex] - Priorities[VertexIndex]));

		// 2.2
		// Zaktualizuj po³o¿enie na podstawie trajektorii
		Priorities[VertexIndex] += Velocities[VertexIndex];
	}
}
