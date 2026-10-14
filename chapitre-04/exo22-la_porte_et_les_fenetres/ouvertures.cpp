// ouvertures.cpp : salle 4 m x 4 m, une porte et deux fenetres plaquees sur les murs
#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKTime/NkTime.h"
#include "NKLogger/NkLog.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cmath>

using namespace nkentseu;
using namespace nkentseu::renderer;

// --- Dimensions de la salle (en metres) ---
static const float INTERIEUR = 2.00f;   // distance de l'origine a la face interieure d'un mur
static const float EPAISSEUR = 0.10f;   // epaisseur d'un mur
static const float HAUTEUR = 2.50f;     // hauteur du plafond
static const float PANNEAU = 0.02f;     // epaisseur d'une ouverture (2 cm)

// --- Le reglage a trouver : de combien on avance les panneaux vers l'interieur ---
static const float AVANCE = 0.00009f;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoOuvertures";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "La porte et les fenetres";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Ouvertures] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Ouvertures] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[Ouvertures] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Ouvertures] Init renderer KO");
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

    NkClock clock;
    float32 total = 0.f;

    while (running && window.IsOpen()) {
        events.PollEvents();
        if (!running)
            break;
        total += clock.Tick().delta;

        if (!renderer->BeginFrame())
            continue;

        // Camera debout, avec un leger balancement pour rendre le defaut visible
        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);
        cam.SetPosition({1.5f + 0.15f * sinf(total * 0.7f), 1.70f, 1.8f});
        cam.SetTarget({-0.8f, 1.2f, -0.6f});

        NkSceneContext sctx;
        sctx.camera = cam;

        NkLightDesc sun;
        sun.type = NkLightType::NK_DIRECTIONAL;
        sun.direction = {-0.4f, -1.f, -0.3f};
        sun.color = {1.f, 0.95f, 0.85f};
        sun.intensity = 3.f; sun.castShadow = false;
        sctx.lights.PushBack(sun); NkLightDesc fill; fill.type = NkLightType::NK_DIRECTIONAL; fill.direction = {-0.6f, -0.3f, -0.8f}; fill.color = {1.f, 1.f, 1.f}; fill.intensity = 2.f; fill.castShadow = false; sctx.lights.PushBack(fill);

        r3d->BeginScene(sctx);

        // Un cube d'echelle (sx, sy, sz) centre en (cx, cy, cz).
        // La boite englobante est en coordonnees du monde : centre -/+ la moitie de l'echelle.
        auto cube = [&](float cx, float cy, float cz, float sx, float sy, float sz, float r, float g, float b) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({cx, cy, cz}) * NkMat4f::Scale({sx, sy, sz});
            dc.aabb = {{cx - sx * 0.5f, cy - sy * 0.5f, cz - sz * 0.5f}, {cx + sx * 0.5f, cy + sy * 0.5f, cz + sz * 0.5f}};
            dc.tint = {r, g, b};
            dc.metallic = 0.f;
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        };

        const float centreMur = INTERIEUR + EPAISSEUR * 0.5f;     // centre d'un mur
        const float longueur = 2.f * INTERIEUR + 2.f * EPAISSEUR; // mur qui recouvre les coins

        // Sol : un plan (sa demi-largeur de base est 1, comme dans le tutoriel 03)
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

        // Murs : fond (z < 0), entree (z > 0), gauche (x < 0), droite (x > 0), plafond
        cube(0.f, HAUTEUR * 0.5f, -centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR * 0.5f, centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(-centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR + EPAISSEUR * 0.5f, 0.f, longueur, EPAISSEUR, longueur, 0.85f, 0.85f, 0.85f);

        // Plan de la face interieure du mur : INTERIEUR. Un panneau de 2 cm dont la face avant
        // est dans ce plan a son centre a INTERIEUR + PANNEAU/2 du centre de la piece, cote mur.
        const float posPanneau = INTERIEUR + PANNEAU * 0.5f - AVANCE;

        // Porte : 0,90 m x 2,00 m, posee au sol, mur du fond, centre en x = -0,70
        cube(-0.70f, 1.00f, -posPanneau, 0.90f, 2.00f, PANNEAU, 0.45f, 0.28f, 0.15f);

        // Fenetres : 1,20 m (en z) x 1,00 m, mur de gauche, centre a 1,40 m du sol
        cube(-posPanneau, 1.40f, -0.90f, PANNEAU, 1.00f, 1.20f, 0.55f, 0.75f, 0.95f);
        cube(-posPanneau, 1.40f, 0.90f, PANNEAU, 1.00f, 1.20f, 0.55f, 0.75f, 0.95f);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
