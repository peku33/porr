#pragma once

#include <vector>
#include <thread>

#include "Task.hpp"
#include "ParticleGroup.hpp"

/*
	Klasa odpowiedzialna za uruchamianie algorytmu w okreœlonej iloœci w¹tków

	Dla ka¿dego w¹tku (liczba okreœlona przez ParticleGroupNumber) tworzona jest grupa zawieraj¹ca ParticleNumberTotal / ParticleGroupNumber cz¹stek
	Ka¿dy w¹tek wykonuje ParticleIterations iteracji
*/
class ParticleGroupRunner
{
	public:
		/*
			Konstruktor.

			T - Zadanie optymalizacji

			ParticleGroupNumber - liczba niezale¿nych grup cz¹stek (w¹tków). Podanie 0 spowoduje automatyczne ustalenie wartoœci na podstawie hardware concurency
			ParticleNumberTotal - liczba cz¹stek we wszystkich grupach. Ka¿da stworzona grupa zostanie podzielona.
			ParticleIterations - liczba iteracji ka¿dej z grup

			ParticleBetterSolutionFoundNoCountMax, Fi1, Fi2 - parametry przekazywane bezpoœrednio do ParticleGroup
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
			// Najlepsza znaleziona cz¹stka przez wszystkie iteracje
			std::optional<const Particle> ParticleBest;

			// Historia iteracji
			std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
		};
		RunResult_t Run();
};

