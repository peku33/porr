#pragma once

#include <vector>
#include <optional>

#include "Particle.hpp"
#include "GraphPath.hpp"

class Task;

/*
	Klasa kontenera roju cz¹stek

	Lokalna grupa cz¹stek, któr¹ mo¿na uruchomiæ na jednym w¹tku. Wszystkie zmienne przyjmowane jako referencje do sta³ych obiektów - w za³o¿eniu gwarantuje to w¹tkow¹ bezpiecznoœæ.
*/
class ParticleGroup
{
	public:
		/*
			Konstruktor

			T - Zadanie optymalizacji

			ParticleNumber - Liczba cz¹stek
			ParticleIterations - Liczba iteracji
			ParticleBetterSolutionFoundNoCountMax - po ilu pe³nych iteracjach bez poprawy jakiejkolwiek cz¹stki wykonaæ reset wag?

			Fi1, Fi2 - Parametry dostrajalne metody aktualizacji œcie¿ki pojedynczej cz¹stki. Opisane w Particle.hpp
		*/
		ParticleGroup(const Task & T, const size_t & ParticleNumber, const size_t & ParticleIterations, const size_t & ParticleBetterSolutionFoundNoCountMax, const double & Fi1, const double & Fi2);

	private:
		const Task & T;

		const size_t ParticleNumber;
		const size_t ParticleIterations;
		const size_t ParticleBetterSolutionFoundNoCountMax;

		const double Fi1;
		const double Fi2;

	private:
		/*
			Kontener cz¹stek algorytmu, inicjowany w konstuktorze.
		*/
		std::vector<Particle> Particles; /* ParticleNumber */

		/*
			Zatrzaœniêta instancja aktualnie najlepszej cz¹stki
		*/
		std::optional<const Particle> ParticleBest;

		/*
			Historia wag œciezek w kolejnych iteracjach
		*/
		std::list<std::optional<GraphPath::PathWeight_t>> PathWeights;

	public:
		const Task & GetTask() const;
		const std::optional<const Particle> & GetParticleBest() const;
		const std::list<std::optional<GraphPath::PathWeight_t>> & GetPathWeights() const;

	public:
		/*
			G³ówna pêtla programu.

			Zwraca czy znaleziono choæ jedno rozwi¹zanie problemu
		*/
		bool Run();
};

