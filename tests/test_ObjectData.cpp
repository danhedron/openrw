#include <boost/test/unit_test.hpp>
#include <data/ModelData.hpp>
#include <loaders/LoaderIDE.hpp>
#include "test_Globals.hpp"

BOOST_AUTO_TEST_SUITE(ObjectDataTests)

BOOST_AUTO_TEST_CASE(test_replace_building) {
    GameState s;

    constexpr GameObjectID kTestObject = 12;

    BOOST_CHECK(s.buildingReplaced(kTestObject, 100, 101));
    BOOST_CHECK(!s.buildingReplaced(kTestObject, 100, 101));
    BOOST_REQUIRE(s.findReplacement(kTestObject));
    BOOST_CHECK_EQUAL(s.findReplacement(kTestObject)->ref, kTestObject);
    BOOST_CHECK_EQUAL(s.findReplacement(kTestObject)->oldModel, 100);
    BOOST_CHECK_EQUAL(s.findReplacement(kTestObject)->newModel, 101);
    BOOST_CHECK_EQUAL(s.findReplacement(kTestObject)->type, 2);
}

BOOST_AUTO_TEST_CASE(test_clear_building_replacement) {
    GameState s;

    constexpr GameObjectID kTestObject = 12;

    BOOST_CHECK(s.buildingReplaced(kTestObject, 100, 101));
    BOOST_CHECK(!s.buildingReplaced(kTestObject, 100, 101));
    BOOST_CHECK(s.findReplacement(kTestObject));
    BOOST_CHECK(s.buildingReplaced(kTestObject, 101, 100));
    BOOST_CHECK(!s.findReplacement(kTestObject));
}

#if RW_TEST_WITH_DATA
BOOST_AUTO_TEST_CASE(test_object_data) {
    {
        LoaderIDE l;

        l.load(Global::get().getGamePath() + "/data/maps/generic.ide", {});

        BOOST_ASSERT(l.objects.find(1100) != l.objects.end());

        auto obj = l.objects[1100].get();

        auto def = dynamic_cast<SimpleModelInfo*>(obj);

        BOOST_ASSERT(def->type() == ModelDataType::SimpleInfo);

        BOOST_CHECK_EQUAL(def->name, "rd_Corner1");
        BOOST_CHECK_EQUAL(def->textureslot, "generic");
        BOOST_CHECK_EQUAL(def->getNumAtomics(), 1);
        BOOST_CHECK_EQUAL(def->getLodDistance(0), 220);
        BOOST_CHECK_EQUAL(def->flags, 0);
    }
    {
        LoaderIDE l;

        l.load(Global::get().getGamePath() + "/data/default.ide", {});

        BOOST_ASSERT(l.objects.find(90) != l.objects.end());

        auto obj = l.objects[90].get();

        auto def = dynamic_cast<VehicleModelInfo*>(obj);

        BOOST_ASSERT(def->type() == ModelDataType::VehicleInfo);

        BOOST_CHECK_EQUAL(def->name, "landstal");
        BOOST_CHECK_EQUAL(def->textureslot, "landstal");
        BOOST_CHECK_EQUAL(def->vehicletype_, VehicleModelInfo::CAR);
        BOOST_CHECK_EQUAL(def->handling_, "LANDSTAL");
        BOOST_CHECK_EQUAL(def->vehiclename_, "LANDSTK");
        BOOST_CHECK_EQUAL(def->vehicleclass_, VehicleModelInfo::RICHFAMILY);
        BOOST_CHECK_EQUAL(def->frequency_, 10);
        BOOST_CHECK_EQUAL(def->wheelmodel_, 164);
        BOOST_CHECK_CLOSE(def->wheelscale_, 0.8f, 0.01f);
    }
}
#endif

BOOST_AUTO_TEST_SUITE_END()
