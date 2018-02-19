#include <boost/test/unit_test.hpp>
#include <serialization/Stream.hpp>
#include <engine/GameSerialization.inl>

#include "test_Globals.hpp"

namespace {
const std::vector<uint8_t> kSaveTestBlock {
    0xBC, 0x00, 0x00, 0x00, 0x27, 0x00, 0x4C, 0x00, 0x55, 0x00, 0x49, 0x00, 0x47, 0x00, 0x49, 0x00,
    0x27, 0x00, 0x53, 0x00, 0x20, 0x00, 0x47, 0x00, 0x49, 0x00, 0x52, 0x00, 0x4C, 0x00, 0x53, 0x00,
    0x27, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0xF5, 0x77, 0x00, 0xEC, 0xFD, 0x7F,
    0x98, 0x00, 0x00, 0x00, 0xD6, 0x07, 0x0C, 0x00, 0x01, 0x00, 0x19, 0x00, 0x00, 0x00, 0x19, 0x00,
    0x36, 0x00, 0x9C, 0x00, 0x01, 0x14, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20, 0x5D, 0x44,
    0x00, 0xA8, 0x98, 0xC3, 0x00, 0x00, 0x58, 0x41, 0xE8, 0x03, 0x00, 0x00, 0x53, 0xC1, 0x06, 0x00,
    0x0E, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0xC1, 0x06, 0x00,
    0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x34, 0x33, 0x33, 0x3F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F,
};
}

class MemoryStream
{
public:
    static constexpr bool Reading = true;
    static constexpr bool Writing = false;

    MemoryStream(const std::vector<uint8_t>& data)
    :   _data(data)
    { }

    bool serialize_raw(void* data, size_t n) {
        if (_cursor+n > _data.size()) { return false; }
        memcpy(data, _data.data() + _cursor, n);
        _cursor += n;
        return true;
    }

private:
    uint8_t _cursor {0};
    const std::vector<uint8_t>& _data;
};
using namespace serialize;

BOOST_AUTO_TEST_SUITE(SaveStreamTests)

BOOST_AUTO_TEST_CASE(test_read_block)
{
    MemoryStream mem (kSaveTestBlock);
    BlockStream <decltype(mem)> block (mem);
    BOOST_REQUIRE_EQUAL(block.size(), 188);
}

BOOST_AUTO_TEST_CASE(test_read_basic)
{
    MemoryStream mem (kSaveTestBlock);
    BlockStream <decltype(mem)> block (mem);

    BasicState state {};

    BOOST_REQUIRE(serialize_value(block, state));

    BOOST_CHECK_EQUAL(state.saveName[0], '\'');
    BOOST_CHECK_EQUAL(state.saveName[1], 'L');
    BOOST_CHECK_EQUAL(state.islandNumber, 1);
    BOOST_CHECK_EQUAL(state.cameraPosition.x, 884.5f);
    BOOST_CHECK_EQUAL(state.gameHour, 14);
    BOOST_CHECK_EQUAL(state.gameMinute, 42);
    BOOST_CHECK_EQUAL(state.frameCounter, 16466);
    BOOST_CHECK_EQUAL(state.nextWeather, 0);
    BOOST_CHECK_EQUAL(state.forcedWeather, uint16_t(-1));
    BOOST_CHECK_EQUAL(state.weatherType, 3);

}

BOOST_AUTO_TEST_SUITE_END()
