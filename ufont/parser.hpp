#ifndef UFONT_PARSER
#define UFONT_PARSER

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iostream>

namespace uf::detail {
	std::string u32_to_str(uint32_t u32) {
		uint8_t p1 = (uint32_t)(u32), p2 = (uint32_t)(u32 >> 8), p3 = (uint32_t)(u32 >> 16), p4 = (uint32_t)(u32 >> 24);
		return std::string{ (char)p4, (char)p3, (char)p2, (char)p1 };
	}

	struct Parser {
		Parser(std::string const& path) : currenntIndex{ 0 } {
			std::ifstream stream(path.c_str(), std::ios::binary);
			std::streampos fsize = stream.tellg();

			if (stream.fail()) {
				std::cout << "Coudlnt Open <" << path << ">\n";
			}

			stream.seekg(0, std::ios::end);
			fsize = stream.tellg() - fsize;
			stream.seekg(0);

			buffer = std::vector<unsigned char>(fsize, unsigned char{});
			stream.read(reinterpret_cast<char*>(buffer.data()), fsize);
			size = buffer.size();
		}

		template<typename T> auto vec(size_t s) { return std::vector<T>(s, T{}); }
		template<typename K, typename V> auto umap(size_t s) { return std::unordered_map<K, V>{}; }

		template<typename T> void arr(std::vector<T>& t) {
			memcpy(&t[0], &buffer[currenntIndex], t.size());
			currenntIndex += t.size();
		}

		uint8_t u8() { return (uint8_t)buffer[currenntIndex++]; }
		uint16_t u16() { return (u8() << 8) | (u8() << 0); }
		uint32_t u32() { return (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }
		uint64_t u64() { return (u8() << 56) | (u8() << 48) | (u8() << 40) | (u8() << 32) | (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }

		int8_t i8() { return u8(); }
		int16_t i16() { return (u8() << 8) | (u8() << 0); }
		int32_t i32() { return (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }
		int64_t i64() { return (u8() << 56) | (u8() << 48) | (u8() << 40) | (u8() << 32) | (u8() << 24) | (u8() << 16) | (u8() << 8) | (u8() << 0); }

		Parser& operator +(uint8_t& v) { v = u8(); return *this; }
		Parser& operator +(uint16_t& v) { v = u16(); return *this; }
		Parser& operator +(uint32_t& v) { v = u32(); return *this; }
		Parser& operator +(uint64_t& v) { v = u64(); return *this; }
		Parser& operator +(int8_t& v) { v = i8(); return *this; }
		Parser& operator +(int16_t& v) { v = i16(); return *this; }
		Parser& operator +(int32_t& v) { v = i32(); return *this; }
		Parser& operator +(int64_t& v) { v = i64(); return *this; }

		size_t rd_position() { return currenntIndex; }
		void set_position(size_t index) { currenntIndex = index; }
	private:
		std::vector<unsigned char> buffer;
		size_t currenntIndex, size;
	};
}

#endif UFONT_PARSER