#ifndef UFONT_UFONT
#define UFONT_UFONT

#include "metric.hpp"
#include "outline.hpp"
#include "bitmap.hpp"
#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <istream>
#include <sstream>
#define NOMINMAX

namespace uf {
	template<typename T>
	T smin(T a, T b) {
		return a < b ? a : b;
	}
	template<typename T>
	T smax(T a, T b) {
		return a > b ? a : b;
	}

	typedef detail::FontCharacter Character;
	typedef detail::FontMetric Metric;
	//typedef detail::FontGlyph Glyph;

	auto load_metric(std::string const& name) {
		return detail::load_metric(name);
	}

	auto load_character(detail::FontMetric const& fm, char c) {
		return detail::d_load_character(fm, c);
	}

	std::unordered_map<char, Character> load_characters(detail::FontMetric const& fm, std::string const& alphabet) {
		std::unordered_map<char, Character> characters;
		for (auto c : alphabet) {
			characters[c] = load_character(fm, c);
		}

		return std::move(characters);
	}

	auto bitmap_scanline(Character const& c, int height) {
		return detail::bitmap(c, height, detail::scanline);
	}

	auto bitmap_fill(Character const& c, int height) {
		return detail::bitmap(c, height, detail::fill);
	}

	auto bitmap_fill_inverse(Character const& c, int height) {
		return detail::bitmap(c, height, detail::fill_inverse);
	}

	auto bitmap_outline(Character const& c, int height) {
		return detail::bitmap(c, height, detail::outline);
	}

	auto bitmap_outline_distance(Character const& c, int height, float distance) {
		detail::gDistance = distance;
		return detail::bitmap(c, height, detail::outline_distance);
	}

	auto bitmap_signed(Character const& c, int height) {
		return detail::bitmap(c, height, detail::distance_field);
	}

	auto bitmap_points(Character const& c, int height) {
		return detail::bitmap(c, height, detail::points);
	}

	auto multi_bitmap_fill(std::string const& text, Metric const& fm, int height) {
		std::vector<std::tuple<std::vector<uint8_t>, int, int>> bitmaps;

		int totalLength = 0;

		for (auto c : text) {
			if (fm.glyphs.count(c) == 0)
				continue;

			auto character = load_character(fm, c);

			bitmaps.push_back(bitmap_fill(character, height));
			totalLength += std::get<1>(bitmaps.back());
			totalLength += 5;
		}

		int step = 0;
		std::vector<uint8_t> data(totalLength * height, 0);

		for (auto& [d, w, h] : bitmaps) {
			for (int i = 0; i < w; i++) {
				for (int j = 0; j < h; j++) {
					if (i + step + j * totalLength < data.size())
						data[(i + step) + j * totalLength] = d[i + j * w];
				}
			}

			step += w + 5;
		}

		return std::make_tuple(data, totalLength, height);
	}

	struct Atlas {
		Atlas() : width(0), height(0) {}
		Atlas(int w, int h) : width(w), height(h), mData(w * h, 0) {}
		Atlas(std::vector<uint8_t> const& data, int w, int h, std::unordered_map<char, std::array<int, 4>> positions) : 
			width(w), height(h), mData(data), positions(positions) {}

		auto& position(char c) {
			return positions[c];
		}
		auto& pixel(int x, int y) {
			return mData[x + y * width];
		}
		size_t pixelCount() const { return mData.size(); }

		void insert(std::vector<uint8_t> const& data, char c, int x, int y, int w, int h) {
			for (int i = 0; i < w; i++) {
				for (int j = 0; j < h; j++) {
					pixel(x + i, y + j) = data[i + j * w];
				}
			}
			positions[c] = { x, y, w, h };
		}

		int w() const { return width; }
		int h() const { return height; }
		auto begin() { return mData.begin(); }
		auto end() { return mData.end(); }
		auto pixels() { return mData; }
	private:
		std::unordered_map<char, std::array<int, 4>> positions;
		std::vector<uint8_t> mData;
		int width, height;
	};

	static Metric gMetric;
	static std::unordered_map<char, Character> gCharacters;
	static Atlas gAtlas;

	auto load_atlas(uf::Metric const& metric, std::unordered_map<char, uf::Character> const& characters, int height) {

		auto scaledCharacters = characters;
		float w = 0, h = 0;
		for (auto [c, ch] : scaledCharacters) {
			ch = ch.scale((float)height / (float)metric.unitsPerEm);
			auto [x, y, ww, hh] = ch.outline_bounds();
			w += ww;
			h = smax(h, hh);
		}

		Atlas atlas(w, h);

		int xStep = 0;
		for (auto [c, ch] : scaledCharacters) {
			ch = ch.scale((float)height / (float)metric.unitsPerEm);
			auto [x, y, ww, hh] = ch.outline_bounds();
			std::vector<uint8_t> data(ww * hh, 0);
			detail::fill(ch, data, ww, hh);
			//detail::anti_alias(data, ww, hh, 1);

			atlas.insert(data, c, xStep, 0, ww, hh);
			
			xStep += ch.width();
	    }

		return std::move(atlas);
	}

	void LoadGlobal(std::string const& s = "C:/Windows/Fonts/calibri.ttf", int size = 48) {
		gMetric = load_metric(s);
		gCharacters = load_characters(gMetric, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}\\|;:'\",.<>/?`~ ");
		gAtlas = load_atlas(gMetric, gCharacters, size);
	}


	int CursorOffset(std::string const& s, int size, int cursorPos) {
		if (cursorPos < 0 || cursorPos >= s.size())
			return 0;
		int offset = 0;
		for (int i = 0; i < cursorPos; i++) {
			if (gCharacters.count(s[i]) == 0)
				continue;
			auto ch = gCharacters[s[i]].scale((float)size / (float)gMetric.unitsPerEm);
			offset += ch.advance();
		}
		return offset;
		
	}

	std::pair<int, int> TextSize(std::string const& s, int size) {
		int textW = 0, textH = 0;
		for (auto c : s) {
			if (gCharacters.count(c) == 0)
				continue;

			auto ch = gCharacters[c].scale((float) size / (float) gMetric.unitsPerEm);
			textW += ch.advance();
			textH = ch.height() > textH ? ch.height() : textH;
		};

		return { textW, textH };
	}

    class TextModel {
        int letter_spacing = 0;
        int word_spacing = 0;
        int line_spacing = 0;

        int alignmentW_ = 1; // 0=Leading, 1=Center, 2=Trailing
        int alignmentH_ = 1; // 0=Top,     1=Center, 2=Bottom
        int overflow_ = 0; // 0=Wrap, 1=Clip, 2=Ellided

        struct CharQuad {
            int x, y, w, h;
        };

        // Stores quads: { x, y, w, h }
        std::vector<CharQuad> characters_;

    public:
        // -- setters for spacing & alignment --
        void setLetterSpacing(int v) { letter_spacing = v; }
        void setWordSpacing(int v) { word_spacing = v; }
        void setLineSpacing(int v) { line_spacing = v; }

        void setAlignmentW(int a) { alignmentW_ = a; }
        void setAlignmentH(int a) { alignmentH_ = a; }
        void setAlignment(int w, int h) { setAlignmentW(w); setAlignmentH(h);}

        void setOverflow(int o) { overflow_ = o; }

        // -- getters --
        int letterSpacing() const { return letter_spacing; }
        int wordSpacing()   const { return word_spacing; }
        int lineSpacing()   const { return line_spacing; }

        int alignmentW()    const { return alignmentW_; }
        int alignmentH()    const { return alignmentH_; }
        int overflow()      const { return overflow_; }

        // -- main layout function --
        
        // Assumed: global font metrics, glyph metrics, and text model members are available.
        void cache(int x, int y, int w, int h, const std::string& text, int height) {
			
			characters_.clear();
			float scale = (float)height / (float)gMetric.unitsPerEm;

			int xOffset = 0, yOffset = 0;
			int largestW = 0, totalHeight = 0;
			for (auto c : text) {
				auto ch = gCharacters[c].scale(scale);

				if (c == ' ') {
					characters_.push_back(CharQuad{ x + xOffset, y + yOffset - ch.bearingV(), 0, 0 });
					xOffset += word_spacing + ch.advance();
				}
				else {
					characters_.push_back({ x + xOffset, y + yOffset - ch.bearingV(), ch.width(), ch.height() });
					xOffset += letter_spacing + ch.advance();
				}
			}
			
			largestW = smax(largestW, xOffset);

			// Adjust horizontal and vertical alignment
			for (auto& quad : characters_) {
				quad.x += (alignmentW_ == 1 ? (w - largestW) / 2 : (alignmentW_ == 2 ? w - largestW : 0));
				quad.y += (alignmentH_ == 1 ? h / 2 : (alignmentH_ == 2 ? h - height : 0));
			}
        }

		int cursorPos(int x, int y) {
			if (x < characters_[0].x) return 0;
			for (int i = 0; i < characters_.size() - 1; i++) {
				if (x >= characters_[i].x && x <= characters_[i + 1].x) return i;
			}
			return characters_.size();
		}


        // direct access if you prefer
        auto& characters() const { return characters_; }

		auto operator[](char c) {
			return gCharacters.at(c);
		}
    };
}

#endif // UFONT_UFONT