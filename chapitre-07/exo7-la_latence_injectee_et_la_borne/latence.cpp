#include "NKMath/NkAngle.h"
#include "NKMath/NkQuat.h"

#include <cmath>
#include <cstdio>

namespace {

using nkentseu::float32;
using nkentseu::math::NkAngle;
using nkentseu::math::NkQuatf;
using nkentseu::math::NkVec3f;

constexpr double kPi = 3.14159265358979323846;
constexpr double kFrequenceHz = 72.0;
constexpr double kPeriode = 1.0 / kFrequenceHz;
constexpr unsigned int kCapacite = 64;
constexpr double kDebutGeste = 1.0;
constexpr double kDureeGeste = 0.400;

struct PoseTimee {
	double tempsSec = 0.0;
	NkQuatf pose;
	NkVec3f vitesse = NkVec3f(0.f, 0.f, 0.f);
};

struct Historique {
	PoseTimee poses[kCapacite];
	unsigned int tete = 0;
	unsigned int nombre = 0;

	void Ajouter(const PoseTimee &pose) {
		if (nombre == 0) {
			tete = 0;
			nombre = 1;
			poses[tete] = pose;
			return;
		}
		tete = (tete + 1u) % kCapacite;
		poses[tete] = pose;
		if (nombre < kCapacite) ++nombre;
	}

	const PoseTimee &PlusAncienne() const {
		const unsigned int index = (tete + kCapacite - (nombre - 1u)) % kCapacite;
		return poses[index];
	}

	// Même règle que le simulateur : dernier échantillon à ou avant la cible;
	// si l'historique ne remonte pas assez loin, son échantillon le plus ancien.
	const PoseTimee &EchantillonA(double maintenantSec, double latenceSec) const {
		const double cible = maintenantSec - latenceSec;
		unsigned int index = tete;
		for (unsigned int i = 0; i < nombre; ++i) {
			if (poses[index].tempsSec <= cible) return poses[index];
			index = (index + kCapacite - 1u) % kCapacite;
		}
		return PlusAncienne();
	}
};

struct Mesure {
	double moyenneAvec = 0.0;
	double maximumAvec = 0.0;
	double moyenneSans = 0.0;
	double maximumSans = 0.0;
	unsigned int nombre = 0;
};

struct Diagnostic {
	bool valide = false;
	double maintenantSec = 0.0;
	PoseTimee echantillon;
	double horizonSec = 0.0;
	NkQuatf prediteSansBorne;
	NkQuatf prediteAvecBorne;
	NkQuatf vraie;
	double ecartMilieuSec = 0.0;
};

double Lacet(double t) {
	const double amplitudeDeg = 90.0;
	if (t <= 0.0) return 0.0;
	if (t >= kDureeGeste) return amplitudeDeg;
	return amplitudeDeg * 0.5 * (1.0 - std::cos(kPi * t / kDureeGeste));
}

NkQuatf PoseVraie(double tempsSec) {
	return NkQuatf::RotateY(NkAngle::FromDeg(
		static_cast<float32>(Lacet(tempsSec - kDebutGeste))));
}

NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to,
						 float32 dtSeconds) {
	if (dtSeconds <= 0.f) return NkVec3f(0.f, 0.f, 0.f);
	NkQuatf delta = (to * from.Conjugate()).Normalized();
	if (delta.w < 0.f) {
		delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
	}
	const float32 angleRad = delta.Angle().Rad();
	if (angleRad <= 1e-6f) return NkVec3f(0.f, 0.f, 0.f);
	return delta.Axis() * (angleRad / dtSeconds);
}

NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse,
				   double dtSeconds, bool bornee) {
	if (bornee) {
		if (dtSeconds > 0.1) dtSeconds = 0.1;
		if (dtSeconds < -0.1) dtSeconds = -0.1;
	}
	const float32 speedRad = vitesse.Len();
	if (speedRad <= 1e-6f) return orientation;
	const NkVec3f axe = vitesse * (1.f / speedRad);
	const NkQuatf delta(NkAngle::FromRad(
		static_cast<float32>(speedRad * dtSeconds)), axe);
	return (delta * orientation).Normalized();
}

float32 ErreurAngulaireDeg(const NkQuatf &affichee, const NkQuatf &vraie) {
	return (vraie * affichee.Conjugate()).Normalized().Angle().Deg();
}

double LacetQuaternionDeg(const NkQuatf &q) {
	double deg = 2.0 * std::atan2(static_cast<double>(q.y),
								  static_cast<double>(q.w)) * 180.0 / kPi;
	if (deg > 180.0) deg -= 360.0;
	if (deg < -180.0) deg += 360.0;
	return deg;
}

bool Simuler(Mesure mesures[5], Diagnostic &diagnostic) {
	constexpr double latencesSec[5] = {0.0, 0.010, 0.020, 0.040, 0.080};
	Historique historique;
	NkQuatf precedente = PoseVraie(0.0);
	const unsigned int nombreFrames = static_cast<unsigned int>(3.0 / kPeriode + 0.5);

	for (unsigned int frame = 0; frame <= nombreFrames; ++frame) {
		const double maintenant = static_cast<double>(frame) * kPeriode;
		const NkQuatf pose = PoseVraie(maintenant);
		PoseTimee courant;
		courant.tempsSec = maintenant;
		courant.pose = pose;
		courant.vitesse = VitesseAngulaire(precedente, pose,
			static_cast<float32>(kPeriode));
		historique.Ajouter(courant);
		precedente = pose;

		if (maintenant >= kDebutGeste && maintenant < kDebutGeste + 0.9) {
			const NkQuatf vraiePourAffichage = PoseVraie(maintenant + kPeriode);
			for (unsigned int j = 0; j < 5; ++j) {
				const PoseTimee &echantillon = historique.EchantillonA(
					maintenant, latencesSec[j]);
				const NkQuatf predite = Extrapoler(echantillon.pose,
					echantillon.vitesse,
					maintenant + kPeriode - echantillon.tempsSec, true);
				const double avec = ErreurAngulaireDeg(predite, vraiePourAffichage);
				const double sans = ErreurAngulaireDeg(echantillon.pose,
					vraiePourAffichage);
				Mesure &m = mesures[j];
				m.moyenneAvec += avec;
				m.moyenneSans += sans;
				if (m.nombre == 0 || avec > m.maximumAvec) m.maximumAvec = avec;
				if (m.nombre == 0 || sans > m.maximumSans) m.maximumSans = sans;
				++m.nombre;
			}
		}

		// À latence d'une seconde, choisir l'image dont l'échantillon retourné
		// est le plus proche du milieu du geste (1,2 s).
		const PoseTimee &retardUneSeconde = historique.EchantillonA(maintenant, 1.0);
		const double ecart = std::fabs(retardUneSeconde.tempsSec - 1.2);
		if (!diagnostic.valide || ecart < diagnostic.ecartMilieuSec) {
			diagnostic.valide = true;
			diagnostic.maintenantSec = maintenant;
			diagnostic.echantillon = retardUneSeconde;
			diagnostic.horizonSec = maintenant + kPeriode - retardUneSeconde.tempsSec;
			diagnostic.prediteSansBorne = Extrapoler(retardUneSeconde.pose,
				retardUneSeconde.vitesse, diagnostic.horizonSec, false);
			diagnostic.prediteAvecBorne = Extrapoler(retardUneSeconde.pose,
				retardUneSeconde.vitesse, diagnostic.horizonSec, true);
			diagnostic.vraie = PoseVraie(maintenant + kPeriode);
			diagnostic.ecartMilieuSec = ecart;
		}
	}

	for (unsigned int j = 0; j < 5; ++j) {
		if (mesures[j].nombre == 0) return false;
		mesures[j].moyenneAvec /= mesures[j].nombre;
		mesures[j].moyenneSans /= mesures[j].nombre;
	}
	return diagnostic.valide;
}

} // namespace

int main() {
	Mesure mesures[5]{};
	Diagnostic diagnostic;
	if (!Simuler(mesures, diagnostic)) {
		std::fprintf(stderr, "Erreur : simulation incomplete.\n");
		return 1;
	}

	constexpr int latencesMs[5] = {0, 10, 20, 40, 80};
	for (unsigned int i = 0; i < 5; ++i) {
		std::printf("latence %d ms : avec %.6f deg (max %.6f), sans %.6f deg (max %.6f)\n",
			latencesMs[i], mesures[i].moyenneAvec, mesures[i].maximumAvec,
			mesures[i].moyenneSans, mesures[i].maximumSans);
	}

	const double horizonMs = diagnostic.horizonSec * 1000.0;
	std::printf("sans borne : echantillon a %.6f s, horizon %.3f ms, predit %.6f deg, vrai %.6f deg\n",
		diagnostic.echantillon.tempsSec, horizonMs,
		LacetQuaternionDeg(diagnostic.prediteSansBorne),
		LacetQuaternionDeg(diagnostic.vraie));
	std::printf("avec borne : echantillon a %.6f s, horizon %.3f ms, predit %.6f deg, vrai %.6f deg\n",
		diagnostic.echantillon.tempsSec, horizonMs,
		LacetQuaternionDeg(diagnostic.prediteAvecBorne),
		LacetQuaternionDeg(diagnostic.vraie));
	return 0;
}
