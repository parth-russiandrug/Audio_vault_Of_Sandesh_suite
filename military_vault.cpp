#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cstdio>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

using namespace std;

// --- INTEGRITY LAYER ---
void print_file_integrity(const char* filename) {
    char command[256];
    sprintf(command, "shasum -a 256 %s", filename);
    FILE* pipe = popen(command, "r");
    if (!pipe) return;
    char buffer[128];
    if (fgets(buffer, 128, pipe)) {
        cout << "\n[INTEGRITY LAYER ACTIVE]" << endl;
        cout << "SHA-256 Digital Fingerprint: " << buffer;
        cout << "Seal Status: SECURED" << endl;
    }
    pclose(pipe);
}

unsigned int generate_seed(string pass) {
    unsigned int seed = 0;
    for(char c : pass) seed = seed * 31 + c;
    return seed;
}

void create_military_vault(const char* cover_in, const char* secret_in, const char* vault_out, string passcode) {
    // FIXED: Separate variables for Cover and Secret to prevent data collision
    unsigned int cChannels, cSampleRate, sChannels, sSampleRate;
    drwav_uint64 cover_frames, secret_frames;

    drwav_int16* pCover = drwav_open_file_and_read_pcm_frames_s16(cover_in, &cChannels, &cSampleRate, &cover_frames, NULL);
    drwav_int16* pSecret = drwav_open_file_and_read_pcm_frames_s16(secret_in, &sChannels, &sSampleRate, &secret_frames, NULL);

    if (!pCover || !pSecret) {
        cout << "Error: Ensure files are in the folder and are 16-bit PCM WAV." << endl;
        return;
    }

    mt19937 rng(generate_seed(passcode));
    uniform_int_distribution<int> jump(1, 5); // Optimized jump range for better space management

    drwav_uint64 cover_idx = 0;
    drwav_uint64 total_cover_samples = cover_frames * cChannels;
    drwav_uint64 secret_total_samples = secret_frames * sChannels;

    for (drwav_uint64 i = 0; i < secret_total_samples; ++i) {
        // XOR Encryption Layer
        drwav_int16 encrypted_sample = pSecret[i] ^ (passcode.empty() ? 0 : passcode[i % passcode.length()]);

        for (int bit = 0; bit < 16; ++bit) { //scattering now there is not any fixed pattern in my bits
            cover_idx += jump(rng);

            if (cover_idx >= total_cover_samples) {
                cout << "\n[CRITICAL ERROR] Cover file too short for this secret message!" << endl;
                drwav_free(pCover, NULL); drwav_free(pSecret, NULL);
                return;
            }

            // LSB Embedding (16th bit)
            int bit_val = (encrypted_sample >> bit) & 1;
            pCover[cover_idx] = (pCover[cover_idx] & ~1) | bit_val;
        }
    }

    drwav wav;
    // Vault must match the Cover's format to remain invisible
    drwav_data_format format = {drwav_container_riff, DR_WAVE_FORMAT_PCM, cChannels, cSampleRate, 16};

    if (drwav_init_file_write(&wav, vault_out, &format, NULL)) {
        drwav_write_pcm_frames(&wav, cover_frames, pCover);
        drwav_uninit(&wav);

        cout << "\n--- VAULT CREATION SUCCESSFUL ---" << endl;
        cout << "Secret Message Frames: " << secret_frames << " (MUST PROVIDE TO DECODER)" << endl;
        print_file_integrity(vault_out);
    }

    drwav_free(pCover, NULL); drwav_free(pSecret, NULL);
}

int main() {
    string pass;
    cout << "Enter SANDESH Passcode: ";
    getline(cin, pass); // FIXED: Actually takes user input now

    create_military_vault("cover.wav", "secret.wav", "military_vault.wav", pass);
    return 0;
}