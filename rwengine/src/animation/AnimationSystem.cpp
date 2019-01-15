#include "AnimationSystem.hpp"

bool findKeyframes(float t, AnimationBone* bone, AnimationKeyframe& f1,
                   AnimationKeyframe& f2, float& alpha) {
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

AnimationKeyframe AnimationBone::getInterpolatedKeyframe(float time) {
    AnimationKeyframe f1, f2;
    float alpha;

    if (findKeyframes(time, this, f1, f2, alpha)) {
        return {glm::normalize(glm::slerp(f1.rotation, f2.rotation, alpha)),
                glm::mix(f1.position, f2.position, alpha),
                glm::mix(f1.scale, f2.scale, alpha), time,
                std::max(f1.id, f2.id)};
    }

    return frames.back();
}

AnimationKeyframe AnimationBone::getKeyframe(float time) {
    for (auto &frame : frames) {
        if (time >= frame.starttime) {
            return frame;
        }
    }
    return frames.back();
}

