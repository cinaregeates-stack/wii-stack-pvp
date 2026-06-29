// Simple C++ starter for Wii (libogc + wiiuse)
// This file focuses on menu, input, MotionPlus swing detection, health, rumble, and SFX placeholders.

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/exi.h>
#include <ogc/machine/processor.h>
#include <ogc/mem/linear.h>
#include <cstring>
#include <cmath>
#include <cstdio>

#define MAX_PLAYERS 4

static GXRModeObj *rmode;
static void *framebuffer[2];

// Game state
int playerCount = 2;
float health[MAX_PLAYERS];
bool alive[MAX_PLAYERS];

// Swing detection history
struct AccelSample { float x,y,z; };
AccelSample lastAccel[MAX_PLAYERS];

// Simple utility: map value from in range to out range
float mapf(float v, float inMin, float inMax, float outMin, float outMax) {
    if (v <= inMin) return outMin;
    if (v >= inMax) return outMax;
    return outMin + (v - inMin) * (outMax - outMin) / (inMax - inMin);
}

void init_video() {
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);

    framebuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    framebuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(video_get_console(rmode), 20, 64, framebuffer[0], 0, rmode->fbWidth, rmode->xfbHeight);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(framebuffer[0]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
}

void init_input() {
    WPAD_Init();
}

void reset_game() {
    for (int i=0;i<MAX_PLAYERS;i++) {
        health[i] = 100.0f;
        alive[i] = true;
        lastAccel[i] = {0,0,0};
    }
}

// Compute swing strength using delta accel magnitude between frames
float compute_swing_strength(int p, const WPADData *d) {
    if (!d) return 0.f;
    float ax = d->gforce.x; // gravity/accel approximate
    float ay = d->gforce.y;
    float az = d->gforce.z;
    float dx = ax - lastAccel[p].x;
    float dy = ay - lastAccel[p].y;
    float dz = az - lastAccel[p].z;
    float mag = sqrtf(dx*dx + dy*dy + dz*dz);
    // store current for next frame
    lastAccel[p].x = ax; lastAccel[p].y = ay; lastAccel[p].z = az;
    return mag;
}

int main(int argc, char **argv) {
    init_video();
    init_input();
    reset_game();

    console_clear();
    printf("wii-stack-pvp - starter build\n");
    printf("Press A to open menu\n");

    bool inMenu = true;
    int menuSel = 0; // 0: start, 1: players++

    while (1) {
        WPAD_ScanPads();
        PAD_ScanPads();

        // Read Wiimotes
        WPADData *data[MAX_PLAYERS];
        for (int i=0;i<MAX_PLAYERS;i++) data[i] = WPAD_Data(i);

        if (inMenu) {
            console_clear();
            printf("=== MENU ===\n");
            printf("Players: %d (use + / - with D-PAD)\n", playerCount);
            printf("Start game: press A\n");
            printf("Press Home at any time to return to menu\n");

            // simple input: player 1 changes selection
            WPADData *p0 = WPAD_Data(0);
            if (p0) {
                if (p0->btns_h & WPAD_BUTTON_A) {
                    inMenu = false;
                    reset_game();
                    VIDEO_WaitVSync(); // debounce
                }
                if (p0->btns_h & WPAD_BUTTON_UP) { if (playerCount < 4) playerCount++; VIDEO_WaitVSync(); }
                if (p0->btns_h & WPAD_BUTTON_DOWN) { if (playerCount > 2) playerCount--; VIDEO_WaitVSync(); }
            }
        } else {
            // Game loop: scan inputs, compute swings, handle hits, show HUD
            console_clear();
            printf("--- Game running (%d players) ---\n", playerCount);
            for (int p=0;p<playerCount;p++) {
                WPADData *d = WPAD_Data(p);
                // detect swing strength
                float strength = compute_swing_strength(p, d);
                // map strength (experimentally) to percent damage
                // typical magnitude range unknown; map 0.0..10.0 to 0..15
                float damagePct = mapf(strength, 0.2f, 8.0f, 3.0f, 15.0f);

                // If large swing detected and A pressed or Z triggered, register hit on a target (simple demo: hit next player)
                bool attacked = false;
                if (d) {
                    if ((d->btns_h & WPAD_BUTTON_A) || strength > 1.5f) {
                        attacked = true;
                    }
                    // block fallback with B
                    bool blocking = (d->btns_h & WPAD_BUTTON_B);
                    if (attacked) {
                        int target = (p+1) % playerCount; // simple nearest target
                        if (!blocking && alive[target]) {
                            // apply damage
                            health[target] -= damagePct;
                            if (health[target] <= 0) { alive[target] = false; health[target] = 0; }
                            printf("P%d hits P%d for %.1f%% damage\n", p+1, target+1, damagePct);
                            // stub: play hit SFX (placeholder)
                            // For real SFX, integrate libogc audio and load WAVs from assets/
                        }
                    }
                }
                // HUD per player
                printf("P%d Health: [", p+1);
                int bars = (int)(health[p] / 5.0f); // 20 bars
                for (int i=0;i<20;i++) putchar(i < bars ? '#' : '-');
                printf("] %.1f%%\n", health[p]);

                // rumble if under 15%
                if (health[p] > 0 && health[p] < 15.0f) {
                    WPAD_Rumble(p, 1);
                } else {
                    WPAD_Rumble(p, 0);
                }
            }

            // Check for reset/home
            WPADData *p0 = WPAD_Data(0);
            if (p0 && (p0->btns_h & WPAD_BUTTON_HOME)) {
                inMenu = true;
                VIDEO_WaitVSync();
            }

            // Simple win detection
            int aliveCount = 0; int lastAlive = -1;
            for (int i=0;i<playerCount;i++) if (alive[i]) { aliveCount++; lastAlive = i; }
            if (aliveCount <= 1) {
                if (aliveCount == 1) printf("Player %d wins!\n", lastAlive+1);
                else printf("All players defeated. Draw.\n");
                printf("Press A to return to menu\n");
                // wait for A
                while (1) {
                    WPAD_ScanPads();
                    if (WPAD_Data(0) && (WPAD_Data(0)->btns_h & WPAD_BUTTON_A)) break;
                    VIDEO_WaitVSync();
                }
                inMenu = true;
            }
        }

        VIDEO_WaitVSync();
    }

    return 0;
}
