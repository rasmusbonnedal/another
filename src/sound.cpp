#include "sound.h"

#include <vector>

void playsound(Audio& audio, const char* buf, size_t len) {
    std::vector<short> data(len);
    for (int i = 0; i < len; ++i) {
        data[i] = buf[i] << 8;
    }
    audio.queue(&data[0], len);
}
