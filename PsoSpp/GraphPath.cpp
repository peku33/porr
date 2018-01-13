#include "GraphPath.hpp"

#define DEBUG_THIS

#ifdef DEBUG_THIS
	#include <iostream>
#endif

GraphPath::GraphPath(const Task & T, Priorities_t && Priorities, VertexIndexes_t && VertexIndexesCache, const PathWeight_t & PathWeightCache):
	T(T),
	Priorities(std::move(Priorities)),
	VertexIndexesCache(std::move(VertexIndexesCache)),
	PathWeightCache(PathWeightCache)
{
}

GraphPath::GraphPath(const Task & T, Priorities_t && Priorities, const PathWeight_t & PathWeightCache):
	T(T),
	Priorities(std::move(Priorities)),
	PathWeightCache(PathWeightCache)
{
}

GraphPath::GraphPath(const GraphPath & Other):
	T(Other.T),
	Priorities(new Priority_t[Other.T.GetGraph().GetSize()]),
	VertexIndexesCache(Other.VertexIndexesCache),
	PathWeightCache(Other.PathWeightCache)
{
	std::copy(Other.Priorities.get(), Other.Priorities.get() + Other.T.GetGraph().GetSize(), Priorities.get());
}

const GraphPath::Priorities_t & GraphPath::GetPriorities() const
{
	return Priorities;
}

const GraphPath::VertexIndexes_t & GraphPath::GetVertexIndexes() const
{
	if(!VertexIndexesCache.has_value())
	{
		std::optional<GraphPath::VertexIndexes_t> VertexIndexes = GraphPath::BuildVertexIndexes(T, Priorities);
		if(!VertexIndexes.has_value())
			throw std::runtime_error("!BuildVertexIndexes for GraphPath");

		VertexIndexesCache.emplace(std::move(VertexIndexes.value()));
	}
	return VertexIndexesCache.value();
}

const GraphPath::PathWeight_t & GraphPath::GetPathWeight() const
{
	if(!PathWeightCache.has_value())
		PathWeightCache.emplace(CalculatePathWeight(T.GetGraph(), GetVertexIndexes()));
	return PathWeightCache.value();
}

bool GraphPath::IsBetterThan(const GraphPath & Other) const
{
	return GetPathWeight() < Other.GetPathWeight();
}

void GraphPath::DumpPath(std::ostream & Stream) const
{
	// Get wartości
	const VertexIndexes_t & VertexIndexes = GetVertexIndexes();
	const PathWeight_t & PathWeight = GetPathWeight();

	// Łączna waga
	Stream << "(" << PathWeight << "): ";

	// Lista wierzchołków i poszczególnych wag
	VertexIndexes_t::const_iterator First = VertexIndexes.cbegin();
	while(true)
	{
		VertexIndexes_t::const_iterator Second = std::next(First);

		if(Second == VertexIndexes.cend())
			break;

		Stream << *First << " -" << (int) T.GetGraph().GetEdgeWeight(*First, *Second) << "-> " << *Second << " ";

		First = Second;
	}
}

GraphPath::PathWeight_t GraphPath::CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes)
{
	// 0 - 1 wierzchołków - ścieżka pusta
	if(VertexIndexes.size() <= 1)
		return 0;

	// Akumulator
	PathWeight_t PathWeight = 0;

	VertexIndexes_t::const_iterator First = VertexIndexes.cbegin();
	while(true)
	{
		VertexIndexes_t::const_iterator Second = std::next(First);

		if(Second == VertexIndexes.cend())
			break;

		PathWeight += G.GetEdgeWeight(*First, *Second);

		First = Second;
	}

	return PathWeight;
}

std::optional<GraphPath::VertexIndexes_t> GraphPath::BuildVertexIndexes(const Task & T, const Priorities_t & Priorities)
{
	// Lista odwiedzonych węzłów, aby nie tworzyć cykli
	std::unique_ptr<bool[]> VisitedNodes(new bool[T.GetGraph().GetSize()]);
	std::fill(VisitedNodes.get(), VisitedNodes.get() + T.GetGraph().GetSize(), false);

	// Rozwiązanie częściowe - bufor na wierzchołki częściowo zbudowanej ścieżki
	VertexIndexes_t VertexIndexes;

	// Zaczynamy od wierzchołka startowego
	Graph::VertexIndex_t CurrentVertexIndex = T.GetVertexIndexStart();

	while(true)
	{
		// Dodaj obecny wierzchołek do historii
		VertexIndexes.push_back(CurrentVertexIndex);

		// Jeśli zadanie zostało spełnione - kończymy
		if(CurrentVertexIndex == T.GetVertexIndexEnd())
			return VertexIndexes;

		// Odznaczamy obecny wierzchołek jako odwiedzony
		VisitedNodes[CurrentVertexIndex] = true;

		// Wybierz następny wierzchołek do przejścia
		// Sprawdź wszystkie wierzchołki, minmalizcując Weight * Priority
		bool AnyFound = false;
		Graph::VertexIndex_t BestNextVertexIndex;
		double BestWeight;

		for(Graph::VertexIndex_t NextVertexIndex = 0; NextVertexIndex < T.GetGraph().GetSize(); NextVertexIndex++)
		{
			// Pomijamy odwiedzone węzły
			if(VisitedNodes[NextVertexIndex])
				continue;

			// Pobieramy wartość krawędzi, sprawdzamy czy w ogóle istnieje
			const Graph::EdgeWeight_t EdgeWeight = T.GetGraph().GetEdgeWeight(CurrentVertexIndex, NextVertexIndex);
			if(EdgeWeight == std::numeric_limits<Graph::EdgeWeight_t>::max())
				continue;

			// Węzeł nie został odwiedzony, krawędź istnieje - obliczamy priotytet
			double Weight = Priorities[NextVertexIndex] * EdgeWeight;

			// Sprawdzamy czy to lepsze rozwiązanie
			if(!AnyFound || Weight < BestWeight)
			{
				AnyFound = true;
				BestNextVertexIndex = NextVertexIndex;
				BestWeight = Weight;
			}
		}

		// Nie ma dokąd pójść
		if(!AnyFound)
			return std::nullopt;

		// Idziemy do najlepszego wierzchołka
		CurrentVertexIndex = BestNextVertexIndex;
	}
}

void GraphPath::MpiBroadcastSend(const int & MpiRankMy, const int & MpiRankTotal, const MPI_Comm & MpiComm) const
{
	for(int MpiRank = 0; MpiRank < MpiRankTotal; MpiRank++)
	{
		// Nie wysyłamy do siebie
		if(MpiRank == MpiRankMy)
			continue;

		MPI_Request PrioritiesRequest;
		if(MPI_Isend(Priorities.get(), T.GetGraph().GetSize(), MPI_DOUBLE, MpiRank, MpiTagPriorities, MpiComm, &PrioritiesRequest) != MPI_SUCCESS)
			throw std::runtime_error("MPI_Isend(Priorities.get(), T.GetGraph().GetSize(), MPI_DOUBLE, MpiRank, MpiTagPriorities, MpiComm, &PrioritiesRequest) != MPI_SUCCESS");

		if(MPI_Request_free(&PrioritiesRequest) != MPI_SUCCESS)
			throw std::runtime_error("MPI_Request_free(&PrioritiesRequest) != MPI_SUCCESS");
		
		MPI_Request PathWeightCacheRequest;
		if(MPI_Isend(&PathWeightCache, 1, MPI_UNSIGNED_LONG_LONG, MpiRank, MpiTagPathWeightCache, MpiComm, &PathWeightCacheRequest) != MPI_SUCCESS)
			throw std::runtime_error("MPI_Isend(&PathWeightCache, 1, MPI_UNSIGNED_LONG_LONG, MpiRank, MpiTagPathWeightCache, MpiComm, &PathWeightCacheRequest) != MPI_SUCCESS");

		if(MPI_Request_free(&PathWeightCacheRequest) != MPI_SUCCESS)
			throw std::runtime_error("MPI_Request_free(&PathWeightCacheRequest) != MPI_SUCCESS");
	}
}
std::optional<GraphPath> GraphPath::MpiTryBroadcastReceive(const Task & T, const MPI_Comm & MpiComm)
{
	int ReceivedAny = 0;
	MPI_Status MpiProbeStatus;
	if(MPI_Iprobe(MPI_ANY_SOURCE, MpiTagPriorities, MpiComm, &ReceivedAny, &MpiProbeStatus) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Iprobe(MPI_ANY_SOURCE, MpiTagPriorities, MpiComm, &ReceivedAny, &MpiProbeStatus) != MPI_SUCCESS");

	if(!ReceivedAny)
		return std::nullopt;

	Priorities_t Priorities(new Priority_t[T.GetGraph().GetSize()]);
	if(MPI_Recv(Priorities.get(), T.GetGraph().GetSize(), MPI_DOUBLE, MpiProbeStatus.MPI_SOURCE, MpiTagPriorities, MpiComm, NULL) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Recv(Priorities.get(), T.GetGraph().GetSize(), MPI_DOUBLE, MpiProbeStatus.MPI_SOURCE, MpiTagPriorities, MpiComm, NULL) != MPI_SUCCESS");

	PathWeight_t PathWeightCache;
	if(MPI_Recv(&PathWeightCache, 1, MPI_UNSIGNED_LONG_LONG, MpiProbeStatus.MPI_SOURCE, MpiTagPathWeightCache, MpiComm, NULL) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Recv(&PathWeightCache, 1, MPI_UNSIGNED_LONG_LONG, MpiProbeStatus.MPI_SOURCE, MpiTagPathWeightCache, MpiComm, NULL) != MPI_SUCCESS");

	return GraphPath(
		T,
		std::move(Priorities),
		PathWeightCache
	);
}