#include "ParticleGroupRunner.hpp"

#include <random>

ParticleGroupRunner::ParticleGroupRunner(const Task & T, const size_t & ParticleGroupNumber, const size_t & ParticleNumberTotal, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2):
	T(T), 
	ParticleGroupNumber(ParticleGroupNumber), ParticleNumberTotal(ParticleNumberTotal), ParticleIterations(ParticleIterations), ParticleBetterSolutionFoundNoCountMax(ParticleBetterSolutionFoundNoCountMax),
	Fi1(Fi1), Fi2(Fi2)
{

}

ParticleGroupRunner::RunResult_t ParticleGroupRunner::Run()
{
	// Je�li u�ytkownik nie poda� liczby w�tk�w - dobieramy automatycznie
	const size_t ParticleGroupNumberReal = ParticleGroupNumber ? ParticleGroupNumber : std::thread::hardware_concurrency();

	// Liczba cz�stek na grup�
	const size_t ParticleNumber = ParticleNumberTotal / ParticleGroupNumberReal;

	// Struktura pojedynczej grupy cz�stek
	struct ParticleGroup_t
	{
		// Instancja grupy cz�stek
		ParticleGroup Instance;
		
		// W�tek
		std::thread Thread;
	};

	// Kontener na grupy cz�stek przypisane do w�tk�w
	std::list<ParticleGroup_t> ParticleGroups;

	std::mt19937 RandomGenerator;

	// Tworzymy cz�stki
	for(size_t I = 0; I < ParticleGroupNumberReal; I++)
	{
		ParticleGroups.emplace_back(ParticleGroup_t {
			// Stw�rz grup� cz�stek
			ParticleGroup
			{
				T,
				ParticleNumber,
				ParticleIterations,
				ParticleBetterSolutionFoundNoCountMax,
				Fi1,
				Fi2,
				RandomGenerator()
			},

			// Stw�rz pusty w�tek
			std::thread()
		});
	}

	// Uruchom w�tki
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread = std::thread(&ParticleGroup::Run, &(PG.Instance));

	// Poczekaj na zako�czenie
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread.join();

	// Aktualnie najlepszy wynik
	std::optional<const Particle> ParticleBest;

	// Historia wynik�w poszczeg�lnych cz�stek
	std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
	HistoryEntries.reserve(ParticleGroups.size());

	// Przejd� przez wszystkie cz�stki i pobierz wyniki
	for(ParticleGroup_t & PG : ParticleGroups)
	{
		const std::optional<const Particle> & PGParticleBest = PG.Instance.GetParticleBest();

		// Je�li cz�stka nic nie znalaz�a - pomi�
		if(!PGParticleBest)
			continue;

		// Je�li ma lepszy wynik - zaktualizuj
		if(!ParticleBest || PGParticleBest.value().GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value()))
		{
			ParticleBest.emplace(
				PGParticleBest.value()
			);
		}

		// Dodaj histori� wynik�w
		HistoryEntries.push_back(
			PG.Instance.GetHistoryEntries()
		);
	}

	return {
		ParticleBest,
		HistoryEntries
	};
}