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
	// Jeśli użytkownik nie podał liczby wątków - dobieramy automatycznie
	const size_t ParticleGroupNumberReal = ParticleGroupNumber ? ParticleGroupNumber : std::thread::hardware_concurrency();

	// Liczba cząstek na grupę
	const size_t ParticleNumber = ParticleNumberTotal / ParticleGroupNumberReal;

	// Struktura pojedynczej grupy cząstek
	struct ParticleGroup_t
	{
		// Instancja grupy cząstek
		ParticleGroup Instance;
		
		// Wątek
		std::thread Thread;
	};

	// Kontener na grupy cząstek przypisane do wątków
	std::list<ParticleGroup_t> ParticleGroups;

	std::mt19937 RandomGenerator;

	// Tworzymy cząstki
	for(size_t I = 0; I < ParticleGroupNumberReal; I++)
	{
		ParticleGroups.emplace_back(ParticleGroup_t {
			// Stwórz grupę cząstek
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

			// Stwórz pusty wątek
			std::thread()
		});
	}

	// Uruchom wątki
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread = std::thread(&ParticleGroup::Run, &(PG.Instance));

	// Poczekaj na zakończenie
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread.join();

	// Aktualnie najlepszy wynik
	std::optional<const Particle> ParticleBest;

	// Historia wyników poszczególnych cząstek
	std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
	HistoryEntries.reserve(ParticleGroups.size());

	// Przejdź przez wszystkie cząstki i pobierz wyniki
	for(ParticleGroup_t & PG : ParticleGroups)
	{
		const std::optional<const Particle> & PGParticleBest = PG.Instance.GetParticleBest();

		// Jeśli cząstka nic nie znalazła - pomiń
		if(!PGParticleBest)
			continue;

		// Jeśli ma lepszy wynik - zaktualizuj
		if(!ParticleBest || PGParticleBest.value().GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value()))
		{
			ParticleBest.emplace(
				PGParticleBest.value()
			);
		}

		// Dodaj historię wyników
		HistoryEntries.push_back(
			PG.Instance.GetHistoryEntries()
		);
	}

	return {
		ParticleBest,
		HistoryEntries
	};
}