#include <iostream>
#include <vector>
#include <string>
#include <random>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

using namespace std;

unsigned int get_seed(string pass) {
    unsigned int seed = 0;
    for(char c : pass) seed = seed * 31 + c;
    return seed;
}

void extract_vault(const char* vault_in, const char* out_path, drwav_uint64 secret_frames, string passcode) {
    unsigned int channels, sampleRate;
    drwav_uint64 vault_frames;

    drwav_int16* pVault = drwav_open_file_and_read_pcm_frames_s16(vault_in, &channels, &sampleRate, &vault_frames, NULL);
    if (!pVault) return;

    mt19937 rng(get_seed(passcode));
    uniform_int_distribution<int> jump(1, 8);

    drwav_uint64 secret_total_samples = secret_frames * channels;
    vector<drwav_int16> revealed(secret_total_samples);

    drwav_uint64 vault_idx = 0;
    for (drwav_uint64 i = 0; i < secret_total_samples; ++i) {
        drwav_int16 reconstructed = 0;
        for (int bit = 0; bit < 16; ++bit) {
            vault_idx += jump(rng);
            if (vault_idx >= vault_frames * channels) break;
            reconstructed |= ((pVault[vault_idx] & 1) << bit);
        }
        revealed[i] = reconstructed ^ passcode[i % passcode.length()];
    }

    drwav wav;
    drwav_data_format format = {drwav_container_riff, DR_WAVE_FORMAT_PCM, channels, sampleRate, 16};
    if (drwav_init_file_write(&wav, out_path, &format, NULL)) {
        drwav_write_pcm_frames(&wav, secret_frames, revealed.data());
        drwav_uninit(&wav);
        cout << "Success! Message extracted to 'revealed.wav'." << endl;
    }
    drwav_free(pVault, NULL);
}

int main() {
    string pass; drwav_uint64 frames;
    cout << "Enter Passcode: ";
    cin >> pass;
    cout << "Enter Frame Count (from Encoder): ";
    cin >> frames;
    extract_vault("military_vault.wav", "revealed.wav", frames, pass);
    return 0;
}