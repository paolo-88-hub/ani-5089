// formats.cpp : une photo ecrite par le moteur dans cinq formats, relue dans chacun.
#define _CRT_SECURE_NO_WARNINGS
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

#include "NKImage/Core/NkImage.h"

using namespace nkentseu;

static const char *PHOTO = "photo.jpeg";
static const int QUALITE_JPEG = 90;

// Ecrit l'image dans le format demande ; renvoie le retour du moteur.
static bool Ecrire(const NkImage &img, int format, const char *fichier) {
    switch (format) {
    case 0: return img.SavePNG(fichier);
    case 1: return img.SaveJPEG(fichier, QUALITE_JPEG);
    case 2: return img.SaveBMP(fichier);
    case 3: return img.SaveQOI(fichier);
    default: return img.SaveTGA(fichier);
    }
}

// Taille d'un fichier sur le disque, en octets (0 s'il n'existe pas).
static unsigned long long TailleDisque(const char *fichier) {
    std::error_code ec;
    const auto n = std::filesystem::file_size(fichier, ec);
    return ec ? 0ULL : (unsigned long long)n;
}

// Ecart moyen par canal R, V, B entre deux images de memes dimensions (0 = identiques, max 255).
static double EcartMoyen(NkImage &a, NkImage &b) {
    const auto pa = a.Pixels();
    const auto pb = b.Pixels();
    const int sa = (int)a.Stride();
    const int sb = (int)b.Stride();
    const int ca = a.Channels();
    const int cb = b.Channels();
    int canaux = ca < cb ? ca : cb;
    if (canaux > 3) canaux = 3;
    double somme = 0.0;
    long long n = 0;
    for (int y = 0; y < a.Height(); ++y) {
        for (int x = 0; x < a.Width(); ++x) {
            for (int c = 0; c < canaux; ++c) {
                const int d = (int)pa[(size_t)y * sa + (size_t)x * ca + c] - (int)pb[(size_t)y * sb + (size_t)x * cb + c];
                somme += (d < 0) ? -d : d;
                ++n;
            }
        }
    }
    return n ? somme / (double)n : 0.0;
}

int main() {
    NkImage photo;
    if (!photo.Load(PHOTO, 4)) {
        std::printf("[ERREUR] Load(\"%s\") a echoue\n", PHOTO);
        return 1;
    }
    const int L0 = photo.Width();
    const int H0 = photo.Height();
    std::printf("Photo : %s, %d x %d, %llu octets sur le disque\n", PHOTO, L0, H0, TailleDisque(PHOTO));
    std::printf("Photo chargee : %llu octets en memoire (%d x %d x 4)\n\n",
                (unsigned long long)L0 * (unsigned long long)H0 * 4ULL, L0, H0);

    const char *noms[5] = {"png", "jpeg", "bmp", "qoi", "tga"};
    const char *fichiers[5] = {"sortie.png", "sortie.jpg", "sortie.bmp", "sortie.qoi", "sortie.tga"};

    FILE *out = std::fopen("formats.txt", "w");
    if (!out) {
        std::printf("[ERREUR] impossible d'ecrire formats.txt\n");
        return 2;
    }

    for (int i = 0; i < 5; ++i) {
        const bool ecrit = Ecrire(photo, i, fichiers[i]);
        const unsigned long long octets = TailleDisque(fichiers[i]);

        // Un fichier ecrit n'est pas un fichier relisible : on le recharge dans un NkImage neuf.
        NkImage relue;
        const bool lu = ecrit && relue.Load(fichiers[i]) && relue.IsValid();

        if (lu) {
            std::fprintf(out, "%s : %llu octets, %d x %d\n", noms[i], octets, relue.Width(), relue.Height());
            const bool memeDim = (relue.Width() == L0 && relue.Height() == H0);
            std::printf("%-5s : %llu octets, %d x %d | dimensions %s la photo", noms[i], octets,
                        relue.Width(), relue.Height(), memeDim ? "identiques a" : "DIFFERENTES de");
            if (memeDim)
                std::printf(" | ecart moyen R,V,B : %.4f / 255", EcartMoyen(photo, relue));
            std::printf("\n");
        } else {
            std::fprintf(out, "%s : %llu octets, illisible\n", noms[i], octets);
            std::printf("%-5s : %llu octets, ILLISIBLE (ecriture %s)\n", noms[i], octets, ecrit ? "reussie" : "refusee");
        }
    }
    std::fclose(out);
    std::printf("\nformats.txt ecrit.\n");
    return 0;
}