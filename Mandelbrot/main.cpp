#include "Mandelbrot.h"

int main(int argc, char** argv)
{
	Mandelbrot application;
	application.init(argc, argv);
	application.loop();

	return 0;
}