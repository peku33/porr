#include "Task.hpp"

Task::Task(const Graph & G, const Graph::VertexIndex_t & VertexIndexStart, const Graph::VertexIndex_t & VertexIndexEnd):
	G(G), VertexIndexStart(VertexIndexStart), VertexIndexEnd(VertexIndexEnd)
{
}

const Graph & Task::GetGraph() const
{
	return G;
}

const Graph::VertexIndex_t & Task::GetVertexIndexStart() const
{
	return VertexIndexStart;
}

const Graph::VertexIndex_t & Task::GetVertexIndexEnd() const
{
	return VertexIndexEnd;
}
