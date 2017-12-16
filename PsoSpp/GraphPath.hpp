#pragma once

#include <list>
#include <functional>
#include <ostream>

#include "Graph.hpp"

/*
	Klasa reprezentuj¹ca œcie¿kê w grafie
*/
class GraphPath
{
	public:
		// Lista wierzcho³ków
		typedef std::list<Graph::VertexIndex_t> VertexIndexes_t;

		// Waga œcie¿ki jako sumy wag krawêdzi
		typedef unsigned long PathWeight_t;

	public:
		/*
			Konstruktor.

			Poniewa¿ lista powstaje zazwyczaj na bazie obiektów tymczasowych - kontruktor przenosz¹cy listê

			G - Graf
			VertexIndexes - lista wierzcho³ków
		*/
		GraphPath(const Graph & G, VertexIndexes_t && VertexIndexes);

	private:
		/*
			Chcemy umo¿liwiæ kopiowanie i przenoszenie, wiêc trzymamy refrence_wrapper
		*/
		std::reference_wrapper<const Graph> GWrapper;

		/*
			Lista wierzcho³ków
		*/
		VertexIndexes_t VertexIndexes;

	private:
		/*
			Obliczona w kontruktorze waga œcie¿ki
		*/
		PathWeight_t PathWeight;

	public:
		const VertexIndexes_t & GetVertexIndexes() const;
		const PathWeight_t & GetPathWeight() const;

	public:
		/*
			Sprawdza czy podana œcie¿ka jest lepsza od drugiej
		*/
		bool IsBetterThan(const GraphPath & Other) const;

	public:
		/*
			Drukuje graficzn¹ reprezentacjê do strumienia
		*/
		void DumpPath(std::ostream & Stream) const;

	public:
		/*
			Metoda pomocnicza obliczaj¹ca d³ugoœæ œcie¿ki dla podanego grafu i listy
		*/
		static PathWeight_t CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes);
};

