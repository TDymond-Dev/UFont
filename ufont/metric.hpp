#ifndef UFONT_FONT
#define UFONT_FONT

#include "parser.hpp"
#include "tbl_bsln.hpp"
#include "tbl_cmap.hpp"
#include "tbl_cvt.hpp"
#include "tbl_directory.hpp"
#include "tbl_format4.hpp"
#include "tbl_glyf.hpp"
#include "tbl_head.hpp"
#include "tbl_hhea.hpp"
#include "tbl_hmtx.hpp"
#include "tbl_kern.hpp"
#include "tbl_loca.hpp"
#include "tbl_maxp.hpp"
#include "tbl_name.hpp"
#include "tbl_os2.hpp"
#include "tbl_post.hpp"

#include <cmath>
#define NOMINMAX

namespace uf::detail {
	template<typename T>
	T smin(T a, T b) {
		return a < b ? a : b;
	}
	template<typename T>
	T smax(T a, T b) {
		return a > b ? a : b;
	}

	struct Coord { 
		float x, y;
		bool offcurve = true;
	};

	struct FontGlyph {
		int16_t xMin, yMax, xMax, yMin;
		int16_t lsb, rsb, advance;
		std::vector<std::vector<Coord>> outline;
		std::vector<tbl_glyf::SimpleGlyph::Flag> flags;
	};

	struct FontCharacter {
		int16_t xMin, yMax, xMax, yMin;
		int16_t lsb, rsb, adv;

		int16_t width() const {
			return xMax - xMin;
		}
		int16_t height() const {
			return (yMax + abs(yMin));
		}
		int16_t bearingH() const {
			return lsb;
		}
		int16_t bearingV() const {
			return yMax;
		}
		int16_t advance() const {
			return adv;
		}
		//int width, height;
		//int bearingV, bearingH;
		//int advance;

		std::vector<std::vector<detail::Coord>> outline;

		auto outline_bounds() const {
			float x = FLT_MAX, y = FLT_MAX, x2 = FLT_MIN, y2 = FLT_MIN;
			if (outline.empty())
				return std::tuple(0.0f, 0.0f, 0.0f, 0.0f);

			for(auto& contour : outline)
				for(auto [cx, cy, offcurve] : contour)
					x = smin(x, cx), y = smin(y, cy), x2 = smax(x2, cx), y2 = smax(y2, cy);
			return std::tuple(x, y, ceil(x2 - x), ceil(y2 - y));
		}

		// font size / units per em
		auto scale(float f) const {
			FontCharacter temp = *this;

			for (auto& contour : temp.outline)
				for (auto& [cx, cy, offcurve] : contour)
					cx *= f, cy *= f;

			temp.adv *= f;
			temp.xMin *= f;
			temp.yMax *= f;
			temp.xMax *= f;
			temp.yMin *= f;
			temp.lsb *= f;
			temp.rsb *= f;

			return temp;
		}
	};

	struct FontMetric {
		bool initialized = false;

		operator bool() const {
			return initialized;
		}

		int16_t unitsPerEm;
		int16_t baseline;
		int16_t advanceMaxWidth;
		int16_t minLeftsideBearing;
		int16_t minRightsideBearing;
		int16_t lowercaseHeight;
		int16_t uppercaseHeight;
		int16_t avgCharWidth;
		int16_t strikeOutPos;
		int16_t strikeOutSize;
		int16_t descent; // The descent is the distance from the base line to the lowest point characters extend to. In practice, some font designers break this rule, e.g. to accommodate a certain character, so it is possible (though rare) that this value will be too small.
		int16_t ascent; // The ascent of a font is the distance from the baseline to the highest position characters extend to. 
		
		// TO DO
		int16_t underlinePos; // post table
		int16_t underlineThickness; // post table
			
		int16_t lineGap;
		int16_t lineWidth;
		int16_t dpi;
		int16_t weight;

		int16_t leading; // The leading is the vertical space between lines of text. It is the sum of the ascent, the descent and the line gap.

		auto operator ()(char c) {
			return glyphs.at(c);
		}

		std::unordered_map<char, FontGlyph> glyphs;

		auto scale(int height) const {
			float scale = (float) height / (float) unitsPerEm;

			FontMetric temp = *this;

			for (auto& [c, glyph] : temp.glyphs) {
				glyph.xMin *= scale;
				glyph.yMax *= scale;
				glyph.xMax *= scale;
				glyph.yMin *= scale;
				glyph.lsb *= scale;
				glyph.rsb *= scale;
				glyph.advance *= scale;

				for (auto& contour : glyph.outline)
					for (auto& [cx, cy, offcurve] : contour)
						cx *= scale, cy *= scale;
			}

			temp.unitsPerEm = height;
			temp.baseline *= scale;
			temp.advanceMaxWidth *= scale;
			temp.minLeftsideBearing *= scale;
			temp.minRightsideBearing *= scale;
			temp.lowercaseHeight *= scale;
			temp.uppercaseHeight *= scale;
			temp.avgCharWidth *= scale;
			temp.strikeOutPos *= scale;
			temp.strikeOutSize *= scale;
			temp.descent *= scale;
			temp.ascent *= scale;
			temp.lineGap *= scale;
			temp.leading *= scale;

			return temp;
		}
	};


	FontMetric load_metric(std::string const& path) {


		FontMetric metric;

		auto offset = [](tbl_directory& dir, std::string name) {
			return dir.tableRecords.at(name).offset;
			};
		auto length = [](tbl_directory& dir, std::string name) {
			return dir.tableRecords.at(name).length;
			};

		Parser parser{ path };

		tbl_directory directory;
		tbl_bsln baseline;
		tbl_head head;
		tbl_name name;
		tbl_os2 os2;
		tbl_cvt cvt;
		tbl_kern kern;
		tbl_maxp maxp;
		tbl_hhea hhea;
		tbl_hmtx hmtx;
		tbl_loca loca;
		tbl_glyf glyf;
		tbl_cmap cmap;
		tbl_post post;
		Format4 fmt4;

		directory.parse(parser);

		if (directory.tableRecords.count("bsln") != 0) 	
			baseline.parse(parser, offset(directory, "bsln"));
		if (directory.tableRecords.count("head") != 0)
			head.parse(parser, offset(directory, "head"));
		if (directory.tableRecords.count("name") != 0)
			name.parse(parser, offset(directory, "name"));
		if (directory.tableRecords.count("OS/2") != 0)
			os2.parse(parser, offset(directory, "OS/2"));
		if (directory.tableRecords.count("cvt ") != 0)
			cvt.parse(parser, offset(directory, "cvt "), length(directory, "cvt "));
		if (directory.tableRecords.count("kern") != 0)
			kern.parse(parser, offset(directory, "kern"));
		if (directory.tableRecords.count("maxp") != 0)
			maxp.parse(parser, offset(directory, "maxp"));
		if (directory.tableRecords.count("hhea") != 0)
			hhea.parse(parser, offset(directory, "hhea"));
		if (directory.tableRecords.count("hmtx") != 0)
			hmtx.parse(maxp.numGlyphs, parser, offset(directory, "hmtx"));
		if (directory.tableRecords.count("loca") != 0)
			loca.parse(head.indexToLocFormat, maxp.numGlyphs, parser, offset(directory, "loca"));
		if (directory.tableRecords.count("glyf") != 0)
			glyf.parse(maxp.numGlyphs, loca.offsets, parser, offset(directory, "glyf"));
		if (directory.tableRecords.count("post") != 0)
			post.parse(parser, offset(directory, "post"));
		if (directory.tableRecords.count("cmap") != 0)
			cmap.parse(parser, offset(directory, "cmap"));
		if (directory.tableRecords.count("cmap") != 0)
			fmt4.parse(parser, offset(directory, "cmap"), length(directory, "cmap"), metric.initialized);


		for (int i = 0; i < 256; i++) {
			char letter = static_cast<char>(i);
			//std::cout << "Loading Character: " << (char) i << "\n";
			FontGlyph glyph;


			uint16_t index = fmt4.retrieve_glyf(letter);
			if (index == UINT16_MAX) {
				//std::cout << "No glyph found for " << (char) letter << std::endl;
				continue;
			}	
			//std::cout << "Loading Character: " << (char) i << " with index " << index <<  "\n";
		
			

			auto& const glyfA = glyf.glyphs[index];

		
			auto& const hmtxA = hmtx.entries[index];
			
			// Glyph Bounding Box
			glyph.xMin = glyfA.xMin;
			glyph.yMin = glyfA.yMin;
			glyph.xMax = glyfA.xMax;
			glyph.yMax = glyfA.yMax;
			glyph.lsb = hmtxA.leftSideBearing;
			
			glyph.advance = hmtxA.advanceWidth;
			glyph.rsb = (hmtxA.advanceWidth - hmtxA.leftSideBearing - (glyfA.xMax - glyfA.xMin));

			// Glyph outline(Simple Glyph
			auto extract_simple_glyph = [&](int gIndex, int xOffset = 0, int yOffset = 0) {
				float xAbs = 0, yAbs = 0;
				auto simple_glyph = glyf.simpleGlyphs[glyf.glyphs[gIndex].index];
				auto glyph_header = glyf.glyphs[gIndex];
				int step = 0;


				for (int i = 0; i < glyph_header.noOfContours; i++) {
					std::vector<Coord> contour;
					auto end = simple_glyph.outline.endPtsOfContours[i];
					for (int j = step; j <= end; j++) {
						xAbs += simple_glyph.outline.xCoords[step], yAbs += simple_glyph.outline.yCoords[step];
						contour.push_back(Coord{ xAbs + xOffset, yAbs + yOffset });
						step++;
					}

					glyph.outline.push_back(contour);
				};

				glyph.flags.insert(glyph.flags.end(), simple_glyph.outline.flags.begin(), simple_glyph.outline.flags.end());
			};
			
			// Glyph Outline
			auto extract_compound_glyph = [&](detail::tbl_glyf::CompoundGlyph const& fg) {
				for (auto const& component : fg.components) {
					extract_simple_glyph(component.index, component.argument1, component.argument2);
				}
			};

			auto glyph_header = glyf.glyphs[index];

		
			if (glyph_header.noOfContours >= 0) {
				extract_simple_glyph(index);
			}
			else {
				for (auto const& cGlyph : glyf.compoundGlyphs) {
					if (cGlyph.index == index) {
						for (auto const& component : cGlyph.components) {
							extract_simple_glyph(component.index, component.argument1, component.argument2);
						}
					}
				}
			}

			metric.glyphs.try_emplace(letter, glyph);
		}

		if (directory.tableRecords.count("head") != 0) {
			metric.unitsPerEm = head.unitsPerEm;
		}

		if (directory.tableRecords.count("bsln") != 0) {
			metric.baseline = baseline.defaultBaseline;
		}

		if (directory.tableRecords.count("OS/2") != 0) {
			metric.lowercaseHeight = os2.sxHeight;
			metric.uppercaseHeight = os2.sCapHeight;
			metric.avgCharWidth = os2.xAvgCharWidth;
			metric.strikeOutPos = os2.yStrikeoutPosition;
			metric.strikeOutSize = os2.yStrikeoutSize;
			metric.weight = os2.usWeightClass;
			metric.lineWidth = os2.usWidthClass;
		}

		if (directory.tableRecords.count("hhea") != 0) {
			metric.advanceMaxWidth = hhea.advanceWidthMax;
			metric.minLeftsideBearing = hhea.minLeftSideBearing;
			metric.minRightsideBearing = hhea.minRightSideBearing;
			metric.descent = hhea.descent;
			metric.ascent = hhea.ascent;
			metric.lineGap = hhea.lineGap;
		}

		if (directory.tableRecords.count("post") != 0) {
			metric.underlinePos = post.underlinePosition;
			metric.underlineThickness = post.underlineThickness;
		}

		metric.dpi = 96;

		if (directory.tableRecords.count("OS/2") != 0 && directory.tableRecords.count("hhea") != 0) {
			metric.leading = (os2.sTypoAscender + os2.sTypoDescender) - hhea.lineGap;

		}

		return metric;
	};
}

#endif // UFONT_FONT