// attenuations.cpp : le gain des quatre modeles d'attenuation, mesure avec le moteur lui-meme
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKLogger/NkLog.h"
#include "NKAudio/NkAudio.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <thread>
#include <chrono>

using namespace nkentseu;
using namespace nkentseu::audio;

static const int RATE = 48000;
static const int TRAMES = 48000;  // une seconde de son par mesure
static const int BLOC = 256;      // rendu par blocs de 256 trames
static const float AMPLITUDE = 0.1f;

static const AttenuationModel MODELES[4] = {AttenuationModel::NONE, AttenuationModel::INVERSE,
                                            AttenuationModel::LINEAR, AttenuationModel::EXPONENTIAL};
static const char *NOMS[4] = {"NONE", "INVERSE", "LINEAR", "EXPONENTIAL"};
static const float DISTANCES[5] = {0.5f, 1.f, 2.f, 4.f, 8.f};

// Joue le son en boucle devant l'auditeur, puis rend TRAMES trames par blocs de BLOC.
// Les echantillons sont ranges gauche, droite, gauche, droite : on mesure les deux oreilles ensemble.
static double Mesure(AudioEngine &eng, const AudioSample &tone, AttenuationModel modele, float d, float rolloff) {
    VoiceParams p;
    p.looping = true;
    p.source3d.positional = true;
    p.source3d.minDistance = 1.f;
    p.source3d.maxDistance = 100.f;
    p.source3d.rolloffFactor = rolloff;
    p.source3d.attenuationModel = modele;
    p.source3d.position[0] = 0.f;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = -d;  // devant l'auditeur, qui regarde vers -Z

    AudioHandle h = eng.Play(tone, p);

    static float buf[BLOC * 2];
    double somme = 0.0;
    long long n = 0;
    for (int f = 0; f < TRAMES; f += BLOC) {
        const int trames = (TRAMES - f < BLOC) ? (TRAMES - f) : BLOC;
        eng.RenderToBuffer(buf, trames, 2);
        for (int i = 0; i < trames * 2; ++i) {
            somme += (double)buf[i] * (double)buf[i];
            ++n;
        }
    }
    eng.Stop(h, 0.f);
    // on rend deux blocs de plus pour que la voix arretee soit bien retiree avant la mesure suivante
    eng.RenderToBuffer(buf, BLOC, 2);
    eng.RenderToBuffer(buf, BLOC, 2);
    return (n > 0) ? sqrt(somme / (double)n) : 0.0;
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoAtt";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    const char *ecoute = getenv("NK_ECOUTE");
    const bool modeEcoute = (ecoute && atoi(ecoute) == 1);

    AudioEngineConfig cfg;
    if (!modeEcoute)
        cfg.backend = AudioBackendType::NULL_OUTPUT;  // mesures : ni fenetre ni sortie sonore

    AudioEngine &eng = AudioEngine::Instance();
    if (!eng.Initialize(cfg)) {
        logger.Error("[Att] Initialize KO");
        return 1;
    }
    eng.SetListenerPosition(0.f, 0.f, 0.f);
    eng.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);  // avant (0,0,-1), haut (0,1,0)

    AudioSample tone = AudioGenerator::GenerateTone(440.f, 2.f, WaveformType::SINE, RATE, AMPLITUDE);

    if (modeEcoute) {
        // Ecoute au casque : NK_MODELE = INVERSE ou LINEAR, NK_DIST = distance en metres
        const char *m = getenv("NK_MODELE");
        const char *dd = getenv("NK_DIST");
        const float d = dd ? (float)atof(dd) : 1.f;
        AttenuationModel modele = AttenuationModel::INVERSE;
        if (m && strcmp(m, "LINEAR") == 0)
            modele = AttenuationModel::LINEAR;
        VoiceParams p;
        p.looping = true;
        p.source3d.positional = true;
        p.source3d.minDistance = 1.f;
        p.source3d.maxDistance = 100.f;
        p.source3d.rolloffFactor = 1.f;
        p.source3d.attenuationModel = modele;
        p.source3d.position[2] = -d;
        logger.Info("[Att] ecoute : {0} a {1} m", m ? m : "INVERSE", d);
        AudioHandle h = eng.Play(tone, p);
        std::this_thread::sleep_for(std::chrono::seconds(4));
        eng.Stop(h, 0.f);
        eng.Shutdown();
        return 0;
    }

    // Les quarante mesures : 2 reglages de rolloff x 4 modeles x 5 distances
    FILE *f = fopen("Applications/ExoAtt/mesures.txt", "w");
    if (!f) {
        logger.Error("[Att] impossible d'ecrire mesures.txt");
        eng.Shutdown();
        return 2;
    }
    for (int r = 1; r <= 2; ++r) {
        for (int m = 0; m < 4; ++m) {
            for (int di = 0; di < 5; ++di) {
                const float d = DISTANCES[di];
                const double ref = Mesure(eng, tone, AttenuationModel::NONE, d, (float)r);  // NONE a la meme distance
                const double val = Mesure(eng, tone, MODELES[m], d, (float)r);
                const double gain = (ref > 0.0) ? val / ref : 0.0;
                fprintf(f, "%s, rolloff %d, %g m : %.3f\n", NOMS[m], r, d, gain);
                logger.Info("[Att] {0}, rolloff {1}, {2} m : rms = {3}, rms NONE = {4}, gain = {5}", NOMS[m], r, d, val, ref, gain);
            }
        }
    }
    fclose(f);
    logger.Info("[Att] mesures terminees : Applications/ExoAtt/mesures.txt");
    eng.Shutdown();
    return 0;
}
