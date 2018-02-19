#ifndef _LIBRW_STREAM_HPP_
#define _LIBRW_STREAM_HPP_
#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <exception>

namespace serialize {

template <class Stream, class T>
bool serialize_value(Stream& stream, T& data) {
  return stream.serialize_raw(static_cast<void*>(&data), sizeof(T));
}

template <class Stream, class T>
void serialize(Stream& stream, T& data) {
  if (!serialize_value(stream, data)) {
    throw std::runtime_error("Failed to serialize");
  }
}

class ReadStream
{
public:
  static constexpr bool Reading = true;
  static constexpr bool Writing = false;

  ReadStream(std::ifstream& file)
    : _file(file) { }

  bool serialize_raw(void* data, size_t n) {
    _file.read(static_cast<char*>(data), n);
    return _file.good();
  }

private:
    std::ifstream& _file;
};

class WriteStream
{
public:
  static constexpr bool Reading = false;
  static constexpr bool Writing = true;

  WriteStream(std::ofstream& file)
    : _file(file) { }

  bool serialize_raw(void* data, size_t n) {
    _file.write(static_cast<char*>(data), n);
    return _file.good();
  }

private:
    std::ofstream& _file;
};


template <class Stream, bool Debug = false>
class BlockStream
{
public:
  static constexpr bool Reading = Stream::Reading;
  static constexpr bool Writing = Stream::Writing;

  BlockStream(Stream& stream)
    : _stream(stream) 
  {
    if (Stream::Reading) {
      std::uint32_t blockSize { 0 };
      if (serialize_value(_stream, blockSize)) {
        if (Debug) {
          std::cout << "R Block [" << blockSize << " bytes]\n";
        }
        _data.resize(blockSize);
        _stream.serialize_raw(_data.data(), blockSize);
      }
    }
  }

  bool serialize_raw(void* data, size_t n) {
    if (Stream::Reading) {
      if (_readCursor + n > _data.size()) return false;
      memcpy(data, _data.data() + _readCursor, n);
      _readCursor += n;
    }
    else {
      auto cursor = _data.size();
      _data.resize(cursor + n);
      memcpy(_data.data() + cursor, data, n);
    }
    return true;
  }

  std::size_t size() const {
    return _data.size();
  }

  bool finish() {
    if (Stream::Reading) {
      return true;
    }
    std::uint32_t blockSize = _data.size();
    if (!serialize_value(_stream, blockSize)) {
        return false;
    }
    if (Debug) {
      std::cout << "W Block [" << blockSize << " bytes]\n";
    }
    return _stream.serialize_raw(_data.data(), _data.size());
  }

private:
  Stream& _stream;
  std::size_t _readCursor{0};
  std::vector<char> _data;
};

}

#endif
