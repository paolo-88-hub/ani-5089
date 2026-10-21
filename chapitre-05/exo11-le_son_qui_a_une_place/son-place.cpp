// son-place.cpp : salle 4 m x 4 m (porte, fenetres, table) avec PoserAuSol / PoserSurTable
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "NKAudio/NkAudio.h"

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::audio;

// --- Salle (en metres) ---
static const float INTERIEUR = 2.00f;
static const float EP_MUR = 0.10f;
static const float HAUTEUR = 2.50f;
static const float PANNEAU = 0.02f;
static const float AVANCE = 0.00009f;

// --- Table (en metres) ---
static const float LONGUEUR = 1.20f;
static const float PROFONDEUR = 0.80f;
static const float EP_PLATEAU = 0.05f;
static const float DESSUS = 0.75f;
static const float PIED = 0.06f;
static const float TABLE_X = 0.60f;
static const float TABLE_Z = 0.70f;
static const float PLATEAU_Y = DESSUS - EP_PLATEAU * 0.5f;
static const float PIED_H = DESSUS - EP_PLATEAU;
static const float PIED_DX = LONGUEUR * 0.5f - PIED * 0.5f;
static const float PIED_DZ = PROFONDEUR * 0.5f - PIED * 0.5f;

// --- Les deux fonctions ---
// Le cube du moteur est unitaire et centre : apres l'echelle, sa hauteur est taille.y,
// donc le centre est monte de taille.y / 2. Translation a gauche : elle agit apres l'echelle.
NkMat4f PoserAuSol(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate({x, taille.y * 0.5f, z})
         * NkMat4f::Scale(taille);
}

NkMat4f PoserSurTable(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate({x, 0.75f + taille.y * 0.5f, z})
         * NkMat4f::Scale(taille);
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoSonPlace";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "La salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Poser] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Poser] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[Poser] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Poser] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // ===== DEBUT SON =====
    // Choix de la position de la camera (1, 2 ou 3) et du mode, a chaque lancement
    int numPos = 1;
    const char *ePos = std::getenv("POSITION_CAMERA");
    if (ePos) numPos = std::atoi(ePos);
    const char *eSans = std::getenv("SANS_POSITIONAL");
    const bool positionnel = !(eSans && eSans[0] == '1');

    // 1 : pres de la fenetre, regard vers -Z ; 2 : au centre ; 3 : coin oppose
    NkVec3f camPos = {-1.2f, 1.60f, -0.9f};
    NkVec3f camCible = {-1.2f, 1.60f, -2.0f};
    if (numPos == 2) { camPos = {0.f, 1.60f, 0.f}; camCible = {0.f, 1.60f, -1.f}; }
    if (numPos == 3) { camPos = {1.7f, 1.70f, 1.7f}; camCible = {-0.3f, 0.8f, -0.3f}; }

    float fx = camCible.x - camPos.x;
    float fy = camCible.y - camPos.y;
    float fz = camCible.z - camPos.z;
    float norme = std::sqrt(fx * fx + fy * fy + fz * fz);
    fx /= norme; fy /= norme; fz /= norme;

    AudioEngineConfig cfgSon;
    cfgSon.sampleRate = 48000;
    if (!AudioEngine::Instance().Initialize(cfgSon)) {
        logger.Error("[Son] Init moteur de son KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 4;
    }

    // L'auditeur prend la place de la camera, une fois au demarrage
    AudioEngine::Instance().SetListenerPosition(camPos.x, camPos.y, camPos.z);
    AudioEngine::Instance().SetListenerOrientation(fx, fy, fz, 0.f, 1.f, 0.f);

    // La pluie : bruit rose de 4 s, en boucle, placee a la premiere fenetre
    AudioSample pluie = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);
    VoiceParams p;
    p.looping = true;
    p.source3d.position[0] = -1.98f;
    p.source3d.position[1] = 1.40f;
    p.source3d.position[2] = -0.90f;
    p.source3d.positional = positionnel;
    p.source3d.minDistance = 0.5f;
    p.source3d.maxDistance = 8.0f;
    AudioEngine::Instance().Play(pluie, p);

    std::cout << "position camera : " << numPos << " (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")" << std::endl;
    std::cout << "positional : " << (positionnel ? 1 : 0) << " ; minDistance 0.5 m ; maxDistance 8 m" << std::endl;
    // ===== FIN SON =====
    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            running = false;
    });

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!renderer->BeginFrame())
            continue;

        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);
        cam.SetPosition(camPos);
        cam.SetTarget(camCible);

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f;
        sun.castShadow = false;
        sctx.lights.PushBack(sun);

        NkLightDesc fill;
        fill.type = NkLightType::NK_DIRECTIONAL;
        fill.direction = {-0.6f, -0.3f, -0.8f};
        fill.color = {1.f, 1.f, 1.f};
        fill.intensity = 2.f;
        fill.castShadow = false;
        sctx.lights.PushBack(fill);

        r3d->BeginScene(sctx);

        // ===== DEBUT DU BLOC COMPTE =====
        // Un cube de matrice m, de dimensions t, dont le centre est en (x, z) et le bas a yBas.
        auto objet = [&](const NkMat4f &m, NkVec3f t, float x, float yBas, float z, float r, float g, float b) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = m;
            dc.aabb = {{x - t.x * 0.5f, yBas, z - t.z * 0.5f}, {x + t.x * 0.5f, yBas + t.y, z + t.z * 0.5f}};
            dc.tint = {r, g, b};
            dc.metallic = 0.f;
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        };

        const float centreMur = INTERIEUR + EP_MUR * 0.5f;
        const float longueur = 2.f * INTERIEUR + 2.f * EP_MUR;

        // Sol : un plan, pas de demi-hauteur, il ne passe pas par PoserAuSol
        {
            NkDrawCall3D dc;
            dc.mesh = meshPlane;
            dc.transform = NkMat4f::Scale({INTERIEUR, 1.f, INTERIEUR});
            dc.aabb = {{-INTERIEUR, 0.f, -INTERIEUR}, {INTERIEUR, 0.f, INTERIEUR}};
            dc.tint = {0.30f, 0.30f, 0.33f};
            dc.metallic = 0.f;
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        }

        // Murs (au sol) et plafond (en hauteur, transformation explicite)
        objet(PoserAuSol({longueur, HAUTEUR, EP_MUR}, 0.f, -centreMur), {longueur, HAUTEUR, EP_MUR}, 0.f, 0.f, -centreMur, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({longueur, HAUTEUR, EP_MUR}, 0.f, centreMur), {longueur, HAUTEUR, EP_MUR}, 0.f, 0.f, centreMur, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({EP_MUR, HAUTEUR, 2.f * INTERIEUR}, -centreMur, 0.f), {EP_MUR, HAUTEUR, 2.f * INTERIEUR}, -centreMur, 0.f, 0.f, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({EP_MUR, HAUTEUR, 2.f * INTERIEUR}, centreMur, 0.f), {EP_MUR, HAUTEUR, 2.f * INTERIEUR}, centreMur, 0.f, 0.f, 0.72f, 0.70f, 0.66f);
        objet(NkMat4f::Translate({0.f, HAUTEUR + EP_MUR * 0.5f, 0.f}) * NkMat4f::Scale({longueur, EP_MUR, longueur}), {longueur, EP_MUR, longueur}, 0.f, HAUTEUR, 0.f, 0.85f, 0.85f, 0.85f);

        const float posPanneau = INTERIEUR + PANNEAU * 0.5f - AVANCE;

        // Porte : posee au sol, mur du fond
        objet(PoserAuSol({0.90f, 2.00f, PANNEAU}, -0.70f, -posPanneau), {0.90f, 2.00f, PANNEAU}, -0.70f, 0.f, -posPanneau, 0.45f, 0.28f, 0.15f);

        // Fenetres : centre a 1,40 m du sol, donc pas au sol (transformation explicite)
        objet(NkMat4f::Translate({-posPanneau, 1.40f, -0.90f}) * NkMat4f::Scale({PANNEAU, 1.00f, 1.20f}), {PANNEAU, 1.00f, 1.20f}, -posPanneau, 0.90f, -0.90f, 0.55f, 0.75f, 0.95f);
        objet(NkMat4f::Translate({-posPanneau, 1.40f, 0.90f}) * NkMat4f::Scale({PANNEAU, 1.00f, 1.20f}), {PANNEAU, 1.00f, 1.20f}, -posPanneau, 0.90f, 0.90f, 0.55f, 0.75f, 0.95f);

        // Plateau : il porte les objets, il ne repose pas sur le sol (transformation explicite)
        objet(NkMat4f::Translate({TABLE_X, PLATEAU_Y, TABLE_Z}) * NkMat4f::Scale({LONGUEUR, EP_PLATEAU, PROFONDEUR}), {LONGUEUR, EP_PLATEAU, PROFONDEUR}, TABLE_X, DESSUS - EP_PLATEAU, TABLE_Z, 0.62f, 0.42f, 0.22f);

        // Quatre pieds, au sol
        const float signes[2] = {-1.f, 1.f};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                objet(PoserAuSol({PIED, PIED_H, PIED}, TABLE_X + signes[i] * PIED_DX, TABLE_Z + signes[j] * PIED_DZ), {PIED, PIED_H, PIED}, TABLE_X + signes[i] * PIED_DX, 0.f, TABLE_Z + signes[j] * PIED_DZ, 0.40f, 0.26f, 0.14f);
            }
        }
        // ===== FIN DU BLOC COMPTE =====

        // Objet pose sur la table (hors comptage : il n'existait pas avant)
        objet(PoserSurTable({0.20f, 0.30f, 0.20f}, TABLE_X, TABLE_Z), {0.20f, 0.30f, 0.20f}, TABLE_X, DESSUS, TABLE_Z, 0.20f, 0.45f, 0.70f);

        renderer->Present();
        renderer->EndFrame();
    }

    // ===== DEBUT SON =====
    AudioEngine::Instance().Shutdown();
    AudioLoader::Free(pluie);
    // ===== FIN SON =====
    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
