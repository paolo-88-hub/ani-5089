#ifndef CHAINE_H
#define CHAINE_H

#include <cstddef>

class ChaineEchange {
public:
    static constexpr std::size_t NombreImagesMaximum = 4;

    explicit ChaineEchange(std::size_t nombreImages) noexcept
        : nombreImages_(nombreImages <= NombreImagesMaximum ? nombreImages : 0),
          prochaineImage_(0),
          imageEmpruntee_(0),
          empruntEnCours_(false) {}

    bool AcquireImage(std::size_t& index) noexcept {
        if (nombreImages_ == 0 || empruntEnCours_) {
            return false;
        }

        imageEmpruntee_ = prochaineImage_;
        empruntEnCours_ = true;
        index = imageEmpruntee_;
        return true;
    }

    bool ReleaseImage() noexcept {
        if (!empruntEnCours_) {
            return false;
        }

        empruntEnCours_ = false;
        prochaineImage_ = (prochaineImage_ + 1) % nombreImages_;
        return true;
    }

private:
    std::size_t nombreImages_;
    std::size_t prochaineImage_;
    std::size_t imageEmpruntee_;
    bool empruntEnCours_;
};

#endif
