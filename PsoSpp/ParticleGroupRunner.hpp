#pragma once

#include <vector>
#include <thread>

#include "Task.hpp"
#include "ParticleGroup.hpp"

/*
	Klasa odpowiedzialna za uruchamianie algorytmu w okre�lonej ilo�ci w�tk�w

	Dla ka�dego w�tku (liczba okre�lona przez ParticleGroupNumber) tworzona jest grupa zawieraj�ca ParticleNumberTotal / ParticleGroupNumber cz�stek
	Ka�dy w�tek wykonuje ParticleIterations iteracji
*/
class ParticleGroupRunner
{
	public:
		/*
			Konstruktor.

			T - Zadanie optymalizacji

			ParticleGroupNumber - liczba niezale�nych grup cz�stek (w�tk�w). Podanie 0 spowoduje automatyczne ustalenie warto�ci na podstawie hardware concurency
			ParticleNumberTotal - liczba cz�stek we wszystkich grupach. Ka�da stworzona grupa zostanie podzielona.
			ParticleIterations - liczba iteracji ka�dej z grup

			ParticleBetterSolutionFoundNoCountMax, Fi1, Fi2 - parametry przekazywane bezpo�rednio do ParticleGroup
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
			// Najlepsza znaleziona cz�stka przez wszystkie iteracje
			std::optional<const Particle> ParticleBest;

			// Historia iteracji
			std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
		};
		RunResult_t Run();
};

