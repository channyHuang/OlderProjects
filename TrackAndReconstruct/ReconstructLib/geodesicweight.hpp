
#ifndef GEODESIC_HPP
#define GEODESIC_HPP

#include "vectorimage.hpp"


class GeodesicWeight : public std::binary_function<int, int, double> {
public:
	GeodesicWeight() { }
	GeodesicWeight(int radius) { initialize(radius); }

	void initialize(int radius);
	void init_weights(const VectorImage &img, int x, int y);

	double operator()(int row, int col) const {
		return weights[row + radius][col + radius];
	}

private:
	int cx, cy;
	int radius;
	std::vector<std::vector<double> > weights;
};

#endif // GEODESIC_HPP
