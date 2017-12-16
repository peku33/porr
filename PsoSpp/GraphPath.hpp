#pragma once

#include <list>
#include <functional>
#include <ostream>

#include "Graph.hpp"

/*
	Klasa reprezentuj�ca �cie�k� w grafie
*/
class GraphPath
{
	public:
		// Lista wierzcho�k�w
		typedef std::list<Graph::VertexIndex_t> VertexIndexes_t;

		// Waga �cie�ki jako sumy wag kraw�dzi
		typedef unsigned long PathWeight_t;

	public:
		/*
			Konstruktor.

			Poniewa� lista powstaje zazwyczaj na bazie obiekt�w tymczasowych - kontruktor przenosz�cy list�

			G - Graf
			VertexIndexes - lista wierzcho�k�w
		*/
		GraphPath(const Graph & G, VertexIndexes_t && VertexIndexes);

	private:
		/*
			Chcemy umo�liwi� kopiowanie i przenoszenie, wi�c trzymamy refrence_wrapper
		*/
		std::reference_wrapper<const Graph> GWrapper;

		/*
			Lista wierzcho�k�w
		*/
		VertexIndexes_t VertexIndexes;

	private:
		/*
			Obliczona w kontruktorze waga �cie�ki
		*/
		PathWeight_t PathWeight;

	public:
		const VertexIndexes_t & GetVertexIndexes() const;
		const PathWeight_t & GetPathWeight() const;

	public:
		/*
			Sprawdza czy podana �cie�ka jest lepsza od drugiej
		*/
		bool IsBetterThan(const GraphPath & Other) const;

	public:
		/*
			Drukuje graficzn� reprezentacj� do strumienia
		*/
		void DumpPath(std::ostream & Stream) const;

	public:
		/*
			Metoda pomocnicza obliczaj�ca d�ugo�� �cie�ki dla podanego grafu i listy
		*/
		static PathWeight_t CalculatePathWeight(const Graph & G, const VertexIndexes_t & VertexIndexes);
};

