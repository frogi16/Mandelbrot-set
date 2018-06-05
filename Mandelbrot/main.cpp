#include "Mandelbrot.h"

int main(int argc, char** argv)
{
	Mandelbrot application;

	if (argc == 4)
	{
		application.init(argc, argv);
	}
	else
	{
		application.init();
	}
	
	application.loop();
	return 0;
}