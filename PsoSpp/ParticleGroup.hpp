#pragma once

#include <vector>
#include <optional>
#include <chrono>
#include <random>

#include "Task.hpp"
#include "Particle.hpp"
#include "GraphPath.hpp"


/*
	Klasa kontenera roju cz¹stek

	Lokalna grupa cz¹stek, któr¹ mo¿na uruchomiæ na jednym w¹tku. Wszystkie zmienne przyjmowane jako referencje do sta³ych obiektów - w za³o¿eniu gwarantuje to w¹tkow¹ bezpiecznoœæ.
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

			ParticleNumber - Liczba cz¹stek
			ParticleIterations - Liczba iteracji
			ParticleBetterSolutionFoundNoCountMax - po ilu pe³nych iteracjach bez poprawy jakiejkolwiek cz¹stki wykonaæ reset wag?

			Fi1, Fi2 - Parametry dostrajalne metody aktualizacji œcie¿ki pojedynczej cz¹stki. Opisane w Particle.hpp

			Seed - wartoœæ inicjuj¹ca generator liczb losowych
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
			Kontener cz¹stek algorytmu, inicjowany w konstuktorze.
		*/
		std::vector<Particle> Particles; /* ParticleNumber */

		/*
			Zatrzaœniêta instancja aktualnie najlepszej cz¹stki
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
			G³ówna pêtla programu.

			Zwraca czy znaleziono choæ jedno rozwi¹zanie problemu
		*/
		bool Run();
};

