#ifndef _RWENGINE_GAMESERIALIZATION_INL_
#define _RWENGINE_GAMESERIALIZATION_INL_
#include <serialization/Stream.hpp>
#include <engine/GameState.hpp>

namespace serialize {
template <class Stream>
bool serialize_value(Stream& stream, glm::vec3& state) {
    serialize(stream, state.x);
    serialize(stream, state.y);
    serialize(stream, state.z);
    return true;
}
}

template <class Stream>
bool serialize_value(Stream& stream, BasicState& state) {
    std::uint32_t unknownConstant = 0x031401;
    std::uint8_t align2[2];
    std::uint8_t align3[3];

    serialize::serialize(stream, state.saveName);
    serialize::serialize(stream, state.saveTime);
    serialize::serialize(stream, unknownConstant);
    if (unknownConstant != 0x031401) {
        return false;
    }
    serialize::serialize(stream, state.islandNumber);
    serialize::serialize(stream, state.cameraPosition);
    serialize::serialize(stream, state.gameMinuteMS);
    serialize::serialize(stream, state.lastTick);
    serialize::serialize(stream, state.gameHour);
    serialize::serialize(stream, align3);
    serialize::serialize(stream, state.gameMinute);
    serialize::serialize(stream, align3);
    serialize::serialize(stream, state.padMode);
    serialize::serialize(stream, align2);
    serialize::serialize(stream, state.timeMS);
    serialize::serialize(stream, state.timeScale);
    serialize::serialize(stream, state.timeStep);
    serialize::serialize(stream, state.timeStep_unclipped);
    serialize::serialize(stream, state.frameCounter);
    serialize::serialize(stream, state.timeStep2);
    serialize::serialize(stream, state.framesPerUpdate);
    serialize::serialize(stream, state.timeScale2);
    serialize::serialize(stream, state.lastWeather);
    serialize::serialize(stream, align2);
    serialize::serialize(stream, state.nextWeather);
    serialize::serialize(stream, align2);
    serialize::serialize(stream, state.forcedWeather);
    serialize::serialize(stream, align2);
    serialize::serialize(stream, state.weatherInterpolation);
    serialize::serialize(stream, state.dateTime);
    serialize::serialize(stream, state.weatherType);
    serialize::serialize(stream, state.cameraData);
    serialize::serialize(stream, state.cameraData2);

    return true;
}


#endif
