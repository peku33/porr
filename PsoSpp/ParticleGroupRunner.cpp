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
	// Jeœli u¿ytkownik nie poda³ liczby w¹tków - dobieramy automatycznie
	const size_t ParticleGroupNumberReal = ParticleGroupNumber ? ParticleGroupNumber : std::thread::hardware_concurrency();

	// Liczba cz¹stek na grupê
	const size_t ParticleNumber = ParticleNumberTotal / ParticleGroupNumberReal;

	// Struktura pojedynczej grupy cz¹stek
	struct ParticleGroup_t
	{
		// Instancja grupy cz¹stek
		ParticleGroup Instance;
		
		// W¹tek
		std::thread Thread;
	};

	// Kontener na grupy cz¹stek przypisane do w¹tków
	std::list<ParticleGroup_t> ParticleGroups;

	std::mt19937 RandomGenerator;

	// Tworzymy cz¹stki
	for(size_t I = 0; I < ParticleGroupNumberReal; I++)
	{
		ParticleGroups.emplace_back(ParticleGroup_t {
			// Stwórz grupê cz¹stek
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

			// Stwórz pusty w¹tek
			std::thread()
		});
	}

	// Uruchom w¹tki
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread = std::thread(&ParticleGroup::Run, &(PG.Instance));

	// Poczekaj na zakoñczenie
	for(ParticleGroup_t & PG : ParticleGroups)
		PG.Thread.join();

	// Aktualnie najlepszy wynik
	std::optional<const Particle> ParticleBest;

	// Historia wyników poszczególnych cz¹stek
	std::vector<ParticleGroup::HistoryEntries_t> HistoryEntries;
	HistoryEntries.reserve(ParticleGroups.size());

	// PrzejdŸ przez wszystkie cz¹stki i pobierz wyniki
	for(ParticleGroup_t & PG : ParticleGroups)
	{
		const std::optional<const Particle> & PGParticleBest = PG.Instance.GetParticleBest();

		// Jeœli cz¹stka nic nie znalaz³a - pomiñ
		if(!PGParticleBest)
			continue;

		// Jeœli ma lepszy wynik - zaktualizuj
		if(!ParticleBest || PGParticleBest.value().GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value()))
		{
			ParticleBest.emplace(
				PGParticleBest.value()
			);
		}

		// Dodaj historiê wyników
		HistoryEntries.push_back(
			PG.Instance.GetHistoryEntries()
		);
	}

	return {
		ParticleBest,
		HistoryEntries
	};
}