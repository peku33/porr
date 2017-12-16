#include "ParticleGroup.hpp"

#include "Task.hpp"
#include "Particle.hpp"
#include "GraphPath.hpp"

ParticleGroup::ParticleGroup(const Task & T, const size_t & ParticleNumber, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2, const std::mt19937::result_type & Seed):
	T(T),
	ParticleNumber(ParticleNumber), ParticleIterations(ParticleIterations), ParticleBetterSolutionFoundNoCountMax(ParticleBetterSolutionFoundNoCountMax),
	Fi1(Fi1), Fi2(Fi2),
	RandomGenerator(Seed)
{

}

const Task & ParticleGroup::GetTask() const
{
	return T;
}

const std::optional<const Particle> & ParticleGroup::GetParticleBest() const
{
	return ParticleBest;
}

const ParticleGroup::HistoryEntries_t & ParticleGroup::GetHistoryEntries() const
{
	return HistoryEntries;
}


bool ParticleGroup::Run()
{
	// Inicjalizacja roju cz¹stek
	Particles.reserve(ParticleNumber);
	for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		Particles.emplace_back(*this, RandomGenerator);

	// G³ówna pêtla programu
	bool BetterSolutionFound = false;

	// Jeœli przez kilka iteracji ¿adna z cz¹stek nie ulegnie poprawie - wykonujemy reinicjalizacjê wag
	size_t ParticleBetterSolutionFoundNoCount = 0;

	for(size_t ParticleIteration = 0; ParticleIteration < ParticleIterations; ParticleIteration++)
	{
		// Czy którakolwiek cz¹sta uleg³a poprawie?
		bool ParticleBetterSolutionFoundAny = false;

		for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		{
			// Referencja na aktulan¹ cz¹stkê
			Particle & ParticleCurrent = Particles.at(ParticleId);

			// Uruchom przeszukiwanie i sprawdŸ czy poprawiliœmy wynik
			const bool ParticleBetterSolutionFound = ParticleCurrent.Run();

			// Zapisujemy jeœli cz¹stka uleg³a poprawie
			ParticleBetterSolutionFoundAny = ParticleBetterSolutionFoundAny || ParticleBetterSolutionFound;

			// Jeœli nie znaleziono lepszego rozwi¹zania - jedziemy dalej
			if(!ParticleBetterSolutionFound)
				continue;

			// Jeœli poprawiamy wynik otoczenia - zapisujemy go
			if(!ParticleBest || ParticleCurrent.GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value()))
			{
				ParticleBest.emplace(ParticleCurrent);
				BetterSolutionFound = true;

				// Zapisz historiê poprawy
				HistoryEntries.push_back({
					std::chrono::steady_clock::now(),
					ParticleBest.value().GetBestGraphPath().value().GetPathWeight()
				});
			}

			// Krok aktualizacji cz¹stki danymi najlepszej cz¹stki (jeœli taka istnieje)
			if(ParticleBest)
			{
				ParticleCurrent.Update(
					Fi1,
					Fi2,
					ParticleBest.value()
				);
			}
		}

		// Czy jakakolwiek czastka uleg³a poprawie?
		if(!ParticleBetterSolutionFoundAny)
		{
			// ¯adna z cz¹stek nie uleg³a poprawie - zwiêkszamy licznik martwych iteracji
			++ParticleBetterSolutionFoundNoCount;

			// Jeœli licznik osi¹gn¹³ du¿¹ wartoœæ - zerujemy go i reinicjalizujemy cz¹stki
			if(ParticleBetterSolutionFoundNoCount >= ParticleBetterSolutionFoundNoCountMax)
			{
				for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
					Particles.at(ParticleId).RandomInitialize();

				// Zaczynamy liczenie od 0
				ParticleBetterSolutionFoundNoCount = 0;
			}
		}
		else
		{
			// Tak - zerujemy licznik
			ParticleBetterSolutionFoundNoCount = 0;
		}
	}

	return BetterSolutionFound;
}