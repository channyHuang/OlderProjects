//---------------------------------------------------------------------
//---------------------------------------------------------------------
#include "adaptiveweight.hpp"

//---------------------------------------------------------------------

namespace {
	const double COLOR_SIGMA = 10.0;
}

using std::sqrt;

//---------------------------------------------------------------------

void AdaptiveWeight::initialize(int radius) {
	this->radius = radius;

	distance_weights.resize(radius + 1);
	for(int ind = 0; ind <= radius; ++ind)
		distance_weights[ind] = std::exp(-ind / (1.0*radius));

	weights.resize(2*radius + 1);
	for(int ind = 0; ind < 2*radius + 1; ++ind)
		weights[ind].resize(2*radius + 1, 0.0);
}

//---------------------------------------------------------------------

void AdaptiveWeight::init_weights(const VectorImage &img, int cx, int cy) {
	this->cx = cx;
	this->cy = cy;

	//
	crgb = img.pixel(cx, cy);

	//
	for(int row = -radius; row <= radius; ++row)
		for(int col = -radius; col <= radius; ++col)
			weights[row + radius][col + radius] = weight(img, row, col);
}

//---------------------------------------------------------------------

double AdaptiveWeight::weight(const VectorImage &img, int row, int col) {
	double weight = 0.0;

	RGBA rgb = img.pixel(cx + col, cy + row);
	if(rgb.isValid()) {
		rgb -= crgb;

		const double diff = sqrt(rgb.r*rgb.r + rgb.g*rgb.g + rgb.b*rgb.b);
		const double w1 = distance_weights[abs(row)]*distance_weights[abs(col)];
		const double w2 = std::exp(-diff / COLOR_SIGMA);

		weight = w1*w2;
		if(std::isnan(weight))
			weight = 0.0;
	}

	return weight;
}