#ifndef _RWENGINE_LOADERIFP_HPP_
#define _RWENGINE_LOADERIFP_HPP_

#include <rw/forward.hpp>

#include <animation/AnimationSystem.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

class LoaderIFP {
    template <class T>
    T* read(char* data, size_t* ofs) {
        size_t b = *ofs;
        *ofs += sizeof(T);
        return reinterpret_cast<T*>(data + b);
    }
    template <class T>
    T* peek(char* data, const size_t* ofs) {
        return reinterpret_cast<T*>(data + *ofs);
    }

    std::string readString(char* data, size_t* ofs);

public:
    struct BASE {
        char magic[4];
        uint32_t size;
    };

    struct INFO {
        BASE base;
        std::uint32_t entries;
        // null terminated string
        // entry data
    };

    struct ANPK {
        BASE base;
        INFO info;
    };

    struct NAME {
        BASE base;
    };

    struct DGAN {
        BASE base;
        INFO info;
    };

    struct CPAN {
        BASE base;
    };

    struct ANIM {
        BASE base;
        char name[28];
        std::uint32_t frames;
        int32_t unk;
        int32_t next;
        int32_t prev;
    };

    struct KFRM {
        BASE base;
    };

    struct Anim {
        std::string name;
    };

    AnimationSet animations;

    bool loadFromMemory(char* data);
};

#endif
