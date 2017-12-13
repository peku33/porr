#pragma once

#include <memory>
#include <ostream>

/*
	Klasa reprezentuje `kwadratowy` nieskierowany graf o rozmiarze SideSize * SideSize
*/
class Graph
{
	public:
		// Id wierzcho³ka
		typedef unsigned int VertexId_t;

		// Waga krawêdzi
		typedef uint8_t EdgeWeight_t;

		// Macierz s¹siedztwa zawieraj¹ca wagi poszczególnych krawêdzi
		// Poniewa¿ celem zadania jest minimalizacja sumarycznej wagi œcie¿ki, brak krawêdzi reprezentowany jest przez std::numeric_limits<EdgeWeight_t>::max()
		typedef std::unique_ptr<Graph::EdgeWeight_t[]> AdjascencyMatrix_t; /* (Size * Size) * (Size * Size) */

	public:
		/*
			SideSize - d³ugoœæ boku (graf jest kwadratowy)
			AdjascencyMatrix - macierz SideSize ^ 4 z wagami boków
		*/
		Graph(const VertexId_t & SideSize, AdjascencyMatrix_t && AdjascencyMatrix);

	private:
		VertexId_t SideSize;
		AdjascencyMatrix_t AdjascencyMatrix;

	public:
		const VertexId_t & GetSideSize() const;

	public:
		/*
			Generuje postaæ grafu któr¹ mo¿na zwizualizowæ.
			https://dreampuf.github.io/GraphvizOnline/
			nale¿y wybraæ Engine: fdp
		*/
		void GraphViz(std::ostream & Stream) const;

		/*
			Rysuje graficzn¹ reprezentacjê macierzy s¹siedztwa
		*/
		void DumpAdjascencyMatrix(std::ostream & Stream) const;

	public:
		/*
			Generowanie grafu metod¹ Waxman'a.

			SideSize - d³ugoœæ boku kwadratowego grafu
			Parametry Alpha i Beta w zakresie 0.0 - 1.0 steruj¹ prawdopodobieñstwem pojawienia siê krawêdzi pomiêdzy dwoma punktami.
			EdgeWeightMin, EdgeWeightMax - minimalna i maksymalna waga krawêdzi
		*/
		static Graph GenerateWaxmanRandom(const VertexId_t & SideSize, const double & Alpha, const double & Beta, const EdgeWeight_t & EdgeWeightMin, const EdgeWeight_t & EdgeWeightMax);
};

