# UFont

A header‑only C++ library for parsing and writing TrueType Font (TTF) files.

## Example Usage

```cpp
#include "ufont/ufont.hpp"

int main() {
    const unsigned int fSize = 48;

    uf::Metric gMetric;
    std::unordered_map<char, uf::Character> gCharacters;
    uf::Atlas gAtlas;

    gMetric = uf::load_metric("C:/Windows/Fonts/Calibri.ttf");
    gCharacters = uf::load_characters(
        gMetric,
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()_+-=[]{}\\|;:'\",.<>/?`~ "
    );
    gAtlas = uf::load_atlas(gMetric, gCharacters, fSize);

    uf::TextModel tm;
    tm.cache(x, y, w, h, "some text to render", fSize);

    int j = 0;
    for (auto const& c : tm.characters()) {
        auto& region = gAtlas.position(text[j++]);

        // Render character 'c' at position (region.x, region.y)
        // with size (region.w, region.h)
    }
}

Notes

Not all features are implemented yet
Not all languages are supported yet
