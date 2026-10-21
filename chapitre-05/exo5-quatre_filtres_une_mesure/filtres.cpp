#include <cstdio>
#include <fstream>
#include <utility>

#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"

using namespace nkentseu;

static const int NOMBRE_MESURES = 20;
static const int TAILLE_SOURCE = 2048;
static const int TAILLE_SORTIE = 256;

struct FiltreInfo
{
    const char *etiquette;
    const char *fichier;
    NkResizeFilter filtre;
};

static const FiltreInfo FILTRES[] = {
    {"NK_NEAREST", "nearest.png", NkResizeFilter::NK_NEAREST},
    {"NK_BILINEAR", "bilinear.png", NkResizeFilter::NK_BILINEAR},
    {"NK_BICUBIC", "bicubic.png", NkResizeFilter::NK_BICUBIC},
    {"NK_LANCZOS3", "lanczos3.png", NkResizeFilter::NK_LANCZOS3},
};

static int CompterDifferences(const NkImage &image, const NkImage &reference)
{
    int differences = 0;
    for (int y = 0; y < TAILLE_SORTIE; ++y)
    {
        for (int x = 0; x < TAILLE_SORTIE; ++x)
        {
            const math::NkColor pixel = image.GetPixel(x, y);
            const math::NkColor base = reference.GetPixel(x, y);
            if (pixel.r != base.r || pixel.g != base.g || pixel.b != base.b || pixel.a != base.a)
                ++differences;
        }
    }
    return differences;
}

int main(int argc, char *argv[])
{
    const char *cheminPhoto = argc > 1 ? argv[1] : "photo.jpeg";
    NkImage photo;
    if (!photo.Load(cheminPhoto, 4) || !photo.IsValid())
    {
        std::fprintf(stderr, "Impossible de charger la photo : %s\n", cheminPhoto);
        return 1;
    }

    NkImage source = photo.Resize(TAILLE_SOURCE, TAILLE_SOURCE, NkResizeFilter::NK_BILINEAR);
    if (!source.IsValid() || source.Width() != TAILLE_SOURCE || source.Height() != TAILLE_SOURCE)
    {
        std::fprintf(stderr, "La creation de la source 2048 x 2048 a echoue.\n");
        return 2;
    }

    NkImage resultats[4];
    double tempsMoyens[4] = {};
    for (int i = 0; i < 4; ++i)
    {
        NkImage echauffement = source.Resize(TAILLE_SORTIE, TAILLE_SORTIE, FILTRES[i].filtre);
        if (!echauffement.IsValid())
        {
            std::fprintf(stderr, "L'echauffement %s a echoue.\n", FILTRES[i].etiquette);
            return 3;
        }

        NkImage sortie;
        double tempsTotal = 0.0;
        for (int mesure = 0; mesure < NOMBRE_MESURES; ++mesure)
        {
            NkChrono chrono;
            sortie = source.Resize(TAILLE_SORTIE, TAILLE_SORTIE, FILTRES[i].filtre);
            tempsTotal += chrono.Elapsed().ToMicroseconds();
            if (!sortie.IsValid())
            {
                std::fprintf(stderr, "La reduction %s a echoue.\n", FILTRES[i].etiquette);
                return 3;
            }
        }
        tempsMoyens[i] = tempsTotal / NOMBRE_MESURES;
        if (sortie.Width() != TAILLE_SORTIE || sortie.Height() != TAILLE_SORTIE ||
            !sortie.SavePNG(FILTRES[i].fichier))
        {
            std::fprintf(stderr, "Impossible de sauvegarder %s en 256 x 256.\n", FILTRES[i].fichier);
            return 4;
        }
        resultats[i] = std::move(sortie);
    }

    std::ofstream mesures("mesures.txt");
    if (!mesures)
    {
        std::fprintf(stderr, "Impossible d'ecrire mesures.txt.\n");
        return 5;
    }
    for (int i = 0; i < 4; ++i)
    {
        const int differences = CompterDifferences(resultats[i], resultats[1]);
        mesures << FILTRES[i].etiquette << " : " << tempsMoyens[i]
                << " us, differences : " << differences << '\n';
    }

    std::printf("Source : %d x %d; sorties : %d x %d.\n", source.Width(), source.Height(),
                TAILLE_SORTIE, TAILLE_SORTIE);
    std::printf("Quatre PNG sauvegardes; moyennes de %d reductions inscrites dans mesures.txt.\n",
                NOMBRE_MESURES);
    return 0;
}