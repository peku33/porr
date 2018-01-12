#pragma once

#include <vector>
#include <thread>

#include "Task.hpp"
#include "ParticleGroup.hpp"

/*
	Klasa odpowiedzialna za uruchamianie algorytmu w określonej ilości wątków

	Dla każdego wątku (liczba określona przez ParticleGroupNumber) tworzona jest grupa zawierająca ParticleNumberTotal / ParticleGroupNumber cząstek
	Każdy wątek wykonuje ParticleIterations iteracji
*/
class ParticleGroupRunner
{
	public:
		/*
			Konstruktor.

			T - Zadanie optymalizacji

			ParticleGroupNumber - liczba niezależnych grup cząstek (wątków). Podanie 0 spowoduje automatyczne ustalenie wartości na podstawie hardware concurency
			ParticleNumberTotal - liczba cząstek we wszystkich grupach. Każda stworzona grupa zostanie podzielona.
			ParticleIterations - liczba iteracji każdej z grup

			ParticleBetterSolutionFoundNoCountMax, Fi1, Fi2 - parametry przekazywane bezpośrednio do ParticleGroup
		*/
		ParticleGroupRunner(const Task & T, const size_t & ParticleGroupNumber, const size_t & ParticleNumberTotal, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2);

	private:
		const Task & T;

		const size_t ParticleGroupNumber;
		const size_t ParticleNumberTotal;
		const size_t ParticleIterations;
		const size_t ParticleBetterSolutionFoundNoCountMax;

		const double Fi1;
		const double Fi2;

	public:
		struct RunResult_t
		{
			// Najlepsza znaleziona cząstka przez wszystkie iteracje
			std::optional<const Particle> ParticleBest;

			// Historia iteracji
			std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
		};
		RunResult_t Run();
};

