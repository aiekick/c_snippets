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
	const double& zoom = 1.4;
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
	int w = 2560; /* Put here what ever width you want */
	int h = 1440; /* Put here what ever height you want */
	
	FILE *f = nullptr;
	size_t headersize = 54;
	size_t datasize = 3*w*h;
	size_t filesize = headersize + datasize;  //w is your image width, h is image height, both int
	auto img = new uint8_t[datasize];
	memset(img,0,datasize);
	int x,y,r,g,b;
	
	dvec4 pixelColor;
	for(int i=0; i<w; ++i) {
		for(int j=0; j<h; ++j) {
			x=i; y=(h-1)-j;
			pixelColor = ComputeImage(dvec2((double)x,(double)y), dvec2((double)w,(double)h));
			r = (int)(pixelColor.x * 255.0);
			g = (int)(pixelColor.y * 255.0);
			b = (int)(pixelColor.z * 255.0);
			img[(x+y*w)*3+2] = (uint8_t)r;
			img[(x+y*w)*3+1] = (uint8_t)g;
			img[(x+y*w)*3+0] = (uint8_t)b;
		}
	}
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};
	
	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);
	
	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);
	
	f = fopen("img.bmp","wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(h-i-1)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}
	fclose(f);
	
	return 0;
}
