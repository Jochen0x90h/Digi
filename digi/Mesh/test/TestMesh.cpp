#include <gtest/gtest.h>

#include <digi/Utility/foreach.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Mesh/Mesh.h>
#include <digi/Math/GTestHelpers.h>

#include "InitLibraries.h"

using namespace digi;


TEST(Mesh, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Mesh, GenerateTangentSpace)
{
	int positionIndices[6];
	int neighbors[6];
	int edgeFlags[2];
	int smoothingGroups[2];

	float3 positions[4];
	int normalIndices[6];
	std::vector<float3> normals;

	positionIndices[0] = 0;
	positionIndices[1] = 1;
	positionIndices[2] = 2;
	positionIndices[3] = 3;
	positionIndices[4] = 1;
	positionIndices[5] = 0;

	//arrayFill(neighbors, -1);
	//neighbors[0] = 1;
	//neighbors[3] = 0;

	edgeFlags[0] = 3;
	edgeFlags[1] = 3;
		
	positions[0] = make_float3(0, 0, 0);
	positions[1] = make_float3(1, 1, 0);
	positions[2] = make_float3(1, 0, 0);
	positions[3] = make_float3(0, 1, 0);

	MeshUtility::generateNeighbors(2, positionIndices, neighbors);

	MeshUtility::generateSmoothingGroups(2, neighbors, edgeFlags, positionIndices, 4, smoothingGroups);

	MeshUtility::generateNormals(2, smoothingGroups, positionIndices, 4, positions, normalIndices, normals);

	// generate tangent space
	std::vector<int> tangentNormalIndices;
	std::vector<float3x2> tangents;
	MeshUtility::generateTangentSpace(normals, tangentNormalIndices, tangents);
}

TEST(Mesh, GenerateTangentSpaceFromUV)
{
	std::vector<int> numVerticesPerPolygon(1);
	numVerticesPerPolygon[0] = 4;
		
	std::vector<int> positionIndices(4);
	positionIndices[0] = 0;
	positionIndices[1] = 1;
	positionIndices[2] = 2;
	positionIndices[3] = 3;
		
	std::vector<float3> positions(4);
	positions[0] = make_float3(0, 0, 0);
	positions[1] = make_float3(1, 0, 0);
	positions[2] = make_float3(1, 1, 0);
	positions[3] = make_float3(0, 1, 0);
		
	std::vector<float3> normals(4);
	normals[0] = make_float3(0, 0, 1);
	normals[1] = make_float3(0, 0, 1);
	normals[2] = make_float3(0, 0, 1);
	normals[3] = make_float3(0, 0, 1);
		
	std::vector<float2> uvs(4);
	uvs[0] = make_float2(0, 0);
	uvs[1] = make_float2(1, 0);
	uvs[2] = make_float2(1, 1);
	uvs[3] = make_float2(0, 1);

	std::vector<int> tangentIndices;
	std::vector<int> tangentNormalIndices;
	std::vector<float3x2> tangents;

	// counter-clock-wise
	MeshUtility::generateTangentSpace(numVerticesPerPolygon,
		positionIndices, positions,
		positionIndices, normals,
		positionIndices, uvs,
		tangentIndices, tangentNormalIndices, tangents);
			
	EXPECT_EPSILON_EQ(tangents[0].x, make_float3(1, 0, 0));
	EXPECT_EPSILON_EQ(tangents[0].y, make_float3(0, 1, 0));
		

	// clock-wise
	std::swap(uvs[1], uvs[3]);
	MeshUtility::generateTangentSpace(numVerticesPerPolygon,
		positionIndices, positions,
		positionIndices, normals,
		positionIndices, uvs,
		tangentIndices, tangentNormalIndices, tangents);
		
	EXPECT_EPSILON_EQ(tangents[0].x, make_float3(0, 1, 0));
	EXPECT_EPSILON_EQ(tangents[0].y, make_float3(1, 0, 0));
		

	// uv's are degenerated
	std::vector<float2> degeneratedUVs = uvs;
	foreach (float2& uv, degeneratedUVs)
		uv *= 1e-6f;
			
	MeshUtility::generateTangentSpace(numVerticesPerPolygon,
		positionIndices, positions,
		positionIndices, normals,
		positionIndices, degeneratedUVs,
		tangentIndices, tangentNormalIndices, tangents);

		
	// positions are degenerated
	std::vector<float3> degeneratedPositions = positions;
	foreach (float3& position, degeneratedPositions)
		position *= 1e-6f;
			
	MeshUtility::generateTangentSpace(numVerticesPerPolygon,
		positionIndices, degeneratedPositions,
		positionIndices, normals,
		positionIndices, uvs,
		tangentIndices, tangentNormalIndices, tangents);


	// triangulate
	std::vector<int> triangleVertexIndices;
	std::vector<int> trianglePolygonIndices;
	MeshUtility::triangulate(numVerticesPerPolygon,
		positionIndices, positions,
		triangleVertexIndices, trianglePolygonIndices);
}

TEST(Mesh, Triangulate) {
	std::vector<int> numVerticesPerPolygon;
	std::vector<int> positionIndices;
	std::vector<float3> positions;
	
	// 4-5
	// | |
	// 2-3
	// | |
	// 0-1
	positions += vector3(0.0f, 0.0f, 0.0f);
	positions += vector3(1.0f, 0.0f, 0.0f);
	positions += vector3(0.0f, 1.0f, 0.0f);
	positions += vector3(1.0f, 1.0f, 0.0f);
	positions += vector3(0.0f, 2.0f, 0.0f);
	positions += vector3(1.0f, 2.0f, 0.0f);
	
	// polygon 0
	numVerticesPerPolygon += 4;
	positionIndices += 0;
	positionIndices += 1;
	positionIndices += 3;
	positionIndices += 2;
	
	// polygon 1
	numVerticesPerPolygon += 4;
	positionIndices += 2;
	positionIndices += 3;
	positionIndices += 5;
	positionIndices += 4;

	std::vector<int> triangleVertexIndices;
	std::vector<int> trianglePolygonIndices;
	bool result = MeshUtility::triangulate(numVerticesPerPolygon, positionIndices, positions,
		triangleVertexIndices,	trianglePolygonIndices);

	// now triangleVertexIndices reference the positionIndices (i.e. index 0 to 7 are used)
	// now trianglePolygonIndices is [0, 0, 1, 1]
	

}

TEST(Mesh, Triangulate2) {
	// test polygon:
	//      *-*
	//     /   \
	//    / *-* \
	//   / /   \ \
	//  *-*     *-*
/*
	std::vector<float3> positions;
	std::vector<int> triangles;

	positions += vector3(0.0f, 0.0f, 0.0f);
	positions += vector3(4.0f, 4.0f, 0.0f);
	positions += vector3(6.0f, 4.0f, 0.0f);
	positions += vector3(10.0f, 0.0f, 0.0f);
	positions += vector3(8.0f, 0.0f, 0.0f);
	positions += vector3(6.0f, 2.0f, 0.0f);
	positions += vector3(4.0f, 2.0f, 0.0f);
	positions += vector3(2.0f, 0.0f, 0.0f);
	//positions += vector3(0.0f, 0.0f, 0.0f);

	MeshUtility::triangulate(positions, triangles);
*/
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
