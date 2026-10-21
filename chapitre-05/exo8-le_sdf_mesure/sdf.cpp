// sdf.cpp : le panneau Sortie, atlas ordinaire contre atlas SDF
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
#include <cstdio>
#include <cmath>

using namespace nkentseu;
using namespace nkentseu::renderer;

// --- La salle (metres), origine au centre du sol ---
static const float INTERIEUR = 2.00f;
static const float EPAISSEUR = 0.10f;
static const float HAUTEUR = 2.50f;

// --- Le panneau ---
static const char *TEXTE = "Sortie";
static const float K = 8.f;             // l'image est 8 fois plus grande que les lettres de l'atlas
static const float PANNEAU_H = 0.20f;   // hauteur du pave : 20 cm
static const float PANNEAU_E = 0.02f;   // epaisseur du pave : 2 cm
static const float ECART = 0.003f;      // decollement du mur : 3 mm
static const float PORTE_X = -0.70f;
static const float PORTE_H = 2.00f;

// FindGlyph rend un pointeur ou une reference selon la version : on accepte les deux.
template <class T> static const NkFontGlyph *VersPtr(T *p) { return p; }
template <class T> static const NkFontGlyph *VersPtr(T &r) { return &r; }

// La regle du nuanceur kNkFontFragSDF, ecrite en C++ : t = clamp((x-a)/(b-a), 0, 1), puis t*t*(3-2t).
static float smoothstep(float a, float b, float x) {
    float t = (x - a) / (b - a);
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    return t * t * (3.f - 2.f * t);
}

// Un atlas et la police qu'il contient.
struct Police {
    NkFontAtlas atlas;
    NkFont *font = nullptr;
    nkft_uint8 *tex = nullptr;
    nkft_int32 tw = 0, th = 0;
};

static bool Construire(Police &p, float taille, bool sdf, const char *nom) {
    if (sdf) {
        p.atlas.sdfMode = true;
        p.atlas.sdfSpread = 6;
    }
    p.font = NkFontEmbedded::AddToAtlas(p.atlas, NkEmbeddedFontId::Inter, taille);
    if (!p.font) {
        logger.Error("[SDF] AddToAtlas KO : {0}", nom);
        return false;
    }
    if (!p.atlas.Build()) {
        logger.Error("[SDF] Build KO : {0}", nom);
        return false;
    }
    p.atlas.GetTexDataAsAlpha8(&p.tex, &p.tw, &p.th);
    if (!p.tex || p.tw <= 0 || p.th <= 0) {
        logger.Error("[SDF] pixels de l'atlas indisponibles : {0}", nom);
        return false;
    }
    logger.Info("[SDF] {0} : {1} x {2}", nom, (int)p.tw, (int)p.th);
    return true;
}

// Somme des advanceX des lettres du mot (en pixels de l'atlas).
static float LargeurTexte(const Police &p) {
    float l = 0.f;
    for (const char *c = TEXTE; *c; ++c) {
        const NkFontGlyph *g = VersPtr(p.font->FindGlyph((unsigned int)(unsigned char)*c));
        if (g)
            l += g->advanceX;
    }
    return l;
}

// Lecture de l'atlas par interpolation bilineaire entre les quatre pixels voisins (valeur 0..255).
static float Lire(const nkft_uint8 *tex, int tw, int th, float u, float v) {
    const float fx = u * (float)tw - 0.5f;
    const float fy = v * (float)th - 0.5f;
    const int x0 = (int)floorf(fx);
    const int y0 = (int)floorf(fy);
    const float ax = fx - (float)x0;
    const float ay = fy - (float)y0;
    auto px = [&](int x, int y) -> float {
        if (x < 0) x = 0;
        if (x > tw - 1) x = tw - 1;
        if (y < 0) y = 0;
        if (y > th - 1) y = th - 1;
        return (float)tex[y * tw + x];
    };
    const float a = px(x0, y0) * (1.f - ax) + px(x0 + 1, y0) * ax;
    const float b = px(x0, y0 + 1) * (1.f - ax) + px(x0 + 1, y0 + 1) * ax;
    return a * (1.f - ay) + b * ay;
}

// Fabrique l'image du mot a K fois la taille des lettres de l'atlas.
// Atlas ordinaire : couverture = valeur / 255. Atlas SDF : smoothstep(0.5 - s, 0.5 + s, valeur / 255).
static bool Fabriquer(const Police &p, bool sdf, float s, int W8, int H8, const char *chemin) {
    const float ascent = (float)p.font->ascent;
    const int base = (int)roundf(K * ascent);  // ligne de base, a K * ascent pixels du haut

    NkImage img = NkImage::Create((uint32)W8, (uint32)H8, 4, 0x1B7F3BFFu);  // fond vert 0xRRGGBBAA
    unsigned char *px = img.Pixels();
    if (!px) {
        logger.Error("[SDF] pixels de l'image indisponibles");
        return false;
    }

    const float texte[3] = {255.f, 255.f, 255.f};  // blanc
    float curseur = 0.f;
    for (const char *c = TEXTE; *c; ++c) {
        const NkFontGlyph *g = VersPtr(p.font->FindGlyph((unsigned int)(unsigned char)*c));
        if (!g)
            continue;
        const float gx0 = K * (curseur + g->x0), gx1 = K * (curseur + g->x1);
        const float gy0 = (float)base + K * g->y0, gy1 = (float)base + K * g->y1;
        const float lw = gx1 - gx0, lh = gy1 - gy0;
        if (lw > 0.f && lh > 0.f) {
            int xa = (int)floorf(gx0), xb = (int)ceilf(gx1);
            int ya = (int)floorf(gy0), yb = (int)ceilf(gy1);
            if (xa < 0) xa = 0;
            if (ya < 0) ya = 0;
            if (xb > W8) xb = W8;
            if (yb > H8) yb = H8;
            for (int y = ya; y < yb; ++y) {
                for (int x = xa; x < xb; ++x) {
                    const float fx = ((float)x + 0.5f - gx0) / lw;
                    const float fy = ((float)y + 0.5f - gy0) / lh;
                    if (fx < 0.f || fx > 1.f || fy < 0.f || fy > 1.f)
                        continue;
                    const float u = g->u0 + fx * (g->u1 - g->u0);
                    const float v = g->v0 + fy * (g->v1 - g->v0);
                    const float valeur = Lire(p.tex, (int)p.tw, (int)p.th, u, v) / 255.f;
                    const float alpha = sdf ? smoothstep(0.5f - s, 0.5f + s, valeur) : valeur;
                    unsigned char *q = px + ((size_t)y * (size_t)W8 + (size_t)x) * 4;
                    for (int k = 0; k < 3; ++k)
                        q[k] = (unsigned char)((float)q[k] + (texte[k] - (float)q[k]) * alpha + 0.5f);
                }
            }
        }
        curseur += g->advanceX;
    }
    logger.Info("[SDF] mise en page {0} : {1} px de large pour une image de {2}", sdf ? "sdf" : "ordinaire",
                (int)ceilf(K * curseur), W8);
    if (!img.SavePNG(chemin)) {
        logger.Error("[SDF] SavePNG KO : {0}", chemin);
        return false;
    }
    return true;
}

static void ConfigureAppData(NkAppData &d) {
    d.appName = "ExoSdf";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
    (void)state;

    // Reglages lus dans l'environnement
    const char *vS = getenv("NK_S");
    const float s = vS ? (float)atof(vS) : 0.02f;               // douceur du bord SDF
    const char *vP = getenv("NK_PANNEAU");
    const bool pannSdf = (vP && atoi(vP) == 1);                  // 0 = ordinaire, 1 = SDF
    const char *vC = getenv("NK_CAM");
    const bool pres = (vC && atoi(vC) == 50);                    // 3 ou 50
    const char *vE = getenv("NK_ESSAIS");
    const bool essais = (vE && atoi(vE) == 1);

    // --- Les trois atlas, construits par le programme ---
    Police ord32, sdf32, ord128;
    if (!Construire(ord32, 32.f, false, "ordinaire 32") || !Construire(sdf32, 32.f, true, "sdf 32") ||
        !Construire(ord128, 128.f, false, "ordinaire 128"))
        return 4;

    // Taille commune des deux images : K * (largeur du mot) sur K * (ascent - descent)
    const int Wm = (int)ceilf(LargeurTexte(ord32));
    const int Hm = (int)ceilf((float)ord32.font->ascent - (float)ord32.font->descent);
    const int W8 = (int)K * Wm;
    const int H8 = (int)K * Hm;
    logger.Info("[SDF] image : {0} x {1}", W8, H8);
    logger.Info("[SDF] image de base : {0} x {1}", Wm, Hm);
    logger.Info("[SDF] s = {0}", s);

    if (essais) {
        // Plusieurs valeurs de s, sans fenetre : on regarde les images et on garde la plus nette
        const float valeurs[6] = {0.002f, 0.005f, 0.01f, 0.02f, 0.05f, 0.1f};
        for (int i = 0; i < 6; ++i) {
            char nom[128];
            snprintf(nom, sizeof(nom), "Applications/ExoSdf/essai_s_%g.png", valeurs[i]);
            Fabriquer(sdf32, true, valeurs[i], W8, H8, nom);
        }
        Fabriquer(ord32, false, 0.f, W8, H8, "Applications/ExoSdf/essai_ordinaire.png");
        return 0;
    }

    const char *pngOrd = "Applications/ExoSdf/panneau_ord.png";
    const char *pngSdf = "Applications/ExoSdf/panneau_sdf.png";
    if (!Fabriquer(ord32, false, 0.f, W8, H8, pngOrd) || !Fabriquer(sdf32, true, s, W8, H8, pngSdf))
        return 5;

    NkWindowConfig winCfg;
    winCfg.title = "Le SDF mesure";
    winCfg.width = 1280;
    winCfg.height = 720;
    winCfg.centered = true;

    NkWindow window(winCfg);
    if (!window.IsValid()) {
        logger.Error("[SDF] Creation fenetre KO");
        return 1;
    }

    NkDeviceInitInfo devInfo{};
    devInfo.surface = window.GetSurfaceDesc();
    devInfo.width = (uint32)window.GetSize().width;
    devInfo.height = (uint32)window.GetSize().height;

    NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
    if (!device || !device->IsValid()) {
        logger.Error("[SDF] Creation device KO");
        window.Close();
        return 2;
    }
    logger.Info("[SDF] Backend : {0}", NkGraphicsApiName(device->GetApi()));

    NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
    cfg.subsystems = NK_SS_RENDER3D;
    NkRenderer *renderer = NkRenderer::Create(device, cfg);
    if (!renderer || !renderer->Initialize()) {
        logger.Error("[SDF] Init renderer KO");
        NkRenderer::Destroy(renderer);
        NkDeviceFactory::Destroy(device);
        window.Close();
        return 3;
    }

    NkRender3D *r3d = renderer->GetRender3D();
    NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
    NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

    // Texture et materiau : charges UNE SEULE FOIS, avant la boucle
    logger.Info("[SDF] panneau pose : {0}", pannSdf ? "sdf" : "ordinaire");
    auto tex = renderer->GetTextures()->Load(pannSdf ? pngSdf : pngOrd);
    auto mat = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
    mat->SetAlbedoMap(tex);
    const auto matH = mat->GetInstHandle();

    const float largeurPanneau = PANNEAU_H * (float)W8 / (float)H8;
    const float zPanneau = -INTERIEUR + PANNEAU_E * 0.5f + ECART;
    const float yPanneau = PORTE_H + 0.02f + PANNEAU_H * 0.5f;

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

        cube(0.f, HAUTEUR * 0.5f, -centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR * 0.5f, centreMur, longueur, HAUTEUR, EPAISSEUR, 0.72f, 0.70f, 0.66f);
        cube(-centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(centreMur, HAUTEUR * 0.5f, 0.f, EPAISSEUR, HAUTEUR, 2.f * INTERIEUR, 0.72f, 0.70f, 0.66f);
        cube(0.f, HAUTEUR + EPAISSEUR * 0.5f, 0.f, longueur, EPAISSEUR, longueur, 0.85f, 0.85f, 0.85f);

        // La porte et, au-dessus, le panneau (ordinaire ou SDF selon NK_PANNEAU)
        cube(PORTE_X, PORTE_H * 0.5f, zPanneau, 0.90f, PORTE_H, PANNEAU_E, 0.45f, 0.28f, 0.15f);
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
