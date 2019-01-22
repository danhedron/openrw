#include <boost/test/unit_test.hpp>
#include <animation/AnimationSystem.hpp>
#include <loaders/LoaderIFP.hpp>
#include <data/ModelData.hpp>
#include "test_Globals.hpp"

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

BOOST_AUTO_TEST_SUITE_END()
