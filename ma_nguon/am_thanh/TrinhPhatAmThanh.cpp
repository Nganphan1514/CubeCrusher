// ma_nguon/am_thanh/TrinhPhatAmThanh.cpp
#include <iostream>

bool musicOn = true;
bool sfxOn = true;

void toggleMusic()
{
    musicOn = !musicOn;
    std::cout << "Music: " << (musicOn ? "ON\n" : "OFF\n");
    // TODO: integrate audio playback library (SDL_mixer) to actually play music
}

void toggleSfx()
{
    sfxOn = !sfxOn;
    std::cout << "SFX: " << (sfxOn ? "ON\n" : "OFF\n");
}
