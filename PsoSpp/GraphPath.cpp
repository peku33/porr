#include "GraphPath.hpp"

GraphPath::GraphPath(const Graph & G, VertexIndexes_t && VertexIndexes):
	GWrapper(G),
	VertexIndexes(std::move(VertexIndexes)),
	PathWeight(CalculatePathWeight(this->GWrapper.get(), this->VertexIndexes))
{

}

const GraphPath::VertexIndexes_t & GraphPath::GetVertexIndexes() const
{
	return VertexIndexes;
}
const GraphPath::PathWeight_t & GraphPath::GetPathWeight() const
{
	return PathWeight;
}

bool GraphPath::IsBetterThan(const GraphPath & Other) const
{
	return PathWeight < Other.PathWeight;
}

void GraphPath::DumpPath(std::ostream & Stream) const
{
	// Łączna waga
	Stream << "(" << PathWeight << "): ";

	// Lista wierzchołków i poszczególnych wag
	VertexIndexes_t::const_iterator First = VertexIndexes.cbegin();
	while(true)
	{
		VertexIndexes_t::const_iterator Second = std::next(First);

		if(Second == VertexIndexes.cend())
			break;

		Stream << *First << " -" << (int) GWrapper.get().GetEdgeWeight(*First, *Second) << "-> " << *Second << " ";

		First = Second;
	}
}

GraphPath::PathWeight_t GraphPath::CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes)
{
	// Waga zerowa, jeśli ścieżka pusta
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