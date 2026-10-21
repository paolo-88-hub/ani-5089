// mesure.cpp : mesure, oreille par oreille, ce que le moteur envoie (sans fenetre, sans sortie sonore)
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"

#include <cstdio>
#include <cmath>
#include <vector>
#include "NKAudio/NkAudio.h"

using namespace nkentseu;
using namespace nkentseu::audio;

static const int TRAMES_TOTAL = 48000;
static const int TRAMES_BLOC = 256;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoMesure";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

// Place la pluie en (x, 0, z), rend 48000 trames par blocs de 256, affiche la RMS de chaque oreille.
static void Mesurer(const char *nom, const AudioSample &pluie, float x, float z, bool positional) {
    AudioEngine &moteur = AudioEngine::Instance();

    VoiceParams p;
    p.looping = true;
    p.source3d.position[0] = x;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = z;
    p.source3d.positional = positional;
    p.source3d.minDistance = 0.5f;
    p.source3d.maxDistance = 8.0f;
    AudioHandle voix = moteur.Play(pluie, p);

    std::vector<float> bloc(TRAMES_BLOC * 2);   // gauche, droite, gauche, droite...
    double sommeG = 0.0;
    double sommeD = 0.0;
    int fait = 0;
    while (fait < TRAMES_TOTAL) {
        int n = TRAMES_TOTAL - fait;
        if (n > TRAMES_BLOC) n = TRAMES_BLOC;
        moteur.RenderToBuffer(bloc.data(), n);
        for (int i = 0; i < n; ++i) {
            double g = bloc[2 * i];
            double d = bloc[2 * i + 1];
            sommeG += g * g;
            sommeD += d * d;
        }
        fait += n;
    }
    moteur.Stop(voix);   // la source ne bouge pas pendant une mesure : on l'arrete avant de la replacer

    double rmsG = std::sqrt(sommeG / TRAMES_TOTAL);
    double rmsD = std::sqrt(sommeD / TRAMES_TOTAL);
    std::printf("%s %d : G %.6f D %.6f\n", nom, positional ? 1 : 0, rmsG, rmsD);
    std::fflush(stdout);
}

int nkmain(const NkEntryState &state) {
    (void)state;

    AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    if (!AudioEngine::Instance().Initialize(cfg)) {
        std::printf("Init moteur de son KO\n");
        return 1;
    }

    // L'auditeur est a l'origine et regarde vers -Z
    AudioEngine::Instance().SetListenerPosition(0.f, 0.f, 0.f);
    AudioEngine::Instance().SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);

    AudioSample pluie = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);

    Mesurer("gauche", pluie, -2.f, 0.f, true);
    Mesurer("gauche", pluie, -2.f, 0.f, false);
    Mesurer("devant", pluie, 0.f, -2.f, true);
    Mesurer("devant", pluie, 0.f, -2.f, false);
    Mesurer("droite", pluie, 2.f, 0.f, true);
    Mesurer("droite", pluie, 2.f, 0.f, false);

    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(pluie);
    return 0;
}