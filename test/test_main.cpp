#include <gtest.h>
#include "..\include\calculator.h"

int main(int argc, char** argv)
{
	screen mew;
	mew.counting();
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}