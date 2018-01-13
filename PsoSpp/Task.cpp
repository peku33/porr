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

void Task::MpiSend(const int & Destination, const MPI_Comm & MpiComm) const
{
	uint64_t VertexIndexes[2] = { VertexIndexStart, VertexIndexEnd };
	if(MPI_Send(VertexIndexes, 2, MPI_UNSIGNED_LONG_LONG, Destination, MpiTagVertexIndexes, MpiComm) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Send(VertexIndexes, 2, MPI_UNSIGNED_LONG_LONG, Destination, MpiTagVertexIndexes, MpiComm) != MPI_SUCCESS");
}

Task Task::MpiReceive(const Graph & G, const int & Source, const MPI_Comm & MpiComm)
{
	uint64_t VertexIndexes[2];
	if(MPI_Recv(VertexIndexes, 2, MPI_UNSIGNED_LONG_LONG, Source, MpiTagVertexIndexes, MpiComm, NULL) != MPI_SUCCESS)
		throw std::runtime_error("MPI_Recv(VertexIndexes, 2, MPI_UNSIGNED_LONG_LONG, Source, MpiTagVertexIndexes, MpiComm, NULL) != MPI_SUCCESS");

	return Task(G, VertexIndexes[0], VertexIndexes[1]);
}
