// mur.cpp : la salle, avec ma photo en texture sur un seul mur
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
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstdlib>
#include <fstream>

using namespace nkentseu;
using namespace nkentseu::renderer;

// --- Dimensions de la salle (en metres), origine au centre du sol ---
static const float INTERIEUR = 2.00f;
static const float EPAISSEUR = 0.10f;
static const float HAUTEUR = 2.50f;

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoMur";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Le premier mur texture";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Mur] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Mur] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[Mur] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Mur] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // NK_MUR=0 : la salle d'avant (aucune texture) ; NK_MUR=1 (defaut) : le mur texture
    bool appliquer = true;
    if (const char *v = getenv("NK_MUR"))
        appliquer = (atoi(v) != 0);
    logger.Info("[Mur] texture appliquee : {0}", appliquer ? "oui" : "non");

    // --- Texture et materiau : charges UNE SEULE FOIS, avant la boucle ---
    // Le chemin part du dossier de lancement : on essaie les trois endroits possibles.
    const char *candidats[] = {"murs/mur.jpg", "Applications/ExoMur/murs/mur.jpg",
                               "../../../../Applications/ExoMur/murs/mur.jpg"};
    const char *chemin = nullptr;
    for (const char *c : candidats) {
        std::ifstream f(c, std::ios::binary);
        if (f.good()) {
            chemin = c;
            break;
        }
    }
    if (!chemin) {
        logger.Error("[Mur] mur.jpg introuvable depuis ce dossier de lancement");
        chemin = candidats[0];
    }
    logger.Info("[Mur] chemin de la texture : {0}", chemin);

    auto tex = renderer->GetTextures()->Load(chemin);
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);
    mat->SetTriplanarTileSize(1.0f);  // la texture se repete tous les metres (facultatif)
    const auto matH = mat->GetInstHandle();

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
        cam.SetPosition({1.5f, 1.70f, 1.5f});
        cam.SetTarget({-0.5f, 1.2f, -0.5f});

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

        // Le pave de la salle : un cube mis a l'echelle, teinte unie
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

        // Variante du pave qui recoit un materiau (la texture) : meme geometrie, teinte blanche
        auto cubeMat = [&](auto materiau, float cx, float cy, float cz, float sx, float sy, float sz) {
            NkDrawCall3D dc;
            dc.mesh = meshCube;
            dc.transform = NkMat4f::Translate({cx, cy, cz}) * NkMat4f::Scale({sx, sy, sz});
            dc.aabb = {{cx - sx * 0.5f, cy - sy * 0.5f, cz - sz * 0.5f}, {cx + sx * 0.5f, cy + sy * 0.5f, cz + sz * 0.5f}};
            dc.material = materiau;
            dc.tint = {1.f, 1.f, 1.f};
            dc.metallic = 0.f;
            dc.roughness = 0.9f;
            r3d->Submit(dc);
        };

        const float centreMur = INTERIEUR + EPAISSEUR * 0.5f;
        const float longueur = 2.f * INTERIEUR + 2.f * EPAISSEUR;

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

        // Mur du fond : le SEUL qui recoit la texture (si NK_MUR=1)
        if (appliquer)
            cubeMat(matH, 0.f, HAUTEUR * 0.5f, -centreMur, longueur, HAUTEUR, EPAISSEUR);
        else
            cube(0.f, HAUTEUR * 0.5f, -centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);

        // Les autres murs et le plafond : inchanges
        cube(0.f, HAUTEUR * 0.5f, centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(-centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR + EPAISSEUR * 0.5f, 0.f, longueur, EPAISSEUR, longueur, 0.85f, 0.85f, 0.85f);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
