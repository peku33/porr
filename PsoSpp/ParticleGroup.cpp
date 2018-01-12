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
	// Inicjalizacja roju cząstek
	Particles.reserve(ParticleNumber);
	for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		Particles.emplace_back(*this, RandomGenerator);

	// Główna pętla programu
	bool BetterSolutionFound = false;

	// Jeśli przez kilka iteracji żadna z cząstek nie ulegnie poprawie - wykonujemy reinicjalizację wag
	size_t ParticleBetterSolutionFoundNoCount = 0;

	for(size_t ParticleIteration = 0; ParticleIteration < ParticleIterations; ParticleIteration++)
	{
		// Czy którakolwiek cząsta uległa poprawie?
		bool ParticleBetterSolutionFoundAny = false;

		for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		{
			// Referencja na aktulaną cząstkę
			Particle & ParticleCurrent = Particles.at(ParticleId);

			// Uruchom przeszukiwanie i sprawdź czy poprawiliśmy wynik
			const bool ParticleBetterSolutionFound = ParticleCurrent.Run();

			// Zapisujemy jeśli cząstka uległa poprawie
			ParticleBetterSolutionFoundAny = ParticleBetterSolutionFoundAny || ParticleBetterSolutionFound;

			// Jeśli cząstka znalazła lepszą ścieżkę - sprawdź, czy nie najlepszą
			// Jeśli poprawiamy wynik otoczenia - zapisujemy go
			if(ParticleBetterSolutionFound && (!ParticleBest || ParticleCurrent.GetBestGraphPath().value().IsBetterThan(ParticleBest.value().GetBestGraphPath().value())))
			{
				ParticleBest.emplace(ParticleCurrent);
				BetterSolutionFound = true;

				// Zapisz historię poprawy
				HistoryEntries.push_back({
					std::chrono::steady_clock::now(),
					ParticleIteration,
					ParticleBest.value().GetBestGraphPath().value().GetPathWeight()
				});
			}

			// Krok aktualizacji cząstki danymi najlepszej cząstki (jeśli taka istnieje)
			if(ParticleBest)
			{
				ParticleCurrent.Update(
					Fi1,
					Fi2,
					ParticleBest.value()
				);
			}
		}

		// Czy jakakolwiek czastka uległa poprawie?
		if(!ParticleBetterSolutionFoundAny)
		{
			// Żadna z cząstek nie uległa poprawie - zwiększamy licznik martwych iteracji
			++ParticleBetterSolutionFoundNoCount;

			// Jeśli licznik osiągnął dużą wartość - zerujemy go i reinicjalizujemy cząstki
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