// cubes.cpp : N cubes en grille, un par un ou groupes, avec mesure de la cadence
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

#include <cstdlib>
#include <cmath>

using namespace nkentseu;
using namespace nkentseu::renderer;

static const int IMAGES_MESURE = 400; // la cadence se mesure sur au moins 400 images

static void ConfigureAppData(NkAppData &d) {
d.appName = "ExoCubes";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state) {
(void)state;

// Reglages lus dans l'environnement : NK_CUBES (nombre) et NK_MODE (0 = un par un, 1 = groupes)
int nbCubes = 100;
int mode = 0;
if (const char *v = getenv("NK_CUBES"))
nbCubes = atoi(v);
if (const char *v = getenv("NK_MODE"))
mode = atoi(v);

NkWindowConfig winCfg;
winCfg.title = "Des cubes et la cadence";
winCfg.width = 1280;
winCfg.height = 720;
winCfg.centered = true;

NkWindow window(winCfg);
if (!window.IsValid()) {
logger.Error("[Cubes] Creation fenetre KO");
return 1;
}

NkDeviceInitInfo devInfo{};
devInfo.surface = window.GetSurfaceDesc();
devInfo.width = (uint32)window.GetSize().width;
devInfo.height = (uint32)window.GetSize().height;

NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
if (!device || !device->IsValid()) {
logger.Error("[Cubes] Creation device KO");
window.Close();
return 2;
}
logger.Info("[Cubes] Backend : {0} | cubes = {1} | mode = {2}", NkGraphicsApiName(device->GetApi()), nbCubes, mode);

NkRendererConfig cfg = NkRendererConfig::ForGame(devInfo.api, devInfo.width, devInfo.height);
cfg.subsystems = NK_SS_RENDER3D;
NkRenderer *renderer = NkRenderer::Create(device, cfg);
if (!renderer || !renderer->Initialize()) {
logger.Error("[Cubes] Init renderer KO");
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
int images = 0;
float32 tempsMesure = 0.f;

// Grille : cote x cote cubes, arete 0,10 m, pas 0,12 m, centree sur l'origine
const int cote = (int)ceilf(sqrtf((float)nbCubes));
const float arete = 0.10f;
const float pas = 0.12f;

while (running && window.IsOpen()) {
events.PollEvents();
const float32 dt = clock.Tick().delta;
if (!renderer->BeginFrame())
continue;

NkCamera3DData camData;
camData.up = {0.f, 1.f, 0.f};
camData.fovY = 60.f;
camData.aspect = (float32)W / (float32)H;
camData.nearPlane = 0.05f;
camData.farPlane = 200.f;
NkCamera3D cam(camData);
cam.SetPosition({0.f, 9.f, 11.f});
cam.SetTarget({0.f, 0.f, 0.f});

NkSceneContext sctx;
sctx.camera = cam;

NkLightDesc sun;
sun.type = NkLightType::NK_DIRECTIONAL;
sun.direction = {-0.4f, -1.f, -0.3f};
sun.color = {1.f, 0.95f, 0.85f};
sun.intensity = 3.f;
sun.castShadow = false;
sctx.lights.PushBack(sun);

r3d->BeginScene(sctx);

// Sol
{
NkDrawCall3D dc;
dc.mesh = meshPlane;
dc.transform = NkMat4f::Scale({8.f, 1.f, 8.f});
dc.aabb = {{-8.f, 0.f, -8.f}, {8.f, 0.f, 8.f}};
dc.tint = {0.30f, 0.30f, 0.33f};
dc.metallic = 0.f;
dc.roughness = 0.9f;
r3d->Submit(dc);
}

if (mode == 0) {
// Facon 1 : un ordre de dessin par cube
for (int n = 0; n < nbCubes; ++n) {
const float cx = ((n % cote) - cote * 0.5f) * pas;
const float cz = ((n / cote) - cote * 0.5f) * pas;
const float cy = arete * 0.5f;
NkDrawCall3D dc;
dc.mesh = meshCube;
dc.transform = NkMat4f::Translate({cx, cy, cz}) * NkMat4f::Scale({arete, arete, arete});
dc.aabb = {{cx - arete * 0.5f, cy - arete * 0.5f, cz - arete * 0.5f}, {cx + arete * 0.5f, cy + arete * 0.5f, cz + arete * 0.5f}};
dc.tint = {1.f, 1.f, 1.f};
dc.metallic = 0.f;
dc.roughness = 0.9f;
r3d->Submit(dc);
}
} else {
// Facon 2 : tous les cubes dans une seule soumission instanciee
NkDrawCallInstanced di;
di.mesh = meshCube;
float minX = 1e9f, minZ = 1e9f, maxX = -1e9f, maxZ = -1e9f;
for (int n = 0; n < nbCubes; ++n) {
const float cx = ((n % cote) - cote * 0.5f) * pas;
const float cz = ((n / cote) - cote * 0.5f) * pas;
const float cy = arete * 0.5f;
di.transforms.PushBack(NkMat4f::Translate({cx, cy, cz}) * NkMat4f::Scale({arete, arete, arete}));
di.tints.PushBack(NkVec3f{1.f, 1.f, 1.f});
minX = fminf(minX, cx - arete * 0.5f);
maxX = fmaxf(maxX, cx + arete * 0.5f);
minZ = fminf(minZ, cz - arete * 0.5f);
maxZ = fmaxf(maxZ, cz + arete * 0.5f);
}
// Boite englobante en coordonnees du monde : elle doit contenir TOUS les cubes
di.aabb = {{minX, 0.f, minZ}, {maxX, arete, maxZ}};
if (nbCubes > 0)
r3d->SubmitInstanced(di);
}

renderer->Present();
renderer->EndFrame();

// Mesure : cadence sur IMAGES_MESURE images, avec le compteur drawCalls du renderer
tempsMesure += dt;
if (++images == IMAGES_MESURE) {
const auto &st = renderer->GetStats();
logger.Info("[Cubes] {0} images en {1} s : {2} images/s | drawCalls = {3}", IMAGES_MESURE, tempsMesure,
(float)IMAGES_MESURE / tempsMesure, st.drawCalls);
images = 0;
tempsMesure = 0.f;
}
}

device->WaitIdle();
NkRenderer::Destroy(renderer);
NkDeviceFactory::Destroy(device);
window.Close();
return 0;
}

