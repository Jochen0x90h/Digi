// we need standard constant macros
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif

#include <map>

#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>

#include "Mesh.h"


namespace digi {

/// @addtogroup Mesh
/// @{

namespace MeshUtility {

// internal edge struct for generateNeighbors
struct Edge
{
	int v1;
	int v2;
	int triangleIndex;
};

bool operator ==(const Edge& a, const Edge& b)
{
	return a.v1 == b.v1 && a.v2 == b.v2;
}

bool operator <(const Edge& a, const Edge& b)
{
	return a.v1 < b.v1 || (a.v1 == b.v1 && a.v2 < b.v2);
}

void generateNeighbors(int numTriangles, const int* vertexIndices, int* neighbors)
{
	std::vector<Edge> edges(numTriangles * 3);

	// fill array with all vertex pairs
	for (int i = 0; i < numTriangles; ++i)
	{
		// iterate over the three edges of the triangle
		for (int e = 0; e < 3; ++e)
		{
			neighbors[i * 3 + e] = -1;

			edges[i * 3 + e].v1 = vertexIndices[i * 3 + e];
			edges[i * 3 + e].v2 = vertexIndices[i * 3 + (e + 1) % 3];
			edges[i * 3 + e].triangleIndex = i;
		}
	}

	// sort vertex pairs
	sort(edges);

	// find neighbors
	for (int i = 0; i < numTriangles; ++i)
	{
		Edge edge;

		// iterate over the three edges of the triangle
		for (int e = 0; e < 3; ++e)
		{
			// build edge in reverse order
			edge.v2 = vertexIndices[i * 3 + e];
			edge.v1 = vertexIndices[i * 3 + (e + 1) % 3];

			// find neighbor (which contains edge in reverse order)
			std::vector<Edge>::iterator it = find(edges, edge);
			if (it != edges.end())
			{
				neighbors[i * 3 + e] = it->triangleIndex;
			}
		}
	}
}

enum ProcessedState
{
	UNPROCESSED,
	QUEUED,
	PROCESSED
};

bool generateSmoothingGroups(int numTriangles, const int* neighbors, const int* edgeFlags, 
	const int* vertexIndices, int numVertices, int* smoothingGroups)
{
	/*
		Each triangle has three neighbors, which are stored in the neighbors 
		array. Therefore the size of the neighbors array must be numTriangles * 3.
		The edgeflags consist of one int per triangle which holds the three 
		edgeflags in the first three bits..

		Per triangle we have a processed flag. we chose one triangle for flooding 
		and mark it as processed. Flooding is processed with a queue where every 
		unprocessed smooth neighbor is pushed. Per vertex we have forbiddenSmoothingGroups 
		flags in one int.
		step1:
			Determine smoothingGroupFlags to use.

			f--a--d
			|D/ \B|
			|/ A \|
			c-----b
			 \ C /
			  \ /
			   e

			We have four triangles (A, B, C, D) and we look at the first (A).
			Now we create the variables a to f that hold forbidden smoothing 
			group flags for the indicated vertices from the array
			vertexForbiddenSmoothingGroups. For each processed smooth neighbor, 
			the smoothing groups of the neigbor are removed from the involved
			vertices:
		
			if B is smooth and processed then a &= ~B; b &= ~B; d &= ~B;
			if C is smooth and processed then b &= ~C; c &= ~C; e &= ~C;
			if D is smooth and processed then c &= ~D; a &= ~D; f &= ~D;

			The combination of a to f (a|b|c | d|e|f) are the forbidden smoothing
			groups.	Find the first smoothing group which is not forbidden and 
			chose it for painting.

		step2:
			add smoothing group flag to processed smooth neighbors.
		
		step3:
			add smoothing group flag flag to vertices of current triangle and 
			vertices of processed smooth neighbors.

		beware of one triangle with multiple edge connections to another triangle
	*/

	
	// state of triangle: unprocessed, queued, processed
	std::vector<ProcessedState> triangleState(numTriangles, UNPROCESSED);
	
	// forbidden smoothing groups for each vertex
	std::vector<int> vertexForbiddenSmoothingGroups(numVertices, 0);
	
	// the number of processed triangle
	int numProcessedTriangles = 0;
	
	// for debug output: the maximum smoothGroup bit
	int highestSmoothGroupBit = 0;

	// for debug output: number of smoothing group overflows
	int numOverflows = 0;

	// clear the smoothing Groups
	for (int i = 0; i < numTriangles; ++i)
		smoothingGroups[i] = 0;

	while (numProcessedTriangles < numTriangles)
	{
		// the current triangle
		int triangleIndex = 0;
		
		// find the first not processed triangle
		while (triangleIndex < numTriangles)
		{
			if (triangleState[triangleIndex] == UNPROCESSED)
			{
				break;
			}
		
			++triangleIndex;
		}

		// initialize the queue
		std::vector<int> triangleFillQueue;
		triangleFillQueue += triangleIndex;
		size_t curQueuePos = 0;

		// process the queue
		while (curQueuePos < triangleFillQueue.size())
		{
			// get the triangle step in the queue and mark current as processed
			triangleIndex = triangleFillQueue[curQueuePos];
			++curQueuePos;
			++numProcessedTriangles;
			triangleState[triangleIndex] = PROCESSED;
			
			// get the abc / def values
			int abcIndices[3];
			int defIndices[3];
			int abc[3];
			int def[3];
			
			// iterate over edges of triangle
			for (int e = 0; e < 3; ++e)
			{
				// initialize abc
				abcIndices[e] = triangleIndex * 3 + e;
				abc[e] = vertexForbiddenSmoothingGroups[vertexIndices[abcIndices[e]]];
				
				// initialize def
				defIndices[e] = -1;
				def[e] = 0;

				// get neighbor
				int neighbor = neighbors[triangleIndex * 3 + e];

				if ((neighbor != -1) // is there a triangle connected?
					&& (edgeFlags[triangleIndex] & (1 << e)) // is edge smooth?
					&& (triangleState[neighbor] == PROCESSED))
				{
					// find the vertex of triangle B/C/D (neighbor) that is not contained 
					// in A (triangleIndex). this is vertex d/e/f
					
					// check on wich side A is connected
					int i;
					for (i = 0; i < 3; ++i)
					{
						if (triangleIndex == neighbors[neighbor * 3 + i])
							break;
					}

					// the opposite vertex is d/e/f and is not contained in A
					int vertexIndex = (i + 2) % 3;

					defIndices[e] = neighbor * 3 + vertexIndex;
					def[e] = vertexForbiddenSmoothingGroups[vertexIndices[defIndices[e]]];
				}
			}

			// step1
			// go through every edge of the current triangle
			for (int e = 0; e < 3; ++e)
			{
				if (edgeFlags[triangleIndex] & (1 << e)) // is edge smooth?
				{
					int neighbor = neighbors[triangleIndex * 3 + e];
					
					if (neighbor == -1)
						continue;

					// add the neighbor to the queue if necessary
					if (triangleState[neighbor] == UNPROCESSED)
					{
						triangleState[neighbor] = QUEUED;
						triangleFillQueue += neighbor;
					}
			
					if (triangleState[neighbor] == PROCESSED)
					{
						// remove smoothing groups of neighbour from the forbidden smoothing groups if neighbour is already processed
						int neighborSmoothingGroups = smoothingGroups[neighbor];
						abc[(e + 0) % 3] &= ~neighborSmoothingGroups;
						abc[(e + 1) % 3] &= ~neighborSmoothingGroups;
						def[(e + 0) % 3] &= ~neighborSmoothingGroups;
					}
				}
			}
	
			// these are the forbidden flags				
			int forbidden = (abc[0] | abc[1] | abc[2]) | (def[0] | def[1] | def[2]);

			// find the first not forbidden
			int smoothGroupBit = 1;
			while ((forbidden & smoothGroupBit) != 0)
			{
				smoothGroupBit <<= 1;
			}
		
			if (smoothGroupBit == 0)
				++numOverflows;

			if (smoothGroupBit > highestSmoothGroupBit)
				highestSmoothGroupBit = smoothGroupBit;

			// step2
			smoothingGroups[triangleIndex] |= smoothGroupBit;

			for (int e = 0; e < 3; ++e)
			{
				if (edgeFlags[triangleIndex] & (1 << e)) // is edge smooth?
				{
					int neighbor = neighbors[triangleIndex * 3 + e];
					
					if (neighbor == -1)
						continue;
					
					if (triangleState[neighbor] == PROCESSED)
					{
						smoothingGroups[neighbor] |= smoothGroupBit;
					}
				}
			}

			// step3
			for (int e = 0; e < 3; e++)
			{
				vertexForbiddenSmoothingGroups[vertexIndices[abcIndices[e]]] |= smoothGroupBit;
				
				if (defIndices[e] != -1)
					vertexForbiddenSmoothingGroups[vertexIndices[defIndices[e]]] |= smoothGroupBit;
			}
		}
	}

	// return false on error (not enough smoothing groups)
	return numOverflows == 0;
}

	
struct SmoothingGroupsNormal
{
	int smoothingGroups;
	float3 normal;
	
	SmoothingGroupsNormal() : smoothingGroups(0) {}
	SmoothingGroupsNormal(int smoothingGroups, const float3& normal)
		: smoothingGroups(smoothingGroups), normal(normal) {}
};

bool generateNormals(int numTriangles, const int* smoothingGroups, const int* positionIndices,
	int numPositions, const float3* positions, int* normalIndices, std::vector<float3>& normals)
{
    /*
		step 1:
		Per position vertex we have an array of smoothing groups and normal (normalsPerPosition).
		For every triangle we do the following:
		We calculate the normal (face normal).
		Then for all three position vertices we search the array if the smoothing
		groups match. We remove all matches from the array and combine the smoothing
		groups by logical or and accumulate the normals.
		Then we add the smoothing groups and normal to the array.
	    
		step 2:
		Normalize normals and create new normal vertex property
	    
		step 3:
		Next we build an array with the starting normal index for every position vertex.
		We write the normals into the normal array according to the order
		in the normalsPerPosition array. Now we generate the normal indices for all
		triangles by searching the smoothing groups in the normalsPerPosition array.
	*/


	// step1
	// iterate over all triangles, build face normals 
	// and build smoothing group normals for every position vertex

	// contains for every position the smoothing groups and normals
	std::vector<std::vector<SmoothingGroupsNormal> > normalsPerPosition(numPositions);

	// iterate over all triangles
	for (int triangleIndex = 0; triangleIndex < numTriangles; ++triangleIndex)
	{
		// get the smoothing groups
		int sg = smoothingGroups[triangleIndex];

		// get the position vertices
		float3 p0 = positions[positionIndices[triangleIndex * 3 + 0]];
		float3 p1 = positions[positionIndices[triangleIndex * 3 + 1]];
		float3 p2 = positions[positionIndices[triangleIndex * 3 + 2]];
		
		// calc the edge vectors
		float3 e0 = p0 - p2;
		float3 e1 = p1 - p0;
		float3 e2 = p2 - p1;

		// calculate the normal and the length of the normal
		float3 normal = cross(e0, e1);
		float len = length(normal);

		// scale to one
		if (len < 1e-10f)
		{
			normal = vector3(0.0f, 0.0f, 1.0f);
			len = 1;
		}
		else
		{
			normal /= len;
		}

		/**
			calculate the weights for every point
			
			weight0: angle enclosed by edges 0 and 1
			len is the sinus of the enclosed angle
			-dot(e0, e1) is the cosinus of the enclosed angle
			
			weight1: angle enclosed by edges 1 and 2

			weight2: angle enclosed by edges 2 and 0
		*/

		float weights[3];
		weights[0] = (float)atan2(len, -dot(e0, e1));
		weights[1] = (float)atan2(dot(cross(e1, e2), normal), -dot(e1, e2));
		weights[2] = pif - weights[0] - weights[1];
		
		// iterate over the three points of the triangle
		for (int p = 0; p < 3; ++p)
		{
			// lookup the array for the position vertex
			std::vector<SmoothingGroupsNormal>& pairArray = 
				normalsPerPosition[positionIndices[triangleIndex * 3 + p]];
			
			// tells how often the smoothing group appears
			int hitCount = 0;

			// holding the cumulated smoothing groups and normal
			// init with current smoothing groups and normal
			SmoothingGroupsNormal tempPair(sg, normal * weights[p]);

			// the current smoothing groups/normal pair
			std::vector<SmoothingGroupsNormal>::iterator pairIt = pairArray.end();
			while (pairIt > pairArray.begin())
			{
				--pairIt;
				if ((pairIt->smoothingGroups & sg) != 0 || 
					(pairIt->smoothingGroups | sg) == 0) // sg == 0 should never happen
				{
					// cumulate
					tempPair.smoothingGroups |= pairIt->smoothingGroups;
					tempPair.normal += pairIt->normal;
					
					// erase from array
					pairIt = pairArray.erase(pairIt);
					++hitCount;
				}
			}

			// add cumulated to array again
			pairArray += tempPair;
		}
	}


	// step2
	// normalize normals and create new normal vertex property
	normals.clear();
	bool normalsAreBuggin = false;

	// for every position vertex the start index of the corresponding normals
	std::vector<int> startingIndices(numPositions);

	// iterate over the position vertices and remember the normalIndexAdd
	for (int x = 0; x < numPositions; ++x)
	{
		// store the starting normal vertex index for this position vertex
		startingIndices[x] = int(normals.size());

		// add the normals to the normals array
		for (size_t y = 0; y < normalsPerPosition[x].size(); ++y)
		{
			float3 normal = normalsPerPosition[x][y].normal;
			
			// normalize
			float len = length(normal);
			if (len < 1e-6f)
			{
				normalsAreBuggin = true;
				normal = vector3(0.0f, 0.0f, 1.0f);
				len = 1;
			}
			
			normals += normal / len;
		}
	}


	// step3
	// build the normal indices
	for (size_t triangleIndex = 0; triangleIndex < size_t(numTriangles); ++triangleIndex)
	{
		int sg = smoothingGroups[triangleIndex];

		for (int p = 0; p < 3; ++p)
		{
			// lookup the array
			std::vector<SmoothingGroupsNormal>& pairArray = 
				normalsPerPosition[positionIndices[triangleIndex * 3 + p]];

			// get the starting index for this normal arrray
			int normalIndex = startingIndices[positionIndices[triangleIndex * 3 + p]];

			// the current pair
			std::vector<SmoothingGroupsNormal>::iterator pairIt = pairArray.begin();
			for (; pairIt != pairArray.end(); ++pairIt)
			{
				if ((pairIt->smoothingGroups & sg) != 0 ||
					(pairIt->smoothingGroups | sg) == 0) // sg == 0 should never happen
				{
					// assign the normal index				
					normalIndices[triangleIndex * 3 + p] = normalIndex;
				}
				
				// increment the normal index
				++normalIndex;
			}
		}
	}

	return true;
}


/// tangent space generation

// we pack normal and uv index and the flip flag into an uint64.
// therefore the normals must not be shared by the positions.
typedef uint64_t Tuple;
typedef std::map<Tuple, int> TupleMap;
typedef std::pair<Tuple, int> TuplePair;

static inline Tuple makeTuple(int positionIndex, int normalIndex, int uvIndex, bool flip)
{
	return uint64_t(normalIndex) | (uint64_t(uvIndex) << 32) | (flip ? UINT64_C(0x8000000000000000) : UINT64_C(0)); 
}

void generateTangentSpace(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, const std::vector<float3>& positions,
	const std::vector<int>& normalIndices, const std::vector<float3>& normals,
	const std::vector<int>& uvIndices, const std::vector<float2>& uvs,
	std::vector<int>& tangentIndices, std::vector<int>& tangentNormalIndices, std::vector<float3x2>& tangents)
{
	size_t numPolygons = numVerticesPerPolygon.size();
	size_t numNormals = normals.size();
	
	// clear and reserve output arrays
	tangentIndices.clear();
	tangentIndices.reserve(numNormals);	
	tangentNormalIndices.clear();
	tangentNormalIndices.reserve(numNormals);
	tangents.clear();
	tangents.reserve(numNormals);
		

	TupleMap map;

	size_t vertexIndex = 0;
	size_t numTangents = 0;
	for (size_t polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		int numPolyVertices = numVerticesPerPolygon[polygonIndex];		

		// the first point (position and uv) is the reference.
		// then we calculate a regression for all other points so that
		// p' = T*u' + B*v'

		// see http://www.arndt-bruenner.de/mathe/scripts/regr.htm
		// F(x) = y = a*x1 + b*x2
		// we have two functions of this style for the relative uv coordinates
		// f1(x) = x1 = u'
		// f2(x) = x2 = v'
		// a = T.xyz (the regression is done three times for x, y and z components)
		// b = B.xyz
		// y = p'.xyz

		// [x1 * x1] [x1 * x2] | [x1 * y]
		// [x2 * x1] [x2 * x2] | [x2 * y]

		// left hand side matrix (diagonal with small values to prevent div by zero)
		float2x2 G = {};
		
		// right hand side vector (tree columns for the three vector components)
		float2x3 g = {};

		// determine area in uv space
		float uvArea = 0.0f;
		
		{
			// first vertex is reference point
			float3 p0 = positions[positionIndices[vertexIndex]];
			float2 uv0 = uvs[uvIndices[vertexIndex]];
			float2 last = {};

			// iterate over polygon vertices
			for (int i = 1; i < numPolyVertices; ++i)
			{
				float2 x = uvs[uvIndices[vertexIndex + i]] - uv0;
				float3 y = positions[positionIndices[vertexIndex + i]] - p0;
				
				G += outer(x, x);
				g += outer(x, y);
				
				// accumulate uv area (is scaled by 2)
				uvArea += last.x * x.y - x.x * last.y;
				last = x;
			}
		}
		
		
		float3x2 tangent = {};
		
		// check if uv's are degenerate
		// note: det(G) = uvArea^2
		if (abs(uvArea) > 1e-10f)
		{
			// uv's are ok
			tangent = transpose(inv(G) * g);
			
			float lt = length(tangent.x);
			float lb = length(tangent.y);
			
			// check if positions are degenerate
			if (lt > 1e-6f && lb > 1e-6f)
			{
				// positions are ok: normalize
				tangent.x /= lt;
				tangent.y /= lb;
			}
			else
			{
				tangent = float3x2();
			}
		}
		
		bool flip = uvArea < 0;
		
		for (int i = 0; i < numPolyVertices; ++i)
		{
			size_t index = vertexIndex + i;
			int positionIndex = positionIndices[index];
			int normalIndex = normalIndices[index];
			
			// calc weight for this corner (is angle)
			float3 p0 = positions[positionIndex];
			float3 e1 = positions[positionIndices[vertexIndex + (i + 1) % numPolyVertices]] - p0;
			float3 e2 = positions[positionIndices[vertexIndex + (i + numPolyVertices - 1) % numPolyVertices]] - p0;
			float weight = atan2(length(cross(e1, e2)), dot(e1, e2));
			

			Tuple tuple = makeTuple(positionIndex, normalIndex, uvIndices[index], flip);
			
			std::pair<TupleMap::iterator, bool> p = map.insert(TuplePair(tuple, numTangents));
			int tangentIndex = p.first->second;
			
			if (p.second)
			{
				// new tangent
				++numTangents;
				tangentNormalIndices += normalIndex;
				tangents += weight * tangent;
			}
			else
			{
				// add to existing tangent
				tangents[tangentIndex] += weight * tangent;
			}
			tangentIndices += tangentIndex;
		}

		vertexIndex += numPolyVertices;
	}
	
	// make tangents perpendicular to normals and normalize tangents
	for (size_t i = 0; i < numTangents; ++i)
	{
		float3 normal = normals[tangentNormalIndices[i]];
		float3x2& tangent = tangents[i];
		
		// project onto tangent plane using the normal
		float3 t = tangent.x - normal * dot(normal, tangent.x);
		float3 b = tangent.y - normal * dot(normal, tangent.y);

		float lt = length(t);
		float lb = length(b);

		// check if tangents are degenerate
		if (lt > 1e-3f && lb > 1e-3f)
		{
			tangent = matrix2(
				t / lt,
				b / lb);
		}
		else
		{
			tangent = generateTangentSpace(normal);
		}
	}
}


void generateTangentSpace(const std::vector<float3>& normals, std::vector<int>& tangentNormalIndices, std::vector<float3x2>& tangents)
{
	size_t numNormals = normals.size();
	
	tangentNormalIndices.resize(numNormals);
	tangents.resize(numNormals);
	
	for (size_t i = 0; i < numNormals; ++i)
	{
		// one tangent space per normal
		tangentNormalIndices[i] = i;

		// generate tangent space perpendicular to normal
		tangents[i] = generateTangentSpace(normals[i]);
	}	
}


// removes duplicate values. [1 1 2 3 4 4 5] -> [1 2 3 4 5]
static void removeDuplicates(std::vector<int>& a)
{
	std::vector<int>::iterator it1 = a.begin();
	if (it1 == a.end())
		return;
	
	std::vector<int>::iterator it2 = it1;
	++it1;
	while (it1 != a.end())
	{
		if (*it1 != *it2)
		{
			++it2;
			*it2 = *it1;
		}
			
		++it1;
	}
	
	a.erase(it2 + 1, a.end());
}

void buildTangentContributors(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, 
	int numTangents, const std::vector<int>& tangentIndices, std::vector<TangentContributors>& tangentContributors)
{
	size_t numPolygons = numVerticesPerPolygon.size();
	
	// determine which position vertices contribute to each tangent (one tangent per normal)
	tangentContributors.resize(numTangents);
	size_t vertexIndex = 0;
	for (size_t polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		// iterate over vertices of polygon
		int numPolyVertices = numVerticesPerPolygon[polygonIndex];
		for (int i = 0; i < numPolyVertices; ++i)
		{
			int positionIndex = positionIndices[vertexIndex + i];
			int tangentIndex = tangentIndices[vertexIndex + i];

			// store corresponding position index. each normal must belong to only one position
			tangentContributors[tangentIndex].positionIndex = positionIndex;

			// all other vertices of this polygon contribute to this tangent space
			for (int j = 0; j < numPolyVertices; ++j)
			{
				int contributingPositionIndex = positionIndices[vertexIndex + j];
				if (contributingPositionIndex != positionIndex)
					tangentContributors[tangentIndex].contributors += contributingPositionIndex;
			}
		}
		vertexIndex += numPolyVertices;
	}
	
	// remove duplicate position indices in normal contributors array
	optimizeTangentContributors(tangentContributors);
}

void optimizeTangentContributors(std::vector<TangentContributors>& tangentContributors)
{
	for (std::vector<TangentContributors>::iterator it = tangentContributors.begin(); it != tangentContributors.end(); ++it)
	{
		sort(it->contributors);
		removeDuplicates(it->contributors);
	}
}


void transferTangentSpace(const std::vector<TangentContributors>& tangentContributors,
	const std::vector<float3>& srcPositions, const std::vector<float3x2>& srcTangents,
	const std::vector<float3>& dstPositions, const std::vector<float3>& dstNormals, const std::vector<int>& tangentNormalIndices,
	std::vector<float3x2>& dstTangents)
{
	size_t numTangents = tangentContributors.size();
	dstTangents.resize(numTangents);
	
	for (size_t tangentIndex = 0; tangentIndex < numTangents; ++tangentIndex)
	{
		const TangentContributors& c = tangentContributors[tangentIndex];
		float3 xp = srcPositions[c.positionIndex];
		float3 yp = dstPositions[c.positionIndex];

		
		// see http://www.arndt-bruenner.de/mathe/scripts/regr.htm
		// F(x) = y = a*x1 + b*x2 + c*x3
		// we have three functions of this style for the three components
		// f1(x) = x1, f2(x) = x2, f3(x) = x3

		// [x1 * x1] [x1 * x2] [x1 * x3] | [x1 * y]
		// [x2 * x1] [x2 * x2] [x2 * x3] | [x2 * y]
		// [x3 * x1] [x3 * x2] [x3 * x3] | [x3 * y]

		// left hand side matrix (diagonal with small values to prevent div by zero)
		double3x3 G = double3x3Identity() * 1e-10;
		
		// right hand side vector (tree columns for the three vector components)
		double3x3 g = double3x3();

		// iterate over contributors to normal
		for (std::vector<int>::const_iterator it = c.contributors.begin(); it != c.contributors.end(); ++it)
		{
			// index of position vertex
			int index = *it;
			
			double3 x = convert_double3(srcPositions[index] - xp);
			double3 y = convert_double3(dstPositions[index] - yp);
			
			G += outer(x, x);
			g += outer(x, y);
		}
		
		float3x3 M = transpose(convert_float3x3(inv(G) * g));
	
/*
		float3x3 M = float3x3();

		// iterate over the three vector components
		for (int j = 0; j < 3; ++j)
		{
			// diagonal with small values to prevent div by zero
			double3x3 G = double3x3Identity() * 1e-10;
			double3 g = double3();
			
			// see http://www.arndt-bruenner.de/mathe/scripts/regr.htm
			// F(x) = yj = aj*x1 + bj*x2 + cj*x3
			// we have three functions of this style for the three components
			// f1(x) = x1, f2(x) = x2, f3(x) = x3

			// [x1 * x1] [x1 * x2] [x1 * x3] | [x1 * y]
			// [x2 * x1] [x2 * x2] [x2 * x3] | [x2 * y]
			// [x3 * x1] [x3 * x2] [x3 * x3] | [x3 * y]

			// iterate over contributors to normal
			for (std::vector<int>::const_iterator it = c.contributors.begin(); it != c.contributors.end(); ++it)
			{
				// index of position vertex
				int index = *it;
				
				float3 x = srcPositions[index] - xp;
				float3 y = dstPositions[index] - yp;
									
				float x1 = x.x;
				float x2 = x.y;
				float x3 = x.z;
				float yj = y[j];
				
				G[0][0] += x1 * x1;
				G[1][0] += x1 * x2;
				G[2][0] += x1 * x3;
				g.x += x1 * yj;							

				G[0][1] += x2 * x1;
				G[1][1] += x2 * x2;
				G[2][1] += x2 * x3;
				g.y += x2 * yj;							

				G[0][2] += x3 * x1;
				G[1][2] += x3 * x2;
				G[2][2] += x3 * x3;
				g.z += x3 * yj;							
			}
			
			float3 abc = float3(inv(G) * g);
		
			M[0][j] = abc.x;
			M[1][j] = abc.y;
			M[2][j] = abc.z;
		}
*/		
		// transform tangents from reference mesh to this mesh
		float3 tangent = M * srcTangents[tangentIndex].x;
		float3 bitangent = M * srcTangents[tangentIndex].y;
		
		// force right angle to normal, but do not normalize
		float3 normal = dstNormals[tangentNormalIndices[tangentIndex]];
		dstTangents[tangentIndex].x = tangent - normal * dot(normal, tangent);
		dstTangents[tangentIndex].y = bitangent - normal * dot(normal, bitangent);
	}
}


// triangulation

bool triangulate(const std::vector<int>& numVerticesPerPolygon,
	const std::vector<int>& positionIndices, const std::vector<float3>& positions,
	std::vector<int>& triangleVertexIndices, std::vector<int>& trianglePolygonIndices)
{
	// warning if polygon with less than 3 triangles
	bool warning = false;
	
	size_t numPolygons = numVerticesPerPolygon.size();

	// clear and reserve output arrays
	triangleVertexIndices.clear();
	triangleVertexIndices.reserve(numPolygons * 3);	
	trianglePolygonIndices.clear();
	trianglePolygonIndices.reserve(numPolygons);
	
	int vertexIndex = 0;
	for (size_t polygonIndex = 0; polygonIndex < numPolygons; ++polygonIndex)
	{
		// number of vertices of polygon
		int numPolyVertices = numVerticesPerPolygon[polygonIndex];
	
		// get vertex indices for polygon (assuming one vertex for each polygon corner)
		std::vector<int> polygonVertexIndices(numPolyVertices);
		std::vector<int> polygonPositionIndices(numPolyVertices);
		for (int i = 0; i < numPolyVertices; ++i)
		{
			polygonVertexIndices[i] = vertexIndex + i;
			polygonPositionIndices[i] = positionIndices[vertexIndex + i];
		}
		vertexIndex += numPolyVertices;
		

		/*
			iteratively cut away the triangle with biggest area in normal-direction from the polygon
			(cross-product dot normal gives highest value)
		*/
		while (numPolyVertices > 3)
		{
			// calc normal of polygon
			float3 normal = {};
			{
				// first vertex is reference point
				float3 p0 = positions[polygonPositionIndices[0]];
				float3 last = positions[polygonPositionIndices[1]] - p0;

				// iterate over polygon vertices		
				for (int i = 2; i < numPolyVertices; ++i)
				{
					float3 p = positions[polygonPositionIndices[i]] - p0;

					// accumulate normal
					normal += cross(last, p);
					last = p;
				}
			}	
/*
		//	
			float3 normal = float3();
			for (int i = 0; i < numPolyVertices; ++i)
			{
				float3 p = positions[polygonPositionIndices[i]];
				float3 a = positions[polygonPositionIndices[(i + 1) % numPolyVertices]] - p;
				float3 b = positions[polygonPositionIndices[(i + numPolyVertices - 1) % numPolyVertices]] - p;

				normal += cross(a, b);		
			}
*/
			// determine triangle with biggest area in normal-direction
			int maxI = 0;
			float maxD = -1e20f;
			for (int i = 0; i < numPolyVertices; ++i)
			{
				float3 p0 = positions[polygonPositionIndices[i]];
				float3 e1 = positions[polygonPositionIndices[(i + 1) % numPolyVertices]] - p0;
				float3 e2 = positions[polygonPositionIndices[(i + numPolyVertices - 1) % numPolyVertices]] - p0;

				float d = dot(cross(e1, e2), normal);
				if (d > maxD)
				{
					maxD = d;
					maxI = i;
				} 
			}
			
			// add triangle
			triangleVertexIndices += polygonVertexIndices[maxI];
			triangleVertexIndices += polygonVertexIndices[(maxI + 1) % numPolyVertices];
			triangleVertexIndices += polygonVertexIndices[(maxI + numPolyVertices - 1) % numPolyVertices];
			
			// add polygon index that this triangle belongs to
			trianglePolygonIndices += polygonIndex;
		
			// cut away vertex with index-index maxI
			polygonPositionIndices.erase(polygonPositionIndices.begin() + maxI);
			polygonVertexIndices.erase(polygonVertexIndices.begin() + maxI);
			--numPolyVertices;
		}
		
		// add last triangle
		if (numPolyVertices == 3)
		{
			add(triangleVertexIndices, polygonVertexIndices);
			trianglePolygonIndices += polygonIndex;
		}
		else
		{
			warning = true;
		}
	}
	
	return warning;
}
/*
namespace {

	// line in plücker coordinates
	// http://www.euclideanspace.com/maths/geometry/elements/line/plucker/index.htm
	struct Line {
		float p0;
		float p1;
		float p2;
		float p3;
		float p4;
		float p5;
	};

	// convert line given as two ponts into plücker coordinates
	Line pointsToLine(float3 p, float3 q) {
		Line line;
		line.p0 = p.x * q.y - q.x * p.y;
		line.p1 = p.x * q.z - q.x * p.z;
		line.p2 = p.x - q.x;
		line.p3 = p.y * q.z - q.y * p.z;
		line.p4 = p.z - q.z;
		line.p5 = q.y - p.y;
		return line;
	}

	Line rayToLine(float3 p, float3 u) {
		Line line;
		line.p0 = p.x * u.y - u.x * p.y;
		line.p1 = p.x * u.z - u.x * p.z;
		line.p2 = -u.x;
		line.p3 = p.y * u.z - u.y * p.z;
		line.p4 = -u.z;
		line.p5 = u.y;
		return line;
	}

	// side operator
	float side(const Line& a, const Line& b) {
		return a.p0 * b.p4 + a.p1 * b.p5 + a.p2 * b.p3 + a.p3 * b.p2 + a.p4 * b.p0 + a.p5 * b.p1;
	}
	
	// returns true if line intersects triangle
	// http://www.loria.fr/~lazard/ARC-Visi3D/Pant-project/code/line_triangle_intersection.C
	// http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
	bool intersectsTriangle(const Line& edge1, const Line& edge2, const Line& edge3, const Line& line) {
		float s1 = side(line, edge1);
		float s2 = side(line, edge2);
		float s3 = side(line, edge3);
		
		return (s1 > 0 && s2 > 0 && s3 > 0) || (s1 < 0 && s2 < 0 && s3 < 0);
	}
	
	bool snip(const std::vector<float3>& positions, int *indices, int u, int v, int w, int nv, float3 normal) {
		float3 a = positions[indices[u]];
		float3 b = positions[indices[v]];
		float3 c = positions[indices[w]];
		
		// check if triangle normal points in same direction as polygon normal
		if (dot(cross(b - a, c - a), normal) < 0.0f)
			return false;
		
		// check if a polygon point lies inside the triangle
		Line edge1 = pointsToLine(a, b);
		Line edge2 = pointsToLine(b, c);
		Line edge3 = pointsToLine(c, a);
		for (int i = 0; i < nv; i++) {
			if (i == u || i == v || i == w)
				continue;
			float3 p = positions[indices[i]];
			if (intersectsTriangle(edge1, edge2, edge3, rayToLine(p, normal)))
				return false;
		}

		return true;
	}
}

// http://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
bool triangulate(const std::vector<float3>& positions, std::vector<int>& triangles) {
	float3 normal = float3();

	int n = int(positions.size());
	if (n < 3)
		return false;

	// calculate normal of polygon
	float3 p0 = positions[0];
	float3 p1 = positions[2];
	float3 d1 = p1 - p0;
	for (int i = 2; i < positions.size(); ++i) {
		float3 p2 = positions[i];
		float3 d2 = p2 - p0;
		
		normal += cross(d1, d2);
		
	}
	normal = normalize(normal);


	int* indices = new int[n];
	for (size_t v = 0; v < n; ++v)
		indices[v] = v;
	
	int nv = n;

	// error detection counter
	int count = 2 * nv;

	// remove nv-2 Vertices, creating 1 triangle every time
	for (int m = 0, v = nv-1; nv > 2;) {
		// if we loop around, it is probably a non-simple polygon
		if (count-- <= 0)
		{
			// Triangulate: ERROR - probable bad polygon!
			delete [] indices;
			return false;
		}

		// three consecutive vertices in current polygon, <u,v,w>
		int u = v; if (u >= nv) u = 0;    // previous
		v = u+1; if (v >= nv) v = 0;      // new v
		int w = v+1; if (w >= nv) w = 0;  // next

		if (snip(positions, indices, u, v, w, nv, normal)) {
			// output Triangle
			triangles.push_back(indices[u]);
			triangles.push_back(indices[v]);
			triangles.push_back(indices[w]);

			m++;

			// remove v from remaining polygon
			for (int i = v + 1; i < nv; ++i)
				indices[i - 1] = indices[i];
			--nv;

			// resest error detection counter
			count = 2 * nv;
		}
	}
	
	delete [] indices;
	return true;
}
*/

// vertex cache optimization

float calcAcmr(const std::vector<int>& triangleVertexIndices, int cacheSize)
{
	int numTriangles = int(triangleVertexIndices.size() / 3);

	int cache[32];
	int cacheIndex = 0;
	fill(cache, -1);

	int numHits = 0;
	cacheSize = clamp(cacheSize, 1, 32);
	
	int numVertexIndices = numTriangles * 3;
	for (int i = 0; i < numVertexIndices; ++i)
	{
		int vertexIndex = triangleVertexIndices[i];
		
		// check if we have a cache hit
		int j;
		for (j = 0; j < cacheSize; ++j)
		{
			if (cache[j] == vertexIndex)
			{
				++numHits;
				break;
			}
		}
		
		// add to cache if no hit
		if (j == cacheSize)
		{
			cache[cacheIndex] = vertexIndex;
			cacheIndex = (cacheIndex + 1) % cacheSize;
		}
	}
	
	int numMisses = numVertexIndices - numHits;
	return float(numMisses) / float(numTriangles);
}

struct OptVertex
{
	int cachePosition;
	float score;
	int numTriangles;
	int triangleIndicesStart;
};

struct OptTriangle
{
	bool used;
	int vertexIndex0;
	int vertexIndex1;
	int vertexIndex2;
	int sequenceNumber;
};

#define CACHE_SIZE 32

static float calcVertexScore(const OptVertex& vertex)
{
		const int maxSizeVertexCache = CACHE_SIZE;
		const float cacheDecayPower = 1.5f;
		const float lastTriScore = 0.75f;
		const float valenceBoostScale = 2.0f;
		const float valenceBoostPower = 0.5f;

		float score = 0.0f;
		int cachePosition = vertex.cachePosition;
		if (cachePosition < 0)
		{
				// Vertex is not in FIFO cache - no score.
		}
		else
		{
				if (cachePosition < 3)
				{
						// This vertex was used in the last triangle,
						// so it has a fixed score, whichever of the three
						// it's in. Otherwise, you can get very different
						// answers depending on whether you add
						// the triangle 1,2,3 or 3,1,2 - which is silly.
						score = lastTriScore;
				}
				else
				{
						//assert(cachePosition < maxSizeVertexCache);

						// Points for being high in the cache.
						const float scaler = 1.0f / (maxSizeVertexCache - 3);

						score = 1.0f - (cachePosition - 3) * scaler;
						score = powf(score, cacheDecayPower);
				}
		}

		// Bonus points for having a low number of tris still to
		// use the vert, so we get rid of lone verts quickly.
		float valenceBoost = powf((float)vertex.numTriangles, -valenceBoostPower);
		score += valenceBoostScale * valenceBoost;

		return score;
}

struct Optimizer
{
	std::vector<OptTriangle> triangles;
	std::vector<OptVertex> vertices;
	std::vector<int> triangleIndices;
	int cache[CACHE_SIZE];
	
	Optimizer(int numTriangles, int numVertices)
		: triangles(numTriangles + 1), vertices(numVertices + 1), triangleIndices(numTriangles * 3)
	{
		fill(cache, numVertices);
	}

	void useVertex(int vertexIndex, int triangleIndex)
	{
		// vertex gets used by a triangle
		OptVertex& vertex = this->vertices[vertexIndex];
		
		// decrement number of still-to-be-added triangles
		--vertex.numTriangles;
		
		// remove triangle from list of still-to-be-added triangles
		int triangleIndicesEnd = vertex.triangleIndicesStart + vertex.numTriangles;
		for (int i = vertex.triangleIndicesStart; i < triangleIndicesEnd; ++i)
		{
			// remove triangle from list of still-to-be-added triangles
			if (this->triangleIndices[i] == triangleIndex)
			{
				this->triangleIndices[i] = this->triangleIndices[triangleIndicesEnd];
				break;
			}	
		}

		// add vertex to head of cache
		{
			int tempVertexIndex = vertexIndex;
			int i = 0;
			do
			{
				int temp = this->cache[i];
				this->cache[i] = tempVertexIndex;
				tempVertexIndex = temp;

				// check if vertex is already in cche
				if (tempVertexIndex == vertexIndex)
					break;

				++i;
			} while (i < CACHE_SIZE);
			
			// check if a vertex falls out of the cache
			if (i == CACHE_SIZE)
				this->vertices[tempVertexIndex].cachePosition = -1;
		}			
	}
};

void optimizeVertexCache(int numVertices, const std::vector<int>& triangleVertexIndices,
	std::vector<int>& newTriangleVertexIndices)
{
	int numTriangles = int(triangleVertexIndices.size() / 3);

	std::vector<int> mapNew2OldTriangleIndices(numTriangles);
	
	Optimizer optimizer(numTriangles, numVertices);
	int numProcessedTriangles = 0;
	
	// initialize triangle data and determine number of triangles per vertex
	for (int i = 0; i < numTriangles; ++i)
	{
		OptTriangle& triangle = optimizer.triangles[i];
		triangle.used = false;
		triangle.vertexIndex0 = triangleVertexIndices[i * 3 + 0];
		triangle.vertexIndex1 = triangleVertexIndices[i * 3 + 1];
		triangle.vertexIndex2 = triangleVertexIndices[i * 3 + 2];
		triangle.sequenceNumber = 0;
		
		++optimizer.vertices[triangle.vertexIndex0].numTriangles;
		++optimizer.vertices[triangle.vertexIndex1].numTriangles;
		++optimizer.vertices[triangle.vertexIndex2].numTriangles;
	}		

	// initialize vertex data
	int triangleIndicesStart = 0;
	for (int i = 0; i < numVertices; ++i)
	{
		OptVertex& vertex = optimizer.vertices[i];
		vertex.cachePosition = -1;

		// calculate initial vertex score
		vertex.score = calcVertexScore(vertex);
		
		// allocate triangle indices			
		vertex.triangleIndicesStart = triangleIndicesStart;
		triangleIndicesStart += vertex.numTriangles;
		
		// reset number of triangles again for next step
		vertex.numTriangles = 0;
	}

	// iterate over triangles and fill triangle indices of vertices
	for (int i = 0; i < numTriangles; ++i)
	{
		OptTriangle& triangle = optimizer.triangles[i];
		
		{
			OptVertex& vertex = optimizer.vertices[triangle.vertexIndex0];
			optimizer.triangleIndices[vertex.triangleIndicesStart + vertex.numTriangles] = i;
			++vertex.numTriangles;
		}

		{
			OptVertex& vertex = optimizer.vertices[triangle.vertexIndex1];
			optimizer.triangleIndices[vertex.triangleIndicesStart + vertex.numTriangles] = i;
			++vertex.numTriangles;
		}

		{
			OptVertex& vertex = optimizer.vertices[triangle.vertexIndex2];
			optimizer.triangleIndices[vertex.triangleIndicesStart + vertex.numTriangles] = i;
			++vertex.numTriangles;
		}
	}
	
	while (true)
	{
		// find unused triangle with best score
		float bestScore = 0.0f;
		int bestIndex = -1;
		for (int i = 0; i < numTriangles; ++i)
		{
			OptTriangle& triangle = optimizer.triangles[i];
			if (!triangle.used)
			{
				float score = optimizer.vertices[triangle.vertexIndex0].score
					+ optimizer.vertices[triangle.vertexIndex1].score
					+ optimizer.vertices[triangle.vertexIndex2].score;

				if (score > bestScore)
				{
					bestScore = score;
					bestIndex = i;
				}
			}
		}
		
		// check if a triangle was found
		if (bestIndex == -1)
			break;
			
		int sequenceNumber = 0;
		while (bestIndex != -1)
		{
			mapNew2OldTriangleIndices[numProcessedTriangles++] = bestIndex;

			OptTriangle& triangle = optimizer.triangles[bestIndex];
			triangle.used = true;

			// use the three vertices
			optimizer.useVertex(triangle.vertexIndex0, bestIndex);
			optimizer.useVertex(triangle.vertexIndex1, bestIndex);
			optimizer.useVertex(triangle.vertexIndex2, bestIndex);

			// update cache position and score for all vertices in cache
			for (int i = 0; i < CACHE_SIZE; ++i)
			{
				OptVertex& vertex = optimizer.vertices[optimizer.cache[i]];
				
				vertex.cachePosition = i;
				vertex.score = calcVertexScore(vertex);
			}
			
			// find best triangle that is referenced by the vertices in the cache
			++sequenceNumber;
			bestScore = 0.0f;
			bestIndex = -1;
			for (int i = 0; i < CACHE_SIZE; ++i)
			{
				OptVertex& vertex = optimizer.vertices[optimizer.cache[i]];

				for (int j = vertex.triangleIndicesStart; j < vertex.triangleIndicesStart + vertex.numTriangles; ++j)
				{
					int triangleIndex = optimizer.triangleIndices[j];
					OptTriangle& triangle = optimizer.triangles[triangleIndex];

					if (triangle.sequenceNumber != sequenceNumber)
					{
						triangle.sequenceNumber = sequenceNumber;
						
						float score = optimizer.vertices[triangle.vertexIndex0].score
							+ optimizer.vertices[triangle.vertexIndex1].score
							+ optimizer.vertices[triangle.vertexIndex2].score;

						if (score > bestScore)
						{
							bestScore = score;
							bestIndex = triangleIndex;
						}
					}
				}				
			}
		}
	}

/*
	// this is for optimizing also the vertex order
	for (int i = 0; i < numVertices; ++i)
	{
		mapOld2NewVertexIndices[i] = -1;
	}
	int numProcessedVertices = 0;
*/
	for (int i = 0; i < numTriangles; ++i)
	{
		OptTriangle& triangle = optimizer.triangles[mapNew2OldTriangleIndices[i]];
/*
		// this is for optimizing also the vertex order
		if (mapOld2NewVertexIndices[triangle.vertexIndex0] == -1)		
			mapOld2NewVertexIndices[triangle.vertexIndex0] = numProcessedVertices++;
		if (mapOld2NewVertexIndices[triangle.vertexIndex1] == -1)		
			mapOld2NewVertexIndices[triangle.vertexIndex1] = numProcessedVertices++;
		if (mapOld2NewVertexIndices[triangle.vertexIndex2] == -1)		
			mapOld2NewVertexIndices[triangle.vertexIndex2] = numProcessedVertices++;
		
		newTriangleVertexIndices[i * 3 + 0] = mapOld2NewVertexIndices[triangle.vertexIndex0];
		newTriangleVertexIndices[i * 3 + 1] = mapOld2NewVertexIndices[triangle.vertexIndex1];
		newTriangleVertexIndices[i * 3 + 2] = mapOld2NewVertexIndices[triangle.vertexIndex2];
*/
		newTriangleVertexIndices[i * 3 + 0] = triangle.vertexIndex0;
		newTriangleVertexIndices[i * 3 + 1] = triangle.vertexIndex1;
		newTriangleVertexIndices[i * 3 + 2] = triangle.vertexIndex2;
	}
/*	
	// handle unused vertices
	for (int i = 0; i < numVertices; ++i)
	{
		if (mapOld2NewVertexIndices[i] == -1)
		{
			mapOld2NewVertexIndices[i] = numProcessedVertices;
			numProcessedVertices++;
		}
	}
*/
}


// bone batching

typedef uint BoneIndex;

struct ConfigurationLess
{
	bool operator ()(const std::vector<BoneIndex>& a, const std::vector<BoneIndex>& b)
	{
		size_t aSize = a.size();
		size_t bSize = b.size();
		if (aSize != bSize)
			return aSize < bSize;
		for (int i = int(aSize - 1); i >= 0; --i)
		{
			BoneIndex ai = a[i];
			BoneIndex bi = b[i];

			if (ai != bi)
				return ai < bi;
		}
		return false;
	}
};

/*
void calcBoneBatching(int numTriangles, int numVertices, const std::vector<int>& vertexIndices, const std::vector<BoneSet>& boneSets,
	const std::vector<int>& maxNumBones, std::vector<BoneBatch>& batches)
{
	/ *
		step 1: build "configurations" for each triangle. a configuration is a list of used bones (set/index).
			build a map from each unique configuration to its list of triangles
	* /	
	int numBoneSets = int(boneSets.size());

	// step 1
	// bone configuration -> list of triangles
	std::map<std::vector<BoneIndex>, std::vector<int> > configurations;
	std::vector<BoneIndex> configuration;
	for (int triangleIndex = 0; triangleIndex < numTriangles; ++triangleIndex)
	{
		for (int setIndex = 0; setIndex < numBoneSets; ++setIndex)
		{
			const BoneSet& boneSet = boneSets[setIndex];
			int numBonesPerVertex = boneSet.numBonesPerVertex;
			const std::vector<int>& boneIndices = boneSet.indices;
			
			// iterate over triangle corners
			for (int j = 0; j < 3; ++j)
			{
				int vertexIndex = vertexIndices[j + triangleIndex * 3];
				
				// iterate over bone indices of vertex
				for (int i = 0; i < numBonesPerVertex; ++i)
				{
					int boneIndex = boneIndices[i + vertexIndex * numBonesPerVertex];

					configuration += (setIndex << 20) | boneIndex;
				}
			}
		}

		// sort configuration and add triangle index to map
		sort(configuration);
		configurations[configuration].insert(triangleIndex);
		configuration.clear();
	}


}
*/


} // namespace MeshUtility

} // namespace digi
