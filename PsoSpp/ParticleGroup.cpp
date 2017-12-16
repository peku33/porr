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
	// Inicjalizacja roju cz�stek
	Particles.reserve(ParticleNumber);
	for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		Particles.emplace_back(*this, RandomGenerator);

	// G��wna p�tla programu
	bool BetterSolutionFound = false;

	// Je�li przez kilka iteracji �adna z cz�stek nie ulegnie poprawie - wykonujemy reinicjalizacj� wag
	size_t ParticleBetterSolutionFoundNoCount = 0;

	for(size_t ParticleIteration = 0; ParticleIteration < ParticleIterations; ParticleIteration++)
	{
		// Czy kt�rakolwiek cz�sta uleg�a poprawie?
		bool ParticleBetterSolutionFoundAny = false;

		for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		{
			// Referencja na aktulan� cz�stk�
			Particle & ParticleCurrent = Particles.at(ParticleId);

			// Uruchom przeszukiwanie i sprawd� czy poprawili�my wynik
			const bool ParticleBetterSolutionFound = ParticleCurrent.Run();

			// Zapisujemy je�li cz�stka uleg�a poprawie
			ParticleBetterSolutionFoundAny = ParticleBetterSolutionFoundAny || ParticleBetterSolutionFound;

			// Je�li nie znaleziono lepszego rozwi�zania - jedziemy dalej
			if(!ParticleBetterSolutionFound)
				continue;

			// Je�li poprawiamy wynik otoczenia - zapisujemy go
			if(!ParticleBest || ParticleCurrent.GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value()))
			{
				ParticleBest.emplace(ParticleCurrent);
				BetterSolutionFound = true;

				// Zapisz histori� poprawy
				HistoryEntries.push_back({
					std::chrono::steady_clock::now(),
					ParticleBest.value().GetBestGraphPath().value().GetPathWeight()
				});
			}

			// Krok aktualizacji cz�stki danymi najlepszej cz�stki (je�li taka istnieje)
			if(ParticleBest)
			{
				ParticleCurrent.Update(
					Fi1,
					Fi2,
					ParticleBest.value()
				);
			}
		}

		// Czy jakakolwiek czastka uleg�a poprawie?
		if(!ParticleBetterSolutionFoundAny)
		{
			// �adna z cz�stek nie uleg�a poprawie - zwi�kszamy licznik martwych iteracji
			++ParticleBetterSolutionFoundNoCount;

			// Je�li licznik osi�gn�� du�� warto�� - zerujemy go i reinicjalizujemy cz�stki
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