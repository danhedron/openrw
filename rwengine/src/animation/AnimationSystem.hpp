#ifndef _RWENGINE_ANIMATIONSYSTEM_HPP_
#define _RWENGINE_ANIMATIONSYSTEM_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class ModelFrame;

namespace animation {

struct KeyFrame {
    glm::quat rotation{1.0f,0.0f,0.0f,0.0f};
    glm::vec3 position{};
    glm::vec3 scale{1.0f};
    float starttime = 0.f;
    int id = 0;

    KeyFrame(glm::quat _rotation, glm::vec3 _position, glm::vec3 _scale, float _starttime, int _id)
        : rotation(_rotation)
        , position(_position)
        , scale(_scale)
        , starttime(_starttime)
        , id(_id) {
    }

    KeyFrame() = default;

    static KeyFrame interpolate(const KeyFrame& a,
                                         const KeyFrame& b,
                                         float time);

    static std::array<const KeyFrame*,2>
    findKeyframes(float t,
                  const std::vector<KeyFrame>& keyframes);
};

struct Bone {
    std::string name;
    int32_t previous;
    int32_t next;
    float duration;

    enum Data { R00, RT0, RTS };

    Data type;
    std::vector<KeyFrame> frames;

    Bone() = default;

    Bone(const std::string& p_name, int32_t p_previous, int32_t p_next,
                  float p_duration, Data p_type,
                  const std::vector<KeyFrame>& p_frames)
        : name(p_name)
        , previous(p_previous)
        , next(p_next)
        , duration(p_duration)
        , type(p_type)
        , frames(p_frames) {
    }

    ~Bone() = default;

    KeyFrame getInterpolatedKeyframe(float time);
};

/**
 * @brief Animation data object, stores bones.
 */
struct Animation {
    std::string name;
    std::unordered_map<std::string, Bone> bones;

    ~Animation() = default;

    float duration;
};

enum class Loop {
    None,
    Repeat
};

using PlaybackID = uint32_t;
static constexpr auto kInvalidPlayback = static_cast<PlaybackID>(-1);

using Callback = std::function<void(float,PlaybackID)>;
using PlaybackCallback = std::pair<float,Callback>;

struct Playback {
    float time;
    float speed;
    float duration;
    Loop loop;
    uint8_t nextCallback;
    uint8_t callbackCount;
    std::array<PlaybackCallback, 2> callbacks;
};

struct PlaybackBone {
    PlaybackID playback;
    std::vector<KeyFrame>* keyframes;
    Bone::Data fields;
    ModelFrame* transform;
};

void ApplyBone(Playback playbacks[], PlaybackBone& bone);

class AnimationSystem {

    std::vector<Playback> playbacks_;
    std::vector<PlaybackID> activePlaybacks_;
    std::vector<PlaybackBone> playbackBones_;

    Playback& get_(PlaybackID);

public:

    PlaybackID createPlayback(Animation&,ModelFrame*,Loop);

    void addCallback(PlaybackID, PlaybackCallback);

    const Playback& get(PlaybackID) const;

    void setTime(PlaybackID, float);

    void removePlayback(PlaybackID);

    size_t activePlaybacks() const {
        return activePlaybacks_.size();
    }

    size_t boneCount() const {
        return playbackBones_.size();
    }

    void update(float time);
};

} // namespace animation

#endif //_RWENGINE_ANIMATIONSYSTEM_HPP_

