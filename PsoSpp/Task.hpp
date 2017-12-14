#pragma once

#include <optional>
#include <list>

#include "Graph.hpp"

/*
	Klasa reprezentuj�ca zadanie optymalizacji
	Wrapper na graf + indeksy w�z�a pocz�tku i ko�ca
*/
class Task
{
	public:
		Task(const Graph & G, const Graph::VertexIndex_t & VertexIndexStart, const Graph::VertexIndex_t & VertexIndexEnd);

	private:
		const Graph & G;
		const Graph::VertexIndex_t VertexIndexStart;
		const Graph::VertexIndex_t VertexIndexEnd;

	public:
		const Graph & GetGraph() const;
		const Graph::VertexIndex_t & GetVertexIndexStart() const;
		const Graph::VertexIndex_t & GetVertexIndexEnd() const;
};

