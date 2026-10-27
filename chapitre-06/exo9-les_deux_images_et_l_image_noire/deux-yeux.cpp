// =============================================================================
//  Ã‰tape 5 â€” la salle, en cubes, aux dimensions du chapitre 2.
//
//  Pas encore de casque, pas encore de tÃªte : une camÃ©ra fixe, posÃ©e Ã  hauteur
//  d'yeux, qui regarde la porte. C'est le dÃ©cor sur lequel tout le reste du
//  livre se jugera, et c'est pour cela qu'il est bÃ¢ti maintenant.
//
//  Toutes les dimensions sont en MÃˆTRES, et viennent du tableau du chapitre 7 :
//      sol 4 x 4, murs 2,5 de haut, porte 0,9 x 2,0,
//      deux fenÃªtres 1,2 x 1,0, table 1,2 x 0,8 avec le plateau Ã  0,75.
//
//  Le cube et la sphÃ¨re de NkMeshSystem sont UNITAIRES, de demi-Ã©tendue 0,5 :
//  une Ã©chelle S donne donc une taille S, et poser un objet au sol demande de
//  le translater de S/2. Le plan de GetPlane() est unitaire LUI AUSSI, de
//  demi-Ã©tendue 0,5 : une Ã©chelle S y donne un cÃ´tÃ© S, et non un demi-cÃ´tÃ©.
//  Se tromper lÃ -dessus donne un sol deux fois trop petit, qui flotte au
//  milieu de la piÃ¨ce, et rien ne le signale.
// =============================================================================
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKImage/Core/NkImage.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

	// â”€â”€ Les dimensions de la salle, en un seul endroit â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
	// Elles sont ici, nommÃ©es, et pas semÃ©es dans le code : au chapitre 21 on
	// les lira depuis un fichier, et ce sont ces noms-lÃ  qui deviendront des
	// champs.
	const float32 kPiece = 4.00f;	  // cÃ´tÃ© intÃ©rieur du sol
	const float32 kPlafond = 2.50f;	  // hauteur sous plafond
	const float32 kMur = 0.10f;		  // Ã©paisseur d'un mur
	const float32 kPorteL = 0.90f;	  // porte : largeur
	const float32 kPorteH = 2.00f;	  //         hauteur
	const float32 kFenL = 1.20f;	  // fenÃªtre : largeur
	const float32 kFenH = 1.00f;	  //           hauteur
	const float32 kFenY = 1.40f;	  //           hauteur de son centre
	const float32 kTableL = 1.20f;	  // table : longueur
	const float32 kTableP = 0.80f;	  //         profondeur
	const float32 kTableH = 0.75f;	  //         hauteur du plateau
	const float32 kYeux = 1.70f;	  // hauteur des yeux, debout

	const float32 kDemi = kPiece * 0.5f;

	// Un pavÃ© posÃ© dans la salle. `centre` est le centre gÃ©omÃ©trique, `taille`
	// les trois dimensions en mÃ¨tres : on ne manipule jamais autre chose.
	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f) {
		NkDrawCall3D dc;
		dc.mesh = meshes->GetCube();
		dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
		dc.aabb = { { centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f,
					  centre.z - taille.z * 0.5f },
					{ centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f,
					  centre.z + taille.z * 0.5f } };
		dc.tint = teinte;
		dc.metallic = 0.f;
		dc.roughness = rugosite;
		r3d->Submit(dc);
	}

	void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes) {
		const NkVec3f grisMur = { 0.72f, 0.70f, 0.66f };
		const NkVec3f grisSol = { 0.38f, 0.36f, 0.34f };
		const NkVec3f bois = { 0.55f, 0.38f, 0.22f };
		const NkVec3f vitre = { 0.75f, 0.85f, 0.92f };

		{ // le sol : le plan est unitaire, une Ã©chelle S donne un cÃ´tÃ© S
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Scale({ kPiece, 1.f, kPiece });
			dc.aabb = { { -kDemi, 0.f, -kDemi }, { kDemi, 0.f, kDemi } };
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		// Quatre murs. La porte est dans celui du fond, les fenÃªtres Ã  gauche.
		Pave(r3d, meshes, { 0.f, yMur, -bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { 0.f, yMur, bord }, { kPiece, kPlafond, kMur }, grisMur);
		Pave(r3d, meshes, { -bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);
		Pave(r3d, meshes, { bord, yMur, 0.f }, { kMur, kPlafond, kPiece }, grisMur);

		// Le plafond. On l'oublie toujours, et le trou ne se voit qu'en levant
		// les yeux, c'est-Ã -dire le jour de la dÃ©monstration.
		Pave(r3d, meshes, { 0.f, kPlafond + kMur * 0.5f, 0.f },
			 { kPiece, kMur, kPiece }, grisMur);

		// La porte, fermÃ©e jusqu'au chapitre 17 : un panneau plaquÃ© contre le
		// mur du fond, dÃ©calÃ© pour ne pas se battre avec lui en profondeur.
		Pave(r3d, meshes, { -0.70f, kPorteH * 0.5f, -kDemi + 0.02f },
			 { kPorteL, kPorteH, 0.04f }, bois, 0.6f);

		// Deux fenÃªtres sur le mur de gauche, pour qu'il y ait un dehors.
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, -0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);
		Pave(r3d, meshes, { -kDemi + 0.02f, kFenY, 0.90f },
			 { 0.04f, kFenH, kFenL }, vitre, 0.15f);

		// La table : un plateau et quatre pieds. Le chapitre 12 y posera un
		// objet, le 21 la dÃ©placera.
		const float32 tx = 0.60f, tz = 0.70f;
		const float32 ep = 0.05f;
		Pave(r3d, meshes, { tx, kTableH - ep * 0.5f, tz },
			 { kTableL, ep, kTableP }, bois, 0.5f);

		const float32 pied = 0.06f;
		const float32 hp = kTableH - ep;
		for (int i = 0; i < 4; ++i) {
			const float32 dx = ((i & 1) ? 1.f : -1.f) * (kTableL * 0.5f - pied);
			const float32 dz = ((i & 2) ? 1.f : -1.f) * (kTableP * 0.5f - pied);
			Pave(r3d, meshes, { tx + dx, hp * 0.5f, tz + dz },
				 { pied, hp, pied }, bois, 0.6f);
		}

		// Le repÃ¨re d'un mÃ¨tre du chapitre 9 : un cube d'un mÃ¨tre de cÃ´tÃ©,
		// posÃ© au sol, contre lequel on juge l'Ã©chelle de tout le reste.
		Pave(r3d, meshes, { -1.10f, 0.50f, 1.20f }, { 1.f, 1.f, 1.f },
			 { 0.85f, 0.80f, 0.30f }, 0.8f);
	}

	uint64 EnvU64(const char *nom, uint64 defaut) {
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0') return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

	struct ResultatDifference {
		uint64 pixelsDifferents = 0;
		uint8 ecartMaximal = 0;
	};

	void SoumettreUnOeil(NkRenderer *renderer, NkICommandBuffer *cmd,
						 const NkVec3f &position, uint64 image) {
		renderer->FlushGraphRebuilds();
		NkRender3D *r3d = renderer->GetRender3D();
		r3d->ResetFrame();
		if (auto *materiaux = renderer->GetMaterialCollection()) materiaux->Upload();

		NkCamera3DData cd;
		cd.position = position;
		cd.target = position + NkVec3f(0.f, 0.f, -1.f);
		cd.up = { 0.f, 1.f, 0.f };
		cd.fovY = 70.f;
		cd.aspect = 640.f / 720.f;
		cd.nearPlane = 0.05f;
		cd.farPlane = 100.f;

		NkSceneContext contexte;
		contexte.camera = NkCamera3D(cd);
		contexte.time = (float32)image / 60.f;
		r3d->SetIBLStrength(0.35f);

		NkLightDesc soleil;
		soleil.type = NkLightType::NK_DIRECTIONAL;
		soleil.direction = { -0.5f, -1.f, -0.35f };
		soleil.color = { 1.f, 0.96f, 0.88f };
		soleil.intensity = 3.f;
		soleil.castShadow = false;
		contexte.lights.PushBack(soleil);

		r3d->BeginScene(contexte);
		SoumettreLaSalle(r3d, renderer->GetMeshSystem());
		if (NkRenderGraph *graphe = renderer->GetRenderGraph()) graphe->Execute(cmd);
	}

	ResultatDifference CalculerDifference(const NkImage &premiere, const NkImage &seconde,
										 NkImage &difference) {
		ResultatDifference resultat;
		for (int32 y = 0; y < 720; ++y) {
			for (int32 x = 0; x < 640; ++x) {
				const NkColor a = premiere.GetPixel(x, y);
				const NkColor b = seconde.GetPixel(x, y);
				const uint8 dr = (uint8)abs((int)a.r - (int)b.r);
				const uint8 dg = (uint8)abs((int)a.g - (int)b.g);
				const uint8 db = (uint8)abs((int)a.b - (int)b.b);
				const uint8 ecart = dr > dg ? (dr > db ? dr : db) : (dg > db ? dg : db);
				if (ecart > 1) ++resultat.pixelsDifferents;
				if (ecart > resultat.ecartMaximal) resultat.ecartMaximal = ecart;
				const uint8 amplifie = (uint8)((unsigned int)ecart * 4u > 255u ? 255u : (unsigned int)ecart * 4u);
				difference.SetPixel(x, y, NkColor(amplifie, amplifie, amplifie, 255));
			}
		}
		return resultat;
	}

} // namespace

int nkmain(const NkEntryState &state) {
	(void)state;
	NkWindowConfig cfg;
	cfg.title = "Ma salle : deux yeux";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;
	NkWindow fenetre(cfg);
	if (!fenetre.IsValid()) {
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;
	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
	if (device == nullptr) {
		std::printf("[MaSalle] aucune API graphique disponible\n");
		return 1;
	}

	NkRendererConfig cfgEcran = NkRendererConfig::For2D(devInit.api, 1280, 720);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize()) {
		std::printf("[MaSalle] renderer d'ecran indisponible\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkOffscreenDesc desc;
	desc.width = 640;
	desc.height = 720;
	desc.hasDepth = true;
	desc.hdr = false;
	desc.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	desc.readable = true;
	desc.readback = true;
	desc.name = "MaSalle_Oeil";
	NkOffscreenTarget *cibleGauche = rEcran->CreateOffscreen(desc);
	NkOffscreenTarget *cibleDroite = rEcran->CreateOffscreen(desc);
	if (cibleGauche == nullptr || cibleDroite == nullptr) {
		std::printf("[MaSalle] creation des cibles hors ecran impossible\n");
		if (cibleGauche != nullptr) rEcran->DestroyOffscreen(cibleGauche);
		if (cibleDroite != nullptr) rEcran->DestroyOffscreen(cibleDroite);
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkRendererConfig cfgSalle = NkRendererConfig::ForGame(devInit.api, 640, 720);
	cfgSalle.Disable(NK_SS_RENDER2D);
	cfgSalle.Disable(NK_SS_TEXT);
	cfgSalle.Disable(NK_SS_OVERLAY);
	cfgSalle.Enable(NK_SS_OFFSCREEN);
	if (EnvU64("MASALLE_POST", 1) == 0) cfgSalle.Disable(NK_SS_POST_PROCESS);

	NkRenderer *rendererGauche = NkRenderer::Create(device, cfgSalle);
	NkRenderer *rendererDroit = NkRenderer::Create(device, cfgSalle);
	if (rendererGauche == nullptr || rendererDroit == nullptr ||
		!rendererGauche->Initialize() || !rendererDroit->Initialize()) {
		std::printf("[MaSalle] initialisation des renderers de salle impossible\n");
		NkRenderer::Destroy(rendererGauche);
		NkRenderer::Destroy(rendererDroit);
		rEcran->DestroyOffscreen(cibleGauche);
		rEcran->DestroyOffscreen(cibleDroite);
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	rendererGauche->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cibleGauche->GetColorResult()));
	rendererDroit->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cibleDroite->GetColorResult()));
	rendererGauche->SetRenderSizeOverride(640, 720);
	rendererDroit->SetRenderSizeOverride(640, 720);

	const NkVec3f tete(0.30f, 1.70f, 1.20f);
	const float demiIPD = 0.063f * 0.5f;
	const NkVec3f positionGauche = tete - NkVec3f(1.f, 0.f, 0.f) * demiIPD;
	const NkVec3f positionDroite = tete + NkVec3f(1.f, 0.f, 0.f) * demiIPD;
	uint64 image = 0;
	bool tourne = true;
	bool capturesReussies = false;
	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);

	while (tourne && fenetre.IsOpen()) {
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev)) {
			if (ev->As<NkWindowCloseEvent>() != nullptr) tourne = false;
		}
		if (!tourne) break;

		if (rEcran->BeginFrame()) {
			NkICommandBuffer *cmd = rEcran->GetCmd();
			SoumettreUnOeil(rendererGauche, cmd, positionGauche, image);
			SoumettreUnOeil(rendererDroit, cmd, positionDroite, image);
			if (NkRender2D *r2d = rEcran->GetRender2D()) {
				r2d->DrawImage(cibleGauche->GetColorResult(), { 0.f, 0.f, 640.f, 720.f });
				r2d->DrawImage(cibleDroite->GetColorResult(), { 640.f, 0.f, 640.f, 720.f });
			}
			rEcran->Present();
			rEcran->EndFrame();
		}
		++image;
		if (image == 30) {
			const bool gaucheOK = cibleGauche->Capture("oeil-gauche.png");
			const bool droiteOK = cibleDroite->Capture("oeil-droit.png");
			capturesReussies = gaucheOK && droiteOK;
			std::printf("[MaSalle] captures a l'image 30 : gauche %s, droite %s\n",
						gaucheOK ? "ok" : "echec", droiteOK ? "ok" : "echec");
		}
		if (imageDeSortie != 0 && image >= imageDeSortie) tourne = false;
	}

	if (!capturesReussies) {
		std::printf("[MaSalle] echec : fermer avant l'image 30 ou capture indisponible\n");
		NkRenderer::Destroy(rendererGauche);
		NkRenderer::Destroy(rendererDroit);
		rEcran->DestroyOffscreen(cibleGauche);
		rEcran->DestroyOffscreen(cibleDroite);
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	const char *tempGauche = "oeil-gauche-vue-oubliee-tmp.png";
	const char *tempDroite = "oeil-droit-vue-oubliee-tmp.png";
	if (!rEcran->BeginFrame()) {
		std::printf("[MaSalle] impossible de lancer le rendu de la vue oubliee\n");
		return 1;
	}
	NkICommandBuffer *cmd = rEcran->GetCmd();
	SoumettreUnOeil(rendererGauche, cmd, tete, image);
	SoumettreUnOeil(rendererDroit, cmd, tete, image);
	rEcran->Present();
	rEcran->EndFrame();
	const bool oublieGaucheOK = cibleGauche->Capture(tempGauche);
	const bool oublieDroiteOK = cibleDroite->Capture(tempDroite);
	if (!oublieGaucheOK || !oublieDroiteOK) {
		std::printf("[MaSalle] capture temporaire de la vue oubliee impossible\n");
		return 1;
	}

	NkImage oeilGauche, oeilDroit, oublieGauche, oublieDroite;
	const bool imagesChargees = oeilGauche.Load("oeil-gauche.png", 4) &&
								 oeilDroit.Load("oeil-droit.png", 4) &&
								 oublieGauche.Load(tempGauche, 4) &&
								 oublieDroite.Load(tempDroite, 4);
	std::remove(tempGauche);
	std::remove(tempDroite);
	if (!imagesChargees || oeilGauche.Width() != 640 || oeilGauche.Height() != 720 ||
		oeilDroit.Width() != 640 || oeilDroit.Height() != 720 ||
		oublieGauche.Width() != 640 || oublieGauche.Height() != 720 ||
		oublieDroite.Width() != 640 || oublieDroite.Height() != 720) {
		std::printf("[MaSalle] captures absentes ou dimensions inattendues\n");
		return 1;
	}

	NkImage difference = NkImage::Create(640, 720, 4);
	NkImage differenceOubliee = NkImage::Create(640, 720, 4);
	if (!difference.IsValid() || !differenceOubliee.IsValid()) {
		std::printf("[MaSalle] creation des images de difference impossible\n");
		return 1;
	}
	const ResultatDifference ecartDeuxYeux = CalculerDifference(oeilGauche, oeilDroit, difference);
	const ResultatDifference ecartVueOubliee = CalculerDifference(oublieGauche, oublieDroite, differenceOubliee);
	if (!difference.SavePNG("difference.png") ||
		!differenceOubliee.SavePNG("difference-vue-oubliee.png")) {
		std::printf("[MaSalle] enregistrement des differences impossible\n");
		return 1;
	}

	std::printf("deux yeux : %llu pixels sur 460800, ecart max %u\n",
				(unsigned long long)ecartDeuxYeux.pixelsDifferents,
				(unsigned int)ecartDeuxYeux.ecartMaximal);
	std::printf("vue oubliee : %llu pixels sur 460800, ecart max %u\n",
				(unsigned long long)ecartVueOubliee.pixelsDifferents,
				(unsigned int)ecartVueOubliee.ecartMaximal);

	NkRenderer::Destroy(rendererGauche);
	NkRenderer::Destroy(rendererDroit);
	rEcran->DestroyOffscreen(cibleGauche);
	rEcran->DestroyOffscreen(cibleDroite);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	std::printf("[MaSalle] %llu images traitees\n", (unsigned long long)image);
	return 0;
}
