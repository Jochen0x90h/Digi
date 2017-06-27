#include "ImageUtil.h"


namespace digi {


// scale data for 1D

static inline float4 sample(const float4* data, float coord)
{
	float _x = floor(coord);
	int x = int(_x);
	float fx = coord - _x;
	
	// current and next data point
	const float4* d1 = data + x;
	const float4* d2 = d1 + 1;
	
	// return interpolated data
	return (1.0f - fx) * *d1 + fx * *d2;
}

static void scaleData1D(float4* dstData, int dstWidth, const float4* srcData, int srcWidth)
{
	// calc scale factor from dest to source
	float s = float(srcWidth) / float(dstWidth);

	// maximum coordinate for source image that interpolated sampling will not go out of bounds
	float maxCoord = float(srcWidth) - 1.01f;
	
	// offset for additional sampling points
	float o = s * 0.5f;
	
	float4* d = dstData;
	for (int i = 0; i < dstWidth; ++i)
	{
		float coord = float(i) * s;
		float coord1 = min(coord, maxCoord);
		float coord2 = min(coord + o, maxCoord);
		
		// sample src data two times for lowpass filtering 
		*d = (sample(srcData, coord1)
			+ sample(srcData, coord2)) * 0.5f;
		
		++d;	
	}
}

// scale data for 2D

static inline float4 sample(const float4* data, int width, float2 coord)
{
	float _x = floor(coord.x);
	float _y = floor(coord.y);
	int x = int(_x);
	int y = int(_y);
	float fx = coord.x - _x;
	float fy = coord.y - _y;
	
	// current and next data point in both dimensions
	const float4* d11 = data + x + width * y;
	const float4* d12 = d11 + 1;
	const float4* d21 = d11 + width;
	const float4* d22 = d21 + 1;
	
	// return interpolated data
	float4 d1 = (1.0f - fx) * *d11 + fx * *d12;
	float4 d2 = (1.0f - fx) * *d21 + fx * *d22;
	return (1.0f - fy) * d1 + fy * d2;
}

static void scaleData2D(float4* dstData, int dstWidth, int dstHeight, const float4* srcData, int srcWidth, int srcHeight)
{
	// calc scale factor from dest to source
	float2 s = vector2(float(srcWidth), float(srcHeight)) / vector2(float(dstWidth), float(dstHeight));
	
	// maximum coordinate for source image that interpolated sampling will not go out of bounds
	float2 maxCoord = vector2(float(srcWidth), float(srcHeight)) - 1.01f;
	
	// offset for additional sampling points
	float2 o = s * 0.5f;
	
	float4* d = dstData;
	for (int j = 0; j < dstHeight; ++j)
	{
		for (int i = 0; i < dstWidth; ++i)
		{
			float2 coord = vector2(float(i), float(j)) * s;
			float2 coord1 = min(coord, maxCoord);
			float2 coord2 = min(coord + o, maxCoord);
			
			// sample src data four times for lowpass filtering 
			*d = (sample(srcData, srcWidth, coord1)
				+ sample(srcData, srcWidth, vector2(coord2.x, coord1.y))
				+ sample(srcData, srcWidth, vector2(coord1.x, coord2.y))
				+ sample(srcData, srcWidth, coord2)) * 0.25f;
			
			++d;	
		}
	}
}

// scale data for 3D

static inline float4 sample(const float4* data, int width, int height, float3 coord)
{
	float _x = floor(coord.x);
	float _y = floor(coord.y);
	float _z = floor(coord.z);
	int x = int(_x);
	int y = int(_y);
	int z = int(_z);
	float fx = coord.x - _x;
	float fy = coord.y - _y;
	float fz = coord.z - _z;
	
	// current and next data point in three dimensions
	const float4* d111 = data + x + width * y + width * height * z;
	const float4* d112 = d111 + 1;
	const float4* d121 = d111 + width;
	const float4* d122 = d121 + 1;
	const float4* d211 = d111 + width * height;
	const float4* d212 = d211 + 1;
	const float4* d221 = d211 + width;
	const float4* d222 = d221 + 1;
	
	// return interpolated data
	float4 d11 = (1.0f - fx) * *d111 + fx * *d112;
	float4 d12 = (1.0f - fx) * *d121 + fx * *d122;
	float4 d21 = (1.0f - fx) * *d211 + fx * *d212;
	float4 d22 = (1.0f - fx) * *d221 + fx * *d222;

	float4 d1 = (1.0f - fy) * d11 + fy * d12;
	float4 d2 = (1.0f - fy) * d21 + fy * d22;

	return (1.0f - fz) * d1 + fz * d2;
}

static void scaleData3D(float4* dstData, int dstWidth, int dstHeight, int dstDepth,
	const float4* srcData, int srcWidth, int srcHeight, int srcDepth)
{
	// calc scale factor from dest to source
	float3 s = vector3(float(srcWidth), float(srcHeight), float(srcDepth))
		/ vector3(float(dstWidth), float(dstHeight), float(dstDepth));

	// maximum coordinate for source image that interpolated sampling will not go out of bounds
	float3 maxCoord = vector3(float(srcWidth), float(srcHeight), float(srcDepth)) - 1.01f;

	// offset for additional sampling points
	float3 o = s * 0.5f;

	float4* d = dstData;
	for (int k = 0; k < dstDepth; ++k)
	{
		for (int j = 0; j < dstHeight; ++j)
		{
			for (int i = 0; i < dstWidth; ++i)
			{
				float3 coord = vector3(float(i), float(j), float(k)) * s;
				float3 coord1 = min(coord, maxCoord);
				float3 coord2 = min(coord + o, maxCoord);
				
				// sample src data 8 times for lowpass filtering 
				*d = (sample(srcData, srcWidth, srcHeight, coord1)
					+ sample(srcData, srcWidth, srcHeight, vector3(coord2.x, coord1.y, coord1.z))
					+ sample(srcData, srcWidth, srcHeight, vector3(coord1.x, coord2.y, coord1.z))
					+ sample(srcData, srcWidth, srcHeight, vector3(coord2.x, coord2.y, coord1.z))
					+ sample(srcData, srcWidth, srcHeight, vector3(coord1.x, coord1.y, coord2.z))
					+ sample(srcData, srcWidth, srcHeight, vector3(coord2.x, coord1.y, coord2.z))
					+ sample(srcData, srcWidth, srcHeight, vector3(coord1.x, coord2.y, coord2.z))
					+ sample(srcData, srcWidth, srcHeight, coord2)) * 0.125f;
				
				++d;	
			}
		}
	}
}

Pointer<Image> scaleFiltered(Pointer<Image> srcImage, int3 size)
{
	// check if float4
	if (srcImage->getFormat().layout != ImageFormat::XYZW32 || srcImage->getFormat().type != ImageFormat::FLOAT)
		return null;

	int numImages = srcImage->getNumImages();
	
	const float4* srcData = srcImage->getData<float4>();
	size_t srcNumPixels = srcImage->getImageNumPixels();
	int srcWidth = srcImage->getWidth();
	int srcHeight = srcImage->getHeight();
	int srcDepth = srcImage->getDepth();

	Pointer<Image> dstImage = new Image(srcImage->getType(), srcImage->getFormat(), size, 1, numImages);
	float4* dstData = dstImage->getData<float4>();
	size_t dstNumPixels = dstImage->getImageNumPixels();
	int dstWidth = dstImage->getWidth();
	int dstHeight = dstImage->getHeight();
	int dstDepth = dstImage->getDepth();

	// get dimensions that are greater than 1
	int srcDim1 = srcWidth;
	int srcDim2 = srcHeight;
	int srcDim3 = srcDepth;
	int dstDim1 = dstWidth;
	int dstDim2 = dstHeight;
	int dstDim3 = dstDepth;			
	if (srcDim1 == 1)
	{
		srcDim1 = srcDim2;
		srcDim2 = srcDim3;
		srcDim3 = 1;
		dstDim1 = dstDim2;
		dstDim2 = dstDim3;
		dstDim3 = 1;
	}
	if (srcDim2 == 1)
	{
		srcDim2 = srcDim3;
		srcDim3 = 1;
		dstDim2 = dstDim3;
		dstDim3 = 1;
	}
		
	// iterate over pages
	for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
	{
		if (srcDim3 > 1)
		{
			// 3 dimensions
			scaleData3D(dstData, dstDim1, dstDim2, dstDim3, (float4*)srcData, srcDim1, srcDim2, srcDim3);
		}
		else if (srcDim2 > 1)
		{
			// 2 dimensions
			scaleData2D(dstData, dstDim1, dstDim2, (float4*)srcData, srcDim1, srcDim2);
		}
		else if (srcDim1 > 1)
		{
			// 1 dimension
			scaleData1D(dstData, dstDim1, (float4*)srcData, srcDim1);
		}
		else
		{
			// 0 dimensions (1 pixel, copy value)
			*dstData = *srcData;
		}

		srcData += srcNumPixels;
		dstData += dstNumPixels;
	}
	
	return dstImage;
}


} // namespace digi
