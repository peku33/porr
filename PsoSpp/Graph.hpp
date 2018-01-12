#pragma once

#include <memory>
#include <ostream>

/*
	Klasa reprezentuje `kwadratowy` nieskierowany graf o rozmiarze SideSize * SideSize
*/
class Graph
{
	public:
		// Id wierzchołka
		typedef unsigned int VertexId_t;

		// Indeks wierzchołka w wektorze / tablicy
		typedef size_t VertexIndex_t;

		// Waga krawędzi
		typedef uint8_t EdgeWeight_t;

		// Macierz sąsiedztwa zawierająca wagi poszczególnych krawędzi
		// Ponieważ celem zadania jest minimalizacja sumarycznej wagi ścieżki, brak krawędzi reprezentowany jest przez std::numeric_limits<EdgeWeight_t>::max()
		typedef std::unique_ptr<Graph::EdgeWeight_t[]> AdjascencyMatrix_t; /* (SideSize * SideSize) * (SideSize * SideSize) */

	public:
		/*
			SideSize - długość boku (graf jest kwadratowy)
			AdjascencyMatrix - macierz SideSize ^ 4 z wagami boków
		*/
		Graph(const VertexId_t & SideSize, AdjascencyMatrix_t && AdjascencyMatrix);

	private:
		VertexId_t SideSize;
		AdjascencyMatrix_t AdjascencyMatrix;

	public:
		/*
			Rozmiar boku grafu (w sensie geometrycznym)
		*/
		const VertexId_t & GetSideSize() const;

		/*
			Rozmiar grafu (w sensie liczby wierzchołków)
		*/
		const VertexIndex_t GetSize() const;

	public:
		const EdgeWeight_t GetEdgeWeight(const VertexIndex_t & Vertex1Index, const VertexIndex_t & Vertex2Index) const;

	public:
		/*
			Generuje postać grafu którą można zwizualizowć.
			https://dreampuf.github.io/GraphvizOnline/
			należy wybrać Engine: fdp
		*/
		void GraphViz(std::ostream & Stream) const;

		/*
			Rysuje graficzną reprezentację macierzy sąsiedztwa
		*/
		void DumpAdjascencyMatrix(std::ostream & Stream) const;

	public:
		/*
			Generowanie grafu metodą Waxman'a.

			SideSize - długość boku kwadratowego grafu
			Parametry Alpha i Beta w zakresie 0.0 - 1.0 sterują prawdopodobieństwem pojawienia się krawędzi pomiędzy dwoma punktami.
			EdgeWeightMin, EdgeWeightMax - minimalna i maksymalna waga krawędzi
		*/
		static Graph GenerateWaxmanRandom(const VertexId_t & SideSize, const double & Alpha, const double & Beta, const EdgeWeight_t & EdgeWeightMin, const EdgeWeight_t & EdgeWeightMax);
};

