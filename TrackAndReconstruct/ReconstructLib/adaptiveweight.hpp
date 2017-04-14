
#ifndef ADAPTIVEWEIGHT_HPP
#define ADAPTIVEWEIGHT_HPP

#include "vectorimage.hpp"

class AdaptiveWeight : public std::binary_function<int, int, double> {
public:
	AdaptiveWeight() { }
	AdaptiveWeight(int radius) { initialize(radius); }

	void initialize(int radius);
	void init_weights(const VectorImage &img, int cx, int cy);
	double weight(const VectorImage &img, int row, int col);

	double operator()(int row, int col) const {
		return weights[row + radius][col + radius];
	}

private:
	RGBA crgb;
	int cx, cy;
	int radius;

	std::vector<double> distance_weights;
	std::vector<std::vector<double> > weights;
};

#endif // ADAPTIVEWEIGHT_HPP
