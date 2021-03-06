#include "Graph.hpp"

#include <iomanip>
#include <random>

Graph::Graph(const VertexId_t & Size, AdjascencyMatrix_t && AdjascencyMatrix):
	SideSize(Size), AdjascencyMatrix(std::move(AdjascencyMatrix))
{

}

const Graph::VertexId_t & Graph::GetSideSize() const
{
	return SideSize;
}

const Graph::VertexIndex_t Graph::GetSize() const
{
	return SideSize * SideSize;
}

const Graph::EdgeWeight_t Graph::GetEdgeWeight(const VertexIndex_t & Vertex1Index, const VertexIndex_t & Vertex2Index) const
{
	// Indeksy kraw�dzi
	const size_t Edge1Index = SideSize * SideSize * Vertex2Index + Vertex1Index;
	// const size_t Edge2Index = SideSize * SideSize * Vertex2Index + Vertex1Index;

	// Pobierz warto�� kraw�dzi
	const EdgeWeight_t EdgeWeight = AdjascencyMatrix[Edge1Index];

	return EdgeWeight;
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

	for(VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
	{
		for(VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
		{
			for(VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
			{
				for(VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
				{
					// Pomini�cie kraw�dzi
					if(Vertex2Y > Vertex1Y || (Vertex2Y == Vertex1Y && Vertex2X >= Vertex1X))
						continue;

					// Indeksy wierzcho�k�w
					const Graph::VertexIndex_t Vertex1Index = SideSize * Vertex1Y + Vertex1X;
					const Graph::VertexIndex_t Vertex2Index = SideSize * Vertex2Y + Vertex2X;

					// Indeksy kraw�dzi
					const size_t Edge1Index = SideSize * SideSize * Vertex2Index + Vertex1Index;
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

void Graph::DumpAdjascencyMatrix(std::ostream & Stream) const
{
	Stream << "---------";
	for(VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
	{
		for(VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
		{
			Stream << "(" << std::setw(2) << Vertex2X << "," << std::setw(2) << Vertex2Y << ") ";
		}
	}
	Stream << "\n";

	for(VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
	{
		for(VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
		{
			Stream << "(" << std::setw(2) << Vertex1X << "," << std::setw(2) << Vertex1Y << ") ";
			for(VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
			{
				for(VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
				{
					// Indeksy wierzcho�k�w
					const Graph::VertexIndex_t Vertex1Index = SideSize * Vertex1Y + Vertex1X;
					const Graph::VertexIndex_t Vertex2Index = SideSize * Vertex2Y + Vertex2X;

					// Indeksy kraw�dzi
					const size_t Edge1Index = SideSize * SideSize * Vertex2Index + Vertex1Index;
					// const size_t Edge2Index = SideSize * SideSize * Vertex2Index + Vertex1Index;

					// Pobierz warto�� kraw�dzi
					const EdgeWeight_t EdgeWeight = AdjascencyMatrix[Edge1Index];

					Stream << std::setw(8) << (int) EdgeWeight;
				}
			}
			Stream << "\n";
		}
	}

	Stream << "\n";
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

	std::mt19937 RandomGenerator;
	std::uniform_real_distribution<double> RDistribution(0.0, 1.0);

	for(Graph::VertexId_t Vertex1Y = 0; Vertex1Y < SideSize; Vertex1Y++)
	{
		for(Graph::VertexId_t Vertex1X = 0; Vertex1X < SideSize; Vertex1X++)
		{
			// Brak kraw�dzi pomi�dzy tymi samymi punktami (na przek�tnej)
			const Graph::VertexIndex_t VertexIndex = SideSize * Vertex1Y + Vertex1X;
			const size_t EdgeIndex = SideSize * SideSize * VertexIndex + VertexIndex;

			AdjascencyMatrix[EdgeIndex] = std::numeric_limits<Graph::EdgeWeight_t>::max();;

			for(Graph::VertexId_t Vertex2Y = 0; Vertex2Y < SideSize; Vertex2Y++)
			{
				for(Graph::VertexId_t Vertex2X = 0; Vertex2X < SideSize; Vertex2X++)
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
					const bool EdgeExists = (AdjascencyProbability) >= RDistribution(RandomGenerator);

					// Indeksy wierzcho�k�w
					const Graph::VertexIndex_t Vertex1Index = SideSize * Vertex1Y + Vertex1X;
					const Graph::VertexIndex_t Vertex2Index = SideSize * Vertex2Y + Vertex2X;

					// Indeksy kraw�dzi
					const size_t Edge1Index = SideSize * SideSize * Vertex2Index + Vertex1Index;
					const size_t Edge2Index = SideSize * SideSize * Vertex1Index + Vertex2Index;

					// Domy�lnie brak kraw�dzi = maksymalna dost�pna waga
					Graph::EdgeWeight_t EdgeWeight = std::numeric_limits<Graph::EdgeWeight_t>::max();

					// Kraw�d� istnieje - ustawiamy losow� wag� z zakresu [EdgeWeightMin, EdgeWeightMax]
					if(EdgeExists)
						EdgeWeight = EdgeWeightMin + (Graph::EdgeWeight_t) (RDistribution(RandomGenerator) * (EdgeWeightMax - EdgeWeightMin));

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