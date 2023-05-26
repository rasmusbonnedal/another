#pragma once

#include <assert.h>
#include <string>
#include <vector>

class MemList {
   public:
    enum { RT_SOUND = 0, RT_MUSIC = 1, RT_POLY_ANIM = 2, RT_PALETTE = 3, RT_BYTECODE = 4, RT_POLY_CINEMATIC = 5 };
    struct MemEntry {
        int type;
        int bankId;
        int offset;
        int size;
        int unpackedSize;
    };

    MemList() : m_init(false) {}
    bool init(const std::string& filename);

    const MemEntry& entry(size_t index) const {
        assert(index < m_meminfo.size());
        return m_meminfo[index];
    }

    size_t entries() const {
        return m_meminfo.size();
    }


   private:
    std::vector<MemEntry> m_meminfo;
    bool m_init;
};

class Resource {
   public:
    Resource(const MemList::MemEntry& me);

    const char* data() const {
        return &m_data[0];
    }

    size_t size() const {
        return m_data.size();
    }

   private:
    std::vector<char> m_data;
};
