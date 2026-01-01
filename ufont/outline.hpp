#ifndef UFONT_OUTLINE
#define UFONT_OUTLINE

#include "metric.hpp"

namespace uf::detail {
	int factorial(int n) {
		int fact = 1;
		for (int i = 1; i <= n; ++i) {
			fact *= i;
		}
		return fact;
	}

	std::vector<detail::Coord> bezierCurve(detail::Coord start, detail::Coord end, std::vector<detail::Coord> controlPoints, int resolution) {
		std::vector<detail::Coord> points;
		int numPoints = controlPoints.size();

		// Add the start and end points to the control points
		controlPoints.insert(controlPoints.begin(), start);
		controlPoints.push_back(end);
		numPoints += 2;

		for (int r = 0; r <= resolution; ++r) {
			float t = float(r) / resolution;
			detail::Coord p = { 0, 0 };

			for (int i = 0; i < numPoints; ++i) {
				float binomialCoeff = float(factorial(numPoints - 1) / (factorial(i) * factorial(numPoints - 1 - i)));
				p.x += binomialCoeff * pow(t, float(i)) * pow(1.0f - t, float(numPoints - 1 - i)) * controlPoints[i].x;
				p.y += binomialCoeff * pow(t, float(i)) * pow(1.0f - t, float(numPoints - 1 - i)) * controlPoints[i].y;
			}

			points.push_back(p);
		}

		return points;
	}

	// Converts the instructions held by a <FontGlyphs> outline into a set of contours made up of floating coordinates
	auto load_outline(detail::FontGlyph const& glyph) {
		std::vector<std::vector<detail::Coord>> scaledOutline{};
		int step = 0;

		for (int i = 0; i < glyph.outline.size(); i++) {
			auto const& contour = glyph.outline[i];
			std::vector<detail::Coord> scaledContour;

			std::vector<detail::Coord> controlPoints;
			detail::Coord start;
			bool curveStart = false;

			for (int j = 0; j < contour.size(); j++) {
				auto flag = glyph.flags[step];	

				if (flag.offCurve) {
					if (!curveStart) {
						start = detail::Coord{ contour[j].x, contour[j].y };
						curveStart = true;
					}
					else {
						auto newPoints = bezierCurve(start, detail::Coord{ contour[j].x, contour[j].y }, controlPoints, 4);
						scaledContour.insert(scaledContour.end(), newPoints.begin(), newPoints.end());
						controlPoints.clear();
						start = detail::Coord{ contour[j].x, contour[j].y };
					}
				}
				else { // These are the control points
					controlPoints.push_back(detail::Coord{ contour[j].x, contour[j].y });
				}
				

				step++;
			}

			auto newPoints = bezierCurve(start, detail::Coord{ contour[0].x, contour[0].y }, controlPoints, 10);
			scaledContour.insert(scaledContour.end(), newPoints.begin(), newPoints.end());
			controlPoints.clear();


			scaledOutline.push_back(scaledContour);
		}

		return scaledOutline;
	};

	detail::FontCharacter d_load_character(FontMetric const& fm, char c) {
	
		auto& glyph = fm.glyphs.at(c);

		FontCharacter scaledChar;
		scaledChar.adv = glyph.advance;

		scaledChar.xMax = glyph.xMax;
		scaledChar.xMin = glyph.xMin;
		scaledChar.yMax = glyph.yMax;
		scaledChar.yMin = glyph.yMin;
		scaledChar.lsb = glyph.lsb;
		scaledChar.rsb = glyph.rsb;
		scaledChar.adv = glyph.advance;

		auto contours = detail::load_outline(glyph);


		scaledChar.outline = contours;
		return scaledChar;
	}
}

#endif UFONT_OUTLINE