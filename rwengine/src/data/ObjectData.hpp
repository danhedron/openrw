#ifndef _RWENGINE_OBJECTDATA_HPP_
#define _RWENGINE_OBJECTDATA_HPP_
#include <objects/ObjectTypes.hpp>

#include <cstdint>

struct BuildingReplacement {
    //! Unknown
    uint32_t type;
    GameObjectID ref;
    uint32_t newModel;
    uint32_t oldModel;
};

#endif
