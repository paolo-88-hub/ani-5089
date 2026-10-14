// table.cpp : une table (plateau + quatre pieds) posee sur le sol
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

// --- Dimensions de la table (en metres) ---
static const float LONGUEUR = 1.20f;    // en x
static const float PROFONDEUR = 0.80f;  // en z
static const float EPAISSEUR = 0.05f;   // epaisseur du plateau
static const float DESSUS = 0.75f;      // hauteur du dessus du plateau
static const float PIED = 0.06f;        // section d'un pied (6 cm sur 6 cm)
static const float TABLE_X = 0.60f;     // centre de la table en x
static const float TABLE_Z = 0.70f;     // centre de la table en z

// --- Valeurs deduites ---
static const float PLATEAU_Y = DESSUS - EPAISSEUR * 0.5f;      // y du centre du plateau
static const float PIED_H = DESSUS - EPAISSEUR;                // hauteur d'un pied : du sol au dessous du plateau
static const float PIED_Y = PIED_H * 0.5f;                     // y du centre d'un pied
static const float PIED_DX = LONGUEUR * 0.5f - PIED * 0.5f;    // decalage d'un pied en x par rapport au centre
static const float PIED_DZ = PROFONDEUR * 0.5f - PIED * 0.5f;  // decalage d'un pied en z par rapport au centre

static void ConfigureAppData(NkAppData &d) {
d.appName = "ExoTable";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
(void)state;

NkWindowConfig winCfg;
winCfg.title = "La table";
winCfg.width = 1280;
winCfg.height = 720;
winCfg.centered = true;

NkWindow window(winCfg);
if (!window.IsValid()) {
logger.Error("[Table] Creation fenetre KO");
return 1;
}

NkDeviceInitInfo devInfo{};
devInfo.surface = window.GetSurfaceDesc();
devInfo.width = (uint32)window.GetSize().width;
devInfo.height = (uint32)window.GetSize().height;

NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
if (!device || !device->IsValid()) {
logger.Error("[Table] Creation device KO");
window.Close();
return 2;
}
logger.Info("[Table] Backend : {0}", NkGraphicsApiName(device->GetApi()));

NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
cfg.subsystems = NK_SS_RENDER3D;
NkRenderer *renderer = NkRenderer::Create(device, cfg);
if (!renderer || !renderer->Initialize()) {
logger.Error("[Table] Init renderer KO");
NkRenderer::Destroy(renderer);
NkDeviceFactory::Destroy(device);
window.Close();
return 3;
}

NkRender3D *r3d = renderer->GetRender3D();
NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
NkMeshHandle meshPlane = renderer->GetMeshSystem()->GetPlane();

// Pour comparer avec votre calcul a la main
logger.Info("[Table] plateau y = {0} | pied hauteur = {1} | pied y = {2}", PLATEAU_Y, PIED_H, PIED_Y);
logger.Info("[Table] pied dx = {0} | pied dz = {1}", PIED_DX, PIED_DZ);

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
camData.fovY = 60.f;
camData.aspect = (float32)W / (float32)H;
camData.nearPlane = 0.05f;
camData.farPlane = 200.f;
NkCamera3D cam(camData);
cam.SetPosition({-1.2f, 1.4f, 3.0f});
cam.SetTarget({TABLE_X, 0.5f, TABLE_Z});

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
fill.direction = {0.5f, -0.3f, -0.8f};
fill.color = {1.f, 1.f, 1.f};
fill.intensity = 2.f;
fill.castShadow = false;
sctx.lights.PushBack(fill);

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

// Sol
{
NkDrawCall3D dc;
dc.mesh = meshPlane;
dc.transform = NkMat4f::Scale({4.f, 1.f, 4.f});
dc.aabb = {{-4.f, 0.f, -4.f}, {4.f, 0.f, 4.f}};
dc.tint = {0.30f, 0.30f, 0.33f};
dc.metallic = 0.f;
dc.roughness = 0.9f;
r3d->Submit(dc);
}

// Plateau
cube(TABLE_X, PLATEAU_Y, TABLE_Z, LONGUEUR, EPAISSEUR, PROFONDEUR, 0.62f, 0.42f, 0.22f);

// Quatre pieds, un a chaque coin, sous le plateau
const float signes[2] = {-1.f, 1.f};
for (int i = 0; i < 2; ++i) {
for (int j = 0; j < 2; ++j) {
cube(TABLE_X + signes[i] * PIED_DX, PIED_Y, TABLE_Z + signes[j] * PIED_DZ, PIED, PIED_H, PIED, 0.40f, 0.26f, 0.14f);
}
}

renderer->Present();
renderer->EndFrame();
}

device->WaitIdle();
NkRenderer::Destroy(renderer);
NkDeviceFactory::Destroy(device);
window.Close();
return 0;
}
