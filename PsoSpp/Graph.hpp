#pragma once

#include <memory>
#include <ostream>

/*
	Klasa reprezentuje `kwadratowy` nieskierowany graf o rozmiarze SideSize * SideSize
*/
class Graph
{
	public:
		// Id wierzcho�ka
		typedef unsigned int VertexId_t;

		// Waga kraw�dzi
		typedef uint8_t EdgeWeight_t;

		// Macierz s�siedztwa zawieraj�ca wagi poszczeg�lnych kraw�dzi
		// Poniewa� celem zadania jest minimalizacja sumarycznej wagi �cie�ki, brak kraw�dzi reprezentowany jest przez std::numeric_limits<EdgeWeight_t>::max()
		typedef std::unique_ptr<Graph::EdgeWeight_t[]> AdjascencyMatrix_t; /* (Size * Size) * (Size * Size) */

	public:
		/*
			SideSize - d�ugo�� boku (graf jest kwadratowy)
			AdjascencyMatrix - macierz SideSize ^ 4 z wagami bok�w
		*/
		Graph(const VertexId_t & SideSize, AdjascencyMatrix_t && AdjascencyMatrix);

	private:
		VertexId_t SideSize;
		AdjascencyMatrix_t AdjascencyMatrix;

	public:
		const VertexId_t & GetSideSize() const;

	public:
		/*
			Generuje posta� grafu kt�r� mo�na zwizualizow�.
			https://dreampuf.github.io/GraphvizOnline/
			nale�y wybra� Engine: fdp
		*/
		void GraphViz(std::ostream & Stream) const;

		/*
			Rysuje graficzn� reprezentacj� macierzy s�siedztwa
		*/
		void DumpAdjascencyMatrix(std::ostream & Stream) const;

	public:
		/*
			Generowanie grafu metod� Waxman'a.

			SideSize - d�ugo�� boku kwadratowego grafu
			Parametry Alpha i Beta w zakresie 0.0 - 1.0 steruj� prawdopodobie�stwem pojawienia si� kraw�dzi pomi�dzy dwoma punktami.
			EdgeWeightMin, EdgeWeightMax - minimalna i maksymalna waga kraw�dzi
		*/
		static Graph GenerateWaxmanRandom(const VertexId_t & SideSize, const double & Alpha, const double & Beta, const EdgeWeight_t & EdgeWeightMin, const EdgeWeight_t & EdgeWeightMax);
};

