// panneau.cpp : le mot Sortie au-dessus de la porte, ecrit avec une police embarquee
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

#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKImage/Core/NkImage.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

using namespace nkentseu;
using namespace nkentseu::renderer;

// --- La salle (metres), origine au centre du sol ---
static const float INTERIEUR = 2.00f;
static const float EPAISSEUR = 0.10f;
static const float HAUTEUR = 2.50f;

// --- Le panneau ---
static const char *TEXTE = "Sortie";
static const float PANNEAU_H = 0.20f;   // hauteur du pave : 20 cm
static const float PANNEAU_E = 0.02f;   // epaisseur du pave : 2 cm
static const float ECART = 0.003f;      // decollement du mur : 3 mm (sinon le panneau clignote)
static const float PORTE_X = -0.70f;    // centre de la porte
static const float PORTE_H = 2.00f;     // hauteur de la porte

// FindGlyph rend un pointeur ou une reference selon la version : on accepte les deux.
template <class T> static const NkFontGlyph *VersPtr(T *p) { return p; }
template <class T> static const NkFontGlyph *VersPtr(T &r) { return &r; }

// Construit l'image du mot a partir de l'atlas, l'enregistre, puis enregistre la version
// retournee eventuellement (NK_FLIP) qui servira de texture. Rend les tailles en pixels.
static bool FabriquerPanneau(const char *pngLisible, const char *pngTexture, const char *flip,
                             int &atlasL, int &atlasH, int &imageL, int &imageH) {
    if (!NkFontEmbedded::IsAvailable(NkEmbeddedFontId::Inter)) {
        logger.Error("[Panneau] police Inter indisponible");
        return false;
    }
    NkFontAtlas atlas;
    NkFont *police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
    if (!police) {
        logger.Error("[Panneau] AddToAtlas KO");
        return false;
    }
    if (!atlas.Build()) {
        logger.Error("[Panneau] Build de l'atlas KO");
        return false;
    }
    nkft_uint8 *tex = nullptr;
    nkft_int32 tw = 0, th = 0;
    atlas.GetTexDataAsAlpha8(&tex, &tw, &th);
    if (!tex || tw <= 0 || th <= 0) {
        logger.Error("[Panneau] pixels de l'atlas indisponibles");
        return false;
    }
    atlasL = (int)tw;
    atlasH = (int)th;

    // Largeur de l'image : somme des advanceX des lettres. Hauteur : ascent - descent.
    float largeur = 0.f;
    for (const char *c = TEXTE; *c; ++c) {
        const NkFontGlyph *g = VersPtr(police->FindGlyph((unsigned int)(unsigned char)*c));
        if (!g) {
            logger.Error("[Panneau] lettre absente de la police");
            return false;
        }
        largeur += g->advanceX;
    }
    const float ascent = (float)police->ascent;
    const float descent = (float)police->descent;
    const int W = (int)ceilf(largeur);
    const int H = (int)ceilf(ascent - descent);
    const int base = (int)roundf(ascent);  // la ligne de base est a ascent pixels du haut

    // Fond vert, 0xRRGGBBAA
    NkImage img = NkImage::Create((uint32)W, (uint32)H, 4, 0x1B7F3BFFu);
    unsigned char *px = img.Pixels();
    if (!px) {
        logger.Error("[Panneau] pixels de l'image indisponibles");
        return false;
    }

    const float texte[3] = {255.f, 255.f, 255.f};  // blanc
    float curseur = 0.f;
    for (const char *c = TEXTE; *c; ++c) {
        const NkFontGlyph *g = VersPtr(police->FindGlyph((unsigned int)(unsigned char)*c));
        const float gx0 = curseur + g->x0, gx1 = curseur + g->x1;
        const float gy0 = (float)base + g->y0, gy1 = (float)base + g->y1;
        const float lw = gx1 - gx0, lh = gy1 - gy0;
        if (lw > 0.f && lh > 0.f) {
            for (int y = (int)floorf(gy0); y < (int)ceilf(gy1); ++y) {
                for (int x = (int)floorf(gx0); x < (int)ceilf(gx1); ++x) {
                    if (x < 0 || x >= W || y < 0 || y >= H)
                        continue;
                    // fraction du pixel dans le rectangle de la lettre, puis position dans l'atlas
                    const float fx = ((float)x + 0.5f - gx0) / lw;
                    const float fy = ((float)y + 0.5f - gy0) / lh;
                    if (fx < 0.f || fx > 1.f || fy < 0.f || fy > 1.f)
                        continue;
                    const float u = g->u0 + fx * (g->u1 - g->u0);
                    const float v = g->v0 + fy * (g->v1 - g->v0);
                    int ax = (int)(u * (float)tw);
                    int ay = (int)(v * (float)th);
                    if (ax < 0) ax = 0;
                    if (ax > tw - 1) ax = (int)tw - 1;
                    if (ay < 0) ay = 0;
                    if (ay > th - 1) ay = (int)th - 1;
                    const float couverture = (float)tex[ay * tw + ax] / 255.f;
                    unsigned char *p = px + ((size_t)y * (size_t)W + (size_t)x) * 4;
                    for (int k = 0; k < 3; ++k)
                        p[k] = (unsigned char)((float)p[k] + (texte[k] - (float)p[k]) * couverture + 0.5f);
                }
            }
        }
        curseur += g->advanceX;
    }
    imageL = W;
    imageH = H;

    if (!img.SavePNG(pngLisible)) {
        logger.Error("[Panneau] SavePNG KO : {0}", pngLisible);
        return false;
    }
    if (std::strchr(flip, 'v'))
        img.FlipVertical();
    if (std::strchr(flip, 'h'))
        img.FlipHorizontal();
    if (!img.SavePNG(pngTexture)) {
        logger.Error("[Panneau] SavePNG KO : {0}", pngTexture);
        return false;
    }
    return true;
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoPanneau";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig winCfg;
    winCfg.title = "Un panneau au-dessus de la porte";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[Panneau] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[Panneau] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[Panneau] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[Panneau] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // Reglages lus dans l'environnement : NK_CAM (3 ou 50) et NK_FLIP (v, h, vh)
    const char *camEnv = getenv("NK_CAM");
    const bool pres = (camEnv && atoi(camEnv) == 50);
    const char *flip = getenv("NK_FLIP");
    if (!flip)
        flip = "";

    // --- L'image du mot : fabriquee une seule fois, avant la boucle ---
    const char *pngLisible = "Applications/ExoPanneau/panneau.png";
    const char *pngTexture = "Applications/ExoPanneau/panneau_tex.png";
    int atlasL = 0, atlasH = 0, imageL = 0, imageH = 0;
    if (!FabriquerPanneau(pngLisible, pngTexture, flip, atlasL, atlasH, imageL, imageH)) {
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 4;
    }
    logger.Info("[Panneau] atlas : {0} x {1}", atlasL, atlasH);
    logger.Info("[Panneau] image : {0} x {1}", imageL, imageH);

    // --- Texture et materiau : charges UNE SEULE FOIS, avant la boucle ---
    auto tex = renderer->GetTextures()->Load(pngTexture);
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);
    const auto matH = mat->GetInstHandle();

    // Le pave garde les proportions de l'image
    const float largeurPanneau = PANNEAU_H * (float)imageL / (float)imageH;
    const float zPanneau = -INTERIEUR + PANNEAU_E * 0.5f + ECART;  // contre le mur du fond, decolle de 3 mm
    const float yPanneau = PORTE_H + 0.02f + PANNEAU_H * 0.5f;      // juste au-dessus de la porte
    logger.Info("[Panneau] pave : {0} m de large, centre y = {1}, z = {2}", largeurPanneau, yPanneau, zPanneau);

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

        // Trois metres : (-0,70 ; 1,70 ; 1,00). Cinquante centimetres : (-0,70 ; 2,20 ; -1,50).
        NkCamera3DData camData;
        camData.up = {0.f, 1.f, 0.f};
        camData.fovY = 70.f;
        camData.aspect = (float32)W / (float32)H;
        camData.nearPlane = 0.05f;
        camData.farPlane = 200.f;
        NkCamera3D cam(camData);
        if (pres)
            cam.SetPosition({PORTE_X, 2.20f, -1.50f});
        else
            cam.SetPosition({PORTE_X, 1.70f, 1.00f});
        cam.SetTarget({PORTE_X, yPanneau, zPanneau});

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

        // Meme pave, mais avec un materiau (la texture du mot)
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

        // Quatre murs et plafond
        cube(0.f, HAUTEUR * 0.5f, -centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR * 0.5f, centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(-centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR + EPAISSEUR * 0.5f, 0.f, longueur, EPAISSEUR, longueur, 0.85f, 0.85f, 0.85f);

        // La porte : 0,90 m x 2,00 m, posee au sol, plaquee contre le mur du fond, decollee de 3 mm
        cube(PORTE_X, PORTE_H * 0.5f, zPanneau, 0.90f, PORTE_H, PANNEAU_E, 0.45f, 0.28f, 0.15f);

        // Le panneau : texture du mot Sortie, 20 cm de haut, au-dessus de la porte
        cubeMat(matH, PORTE_X, yPanneau, zPanneau, largeurPanneau, PANNEAU_H, PANNEAU_E);

        renderer->Present();
        renderer->EndFrame();
    }

    device->WaitIdle();
    NkRenderer::Destroy(renderer);
    NkDeviceFactory::Destroy(device);
    window.Close();
    return 0;
}
