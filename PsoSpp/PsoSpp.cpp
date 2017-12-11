#include <iostream>

#include "Graph.hpp"

int main()
{
	const Graph G1 = Graph::GenerateWaxmanRandom(10, 1.0, 0.20, 10, 100);
	G1.GraphViz(std::cout);
    return 0;
}

