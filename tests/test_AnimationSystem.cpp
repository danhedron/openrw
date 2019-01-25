#include <boost/test/unit_test.hpp>
#include <animation/AnimationSystem.hpp>
#include <loaders/LoaderIFP.hpp>
#include <data/ModelData.hpp>
#include "test_Globals.hpp"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace glm {
std::ostream& operator<< (std::ostream& o, const glm::mat4 m) {
    return o << glm::to_string(m);
}
}

using namespace animation;

BOOST_AUTO_TEST_SUITE(AnimationSystemTests)

struct InterpolateTestFixture {
    KeyFrame a_ {};
    KeyFrame b_ {
        glm::angleAxis(glm::radians(90.f), glm::vec3{0.f, 1.f, 0.f}),
            {10.f, 0.f, 0.f},
            {1.f, 1.f, 2.f},
            10.f, 0
    };
};

BOOST_FIXTURE_TEST_CASE(interpolate_midpoint, InterpolateTestFixture) {
    const auto v = KeyFrame::interpolate(a_, b_, 5.f);
    BOOST_CHECK_EQUAL(v.position, glm::vec3(5.f, 0.f, 0.f));
    BOOST_CHECK_EQUAL(v.scale, glm::vec3(1.f, 1.f, 1.5f));
}

BOOST_FIXTURE_TEST_CASE(interpolate_begin, InterpolateTestFixture) {
    const auto v = KeyFrame::interpolate(a_, b_, 0.f);
    BOOST_CHECK_EQUAL(v.position, glm::vec3(0.f, 0.f, 0.f));
    BOOST_CHECK_EQUAL(v.scale, glm::vec3(1.f, 1.f, 1.f));
}

BOOST_FIXTURE_TEST_CASE(interpolate_end, InterpolateTestFixture) {
    const auto v = KeyFrame::interpolate(a_, b_, 10.f);
    BOOST_CHECK_EQUAL(v.position, glm::vec3(10.f, 0.f, 0.f));
    BOOST_CHECK_EQUAL(v.scale, glm::vec3(1.f, 1.f, 2.f));
}

struct FindKeyFrameTestFixture {
    std::vector<KeyFrame> keyframes {
        { {}, {}, {}, 0.f, 0 },
        { {}, {}, {}, 1.f, 0 },
        { {}, {}, {}, 5.f, 0 },
    };
};

BOOST_FIXTURE_TEST_CASE(get_begin, FindKeyFrameTestFixture) {
    const auto result = KeyFrame::findKeyframes(0.f, keyframes);
    BOOST_CHECK_EQUAL(result[0]->starttime, 0.f);
    BOOST_CHECK_EQUAL(result[1]->starttime, 0.f);
}

BOOST_FIXTURE_TEST_CASE(get_mid, FindKeyFrameTestFixture) {
    const auto result = KeyFrame::findKeyframes(0.5f, keyframes);
    BOOST_CHECK_EQUAL(result[0]->starttime, 0.f);
    BOOST_CHECK_EQUAL(result[1]->starttime, 1.f);
}

BOOST_FIXTURE_TEST_CASE(get_end, FindKeyFrameTestFixture) {
    const auto result = KeyFrame::findKeyframes(5.f, keyframes);
    BOOST_CHECK_EQUAL(result[0]->starttime, 1.f);
    BOOST_CHECK_EQUAL(result[1]->starttime, 5.f);
}

BOOST_FIXTURE_TEST_CASE(get_after, FindKeyFrameTestFixture) {
    const auto result = KeyFrame::findKeyframes(10.f, keyframes);
    BOOST_CHECK_EQUAL(result[0]->starttime, 5.f);
    BOOST_CHECK_EQUAL(result[1]->starttime, 5.f);
}

struct ApplyBoneTransformTestFixture {
    std::vector<KeyFrame> keyframes{
        {{1.f, 0.f, 0.f, 0.f},                     {}, {}, 0.f, 0},
        {glm::angleAxis(glm::radians(90.f),
                        glm::vec3{0.f, 1.f, 0.f}), {}, {}, 1.f, 0},
        {{1.f, 0.f, 0.f, 0.f},                     {}, {}, 2.f, 0},
    };
    ModelFrame transform {};
    Playback playbacks[1] {
        {0.f, 1.f, 1.f, Loop::None, 0, 0, {} },
    };
    PlaybackBone bone {0, &keyframes, Bone::R00, &transform};
};

BOOST_FIXTURE_TEST_CASE(test, ApplyBoneTransformTestFixture) {
    playbacks[0].time = 1.f;
    ApplyBone(playbacks, bone);
    const auto t = glm::angleAxis(glm::radians(90.f),
                        glm::vec3{0.f, 1.f, 0.f});
    const auto q = glm::toQuat(transform.getTransform());
    BOOST_CHECK_CLOSE(q.x, t.x, 0.00001f);
    BOOST_CHECK_CLOSE(q.y, t.y, 0.00001f);
    BOOST_CHECK_CLOSE(q.z, t.z, 0.00001f);
    BOOST_CHECK_CLOSE(q.w, t.w, 0.00001f);
}

struct CreatePlaybackTestFixture {
    Animation animation {"test", {}, 1.f};
    AnimationSystem system;
    ModelFrame frame;
};

BOOST_FIXTURE_TEST_CASE(playback_can_be_created, CreatePlaybackTestFixture) {
    BOOST_CHECK_EQUAL(system.activePlaybacks(), 0);
    auto playback = system.createPlayback(animation, &frame, Loop::None);
    BOOST_CHECK_NE(playback, kInvalidPlayback);
    BOOST_CHECK_EQUAL(system.activePlaybacks(), 1);
}

BOOST_FIXTURE_TEST_CASE(playback_can_be_removed, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::None);
    BOOST_CHECK_EQUAL(system.activePlaybacks(), 1);
    system.removePlayback(playback);
    BOOST_CHECK_EQUAL(system.activePlaybacks(), 0);
}

BOOST_FIXTURE_TEST_CASE(recyling_playback_ids, CreatePlaybackTestFixture) {
    auto original = system.createPlayback(animation, &frame, Loop::None);
    system.removePlayback(original);
    auto second = system.createPlayback(animation, &frame, Loop::None);
    BOOST_CHECK_EQUAL(original, second);
}

BOOST_FIXTURE_TEST_CASE(playback_animation_time, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::None);
    BOOST_CHECK_EQUAL(system.get(playback).time, 0.f);
    system.update(0.5f);
    BOOST_CHECK_EQUAL(system.get(playback).time, 0.5f);
}

BOOST_FIXTURE_TEST_CASE(playback_animation_loops, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::Repeat);
    system.update(0.5f);
    BOOST_CHECK_EQUAL( system.get(playback).time, 0.5f );
    system.update(1.0f);
    BOOST_CHECK_EQUAL( system.get(playback).time, 0.5f );
}

BOOST_FIXTURE_TEST_CASE(playback_callback, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::Repeat);
    size_t hits = 0;
    auto cb = [&](float, PlaybackID) { hits++; };
    system.addCallback(playback, {0.25f, cb});
    system.addCallback(playback, {1.f, cb});
    system.update(0.5f);
    BOOST_CHECK_EQUAL(hits, 1);
    system.update(0.5f);
    BOOST_CHECK_EQUAL(hits, 2);
    system.update(0.5f);
    BOOST_CHECK_EQUAL(hits, 3);
}

BOOST_FIXTURE_TEST_CASE(playback_set_time, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::Repeat);
    system.setTime(playback, 0.5f);
    BOOST_CHECK_EQUAL(system.get(playback).time, 0.5f);
}

BOOST_FIXTURE_TEST_CASE(playback_time_callback, CreatePlaybackTestFixture) {
    auto playback = system.createPlayback(animation, &frame, Loop::Repeat);
    size_t hits = 0;
    auto cb = [&](float, PlaybackID) { hits++; };
    system.addCallback(playback, {0.25f, cb});
    system.update(0.5f);
    BOOST_CHECK_EQUAL(hits, 1);
    system.setTime(playback, 0.0f);
    system.update(0.5f);
    BOOST_CHECK_EQUAL(hits, 2);
}

struct PlaybackBoneTransformation {
    KeyFrame identity {
        glm::angleAxis(glm::radians(0.f), glm::vec3{0.f, 1.f, 0.f}),
        {0.f, 0.f, 0.f},
        {1.f, 1.f, 2.f},
        0.f, 0
    };

    KeyFrame rotated {
        glm::angleAxis(glm::radians(90.f), glm::vec3{0.f,-1.f, 0.f}),
        {0.f, 0.f, 0.f},
        {1.f, 1.f, 2.f},
        0.5f, 0
    };

    KeyFrame translated {
        glm::angleAxis(glm::radians(0.f), glm::vec3{0.f, 1.f, 0.f}),
        {1.f, 0.f, 0.f},
        {1.f, 1.f, 2.f},
        1.f, 0
    };

    Animation animation{
        "test",
        {{
             {"root", {"root", -1, 1, 0.5f, Bone::Data::RT0, {
                 identity,
                 rotated
             }}},
             {"child", {"child", 0, -1, 1.f, Bone::Data::RT0, {
                 identity,
                 translated
             }}},
         }},
        1.f
    };
    ModelFrame root { 0 };
    ModelFramePtr child = std::make_shared<ModelFrame>(
        1, glm::mat3{1.f}, glm::vec3{1.f, 0.f, 0.f});
    AnimationSystem system;

    PlaybackBoneTransformation ()
    {
        root.setName("root");
        child->setName("child");
        root.addChild(child);
    }
};

BOOST_FIXTURE_TEST_CASE(bone_count, PlaybackBoneTransformation) {
    auto playback = system.createPlayback(animation, &root, Loop::None);
    BOOST_CHECK_EQUAL(system.boneCount(), 2);
    system.removePlayback(playback);
    BOOST_CHECK_EQUAL(system.boneCount(), 0);
}

BOOST_FIXTURE_TEST_CASE(bone_transform, PlaybackBoneTransformation) {
    auto playback = system.createPlayback(animation, &root, Loop::None);
    RW_UNUSED(playback);
    system.update(0.0f);
    {
        const auto expected = glm::translate(glm::mat4(1.f), {1.f, 0.f, 0.f});
        BOOST_CHECK_EQUAL( child->getWorldTransform(), expected );
    }
    system.update(1.0f);
    {
        const auto expected = glm::mat4_cast(
            glm::angleAxis(glm::radians(90.f), glm::vec3{0.f,-1.f, 0.f})) *
            glm::translate(glm::mat4(1.f), {2.f, 0.f, 0.f});
        BOOST_CHECK_EQUAL( child->getWorldTransform(), expected );
    }
}

BOOST_AUTO_TEST_SUITE_END()
