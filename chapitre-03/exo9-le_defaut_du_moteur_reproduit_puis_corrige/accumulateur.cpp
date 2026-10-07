#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKTime/NkClock.h"

#include <cstdio>
#include <vector>

using namespace nkentseu;

class AccumulateurSouris
{
public:
    AccumulateurSouris()
    {
        mGarde = NkEvents().AddEventCallbackGuard<NkMouseRawEvent>([this](NkMouseRawEvent *e)
                                                                   {
            mTotalX += e->GetDeltaX();
            mTotalY += e->GetDeltaY(); });
    }

    void Consommer(int &dx, int &dy)
    {
        dx = mTotalX;
        dy = mTotalY;
        mTotalX = 0;
        mTotalY = 0;
    }

private:
    NkCallbackGuard mGarde;
    int mTotalX = 0;
    int mTotalY = 0;
};

struct Mesure
{
    int avantX;
    int avantY;
    int apresX;
    int apresY;
};

static void EcrireSerie(const char *chemin, const std::vector<Mesure> &mesures,
                        size_t debut, bool apres)
{
    std::FILE *f = std::fopen(chemin, "w");
    if (!f)
    {
        return;
    }
    for (size_t i = debut; i < debut + 10; ++i)
    {
        const Mesure &m = mesures[i];
        int x = apres ? m.apresX : m.avantX;
        int y = apres ? m.apresY : m.avantY;
        std::fprintf(f, "%d %d\n", x, y);
    }
    std::fclose(f);
}

static void EcrireTout(const std::vector<Mesure> &mesures)
{
    std::FILE *f = std::fopen("mesure_complete.txt", "w");
    if (!f)
    {
        return;
    }
    std::fprintf(f, "image avant_x avant_y apres_x apres_y\n");
    size_t total = mesures.size();
    for (size_t i = 0; i < total; ++i)
    {
        const Mesure &m = mesures[i];
        std::fprintf(f, "%zu %d %d %d %d\n", i + 1, m.avantX, m.avantY, m.apresX, m.apresY);
    }
    std::fclose(f);
}

int nkmain(const NkEntryState &etat)
{
    (void)etat;

    NkWindowConfig config;
    config.title = "ESPACE : mesurer (bougez vite, puis arretez-vous)";
    config.width = 1280;
    config.height = 720;

    NkWindow fenetre(config);
    if (!fenetre.IsValid())
    {
        return 1;
    }

    AccumulateurSouris accumulateur;

    const size_t IMAGES_MAX = 1200;
    const int IMAGES_IMMOBILES = 30;
    bool enregistre = false;
    bool aBouge = false;
    int immobiles = 0;
    std::vector<Mesure> mesures;

    NkCallbackGuard gardeTouche = NkEvents().AddEventCallbackGuard<NkKeyPressEvent>(
        [&](NkKeyPressEvent *e)
        {
            if (e->GetKey() == NkKey::NK_SPACE && !enregistre && mesures.empty())
            {
                enregistre = true;
                fenetre.SetTitle("Mesure en cours : bougez vite, puis arretez");
            }
        });

    while (fenetre.IsOpen())
    {
        NkEvents().PollEvents();

        const auto &souris = NkEvents().GetInputState().mouse;
        int dx = 0;
        int dy = 0;
        accumulateur.Consommer(dx, dy);

        if (enregistre)
        {
            mesures.push_back({souris.rawDeltaX, souris.rawDeltaY, dx, dy});
            if (dx != 0 || dy != 0)
            {
                aBouge = true;
                immobiles = 0;
            }
            else if (aBouge)
            {
                immobiles++;
            }
            bool arrete = aBouge && immobiles >= IMAGES_IMMOBILES;
            if (arrete || mesures.size() >= IMAGES_MAX)
            {
                break;
            }
        }
        NkClock::Sleep((int64)16);
    }

    if (mesures.size() < 10)
    {
        return 0;
    }

    size_t nombre = mesures.size();
    size_t derniere = 0;
    for (size_t i = 0; i < nombre; ++i)
    {
        if (mesures[i].apresX != 0 || mesures[i].apresY != 0)
        {
            derniere = i;
        }
    }
    size_t debut = 0;
    if (derniere >= 4)
    {
        debut = derniere - 4;
    }
    if (debut + 10 > nombre)
    {
        debut = nombre - 10;
    }

    EcrireSerie("avant.txt", mesures, debut, false);
    EcrireSerie("apres.txt", mesures, debut, true);
    EcrireTout(mesures);
    return 0;
}