// interfaces.cpp : la meme salle sur deux interfaces graphiques imposees.
// Choix de l'interface par la variable d'environnement NK_API :
//   software | opengl   (defaut : software)
#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

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

using namespace nkentseu;
using namespace nkentseu::renderer;

// --- Les deux interfaces imposees ---
static const NkGraphicsApi API_LOGICIEL = NkGraphicsApi::NK_GFX_API_SOFTWARE; // rendu logiciel
static const NkGraphicsApi API_SECONDE = NkGraphicsApi::NK_GFX_API_OPENGL;    // seconde interface

// --- Mesure ---
static const int IMAGES_ECHAUFFEMENT = 30; // chargements : non comptees
static const int IMAGES_MESUREES = 400;

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

NkMat4f PoserAuSol(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate({x, taille.y * 0.5f, z})
         * NkMat4f::Scale(taille);
}

NkMat4f PoserSurTable(NkVec3f taille, float x, float z) {
    return NkMat4f::Translate({x, 0.75f + taille.y * 0.5f, z})
         * NkMat4f::Scale(taille);
}

// Ecrit le resultat (nom + temps, ou refus + message) dans resultat_<variante>.txt,
// dans le dossier donne par NK_SORTIE s'il existe.
static void EcrireResultat(const std::string &variante, const std::string &texte) {
    const char *dir = std::getenv("NK_SORTIE");
    std::string chemin = (dir && *dir) ? std::string(dir) + "\\" : std::string();
    std::ofstream f(chemin + "resultat_" + variante + ".txt");
    f << texte << "\n";
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoInterfaces";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    const char *envVar = std::getenv("NK_API");
    const std::string variante = (envVar && *envVar) ? envVar : "software";
    const NkGraphicsApi api = (variante == "opengl") ? API_SECONDE : API_LOGICIEL;

    NkWindowConfig winCfg;
    winCfg.title = "La salle";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Interfaces] Creation fenetre KO");
        EcrireResultat(variante, "refus Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    // L'interface est imposee ici : plus de detection automatique
    NkIDevice *device = NkDeviceFactory::CreateForApi(api, devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Interfaces] Creation device KO pour {0}", NkGraphicsApiName(api));
        EcrireResultat(variante, std::string("refus Creation device KO pour ") + std::string(NkGraphicsApiName(api)));
        if (device)
            NkDeviceFactory::Destroy(device);
        window.Close();
        return 2;
    }
    logger.Info("[Interfaces] Interface : {0}", NkGraphicsApiName(device->GetApi()));

    // Le renderer recoit la meme interface que le peripherique : CreateForApi ne la recopie pas
    NkRendererConfig cfg = NkRendererConfig::ForGame(api, devInfo.width, devInfo.height);
    cfg.api = api;
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Interfaces] Init renderer KO pour {0}", NkGraphicsApiName(api));
        EcrireResultat(variante, std::string("refus Init renderer KO pour ") + std::string(NkGraphicsApiName(api)));
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    bool running = true;
    uint32 W = devInfo.width;
    uint32 H = devInfo.height;
    NkEventSystem &events = NkEvents();
    events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *) { running = false; });
    events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e) {
        if (e->GetKey() == NkKey::NK_ESCAPE)
            running = false;
    });

    using Horloge = std::chrono::steady_clock;
    int image = 0;
    int mesurees = 0;
    double totalMs = 0.0;

    while (running && window.IsOpen()) {
        const auto debut = Horloge::now();
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
        cam.SetPosition({1.7f, 1.70f, 1.7f});
        cam.SetTarget({-0.3f, 0.8f, -0.3f});

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

        // Sol
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

        // Murs et plafond
        objet(PoserAuSol({longueur, HAUTEUR, EP_MUR}, 0.f, -centreMur), {longueur, HAUTEUR, EP_MUR}, 0.f, 0.f, -centreMur, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({longueur, HAUTEUR, EP_MUR}, 0.f, centreMur), {longueur, HAUTEUR, EP_MUR}, 0.f, 0.f, centreMur, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({EP_MUR, HAUTEUR, 2.f * INTERIEUR}, -centreMur, 0.f), {EP_MUR, HAUTEUR, 2.f * INTERIEUR}, -centreMur, 0.f, 0.f, 0.72f, 0.70f, 0.66f);
        objet(PoserAuSol({EP_MUR, HAUTEUR, 2.f * INTERIEUR}, centreMur, 0.f), {EP_MUR, HAUTEUR, 2.f * INTERIEUR}, centreMur, 0.f, 0.f, 0.72f, 0.70f, 0.66f);
        objet(NkMat4f::Translate({0.f, HAUTEUR + EP_MUR * 0.5f, 0.f}) * NkMat4f::Scale({longueur, EP_MUR, longueur}), {longueur, EP_MUR, longueur}, 0.f, HAUTEUR, 0.f, 0.85f, 0.85f, 0.85f);

        const float posPanneau = INTERIEUR + PANNEAU * 0.5f - AVANCE;

        // Porte
        objet(PoserAuSol({0.90f, 2.00f, PANNEAU}, -0.70f, -posPanneau), {0.90f, 2.00f, PANNEAU}, -0.70f, 0.f, -posPanneau, 0.45f, 0.28f, 0.15f);

        // Fenetres
        objet(NkMat4f::Translate({-posPanneau, 1.40f, -0.90f}) * NkMat4f::Scale({PANNEAU, 1.00f, 1.20f}), {PANNEAU, 1.00f, 1.20f}, -posPanneau, 0.90f, -0.90f, 0.55f, 0.75f, 0.95f);
        objet(NkMat4f::Translate({-posPanneau, 1.40f, 0.90f}) * NkMat4f::Scale({PANNEAU, 1.00f, 1.20f}), {PANNEAU, 1.00f, 1.20f}, -posPanneau, 0.90f, 0.90f, 0.55f, 0.75f, 0.95f);

        // Plateau
        objet(NkMat4f::Translate({TABLE_X, PLATEAU_Y, TABLE_Z}) * NkMat4f::Scale({LONGUEUR, EP_PLATEAU, PROFONDEUR}), {LONGUEUR, EP_PLATEAU, PROFONDEUR}, TABLE_X, DESSUS - EP_PLATEAU, TABLE_Z, 0.62f, 0.42f, 0.22f);

        // Quatre pieds
        const float signes[2] = {-1.f, 1.f};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                objet(PoserAuSol({PIED, PIED_H, PIED}, TABLE_X + signes[i] * PIED_DX, TABLE_Z + signes[j] * PIED_DZ), {PIED, PIED_H, PIED}, TABLE_X + signes[i] * PIED_DX, 0.f, TABLE_Z + signes[j] * PIED_DZ, 0.40f, 0.26f, 0.14f);
            }
        }

        // Objet pose sur la table
        objet(PoserSurTable({0.20f, 0.30f, 0.20f}, TABLE_X, TABLE_Z), {0.20f, 0.30f, 0.20f}, TABLE_X, DESSUS, TABLE_Z, 0.20f, 0.45f, 0.70f);

        renderer->Present();
        renderer->EndFrame();

        // Mesure : on saute les images d'echauffement, puis on compte 400 images
        const double ms = std::chrono::duration<double, std::milli>(Horloge::now() - debut).count();
        if (image >= IMAGES_ECHAUFFEMENT && mesurees < IMAGES_MESUREES) {
            totalMs += ms;
            ++mesurees;
            if (mesurees == IMAGES_MESUREES) {
                const double moyenne = totalMs / IMAGES_MESUREES;
                logger.Info("[Interfaces] {0} : {1} ms par image en moyenne sur {2} images",
                            NkGraphicsApiName(device->GetApi()), moyenne, IMAGES_MESUREES);
                EcrireResultat(variante, std::string(NkGraphicsApiName(device->GetApi())) + " " + std::to_string(moyenne));
                // La fenetre reste ouverte : fais la capture, puis Echap pour quitter.
            }
        }
        ++image;
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}