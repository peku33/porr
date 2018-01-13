#pragma once

#include <optional>
#include <random>

#include "Task.hpp"
#include "GraphPath.hpp"

class ParticleGroup;

/*
	Cząstka algorytmu PSO korzystająca z "cost-priority-based particle encoding/decoding"
	https://www.emis.de/journals/HOA/DDNS/Volume2007/27383.pdf
	Rodział 3.2
*/
class Particle
{
	public:
		typedef double Velocity_t;
		typedef std::unique_ptr<Velocity_t[]> Velocities_t;

	public:
		/*
			Konstruktor. Inicjuje cząstkę losowymi priorytetami i wektorem prędkości

			PG - Grupa cząstek do której należy ta cząstka
			RandomGenerator - Generator liczb losowych
		*/
		Particle(const ParticleGroup & PG, std::mt19937 & RandomGenerator);

	private:
		const ParticleGroup & PG;
		std::mt19937 & RandomGenerator;

	private:
		/*
			Kolekcje priorytetów i prędkości dla kolejnych węzłów
		*/
		GraphPath::Priorities_t Priorities;
		Velocities_t Velocities;

	private:
		/*
			Ścieżka dla najlepszego lokalnego rozwiązania
		*/
		std::optional<const GraphPath> GraphPathBest;

	public:
		const std::optional<const GraphPath> & GetGraphPathBest() const;

	public:
		/*
			Inicjuje priorytety losowymi danymi
			"Miękki reset" węzła, przydatny przy osadzeniu na minimum lokalnym
		*/
		void RandomInitialize();

		/*
			Krok iteracji węzła.
			Dla posiadanych priotytetów i prędkości poszukuje kolejnego rozwiązania.
			Jeśli takie znajdzie - zapisuje BestPriorities i BestGraphPath oraz zwraca true
			Jeśli nie znajdzie - zwraca false
		*/
		bool Run();

		/*
			Wykonuje aktualizację prędkości cząstki korzystając z obecnie najlepszego węzła w grupie.

			Parametry dostrajalne Fi1 Fi2, (warunek: Fi1 + Fi2 > 4) określają moc wpływu kolejno:
				- lokalnej najlepszej cząstki
				- grupowej najlepszej cząstki (ParticleBest)
			Na kierunek przemieszczania się tej cząstki
		*/
		void Update(const double & Fi1, const double & Fi2, const std::optional<const GraphPath> & GraphPathBestGlobal);
};

