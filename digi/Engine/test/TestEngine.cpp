#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Math/GTestHelpers.h>
#include <digi/Engine/Track.h>

#include "InitLibraries.h"


using namespace digi;


TEST(Engine, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Engine, Track)
{
	float xValues[] = {0.0f, 1.0f, 2.0};
	
	// linear ramp as hermite spline: p0, t0, t1, p1
	float hermiteKeys[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f};
	
	// linear ramp as bezier spline: p0, p1, p2, p3
	float bezierKeys[] = {0.0f, 1.0f/3.0f, 2.0f/3.0f, 1.0f, 4.0f/3.0f, 5.0f/3.0f, 2.0f};
	
	for (float i = 0; i <= 20; ++i)
	{
		float x = float(i) * 0.1f;
		
		float hy = evalHermiteTrack(xValues, hermiteKeys, 3, x);
		float by = evalBezierTrack(xValues, bezierKeys, 3, x);
		
		EXPECT_EPSILON_EQ(hy, x);
		EXPECT_EPSILON_EQ(by, x);
	}
}

TEST(Engine, ShortTrack)
{
	// double key at 3
	uint16_t xValues[] = {0, 3, 3, 6};
		
	// linear ramp as bezier spline: p0, p1, p2, p3
	uint16_t bezierKeys[] = {0, 1, 2, 3, 3, 3, 3, 4, 5, 6};
	
	for (float i = 31; i <= 60; ++i)
	{
		float x = float(i) * 0.1f;
		
		float by = evalBezierTrack(xValues, bezierKeys, 4, x);
		
		EXPECT_EPSILON_EQ(by, x);
	}
}

TEST(Engine, WeightedTrack)
{
	// linear ramp
	float xValues[] = {0.0, 1.0f};

	// hermite spline: yp0, yt0, xt0, xt1, yt1, yp1
	float hermiteKeys[] = {0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f};

	// bezier spline: yp0, yp1, xp1, xp2, yp2, yp3
	float bezierKeys[] = {0.0f, 1.0f/3.0f, 1.0f/6.0f, 5.0f/6.0f, 2.0f/3.0f, 1.0f};
	
	for (float i = 0; i <= 10; ++i)
	{
		float x = float(i) * 0.1f;
		
		float hy = evalWeightedHermiteTrack(xValues, hermiteKeys, 2, x);
		float by = evalWeightedBezierTrack(xValues, bezierKeys, 2, x);
		
		EXPECT_EPSILON_EQ(hy, by);
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
