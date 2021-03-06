#include "test_Globals.hpp"

#include <engine/GameWorld.hpp>
#include <audio/SoundSource.hpp>

BOOST_AUTO_TEST_SUITE(AudioLoadingTests, DATA_TEST_PREDICATE)

// @todo Shfil119 implement
// This test requires assets
struct F {
    SoundManager manager{Global::get().e};
};

BOOST_FIXTURE_TEST_CASE(testBufferIsPlaying, F) {
    auto audioPath =
        Global::get().e->data->index.findFilePath("audio/A1_a.wav");

    manager.loadSound("A1_a", audioPath.string());

    auto& sound = manager.getSoundRef("A1_a");
    BOOST_REQUIRE(sound.source->decodedFrames > 0);

    BOOST_REQUIRE(sound.isPlaying() == false);

    sound.play();
    BOOST_REQUIRE(sound.isPlaying() == true);

    sound.play();
    BOOST_REQUIRE(sound.isPlaying() == true);

    sound.pause();
    BOOST_REQUIRE(sound.isPaused() == true);

    sound.stop();
    BOOST_REQUIRE(sound.isStopped() == true);
}

BOOST_FIXTURE_TEST_CASE(testDecodingFramesOfMusic, F) {
    auto audioPath =
        Global::get().e->data->index.findFilePath("audio/A1_a.wav");

    manager.loadSound("A1_a", audioPath.string());

    auto& sound = manager.getSoundRef("A1_a");
    BOOST_REQUIRE(sound.source->decodedFrames > 0);
}

BOOST_FIXTURE_TEST_CASE(testDecodingFramesOfSfx, F) {
    manager.createSfxInstance(157);  // Callahan Bridge fire

    auto& sound = manager.getSfxSourceRef(157);
    BOOST_REQUIRE(sound.source->decodedFrames > 0);
}

BOOST_AUTO_TEST_SUITE_END()
