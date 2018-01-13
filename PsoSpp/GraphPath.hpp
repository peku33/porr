#pragma once

#include <list>
#include <optional>
#include <ostream>

#include <mpi.h>

#include "Graph.hpp"
#include "Task.hpp"

/*
	Klasa reprezentująca ścieżkę w grafie
*/
class GraphPath
{
	public:
		// Priorytet
		typedef double Priority_t;

		// Lista priorytetów
		typedef std::unique_ptr<Priority_t[]> Priorities_t; /* G.GetSize() * G.GetSize() */

		// Lista wierzchołków
		typedef std::list<Graph::VertexIndex_t> VertexIndexes_t;

		// Waga ścieżki jako sumy wag krawędzi
		typedef uint64_t PathWeight_t;

	public:
		GraphPath(const Task & T, Priorities_t && Priorities, VertexIndexes_t && VertexIndexesCache, const PathWeight_t & PathWeightCache);
		GraphPath(const Task & T, Priorities_t && Priorities, const PathWeight_t & PathWeightCache);
		GraphPath(const GraphPath & Other);

	// Składowe bezpośrednie
	private:
		// Graf
		const Task & T;

		// Lista priorytetów budujących tą ścieżkę
		Priorities_t Priorities;

	// Składowe pochodne nieinicjalizowane od razu
	private:
		// Lista wierzchołków
		mutable std::optional<const VertexIndexes_t> VertexIndexesCache;
		
		// Wyaga ścieżki
		mutable std::optional<const PathWeight_t> PathWeightCache;

	public:
		const Priorities_t & GetPriorities() const;

		const VertexIndexes_t & GetVertexIndexes() const;
		const PathWeight_t & GetPathWeight() const;

	public:
		/*
			Sprawdza czy podana ścieżka jest lepsza od drugiej
		*/
		bool IsBetterThan(const GraphPath & Other) const;

	public:
		/*
			Drukuje graficzną reprezentację do strumienia
		*/
		void DumpPath(std::ostream & Stream) const;

	// Pomocnicze
	public:
		/*
			Metoda pomocnicza obliczająca długość ścieżki dla podanego grafu i listy
		*/
		static PathWeight_t CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes);

		/*
			Na podstawie podanych priorytetów próbuje zbudować ścieżkę w grafie.

			W przypadku powodzenia - zwraca listę wierzchołków
			W przypadku braku ścieżki - std::nulloptr
		*/
		static std::optional<VertexIndexes_t> BuildVertexIndexes(const Task & T, const Priorities_t & Priorities);

	/*
		MPI
	*/
	private:
		static const int MpiTagBase = 300;
		static const int MpiTagPriorities = MpiTagBase + 0;
		static const int MpiTagPathWeightCache = MpiTagBase + 1;

	public:
		void MpiBroadcastSend(const int & MpiRankMy, const int & MpiRankTotal, const MPI_Comm & MpiComm) const;
		static std::optional<GraphPath> MpiTryBroadcastReceive(const Task & T, const MPI_Comm & MpiComm);
};

