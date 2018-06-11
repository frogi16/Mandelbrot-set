#include "Mandelbrot.h"

int main(int argc, char** argv)
{
	Mandelbrot application(argc, argv);
	application.loop();

	return 0;
}