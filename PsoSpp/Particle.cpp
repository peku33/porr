#include "Particle.hpp"

#include <algorithm>

#include "ParticleGroup.hpp"
#include "Task.hpp"
#include "Graph.hpp"
#include "GraphPath.hpp"

Particle::Particle(const ParticleGroup & PG, std::mt19937 & RandomGenerator):
	PG(PG),
	RandomGenerator(RandomGenerator),
	Priorities(new Priority_t[PG.GetTask().GetGraph().GetSize()]),
	Velocities(new Velocity_t[PG.GetTask().GetGraph().GetSize()]),
	BestPriorities(new Priority_t[PG.GetTask().GetGraph().GetSize()])
{
	// Inicjalizacja losowego stanu
	RandomInitialize();
}

Particle::Particle(const Particle & Other):
	PG(Other.PG),
	RandomGenerator(Other.RandomGenerator),
	Priorities(new Priority_t[PG.GetTask().GetGraph().GetSize()]),
	Velocities(new Velocity_t[PG.GetTask().GetGraph().GetSize()]),
	BestPriorities(new Priority_t[PG.GetTask().GetGraph().GetSize()]),
	BestGraphPath(Other.BestGraphPath)
{
	std::copy(Other.Priorities.get(), Other.Priorities.get() + PG.GetTask().GetGraph().GetSize(), Priorities.get());
	std::copy(Other.Velocities.get(), Other.Velocities.get() + PG.GetTask().GetGraph().GetSize(), Velocities.get());
	std::copy(Other.BestPriorities.get(), Other.BestPriorities.get() + PG.GetTask().GetGraph().GetSize(), BestPriorities.get());
}



const std::optional<const GraphPath>& Particle::GetBestGraphPath() const
{
	return BestGraphPath;
}

void Particle::RandomInitialize()
{
	std::uniform_real_distribution<double> PriorityDistribution(0.0, 1.0);
	std::uniform_real_distribution<double> VelocityDistribution(0.0, 1.0);

	// Losowe Priorities, Velocities, [0.0; 1.0]
	std::generate(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSize(), [&] { return PriorityDistribution(RandomGenerator); });
	std::generate(Velocities.get(), Velocities.get() + PG.GetTask().GetGraph().GetSize(), [&] { return VelocityDistribution(RandomGenerator); });

	// Je�li w�ze� nie niesie za sob� �adnego najlepszego rozwi�zania - r�wnie� inicjujemy w�ze� najlepszego rozwi�zania jako w�ze� lokalnego rozwi�zania
	if(!BestGraphPath)
	{
		std::copy(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSize(), BestPriorities.get());
	}
}

bool Particle::Run()
{
	// Lista odwiedzonych w�z��w, aby nie tworzy� cykli
	std::unique_ptr<bool[]> VisitedNodes(new bool[PG.GetTask().GetGraph().GetSize()]);
	std::fill(VisitedNodes.get(), VisitedNodes.get() + PG.GetTask().GetGraph().GetSize(), false);

	// Rozwi�zanie cz�ciowe - bufor na wierzcho�ki cz�ciowo zbudowanej �cie�ki
	GraphPath::VertexIndexes_t PartialSolution;

	// Zaczynamy od wierzcho�ka startowego
	Graph::VertexIndex_t CurrentVertexIndex = PG.GetTask().GetVertexIndexStart();

	while(true)
	{
		// Dodaj obecny wierzcho�ek do historii
		PartialSolution.push_back(CurrentVertexIndex);

		// Je�li zadanie zosta�o spe�nione - ko�czymy
		if(CurrentVertexIndex == PG.GetTask().GetVertexIndexEnd())
		{
			// Czy �cie�ka uleg�a poprawie?
			GraphPath GP(
				PG.GetTask().GetGraph(),
				std::move(PartialSolution)
			);

			// Obecna �cie�ka istnieje, a nowa nie jest lepsza, kryterium nie uleg�o poprawie
			if(BestGraphPath && !GP.IsBetterThan(BestGraphPath.value()))
				return false;

			// Zapami�taj obecny stan jako najlepszy
			std::copy(Priorities.get(), Priorities.get() + PG.GetTask().GetGraph().GetSize(), BestPriorities.get());

			// Zapisujemy �cie�k�
			BestGraphPath.emplace(
				std::move(GP)
			);

			// �cie�ka uleg�a poprawie
			return true;
		}

		// Odznaczamy obecny wierzcho�ek jako odwiedzony
		VisitedNodes[CurrentVertexIndex] = true;

		// Wybierz nast�pny wierzcho�ek do przej�cia
		// Sprawd� wszystkie wierzcho�ki, minmalizcuj�c Weight * Priority
		bool AnyFound = false;
		Graph::VertexIndex_t BestNextVertexIndex;
		double BestWeight;

		for(Graph::VertexIndex_t NextVertexIndex = 0; NextVertexIndex < PG.GetTask().GetGraph().GetSize(); NextVertexIndex++)
		{
			// Pomijamy odwiedzone w�z�y
			if(VisitedNodes[NextVertexIndex])
				continue;

			// Pobieramy warto�� kraw�dzi, sprawdzamy czy w og�le istnieje
			const Graph::EdgeWeight_t EdgeWeight = PG.GetTask().GetGraph().GetEdgeWeight(CurrentVertexIndex, NextVertexIndex);
			if(EdgeWeight == std::numeric_limits<Graph::EdgeWeight_t>::max())
				continue;

			// W�ze� nie zosta� odwiedzony, kraw�d� istnieje - obliczamy priotytet
			double Weight = Priorities[NextVertexIndex] * EdgeWeight;

			// Sprawdzamy czy to lepsze rozwi�zanie
			if(!AnyFound || Weight < BestWeight)
			{
				AnyFound = true;
				BestNextVertexIndex = NextVertexIndex;
				BestWeight = Weight;
			}
		}

		// Nie ma dok�d p�j��
		if(!AnyFound)
			return false;

		// Idziemy do najlepszego wierzcho�ka
		CurrentVertexIndex = BestNextVertexIndex;
	}
}

void Particle::Update(const double & Fi1, const double & Fi2, const Particle & ParticleBest)
{
	std::uniform_real_distribution<double> RDistribution(0.0, 1.0);

	// 2.4
	const double Phi = Fi1 + Fi2;
	const double Chi = 1.0 / (2 * abs(2 - Phi - sqrt(pow(Phi, 2) - 4.0 * Phi)));

	for(Graph::VertexIndex_t VertexIndex = 0; VertexIndex < PG.GetTask().GetGraph().GetSize(); VertexIndex++)
	{
		// 2.3
		// Zaktualizuj trajektori� na podstawie pozycji swojej i najlepszego z otoczenia
		const double R1 = RDistribution(RandomGenerator);
		const double R2 = RDistribution(RandomGenerator);
		Velocities[VertexIndex] = Chi * (Velocities[VertexIndex] + Fi1 * R1 * (BestPriorities[VertexIndex] - Priorities[VertexIndex]) + Fi2 * R2 * (ParticleBest.BestPriorities[VertexIndex] - Priorities[VertexIndex]));

		// 2.2
		// Zaktualizuj po�o�enie na podstawie trajektorii
		Priorities[VertexIndex] += Velocities[VertexIndex];
	}
}
