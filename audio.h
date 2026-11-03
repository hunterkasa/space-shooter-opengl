#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <mmsystem.h>

inline void playLaserSound() {
    PlaySound(TEXT("laser.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playEnemyLaserSound() {
    PlaySound(TEXT("laser_enemy.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playPlayerHitSound() {
    PlaySound(TEXT("hit_player.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playPlayerDeathSound() {
    PlaySound(TEXT("explosion.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playEnemyDeathSound() {
    PlaySound(TEXT("explosion_enemy.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playStartSound() {
    PlaySound(TEXT("start.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playWinSound() {
    PlaySound(TEXT("win.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

inline void playLoseSound() {
    PlaySound(TEXT("lose.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}
#endif
