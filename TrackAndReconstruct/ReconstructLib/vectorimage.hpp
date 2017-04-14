
#ifndef VECTORIMAGE_HPP
#define VECTORIMAGE_HPP

#include <QtGui/QImage>

struct RGBA {
	double r, g, b, a;
	RGBA() : r(0), g(0), b(0), a(255) { }
	RGBA(double c) : r(c), g(c), b(c), a(255) { }
	RGBA(double r, double g, double b) : r(r), g(g), b(b), a(255) { }
	RGBA(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) { }

	RGBA& operator+=(const RGBA &o) {
		r += o.r;
		g += o.g;
		b += o.b;
		return *this;
	}

	RGBA& operator-=(const RGBA &o) {
		r -= o.r;
		g -= o.g;
		b -= o.b;
		return *this;
	}

	RGBA& operator*=(double s) {
		r *= s;
		g *= s;
		b *= s;
		return *this;
	}

	bool isValid() const {
		return !(std::isnan(r) || std::isnan(g) || std::isnan(b));
	}

	double toGray() const {
		return (0.11*r + 0.59*g + 0.3*b);
	}

	bool operator==(const RGBA &o) const {
		return (fabs(r - o.r) < 1e-10
				&& fabs(g - o.g) < 1e-10
				&& fabs(b - o.b) < 1e-10
				&& fabs(a - o.a) < 1e-10);
	}

	bool operator!=(const RGBA &o) const {
		return !operator==(o);
	}
};

extern const RGBA RED;
extern const RGBA GREEN;
extern const RGBA BLUE;
extern const RGBA YELLOW;
extern const RGBA MAGENTA;
extern const RGBA BLACK;
extern const RGBA WHITE;
extern const RGBA INVALID;

class VectorImage {
public:
	//! .
	static VectorImage fromFile(const std::string &file);

	//! .
	static VectorImage fromQImage(QImage img);

	//! .
	static QImage toQImage(const VectorImage &img, bool includeAlpha = true);

public:
	VectorImage();
	VectorImage(int w, int h, const RGBA &fillVal = WHITE);

	bool isNull() const { return (w <= 0 || h <= 0 || data.size() == 0); }
	int width()   const { return w; }
	int height()  const { return h; }

	//! .
	VectorImage & fill(const RGBA &rgb);

	//! .
	void setPixel(int x, int y, const RGBA &rgb);

	//! .
	const RGBA & pixel(int x, int y) const;

	//! .
	RGBA sample(double x, double y) const;

private:
	//! .
	VectorImage(int w, int h, std::vector<RGBA>& data);

private:
	int w, h;
	std::vector<RGBA> data;
};

#endif // VECTORIMAGE_HPP
