#include "Graph.hpp"

#include <iomanip>

Graph::Graph(const VertexId_t & Size, AdjascencyMatrix_t && AdjascencyMatrix):
	SideSize(Size), AdjascencyMatrix(std::move(AdjascencyMatrix))
{

}

const Graph::VertexId_t & Graph::GetSideSize() const
{
	return SideSize;
}

void Graph::GraphViz(std::ostream & Stream) const
{
	Stream << "graph {\n";
	for(VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
	{
		for(VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
		{
			Stream << "x" << Vertex1X << "y" << Vertex1Y << " [\n";
			Stream << "\tlabel = \"(" << Vertex1X << ":" << Vertex1Y << ")\"\n";
			Stream << "\tpos = \"" << Vertex1X << "," << Vertex1Y << "!\"\n";
			Stream << "]\n";
		}
	}

	for(VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
	{
		for(VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
		{
			for(VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
			{
				for(VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
				{
					// Pomini�cie kraw�dzi
					if(Vertex2Y > Vertex1Y || (Vertex2Y == Vertex1Y && Vertex2X >= Vertex1X))
						continue;

					// Indeksy wierzcho�k�w
					const Graph::VertexId_t Vertex1Index = SideSize * Vertex1X + Vertex1Y;
					const Graph::VertexId_t Vertex2Index = SideSize * Vertex2X + Vertex2Y;

					// Indeksy kraw�dzi
					const size_t Edge1Index = SideSize * SideSize * Vertex1Index + Vertex2Index;
					// const size_t Edge2Index = SideSize * SideSize * Vertex2Index + Vertex1Index;

					// Pobierz warto�� kraw�dzi
					const EdgeWeight_t EdgeWeight = AdjascencyMatrix[Edge1Index];

					// Je�li kraw�d� nie istneije - jedziemy dalej
					if(EdgeWeight == std::numeric_limits<EdgeWeight_t>::max())
						continue;

					// Dodaj definicj� kraw�dzi
					Stream << "\tx" << Vertex1X << "y" << Vertex1Y << " -- x" << Vertex2X << "y" << Vertex2Y << " [ label = \"" << (unsigned int) EdgeWeight << "\" ];\n";
				}
			}
		}
	}
	Stream << "}\n";
}

Graph Graph::GenerateWaxmanRandom(const VertexId_t & SideSize, const double & Alpha, const double & Beta, const EdgeWeight_t & EdgeWeightMin, const EdgeWeight_t & EdgeWeightMax)
{
	// Sprawdzenie sensowno�ci parametr�w generowania
	if(SideSize <= 0)
		throw std::invalid_argument("SideSize <= 0");

	if(Alpha < 0 || Alpha > 1)
		throw std::invalid_argument("Alpha < 0 || Alpha > 1");

	if(Beta < 0 || Beta > 1)
		throw std::invalid_argument("Beta < 0 || Beta > 1");

	if(EdgeWeightMin >= EdgeWeightMax || EdgeWeightMin <= 0 || EdgeWeightMax >= std::numeric_limits<EdgeWeight_t>::max())
		throw std::invalid_argument("EdgeWeightMin >= EdgeWeightMax || EdgeWeightMin <= 0 || EdgeWeightMax >= std::numeric_limits<EdgeWeight_t>::max()");

	// Macierz s�siedztwa
	AdjascencyMatrix_t AdjascencyMatrix(new EdgeWeight_t[(SideSize * SideSize) * (SideSize * SideSize)]);
	// std::fill(AdjascencyMatrix.get(), AdjascencyMatrix.get() + (SideSize * SideSize) * (SideSize * SideSize), 0);

	// Traktujemy (VertexId1, VertexId2) jako (x, y)
	const double VertexDistanceEuclideanMaximal = sqrt(2 * (SideSize * SideSize));

	// Incjalizacja generatora liczb losowych
	srand((unsigned int) time(NULL));

	for(Graph::VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
	{
		for(Graph::VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
		{
			for(Graph::VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
			{
				for(Graph::VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
				{
					// Pomini�cie kraw�dzi
					if(Vertex2Y > Vertex1Y || (Vertex2Y == Vertex1Y && Vertex2X >= Vertex1X))
						continue;

					// Odleg�o�� pomi�dzy dwoma wierzcho�kami
					const double VertexDistanceX = 1.0 * Vertex2X - 1.0 * Vertex1X;
					const double VertexDistanceY = 1.0 * Vertex2Y - 1.0 * Vertex1Y;
					const double VertexDistanceEuclidean = sqrt(VertexDistanceX * VertexDistanceX + VertexDistanceY * VertexDistanceY);

					// Formu�a Waxmana - prawdopodobie�stwo s�siedzstwa VertexId1, VertexId2
					const double AdjascencyProbability = Alpha * exp((-1.0 * VertexDistanceEuclidean) / (Beta * VertexDistanceEuclideanMaximal));

					// Czy kraw�d� istnieje?
					const bool EdgeExists = (AdjascencyProbability) >= (1.0 * rand() / RAND_MAX);

					// Indeksy wierzcho�k�w
					const Graph::VertexId_t Vertex1Index = SideSize * Vertex1X + Vertex1Y;
					const Graph::VertexId_t Vertex2Index = SideSize * Vertex2X + Vertex2Y;

					// Indeksy kraw�dzi
					const size_t Edge1Index = SideSize * SideSize * Vertex1Index + Vertex2Index;
					const size_t Edge2Index = SideSize * SideSize * Vertex2Index + Vertex1Index;

					// Domy�lnie brak kraw�dzi = maksymalna dost�pna waga
					Graph::EdgeWeight_t EdgeWeight = std::numeric_limits<Graph::EdgeWeight_t>::max();

					// Kraw�d� istnieje - ustawiamy losow� wag� z zakresu [EdgeWeightMin, EdgeWeightMax]
					if(EdgeExists)
						EdgeWeight = EdgeWeightMin + (Graph::EdgeWeight_t) ((1.0 * rand() / RAND_MAX) * (EdgeWeightMax - EdgeWeightMin));

					// Wpisujemy dane do macierzy
					AdjascencyMatrix[Edge1Index] = EdgeWeight;
					AdjascencyMatrix[Edge2Index] = EdgeWeight;
				}
			}
		}
	}

	// Zwracamy docelowy obiekt grafu
	return Graph{ SideSize, std::move(AdjascencyMatrix) };
}