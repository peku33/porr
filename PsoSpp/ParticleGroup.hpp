#pragma once

#include <vector>
#include <optional>

#include "Particle.hpp"
#include "GraphPath.hpp"

class Task;

/*
	Klasa kontenera roju cz�stek

	Lokalna grupa cz�stek, kt�r� mo�na uruchomi� na jednym w�tku. Wszystkie zmienne przyjmowane jako referencje do sta�ych obiekt�w - w za�o�eniu gwarantuje to w�tkow� bezpieczno��.
*/
class ParticleGroup
{
	public:
		/*
			Konstruktor

			T - Zadanie optymalizacji

			ParticleNumber - Liczba cz�stek
			ParticleIterations - Liczba iteracji
			ParticleBetterSolutionFoundNoCountMax - po ilu pe�nych iteracjach bez poprawy jakiejkolwiek cz�stki wykona� reset wag?

			Fi1, Fi2 - Parametry dostrajalne metody aktualizacji �cie�ki pojedynczej cz�stki. Opisane w Particle.hpp
		*/
		ParticleGroup(const Task & T, const size_t & ParticleNumber, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2);

	private:
		const Task & T;

		const size_t ParticleNumber;
		const size_t ParticleIterations;
		const size_t ParticleBetterSolutionFoundNoCountMax;

		const double Fi1;
		const double Fi2;

	private:
		/*
			Kontener cz�stek algorytmu, inicjowany w konstuktorze.
		*/
		std::vector<Particle> Particles; /* ParticleNumber */

		/*
			Zatrza�ni�ta instancja aktualnie najlepszej cz�stki
		*/
		std::optional<const Particle> ParticleBest;

		/*
			Historia wag �ciezek w kolejnych iteracjach
		*/
		std::list<std::optional<GraphPath::PathWeight_t>> PathWeights;

	public:
		const Task & GetTask() const;
		const std::optional<const Particle> & GetParticleBest() const;
		const std::list<std::optional<GraphPath::PathWeight_t>> & GetPathWeights() const;

	public:
		/*
			G��wna p�tla programu.

			Zwraca czy znaleziono cho� jedno rozwi�zanie problemu
		*/
		bool Run();
};

