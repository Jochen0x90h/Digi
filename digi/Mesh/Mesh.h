#ifndef digi_Mesh_h
#define digi_Mesh_h

#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Mesh
/// @{

namespace MeshUtility {


/// generate the neighbor list from the vertex indices.
/// two triangles are connected if they share two vertex indices.
/// each triangle has three neighbors, one for each edge. -1 stands for no neighbor.
void generateNeighbors(int numTriangles, const int* vertexIndices, int* neighbors);

/// generate smoothing groups from edge flags
bool generateSmoothingGroups(int numTriangles, const int* neighbors, const int* edgeFlags, 
	const int* vertexIndices, int numVertices, int* smoothingGroups);


/// generate normals
///
/// numTriangles    - number of triangles
/// smoothingGroups - 32 smoothing group flags per triangle
/// positionIndices - 3 position indices per triangle
/// numPositions    - number of positions
/// positions       - positions
/// normalIndices   - (out) 3 generated normal indices per triangle
/// normals         - (out) normals
bool generateNormals(int numTriangles, const int* smoothingGroups,
	const int* positionIndices, int numPositions, const float3* positions,
	int* normalIndices, std::vector<float3>& normals);


// tangent space generation

/// generate tangent space from uv set (http://www.terathon.com/code/tangent.html)
///
/// important: normals must belong to exactly one position and must not be shared
/// tangentIndices       - (out) 3 generated tangent indices per triangle
/// tangentNormalIndices - (out) for each tangent the normal index
/// tangents             - (out) tangents and bitangents
void generateTangentSpace(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, const std::vector<float3>& positions,
	const std::vector<int>& normalIndices, const std::vector<float3>& normals,
	const std::vector<int>& uvIndices, const std::vector<float2>& uvs,
	std::vector<int>& tangentIndices, std::vector<int>& tangentNormalIndices, std::vector<float3x2>& tangents); 

// generate a tangent space for the given normal
static inline float3x2 generateTangentSpace(float3 normal)
{
		float3 n = abs(normal);
		float3 b;
		if (n.x < n.y && n.x < n.z)
			b = vector3(1.0f, 0.0f, 0.0f);
		else if (n.y < n.z)
			b = vector3(0.0f, 1.0f, 0.0f);
		else
			b = vector3(0.0f, 0.0f, 1.0f);

		float3 tangent = normalize(cross(b, normal));
		return matrix2(
			tangent,
			cross(normal, tangent));
}

/// create arbitrary tangent space (without uv set)
void generateTangentSpace(const std::vector<float3>& normals, std::vector<int>& tangentNormalIndices, std::vector<float3x2>& tangents);


/// transfer tangent space from one mesh to antoher with the same topology
struct TangentContributors
{
	int positionIndex;
	
	// indices of other positions that contribute to this tangent space
	std::vector<int> contributors;
};

// build tangent contributors
void buildTangentContributors(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, 
	int numTangents, const std::vector<int>& tangentIndices, std::vector<TangentContributors>& tangentContributors);

// remove duplicate position indices in normal contributors array
void optimizeTangentContributors(std::vector<TangentContributors>& tangentContributors);

/*
	transfer tangent space from reference mesh to this mesh
	using correlation of the normal contributors
*/
void transferTangentSpace(const std::vector<TangentContributors>& tangentContributors,
	const std::vector<float3>& srcPositions, const std::vector<float3x2>& srcTangents,
	const std::vector<float3>& dstPositions, const std::vector<float3>& dstNormals, const std::vector<int>& tangentNormalIndices,
	std::vector<float3x2>& dstTangents);


// triangulation

// simple triangulation of polygon without holes.
// triangleVertexIndices index the global polygon vertex indices for each triangle
// trianglePolygonIndices is for each triangle the polygon it belongs to
bool triangulate(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, const std::vector<float3>& positions,
	std::vector<int>& triangleVertexIndices, std::vector<int>& trianglePolygonIndices);

// simple triangulation of polygon without holes
//bool triangulate(const std::vector<float3>& positions, std::vector<int>& triangles);


// vertex cache optimization

// calculate average cache miss ratio for fifo vertex cache. cache size is 3 to 32.
float calcAcmr(const std::vector<int>& triangleVertexIndices, int cacheSize);

/*
	vertex cache optimization according to
	http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
	
	newTriangleVertexIndices contains the triangle vertex indices for the triangles in optimized order.
	newTriangleVertexIndices can be the same variable as triangleVertexIndices
*/
void optimizeVertexCache(int numVertices, const std::vector<int>& triangleVertexIndices,
	std::vector<int>& newTriangleVertexIndices);


// bone batching
/*
struct BoneSet
{
	// number of bones
	int numBones;
	
	// number of bones per vertex (e.g. 4)
	int numBonesPerVertex;
	
	// for each vetex the bone indices
	std::vector<int> indices;
};


struct BoneBatch
{
	// triangles (new to old) for all triangles in this batch
	std::vector<int> triangles;

	// bone indices (new to old) for all bone sets in this batch
	std::vector<std::vector<int> > boneIndices;

	std::vector<BoneSet> boneSets;
};


void calcBoneBatching(int numTriangles, int numVertices, const std::vector<int>& vertexIndices, const std::vector<BoneSet>& boneSets,
	const std::vector<int>& maxNumBones, std::vector<BoneBatch>& batches);
*/


} // namespace MeshUtility

} // namespace digi

#endif
