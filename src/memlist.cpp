#include "memlist.h"

#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#define NOMINMAX
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
        stats[d.type]++;
        stats_unp_size[d.type] += d.unpackedSize;
        stats_pack_size[d.type] += d.size;
    }
    fclose(f1);
    return true;
}

extern "C" {
uint32_t bytekiller_unpack(uint8_t* dst, int dstSize, const uint8_t* src, int srcSize);
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
    if (me.unpackedSize != me.size) {
        std::vector<char> unp(me.unpackedSize);
        uint32_t result = bytekiller_unpack((uint8_t*)unp.data(), (int)unp.size(), (uint8_t*)m_data.data(), (int)m_data.size());
        m_data = unp;
    }
    m_me = me;
    fclose(f1);
}

std::string Resource::dump() const {
    std::string retval;
    char buf[256];
    for (int i = 0; i < (m_data.size() + 7) / 8; ++i) {
        int ofs = i * 8;
        int num = std::min(8, (int)m_data.size() - ofs);
        sprintf(buf, "%4.4x: ", ofs);
        retval.append(buf);
        for (int j = 0; j < num; j++) {
            sprintf(buf, "%2.2x ", (char)data()[ofs + j] & 0xff);
            retval.append(buf);
        }
        for (int j = num; j < 8; ++j) {
            sprintf(buf, "   ");
            retval.append(buf);
        }
        sprintf(buf, "   ");
        retval.append(buf);
        for (int j = 0; j < num; j++) {
            char c = data()[ofs + j];
            if (c < 32 || (c & 0x80)) c = '.';
            sprintf(buf, "%c", c);
            retval.append(buf);
        }
        sprintf(buf, "\n");
        retval.append(buf);
    }
    return retval;
}

void Resource::dump(int width) const {
    for (int i = 0; i < (m_data.size() + width - 1) / width; ++i) {
        int ofs = i * width;
        int num = std::min(width, (int)m_data.size() - ofs);
        printf("%4.4x: ", ofs);
        for (int j = 0; j < num; j++) {
            char c = data()[ofs + j];
            if (c < 32 || (c & 0x80)) c = '.';
            printf("%c", c);
        }
        printf("\n");
    }
}

ResourceMgr::ResourceMgr(const MemList& memlist) {
    for (int i = 0; i < memlist.entries(); ++i) {
        m_resources.emplace_back(memlist.entry(i));    
    }
}

const Resource& ResourceMgr::get(size_t i) const {
    return m_resources[i];
}

size_t ResourceMgr::size() const {
    return m_resources.size();
}
