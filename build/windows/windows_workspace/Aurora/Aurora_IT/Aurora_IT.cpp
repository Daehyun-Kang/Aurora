// Aurora_IT.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
extern "C" {
	extern int az_test_main(int argc, char *argv[]);
}

int main(int argc, char *argv[])
{
	return az_test_main(argc, argv);
}

