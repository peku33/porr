#include "ParticleGroup.hpp"

#include "Task.hpp"
#include "Particle.hpp"
#include "GraphPath.hpp"

#define DEBUG_THIS

#ifdef DEBUG_THIS
	#include <iostream>
#endif

ParticleGroup::ParticleGroup(const Task & T, const size_t & ParticleNumber, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2, std::mt19937 & RandomGenerator, const int & MpiRankMy, const int & MpiRankTotal, const MPI_Comm & MpiComm):
	T(T),
	ParticleNumber(ParticleNumber), ParticleIterations(ParticleIterations), ParticleBetterSolutionFoundNoCountMax(ParticleBetterSolutionFoundNoCountMax),
	Fi1(Fi1), Fi2(Fi2),
	MpiRankMy(MpiRankMy), MpiRankTotal(MpiRankTotal), MpiComm(MpiComm),
	RandomGenerator(RandomGenerator)
{

}

const Task & ParticleGroup::GetTask() const
{
	return T;
}

const std::optional<const GraphPath> & ParticleGroup::GetGraphPathBest() const
{
	return GraphPathBest;
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
		#ifdef DEBUG_THIS
			std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Iteration: " << ParticleIteration << std::endl;
		#endif

		// Czy którakolwiek cząsta uległa poprawie?
		bool ParticleBetterSolutionFoundAny = false;

		for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
		{
			//=================================================================
			// Odebranie globalnie najlepszej cząstki
			//=================================================================

			// Czy którykolwiek ze zdalnych procesów zgłosił poprawę ścieżki?
			// Dopóki kolejka nie jest pusta - pobieramy kolejne wartości
			while(true)
			{
				//#ifdef DEBUG_THIS
				//	std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBestGraphPathOpt start" << std::endl;
				//#endif
				std::optional<GraphPath> MpiBestGraphPathOpt = GraphPath::MpiTryBroadcastReceive(T, MpiComm);
				//#ifdef DEBUG_THIS
				//	std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBestGraphPathOpt end" << std::endl;
				//#endif

				// Nie ma żadnej przychodzącej ścieżki - przerywamy pobieranie
				if(!MpiBestGraphPathOpt)
					break;

				#ifdef DEBUG_THIS
					std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBestGraphPathOpt received" << std::endl;
				#endif

				// Wyłuskujemy obiekt ścieżki
				GraphPath & MpiBestGraphPath = MpiBestGraphPathOpt.value();

				// Czy ścieżka jest lepsza?
				if(!GraphPathBest || MpiBestGraphPath.IsBetterThan(GraphPathBest.value()))
				{
					#ifdef DEBUG_THIS
						std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBestGraphPathOpt better: " << MpiBestGraphPath.GetPathWeight() << std::endl;
					#endif

					// Tak - zmieniamy naszą na nową
					GraphPathBest.emplace(std::move(MpiBestGraphPath));
				}
			}

			//=================================================================
			// Lokalna iteracja
			//=================================================================

			// Referencja na aktulaną cząstkę
			Particle & ParticleCurrent = Particles.at(ParticleId);

			// Uruchom przeszukiwanie i sprawdź czy poprawiliśmy wynik
			// Czy cząstka poprawiła samą siebie?
			const bool ParticleBetterSolutionFound = ParticleCurrent.Run();

			// Zapisujemy jeśli cząstka uległa poprawie
			ParticleBetterSolutionFoundAny = ParticleBetterSolutionFoundAny || ParticleBetterSolutionFound;

			// Czy ścieżka znaleziona przez cząstkę jest lepsza od maksimum runnera?
			bool ParticleSolutionBetter = false;

			// Jeśli cząstka znalazła lepszą ścieżkę - sprawdź, czy nie najlepszą
			// Jeśli poprawiamy wynik otoczenia - zapisujemy go
			if(ParticleBetterSolutionFound && (!GraphPathBest || ParticleCurrent.GetGraphPathBest().value().IsBetterThan(GraphPathBest.value())))
			{
				// Zapamiętaj najlepszą ścieżkę
				GraphPathBest.emplace(ParticleCurrent.GetGraphPathBest().value());

				// Obecna cząstka znalazła lepsze rozwiązanie
				ParticleSolutionBetter = true;

				// Runner znalazł lepsze rozwiązanie
				BetterSolutionFound = true;

				// Zapisz historię poprawy
				HistoryEntries.push_back({
					std::chrono::steady_clock::now(),
					ParticleIteration,
					GraphPathBest.value().GetPathWeight()
				});
			}

			ParticleCurrent.Update(
				Fi1,
				Fi2,
				GraphPathBest
			);

			//=================================================================
			// Publikacja wyników
			//=================================================================
			if(ParticleSolutionBetter)
			{
				#ifdef DEBUG_THIS
					std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  ParticleSolutionBetter: " << GraphPathBest.value().GetPathWeight() << std::endl;
				#endif

				// Jeśli nastąpiła poprawa - publikuj wyniki
				#ifdef DEBUG_THIS
					std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBroadcastSend start" << std::endl;
				#endif
				GraphPathBest.value().MpiBroadcastSend(MpiRankMy, MpiRankTotal, MpiComm);
				#ifdef DEBUG_THIS
					std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  MpiBroadcastSend end" << std::endl;
				#endif
			}
		}

		// Czy jakakolwiek czastka uległa poprawie?
		if(ParticleBetterSolutionFoundAny)
		{
			// Tak - zerujemy licznik
			ParticleBetterSolutionFoundNoCount = 0;
		}
		else
		{
			// Żadna z cząstek nie uległa poprawie - zwiększamy licznik martwych iteracji
			++ParticleBetterSolutionFoundNoCount;

			// Jeśli licznik osiągnął dużą wartość - zerujemy go i reinicjalizujemy cząstki
			if(ParticleBetterSolutionFoundNoCount >= ParticleBetterSolutionFoundNoCountMax)
			{
				#ifdef DEBUG_THIS
					std::cout << "[" << MpiRankMy << " @ " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() << "]  Reinitializing" << std::endl;
				#endif

				for(size_t ParticleId = 0; ParticleId < ParticleNumber; ParticleId++)
					Particles.at(ParticleId).RandomInitialize();

				// Zaczynamy liczenie od 0
				ParticleBetterSolutionFoundNoCount = 0;
			}
		}
	}

	return BetterSolutionFound;
}