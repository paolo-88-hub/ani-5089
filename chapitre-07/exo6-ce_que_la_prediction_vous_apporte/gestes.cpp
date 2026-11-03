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

struct Geste {
	const char *nom;
	double amplitudeDeg;
	double dureeSec;
};

// Quart de cosinus : la vitesse est nulle au départ et à l'arrivée.
double Lacet(double t, double amplitudeDeg, double dureeSec) {
	if (t <= 0.0) return 0.0;
	if (t >= dureeSec) return amplitudeDeg;
	return amplitudeDeg * 0.5 * (1.0 - std::cos(kPi * t / dureeSec));
}

NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to,
						 float32 dtSeconds) {
	if (dtSeconds <= 0.f) return NkVec3f(0.f, 0.f, 0.f);

	NkQuatf delta = (to * from.Conjugate()).Normalized();
	// q et -q représentent la même rotation : garder le chemin court.
	if (delta.w < 0.f) {
		delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
	}
	const float32 angleRad = delta.Angle().Rad();
	if (angleRad <= 1e-6f) return NkVec3f(0.f, 0.f, 0.f);
	return delta.Axis() * (angleRad / dtSeconds);
}

NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse,
				   float32 dtSeconds) {
	const float32 speedRad = vitesse.Len();
	if (speedRad <= 1e-6f) return orientation;

	const NkVec3f axis = vitesse * (1.f / speedRad);
	const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
	return (delta * orientation).Normalized();
}

float32 ErreurAngulaireDeg(const NkQuatf &a, const NkQuatf &b) {
	return (b * a.Conjugate()).Normalized().Angle().Deg();
}

bool Mesurer(const Geste &geste, double &moyenneAvec, double &moyenneSans) {
	if (geste.dureeSec <= 0.0 || geste.amplitudeDeg < 0.0) return false;

	double sommeAvec = 0.0;
	double sommeSans = 0.0;
	unsigned long long nombre = 0;
	NkQuatf precedente = NkQuatf::RotateY(NkAngle::FromDeg(
		static_cast<float32>(Lacet(0.0, geste.amplitudeDeg, geste.dureeSec))));

	// Une mesure par période, uniquement pendant le geste. La première pose
	// est au temps zéro; chaque pose est comparée à la vérité une période après.
	for (unsigned long long i = 0;; ++i) {
		const double t = static_cast<double>(i) * kPeriode;
		if (t >= geste.dureeSec) break;

		const NkQuatf pose = NkQuatf::RotateY(NkAngle::FromDeg(
			static_cast<float32>(Lacet(t, geste.amplitudeDeg, geste.dureeSec))));
		const NkQuatf vraiePose = NkQuatf::RotateY(NkAngle::FromDeg(
			static_cast<float32>(Lacet(t + kPeriode, geste.amplitudeDeg, geste.dureeSec))));
		const NkVec3f vitesse = VitesseAngulaire(precedente, pose,
			static_cast<float32>(kPeriode));
		const NkQuatf predite = Extrapoler(pose, vitesse,
			static_cast<float32>(kPeriode));

		sommeAvec += ErreurAngulaireDeg(vraiePose, predite);
		sommeSans += ErreurAngulaireDeg(vraiePose, pose);
		++nombre;
		precedente = pose;
	}

	if (nombre == 0) return false;
	moyenneAvec = sommeAvec / static_cast<double>(nombre);
	moyenneSans = sommeSans / static_cast<double>(nombre);
	return std::isfinite(moyenneAvec) && std::isfinite(moyenneSans);
}

void Imprimer(const Geste &geste) {
	double avec = 0.0;
	double sans = 0.0;
	if (!Mesurer(geste, avec, sans)) {
		std::printf("%s : mesure impossible\n", geste.nom);
		return;
	}
	const double rapport = avec > 1e-12 ? sans / avec : 0.0;
	std::printf("%s : avec %.6f deg, sans %.6f deg, rapport %.6f\n",
			geste.nom, avec, sans, rapport);
}

} // namespace

int main() {
	constexpr Geste gestes[] = {
		{"rapide (90 deg en 400 ms)", 90.0, 0.400},
		{"lent (30 deg en 2000 ms)", 30.0, 2.000},
		{"demi-tour (180 deg en 160 ms)", 180.0, 0.160},
	};
	for (const Geste &geste : gestes) Imprimer(geste);
	return 0;
}
