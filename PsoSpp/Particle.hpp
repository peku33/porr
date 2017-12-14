#pragma once

#include <optional>

#include "Task.hpp"
#include "GraphPath.hpp"

class ParticleGroup;

/*
	Cz�stka algorytmu PSO korzystaj�ca z "cost-priority-based particle encoding/decoding"
	https://www.emis.de/journals/HOA/DDNS/Volume2007/27383.pdf
	Rodzia� 3.2
*/
class Particle
{
	public:
		typedef double Priority_t;
		typedef double Velocity_t;

	public:
		/*
			Konstruktor. Inicjuje cz�stk� losowymi priorytetami i wektorem pr�dko�ci
		*/
		Particle(const ParticleGroup & PG);
		
		/*
			Konstruktor kopiuj�cy
		*/
		Particle(const Particle & Other);

	private:
		const ParticleGroup & PG;

	private:
		/*
			Kolekcje priorytet�w i pr�dko�ci dla kolejnych w�z��w
		*/
		const std::unique_ptr<Priority_t[]> Priorities;
		const std::unique_ptr<Velocity_t[]> Velocities;


	private:
		/*
			Wektor priorytet�w dla najlepszego lokalnego rozwi�zania.
			Ma sens tylko gdy (bool) BestGraphPath
		*/
		const std::unique_ptr<Priority_t[]> BestPriorities;

		/*
			�cie�ka dla najlepszego lokalnego rozwi�zania
		*/
		std::optional<const GraphPath> BestGraphPath;

	public:
		const std::optional<const GraphPath> & GetBestGraphPath() const;

	public:
		/*
			Inicjuje priorytety losowymi danymi
			"Mi�kki reset" w�z�a, przydatny przy osadzeniu na minimum lokalnym
		*/
		void RandomInitialize();

		/*
			Krok iteracji w�z�a.
			Dla posiadanych priotytet�w i pr�dko�ci poszukuje kolejnego rozwi�zania.
			Je�li takie znajdzie - zapisuje BestPriorities i BestGraphPath oraz zwraca true
			Je�li nie znajdzie - zwraca false
		*/
		bool Run();

		/*
			Wykonuje aktualizacj� pr�dko�ci cz�stki korzystaj�c z obecnie najlepszego w�z�a w grupie.

			Parametry dostrajalne Fi1 Fi2, (warunek: Fi1 + Fi2 > 4) okre�laj� moc wp�ywu kolejno:
				- lokalnej najlepszej cz�stki
				- grupowej najlepszej cz�stki (ParticleBest)
			Na kierunek przemieszczania si� tej cz�stki
		*/
		void Update(const double & Fi1, const double & Fi2, const Particle & ParticleBest);
};

