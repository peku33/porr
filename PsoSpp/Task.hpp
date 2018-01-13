#pragma once

#include <mpi.h>

#include <optional>
#include <list>

#include "Graph.hpp"

/*
	Klasa reprezentująca zadanie optymalizacji
	Wrapper na graf + indeksy węzła początku i końca
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

	/*
		MPI
	*/
	private:
		static const int MpiTagBase = 200;
		static const int MpiTagVertexIndexes = MpiTagBase + 0;

	public:
		void MpiSend(const int & Destination, const MPI_Comm & MpiComm) const;
		static Task MpiReceive(const Graph & G, const int & Source, const MPI_Comm & MpiComm);
};

