#include <boost/test/unit_test.hpp>
#include <data/Clump.hpp>
#include <engine/Animator.hpp>
#include <loaders/LoaderIFP.hpp>
#include <glm/gtx/string_cast.hpp>
#include "test_Globals.hpp"

BOOST_AUTO_TEST_SUITE(AnimationTests, DATA_TEST_PREDICATE)

BOOST_AUTO_TEST_CASE(test_matrix) {
    {
        auto animation = std::make_shared<animation::Animation>();

        /** Models are currently needed to relate animation bones <=> model
         * frame #s. */
        auto test_model = Global::get().d->loadClump("player.dff");

        Animator animator(test_model);

        animation->duration = 1.f;
        animation->bones.emplace(
            "player", animation::Bone(
                          "player", 0, 0, 1.0f, animation::Bone::RT0,
                          std::vector<animation::KeyFrame>{
                              {glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                               glm::vec3(0.f, 0.f, 0.f), glm::vec3(), 0.f, 0},
                              {glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                               glm::vec3(0.f, 1.f, 0.f), glm::vec3(), 1.0f, 1},
                          }));

        animator.playAnimation(0, animation, 1.f, false);

        animator.tick(0.0f);

        const auto& root = test_model->findFrame("player");

        BOOST_CHECK(glm::vec3(root->getTransform()[3]) ==
                    glm::vec3(0.f, 0.f, 0.f));

        animator.tick(1.0f);

        BOOST_CHECK(glm::vec3(root->getTransform()[3]) ==
                    glm::vec3(0.f, 1.f, 0.f));
    }
}

BOOST_AUTO_TEST_SUITE_END()
