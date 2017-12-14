#pragma once

#include <optional>

#include "Task.hpp"
#include "GraphPath.hpp"

class ParticleGroup;

/*
	Cz¹stka algorytmu PSO korzystaj¹ca z "cost-priority-based particle encoding/decoding"
	https://www.emis.de/journals/HOA/DDNS/Volume2007/27383.pdf
	Rodzia³ 3.2
*/
class Particle
{
	public:
		typedef double Priority_t;
		typedef double Velocity_t;

	public:
		/*
			Konstruktor. Inicjuje cz¹stkê losowymi priorytetami i wektorem prêdkoœci
		*/
		Particle(const ParticleGroup & PG);
		
		/*
			Konstruktor kopiuj¹cy
		*/
		Particle(const Particle & Other);

	private:
		const ParticleGroup & PG;

	private:
		/*
			Kolekcje priorytetów i prêdkoœci dla kolejnych wêz³ów
		*/
		const std::unique_ptr<Priority_t[]> Priorities;
		const std::unique_ptr<Velocity_t[]> Velocities;


	private:
		/*
			Wektor priorytetów dla najlepszego lokalnego rozwi¹zania.
			Ma sens tylko gdy (bool) BestGraphPath
		*/
		const std::unique_ptr<Priority_t[]> BestPriorities;

		/*
			Œcie¿ka dla najlepszego lokalnego rozwi¹zania
		*/
		std::optional<const GraphPath> BestGraphPath;

	public:
		const std::optional<const GraphPath> & GetBestGraphPath() const;

	public:
		/*
			Inicjuje priorytety losowymi danymi
			"Miêkki reset" wêz³a, przydatny przy osadzeniu na minimum lokalnym
		*/
		void RandomInitialize();

		/*
			Krok iteracji wêz³a.
			Dla posiadanych priotytetów i prêdkoœci poszukuje kolejnego rozwi¹zania.
			Jeœli takie znajdzie - zapisuje BestPriorities i BestGraphPath oraz zwraca true
			Jeœli nie znajdzie - zwraca false
		*/
		bool Run();

		/*
			Wykonuje aktualizacjê prêdkoœci cz¹stki korzystaj¹c z obecnie najlepszego wêz³a w grupie.

			Parametry dostrajalne Fi1 Fi2, (warunek: Fi1 + Fi2 > 4) okreœlaj¹ moc wp³ywu kolejno:
				- lokalnej najlepszej cz¹stki
				- grupowej najlepszej cz¹stki (ParticleBest)
			Na kierunek przemieszczania siê tej cz¹stki
		*/
		void Update(const double & Fi1, const double & Fi2, const Particle & ParticleBest);
};

