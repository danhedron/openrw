#include "AnimationSystem.hpp"

#include <rw/debug.hpp>

namespace animation {

KeyFrame
KeyFrame::interpolate(const KeyFrame &a,
                      const KeyFrame &b,
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
                        const std::vector<KeyFrame> &frames) {
    RW_ASSERT(!frames.empty());

    std::array<const KeyFrame *, 2> result{{&frames[0], &frames[0]}};

    for (auto i = 0u; i < frames.size(); ++i) {
        auto &frame = frames[i];
        if (frame.starttime >= t) {
            result[1] = &frame;
            break;
        }
        result[0] = &frame;
    }

    return result;
}

bool findKeyframes(float t, Bone *bone, KeyFrame &f1,
                   KeyFrame &f2, float &alpha) {
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

} // namespace animation
