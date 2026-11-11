#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKWindow/NKMain.h"
#include "../../Nkentseu/Kernel/Runtime/NKXR/src/NKXR/NkXrSwapchain.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <thread>
#include <chrono>

using namespace nkentseu;
using namespace nkentseu::xr;

namespace {

constexpr uint32 kNombreImages = 3;
constexpr uint32 kNombreTours = 30;
constexpr uint32 kTourRedimensionnement = 15;

struct ChaineOeil {
    NkXrSwapchain chaine;
    NkXrEye oeil = NkXrEye::NK_XR_EYE_LEFT;
};

void ConfigurerApplication(NkAppData& donnees) {
    donnees.appName = "Deux chaines, deux yeux";
}

NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigurerApplication)

bool CreerChaine(ChaineOeil& oeil, NkXrEye identifiant,
                 uint32 largeur, uint32 hauteur) {
    oeil.oeil = identifiant;

    NkXrSwapchainDesc description;
    description.width = largeur;
    description.height = hauteur;
    description.imageCount = kNombreImages;
    description.eye = identifiant;
    description.name = identifiant == NkXrEye::NK_XR_EYE_LEFT
                           ? "gauche"
                           : "droit";
    if (!oeil.chaine.Init(description)) {
        return false;
    }

    return true;
}

void EnregistrerPoignees(ChaineOeil& oeil, uint64& prochainePoignee) {
    // Chaque index reçoit une poignée opaque distincte. La chaîne conserve
    // cette valeur sans en interpréter le contenu.
    for (uint32 index = 0; index < kNombreImages; ++index) {
        oeil.chaine.SetImageNative(index, prochainePoignee++);
    }
}

bool FaireUnTour(ChaineOeil& oeil, uint32& indexSoumis) {
    uint32 indexAcquis = 0;
    if (!oeil.chaine.AcquireImage(indexAcquis)) {
        return false;
    }

    const uint64 poigneeImage = oeil.chaine.GetImageNative(indexAcquis);
    if (poigneeImage == 0 || !oeil.chaine.ReleaseImage()) {
        return false;
    }

    indexSoumis = oeil.chaine.GetLastReleasedIndex();
    return oeil.chaine.GetImageNative(indexSoumis) == poigneeImage;
}

} // namespace

int nkmain(const NkEntryState& etat) {
    (void)etat;

    std::ofstream trace("trace.txt", std::ios::out | std::ios::trunc);
    std::ofstream resize("resize.txt", std::ios::out | std::ios::trunc);
    if (!trace || !resize) {
        std::cerr << "Impossible d'ouvrir les fichiers de trace.\n";
        return 1;
    }

    NkWindowConfig configuration;
    configuration.title = "Deux chaines, deux yeux";
    configuration.width = 1280;
    configuration.height = 720;
    configuration.centered = true;
    NkWindow fenetre(configuration);
    if (!fenetre.IsValid()) {
        std::cerr << "Impossible de creer la fenetre.\n";
        return 1;
    }

    ChaineOeil gauche;
    ChaineOeil droite;
    uint64 prochainePoignee = 1;
    if (!CreerChaine(gauche, NkXrEye::NK_XR_EYE_LEFT, 640, 720)
        || !CreerChaine(droite, NkXrEye::NK_XR_EYE_RIGHT, 640, 720)) {
        std::cerr << "Impossible d'initialiser les deux chaines.\n";
        return 1;
    }
    EnregistrerPoignees(gauche, prochainePoignee);
    EnregistrerPoignees(droite, prochainePoignee);

    bool continuer = true;
    for (uint32 image = 1; image <= kNombreTours && continuer; ++image) {
        NkEvent* evenement = nullptr;
        while (NkEvents().PollEvent(evenement)) {
            if (evenement->As<NkWindowCloseEvent>() != nullptr) {
                continuer = false;
            }
        }
        if (!continuer) {
            break;
        }

        if (image == kTourRedimensionnement + 1) {
            resize << "Fenetre redimensionnee : 1280x720 -> 1600x900\n";
            fenetre.SetSize(1600, 900);

            resize << "Detruit chaine gauche\n";
            gauche.chaine = NkXrSwapchain{};
            resize << "Detruit chaine droite\n";
            droite.chaine = NkXrSwapchain{};

            resize << "Recree chaine gauche\n";
            if (!CreerChaine(gauche, NkXrEye::NK_XR_EYE_LEFT, 800, 900)) {
                return 1;
            }
            resize << "Reenregistre les trois poignees de gauche\n";
            EnregistrerPoignees(gauche, prochainePoignee);

            resize << "Recree chaine droite\n";
            if (!CreerChaine(droite, NkXrEye::NK_XR_EYE_RIGHT, 800, 900)) {
                return 1;
            }
            resize << "Reenregistre les trois poignees de droite\n";
            EnregistrerPoignees(droite, prochainePoignee);
            resize.flush();
        }

        uint32 indexGauche = 0;
        uint32 indexDroit = 0;
        if (!FaireUnTour(gauche, indexGauche)
            || !FaireUnTour(droite, indexDroit)) {
            std::cerr << "Erreur dans le cycle Acquire/Release.\n";
            return 1;
        }

        trace << "image : " << image << ", gauche : " << indexGauche
              << ", droit : " << indexDroit << '\n';
        trace.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
