#include <stdio.h>
#include <string.h>
#include "theme.h"

int currentThemeIndex = 0;
Theme themes[8];

Color hexToColor(const char* hex) {
    int r, g, b;
    sscanf(hex, "#%02x%02x%02x", &r, &g, &b);
    return (Color){r / 255.0f, g / 255.0f, b / 255.0f, 1.0f}; // Alpha is set to 1.0f (fully opaque)
}

void initThemes() {
    themes[0] = (Theme){
        .name = "Molokai",
        .bg = hexToColor("#1C1E1F"),
        .cursor = hexToColor("#FB2874"),
        .text = hexToColor("#D6D6D4"),
        .modeline = hexToColor("#2D2E2E"),
    };

    themes[1] = (Theme){
        .name = "dark",
        .bg = hexToColor("#18181B"),
        .cursor = hexToColor("#cd9575"),
        .text = hexToColor("#e4e4e8"),
        .modeline = hexToColor("#222225"),
    };

    themes[2] = (Theme){
        .name = "Gum",
        .bg = hexToColor("#14171E"),
        .cursor = hexToColor("#D6A0D1"),
        .text = hexToColor("#D4D4D6"),
        .modeline = hexToColor("#191D26"),
    };

    themes[3] = (Theme){
        .name = "Ocean",
        .bg = hexToColor("#1A1A25"),
        .cursor = hexToColor("#F2F2F2"),
        .text = hexToColor("#E6E6E8"),
        .modeline = hexToColor("#252534"),
    };

    themes[4] = (Theme){
        .name = "Catppuccin",
        .bg = hexToColor("#1E1E2E"),
        .cursor = hexToColor("#B4BEFE"),
        .text = hexToColor("#CDD6F4"),
        .modeline = hexToColor("#181825"),
    };

    themes[5] = (Theme){
        .name = "Tokyonight",
        .bg = hexToColor("#1A1B26"),
        .cursor = hexToColor("#7AA2F7"),
        .text = hexToColor("#A9B1D6"),
        .modeline = hexToColor("#161620"),
    };
  
    themes[6] = (Theme){
        .name = "Doom-one",
        .bg = hexToColor("#282C34"),
        .cursor = hexToColor("#51AFEF"),
        .text = hexToColor("#BBC2CF"),
        .modeline = hexToColor("#1D2026"),
    };

    themes[7] = (Theme){
        .name = "City-lights",
        .bg = hexToColor("#1D252C"),
        .cursor = hexToColor("#51AFEF"),
        .text = hexToColor("#A0B3C5"),
        .modeline = hexToColor("#181F25"),
    };
}


void nextTheme() {
    currentThemeIndex++;
    if (currentThemeIndex >= sizeof(themes) / sizeof(Theme)) {
        currentThemeIndex = 0;
    }
    printf("Switched to next theme: %s\n", CT.name);
}

void previousTheme() {
    currentThemeIndex--;
    if (currentThemeIndex < 0) {
        currentThemeIndex = sizeof(themes) / sizeof(Theme) - 1;
    }
    printf("Switched to previous theme: %s\n", CT.name);
}


void loadTheme(const char* themeName) {
    for (int i = 0; i < sizeof(themes) / sizeof(Theme); i++) {
        if (strcmp(themes[i].name, themeName) == 0) {
            currentThemeIndex = i;
            return;
        }
    }
    // Handle the case where the theme is not found
    printf("Theme '%s' not found.\n", themeName);
}
