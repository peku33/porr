#pragma once

#include <vector>
#include <optional>
#include <chrono>
#include <random>

#include "Task.hpp"
#include "Particle.hpp"
#include "GraphPath.hpp"


/*
	Klasa kontenera roju cz�stek

	Lokalna grupa cz�stek, kt�r� mo�na uruchomi� na jednym w�tku. Wszystkie zmienne przyjmowane jako referencje do sta�ych obiekt�w - w za�o�eniu gwarantuje to w�tkow� bezpieczno��.
*/
class ParticleGroup
{
	public:
		struct HistoryEntry_t
		{
			std::chrono::steady_clock::time_point TimePoint;
			size_t ParticleIteration;
			GraphPath::PathWeight_t PathWeight;
		};
		typedef std::list<HistoryEntry_t> HistoryEntries_t;

	public:
		/*
			Konstruktor

			T - Zadanie optymalizacji

			ParticleNumber - Liczba cz�stek
			ParticleIterations - Liczba iteracji
			ParticleBetterSolutionFoundNoCountMax - po ilu pe�nych iteracjach bez poprawy jakiejkolwiek cz�stki wykona� reset wag?

			Fi1, Fi2 - Parametry dostrajalne metody aktualizacji �cie�ki pojedynczej cz�stki. Opisane w Particle.hpp

			Seed - warto�� inicjuj�ca generator liczb losowych
		*/
		ParticleGroup(const Task & T, const size_t & ParticleNumber, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2, const std::mt19937::result_type & Seed);

	private:
		const Task & T;

		const size_t ParticleNumber;
		const size_t ParticleIterations;
		const size_t ParticleBetterSolutionFoundNoCountMax;

		const double Fi1;
		const double Fi2;

	private:
		/*
			Generator liczb losowych
		*/
		std::mt19937 RandomGenerator;

		/*
			Kontener cz�stek algorytmu, inicjowany w konstuktorze.
		*/
		std::vector<Particle> Particles; /* ParticleNumber */

		/*
			Zatrza�ni�ta instancja aktualnie najlepszej cz�stki
		*/
		std::optional<const Particle> ParticleBest;

		/*
			Historia wag w czasie
		*/
		HistoryEntries_t HistoryEntries;

	public:
		const Task & GetTask() const;
		const std::optional<const Particle> & GetParticleBest() const;
		const HistoryEntries_t & GetHistoryEntries() const;

	public:
		/*
			G��wna p�tla programu.

			Zwraca czy znaleziono cho� jedno rozwi�zanie problemu
		*/
		bool Run();
};

