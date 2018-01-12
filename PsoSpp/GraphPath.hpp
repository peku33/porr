#pragma once

#include <list>
#include <functional>
#include <ostream>

#include "Graph.hpp"

/*
	Klasa reprezentująca ścieżkę w grafie
*/
class GraphPath
{
	public:
		// Lista wierzchołków
		typedef std::list<Graph::VertexIndex_t> VertexIndexes_t;
		
		// Waga ścieżki jako sumy wag krawędzi
		typedef unsigned long PathWeight_t;

	public:
		/*
			Konstruktor.

			Ponieważ lista powstaje zazwyczaj na bazie obiektów tymczasowych - kontruktor przenoszący listę

			G - Graf
			VertexIndexes - lista wierzchołków
		*/
		GraphPath(const Graph & G, VertexIndexes_t && VertexIndexes);

	private:
		/*
			Chcemy umożliwić kopiowanie i przenoszenie, więc trzymamy refrence_wrapper
		*/
		std::reference_wrapper<const Graph> GWrapper;

		/*
			Lista wierzchołków
		*/
		VertexIndexes_t VertexIndexes;

	private:
		/*
			Obliczona w kontruktorze waga ścieżki
		*/
		PathWeight_t PathWeight;

	public:
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

	public:
		/*
			Metoda pomocnicza obliczająca długość ścieżki dla podanego grafu i listy
		*/
		static PathWeight_t CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes);
};

