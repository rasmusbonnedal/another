#include "memlist.h"

#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

bool MemList::init(const std::string& filename) {
    FILE* f1 = fopen(filename.c_str(), "rb");
    if (f1 == 0) {
        return false;
    }
    std::vector<int> data;
    while(true) {
        int v;
        if (fread(&v, 1, 4, f1) == 4) {
            data.push_back(htonl(v));
        } else {
            break;
        }
    }
    int index = 0;
    while ((data[index] & 0xffff) == 0) {
        // printf("%d: 0x%x\n", index / 5, data[index]);
        MemEntry me;
        me.type = data[index++] >> 16;
        me.bankId = data[index++];
        me.offset = data[index++];
        me.size = data[index++];
        me.unpackedSize = data[index++];
        m_meminfo.push_back(std::move(me));
    }

    std::map<int, int> stats;
    std::map<int, int> stats_unp_size;
    std::map<int, int> stats_pack_size;

    for (int i = 0; i < m_meminfo.size(); ++i) {
        const auto& d = m_meminfo[i];
        if (d.size == d.unpackedSize) {
            printf("%d: 0x%x\t0x%x\t0x%x\t0x%x\t0x%x\n", i, d.type, d.bankId, d.offset, d.size, d.unpackedSize);
        }
        stats[d.type]++;
        stats_unp_size[d.type] += d.unpackedSize;
        stats_pack_size[d.type] += d.size;
    }
    for (auto& [k, v] : stats) {
        printf("%d: %d %d %d\n", k, v, stats_pack_size[k], stats_unp_size[k]);
    }

    fclose(f1);
    return true;
}

Resource::Resource(const MemList::MemEntry& me) {
    if (me.size == 0) {
        return;
    }
    std::string filename = std::string("..\\..\\data\\bank0") + me.bankId["0123456789abcdef"];
    FILE* f1 = fopen(filename.c_str(), "rb");
    if (f1 == 0) {
        std::cout << "Could not open " << filename << std::endl;
        return;
    }
    m_data.resize(me.size);
    fseek(f1, me.offset, SEEK_SET);
    if (fread(&m_data[0], 1, me.size, f1) != me.size) {
        std::cout << "Error reading " << me.size << " bytes from " << filename << std::endl;
        return;
    }
    fclose(f1);
}
