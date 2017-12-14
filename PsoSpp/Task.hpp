#pragma once

#include <optional>
#include <list>

#include "Graph.hpp"

/*
	Klasa reprezentuj¹ca zadanie optymalizacji
	Wrapper na graf + indeksy wêz³a pocz¹tku i koñca
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

