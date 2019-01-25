#include "AnimationSystem.hpp"

#include <data/Clump.hpp>

#include <rw/debug.hpp>

#include <algorithm>

namespace animation {

KeyFrame
KeyFrame::interpolate(const KeyFrame& a,
                      const KeyFrame& b,
                      float time) {
    auto alpha = (time - a.starttime) / (b.starttime - a.starttime);
    return {
        glm::normalize(glm::slerp(a.rotation, b.rotation, alpha)),
        glm::mix(a.position, b.position, alpha),
        glm::mix(a.scale, b.scale, alpha), time,
        std::max(a.id, b.id)
    };
}

std::array<const KeyFrame *, 2>
KeyFrame::findKeyframes(float t,
                        const std::vector<KeyFrame>& frames) {
    RW_ASSERT(!frames.empty());

    std::array<const KeyFrame *, 2> result{{&frames[0], &frames[0]}};

    for (auto i = 0u; i < frames.size(); ++i) {
        auto& frame = frames[i];
        if (frame.starttime >= t) {
            result[1] = &frame;
            return result;
        }
        result[0] = &frame;
    }

    result[1] = result[0];

    return result;
}

bool findKeyframes(float t, Bone *bone, KeyFrame& f1,
                   KeyFrame& f2, float& alpha) {
    for (size_t f = 0; f < bone->frames.size(); ++f) {
        if (t <= bone->frames[f].starttime) {
            f2 = bone->frames[f];

            if (f == 0) {
                if (bone->frames.size() != 1) {
                    f1 = bone->frames.back();
                } else {
                    f1 = f2;
                }
            } else {
                f1 = bone->frames[f - 1];
            }

            float tdiff = (f2.starttime - f1.starttime);
            if (tdiff == 0.f) {
                alpha = 1.f;
            } else {
                alpha = glm::clamp((t - f1.starttime) / tdiff, 0.f, 1.f);
            }

            return true;
        }
    }
    return false;
}

KeyFrame Bone::getInterpolatedKeyframe(float time) {
    KeyFrame f1, f2;
    float alpha;

    if (findKeyframes(time, this, f1, f2, alpha)) {
        return {glm::normalize(glm::slerp(f1.rotation, f2.rotation, alpha)),
                glm::mix(f1.position, f2.position, alpha),
                glm::mix(f1.scale, f2.scale, alpha), time,
                std::max(f1.id, f2.id)};
    }

    return frames.back();
}

void ApplyBone(Playback *playbacks, PlaybackBone& bone) {
    const auto& playback = playbacks[bone.playback];
    const auto& keyframes = *bone.keyframes;
    const auto& active = KeyFrame::findKeyframes(playback.time, keyframes);
    const auto& p = active[0] != active[1] ?
        KeyFrame::interpolate(*active[0], *active[1], playback.time) :
        *active[0];
    if (bone.fields == Bone::RT0)
        bone.transform->setTranslation(
            bone.transform->getDefaultTranslation() + p.position);
    bone.transform->setRotation(glm::mat3_cast(p.rotation));
}

PlaybackID AnimationSystem::createPlayback(Animation& a, ModelFrame* root,
                                           Loop loop) {
    PlaybackID playbackID = 0;
    for (auto it = activePlaybacks_.begin();
         it != activePlaybacks_.end();
         ++it, playbackID++) {
        if (*it != playbackID) break;
    }

    if (playbackID == playbacks_.size()) {
        playbacks_.resize(playbacks_.size() + 1);
    }
    playbacks_[playbackID] = {0.f, 1.f, a.duration, loop, 0, 0, {}};

    for (auto&[name, bone] : a.bones) {
        auto frame = root->getName() == name ?
                     root : root->findDescendant(name);
        if (!frame) {
            continue;
        }

        RW_ASSERT(std::find_if(playbackBones_.begin(), playbackBones_.end(),
                               [&](const PlaybackBone& b) {
                                   return b.transform == frame;
                               })
                  == playbackBones_.end());

        playbackBones_.push_back({playbackID, &bone.frames, bone.type, frame});
    }

    activePlaybacks_.push_back(playbackID);
    return playbackID;
}

void AnimationSystem::removePlayback(PlaybackID playback) {
    RW_ASSERT(std::find(activePlaybacks_.begin(),
                        activePlaybacks_.end(),
                        playback) != activePlaybacks_.end());

    playbackBones_.erase(
        std::remove_if(playbackBones_.begin(),
                       playbackBones_.end(),
                       [&](const PlaybackBone& pb) {
            return pb.playback == playback;
        }),
        playbackBones_.end());

    activePlaybacks_.erase(std::remove(activePlaybacks_.begin(),
                                       activePlaybacks_.end(),
                                       playback),
                           activePlaybacks_.end());
}

Playback& AnimationSystem::get_(PlaybackID playback) {
    RW_ASSERT(std::find(activePlaybacks_.begin(),
                        activePlaybacks_.end(),
                        playback) != activePlaybacks_.end());
    return playbacks_[playback];
}

const Playback& AnimationSystem::get(PlaybackID playback) const {
    RW_ASSERT(std::find(activePlaybacks_.begin(),
                        activePlaybacks_.end(),
                        playback) != activePlaybacks_.end());
    return playbacks_[playback];
}

void AnimationSystem::update(float time) {
    for (auto& i : activePlaybacks_) {
        auto& playback = playbacks_[i];
        playback.time += playback.speed * time;
    }

    for (auto& i : activePlaybacks_) {
        auto& playback = playbacks_[i];
        auto& callbacks = playbacks_[i].callbacks;
        while (playback.nextCallback < playback.callbackCount &&
            callbacks[playback.nextCallback].first <= playback.time)
        {
            callbacks[playback.nextCallback].second(playback.time, i);
            playback.nextCallback++;
        }
    }

    activePlaybacks_.erase(
        std::remove_if(
            activePlaybacks_.begin(), activePlaybacks_.end(),
            [&](const PlaybackID i) {
                auto& p = playbacks_[i];
                if (p.time >= p.duration) {
                    if (p.loop == Loop::None) {
                        return true;
                    }
                    p.time = std::fmod(p.time, p.duration);
                    p.nextCallback = 0;
                }
                return false;
            }), activePlaybacks_.end());

    for (auto& playbackBone : playbackBones_) {
        ApplyBone(playbacks_.data(), playbackBone);
    }
}

void AnimationSystem::addCallback(PlaybackID id, PlaybackCallback c) {
    auto& playback = get_(id);
    RW_ASSERT(playback.callbackCount < playback.callbacks.size());
    playback.callbacks[playback.callbackCount++] = c;
    std::sort(begin(playback.callbacks),
        begin(playback.callbacks) + playback.callbackCount,
        [&](auto& a, auto& b) { return a.first < b.first; });
}

void AnimationSystem::setTime(PlaybackID id, float time) {
    auto& playback = get_(id);
    playback.time = time;
    for (uint8_t i = 0u; i < playback.callbackCount; ++i) {
        if (playback.callbacks[i].first >= time) {
            playback.nextCallback = i;
            break;
        }
    }
}

} // namespace animation
