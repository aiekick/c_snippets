#include <iostream>
#include <cstring>
#include <cstdint>
#include <cmath>

struct dvec4 { double x=0.0,y=0.0,z=0.0,w=0.0; };
struct dvec2{double x=0.0,y=0.0; dvec2(const double& vx, const double& vy) : x(vx),y(vy){}};

#define dmin(a, b) (((a)<(b)) ? (a) : (b))
#define dmax(a, b) (((a)>(b)) ? (a) : (b))
#define dclamp(v, inf, sup) dmin(dmax(v,inf),sup)

dvec4 ComputeImage(const dvec2& vFragCoord, const dvec2& vSize)
{
	dvec4 pixelColor;
	const double& zoom = 0.93;
	const double& threshold = 0.14;
	const double mns = zoom / dmin(vSize.x, vSize.y);
	double zx = (vFragCoord.x * 2.0 - (double)vSize.x) * mns;
	double zy = (vFragCoord.y * 2.0 - (double)vSize.y) * mns;
	const double& px = 1.0;
	const double& py = 0.3;
	double d = 0.0;
	for (int i = 0; i < 12; i++)
	{
		d = zx * zx + zy * zy;
		if (d < 4.0)                        // 4 mean 2 * 2 (opt fix with dot instead of length
		{
			double tx = zx;                 // tmp x
			zx = zx * zx - zy * zy - px;    // julia zx
			zy = 2.0 * tx * zy - py;        // julia zy
		}
	}
	
	// distance field formula
	const double de = sqrt(d) * log(d);
	const double dd = 1.0 / d;
	
	pixelColor.x = dclamp(0.15 * dd + de, 0.0, 1.0);
	pixelColor.y = dclamp(0.1 * dd + de, 0.0, 1.0);
	pixelColor.z = dclamp(0.1 * dd + de, 0.0, 1.0);
	pixelColor.w = dclamp(1.0 * dd + de, 0.0, 1.0);
	return pixelColor;
}

int main ()
{
	const int& w = 2560;
	const int& h = 1440;
	
	const size_t datasize = 3*w*h;
	auto img = new uint8_t[datasize];
	memset(img,0,datasize);
	
	size_t x,y;
	dvec4 pixelColor;
	for(int i=0; i<w; ++i) {
		for(int j=0; j<h; ++j) {
			x=i; y=(h-1)-j;
			pixelColor = ComputeImage(dvec2((double)x,(double)y), dvec2((double)w,(double)h));
			img[(x+y*w)*3+2] = (uint8_t)(pixelColor.x * 255.0);
			img[(x+y*w)*3+1] = (uint8_t)(pixelColor.y * 255.0);
			img[(x+y*w)*3+0] = (uint8_t)(pixelColor.z * 255.0);
		}
	}
	
	const size_t headersize = 54;
	const size_t filesize = headersize + datasize;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, headersize,0,0,0};
	bmpfileheader[2] = (uint8_t)(filesize);
	bmpfileheader[3] = (uint8_t)(filesize>> 8);
	bmpfileheader[4] = (uint8_t)(filesize>>16);
	bmpfileheader[5] = (uint8_t)(filesize>>24);
	
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	bmpinfoheader[4] = (uint8_t)(w);
	bmpinfoheader[5] = (uint8_t)(w>> 8);
	bmpinfoheader[6] = (uint8_t)(w>>16);
	bmpinfoheader[7] = (uint8_t)(w>>24);
	bmpinfoheader[8] = (uint8_t)(h);
	bmpinfoheader[9] = (uint8_t)(h>> 8);
	bmpinfoheader[10] = (uint8_t)(h>>16);
	bmpinfoheader[11] = (uint8_t)(h>>24);
	
	unsigned char bmppad[3] = {0,0,0};
	auto f = fopen("img.bmp","wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; ++i)
	{
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}
	fclose(f);
	
	return 0;
}
