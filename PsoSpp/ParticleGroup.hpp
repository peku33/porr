#pragma once

#include <vector>
#include <optional>
#include <chrono>
#include <random>

#include "Task.hpp"
#include "Particle.hpp"
#include "GraphPath.hpp"


/*
	Klasa kontenera roju cząstek

	Lokalna grupa cząstek, którą można uruchomić na jednym wątku. Wszystkie zmienne przyjmowane jako referencje do stałych obiektów - w założeniu gwarantuje to wątkową bezpieczność.
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

			ParticleNumber - Liczba cząstek
			ParticleIterations - Liczba iteracji
			ParticleBetterSolutionFoundNoCountMax - po ilu pełnych iteracjach bez poprawy jakiejkolwiek cząstki wykonać reset wag?

			Fi1, Fi2 - Parametry dostrajalne metody aktualizacji ścieżki pojedynczej cząstki. Opisane w Particle.hpp

			Seed - wartość inicjująca generator liczb losowych
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
			Kontener cząstek algorytmu, inicjowany w konstuktorze.
		*/
		std::vector<Particle> Particles; /* ParticleNumber */

		/*
			Zatrzaśnięta instancja aktualnie najlepszej cząstki
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
			Główna pętla programu.

			Zwraca czy znaleziono choć jedno rozwiązanie problemu
		*/
		bool Run();
};

