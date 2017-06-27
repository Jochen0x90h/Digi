#include <vector>

#include <digi/Utility/VectorUtility.h>
#include <digi/System/File.h>
#include <digi/System/IOCatcher.h>
#include <digi/Data/DataException.h>

#include <jpeglib.h>

#include "JPEGWrapper.h"

#ifdef _WIN32
	#define fopen(path, mode) _wfopen(path, L##mode)
#endif


namespace digi {

namespace
{
	// source state, buffer and guard
	// see jdatasrc.c in libjpeg
	struct Source : public jpeg_source_mgr
	{
		Source(jpeg_decompress_struct& cinfo, Pointer<IODevice> dev);
		~Source()
		{
			jpeg_destroy_decompress(&this->cinfo);
		}

		void checkState()
		{
			// check if an io error was encountered
			this->ioCatcher.checkState();

			// check if end of file was encountered
			if (this->eof)
				throw DataException(this->ioCatcher.dev, DataException::UNEXPECTED_END_OF_DATA);
		}

		jpeg_decompress_struct& cinfo;
		IOCatcher ioCatcher;
		bool eof;
		JOCTET buffer[4096];
	};

	void initSource(j_decompress_ptr cinfo)
	{
	}

	boolean fillInputBuffer(j_decompress_ptr cinfo)
	{
		Source* src = (Source*)cinfo->src;

		size_t numRead = src->ioCatcher.read(src->buffer, sizeof(src->buffer));

		if (numRead == -1 || numRead == 0)
		{
			// error or end of file: insert EOI marker
			src->eof = numRead == 0;
		    src->buffer[0] = (JOCTET)0xFF;
			src->buffer[1] = (JOCTET)JPEG_EOI;
			numRead = 2;
		}
		
		src->next_input_byte = src->buffer;
		src->bytes_in_buffer = numRead;
		return TRUE;
	}

	void skipInputData(j_decompress_ptr cinfo, long num_bytes)
	{
		jpeg_source_mgr* src = cinfo->src;

		while (num_bytes > (long)src->bytes_in_buffer)
		{
			num_bytes -= (long)src->bytes_in_buffer;
			src->fill_input_buffer(cinfo);
		}
		src->next_input_byte += (size_t)num_bytes;
		src->bytes_in_buffer -= (size_t)num_bytes;
	}

	void termSource(j_decompress_ptr cinfo)
	{
		// no work necessary here
	}

	Source::Source(jpeg_decompress_struct& cinfo, Pointer<IODevice> dev)
		: cinfo(cinfo), ioCatcher(dev), eof(false)
	{
		// init
		jpeg_create_decompress(&cinfo);

		// set io functions
		this->init_source = initSource;
		this->fill_input_buffer = fillInputBuffer;
		this->skip_input_data = skipInputData;
		this->resync_to_restart = jpeg_resync_to_restart; // use default method
		this->term_source = termSource;
			
		// start with empty buffer
		this->bytes_in_buffer = 0;
		this->next_input_byte = NULL;

		// set io functions
		cinfo.src = this;
	}


	// destination state, buffer and guard
	// see jdatadst.c in libjpeg
	struct Destination : public jpeg_destination_mgr
	{
		Destination(jpeg_compress_struct& cinfo, Pointer<IODevice> dev);
		~Destination()
		{
			jpeg_destroy_compress(&this->cinfo);
		}

		void checkState()
		{
			// check if an io error was encountered
			this->ioCatcher.checkState();
		}

		jpeg_compress_struct& cinfo;
		IOCatcher ioCatcher;
		JOCTET buffer[4096];
	};

	void initDestination(j_compress_ptr cinfo)
	{
	}

	boolean emptyOutputBuffer(j_compress_ptr cinfo)
	{
		Destination* dst = (Destination*)cinfo->dest;

		dst->ioCatcher.write(dst->buffer, sizeof(dst->buffer));

		dst->free_in_buffer = sizeof(dst->buffer);
		dst->next_output_byte = dst->buffer;

		return TRUE;
	}

	void termDestination(j_compress_ptr cinfo)
	{
		Destination* dst = (Destination*)cinfo->dest;

		dst->ioCatcher.write(dst->buffer, sizeof(dst->buffer) - dst->free_in_buffer);
	}

	Destination::Destination(jpeg_compress_struct& cinfo, Pointer<IODevice> dev)
		: cinfo(cinfo), ioCatcher(dev)
	{
		// init
		jpeg_create_compress(&cinfo);

		// set io functions
		this->init_destination = initDestination;
		this->empty_output_buffer = emptyOutputBuffer;
		this->term_destination = termDestination;
			
		// start with empty buffer
		this->free_in_buffer = sizeof(this->buffer);
		this->next_output_byte = this->buffer;

		// set io functions
		cinfo.dest = this;
	}

} // anonymous namespace

Pointer<Image> loadJPEG(const fs::path& path)
{
	Pointer<File> f = File::open(path, File::READ);		
	Pointer<Image> image = loadJPEG(f);
	f->close();
	return image;
}

Pointer<Image> loadJPEG(Pointer<IODevice> dev)
{
	// jpeg variables
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	// init, set io functions and automatic cleanup on exception
	cinfo.err = jpeg_std_error(&jerr);
	Source source(cinfo, dev);

	// read header
	jpeg_read_header(&cinfo, TRUE);

	// check if a read error occured
	source.checkState();

	// determine image format
	ImageFormat format;
	if (cinfo.out_color_space == JCS_GRAYSCALE)
	{
		format.mapping = ImageFormat::Y;
		format.layout = ImageFormat::X8;
	
	}
	else if (cinfo.out_color_space == JCS_RGB)
	{
		format.mapping = ImageFormat::RGB;
		format.layout = ImageFormat::XYZ8;		
	}
	else
	{
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);
	}
	format.type = ImageFormat::UNORM;
	int numChannels = format.getNumChannels();

	// read data
	jpeg_start_decompress(&cinfo);
	int width = cinfo.output_width;
	int height = cinfo.output_height;
	Pointer<Image> image = new Image(Image::IMAGE, format, width, height);
	JSAMPROW data = image->getData<JSAMPLE>();
	int stride = width * numChannels;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &data, 1);
		data += stride;

		// check if a read error occured
		source.checkState();
	}
	jpeg_finish_decompress(&cinfo);

	// the source closes the jpeg image, also when an exception is thrown		
	return image;
}

void saveJPEG(const fs::path& path, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	Pointer<File> f = File::create(path);
	saveJPEG(f, image, quality, mipmapIndex, imageIndex);
	f->close();
}

void saveJPEG(Pointer<IODevice> dev, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	if (mipmapIndex < 0)
		mipmapIndex = 0;

	// determine size
	int width = max(image->getWidth() >> mipmapIndex, 1);
	int height = max(image->getHeight() >> mipmapIndex, 1);

	// determine jpeg format
	ImageFormat format = image->getFormat();
	int componentSize = format.getComponentSize();
	int channelCount = format.getNumChannels();
	J_COLOR_SPACE colorType = JCS_UNKNOWN;
	if ((format.type == ImageFormat::UNORM || format.type == ImageFormat::UINT)
		&& format.getLayoutType() == BufferFormat::STANDARD)
	{
		if (channelCount == 1) // write one component as gray independent of mapping
		{
			// gray
			colorType = JCS_GRAYSCALE;
		}
		else if (channelCount == 3 && (format.mapping == ImageFormat::RGB))// || format.mapping == ImageFormat::BGR))
		{
			// color
			colorType = JCS_RGB;
		}
	}
	
	// check if valid
	if (componentSize > 1 || colorType == JCS_UNKNOWN)
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);

	// jpeg variables
	jpeg_compress_struct cinfo;
	jpeg_error_mgr jerr;
	
	// init, set io functions and automatic cleanup on exception
	cinfo.err = jpeg_std_error(&jerr);
	Destination destination(cinfo, dev);

	// write header
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = channelCount;
	cinfo.in_color_space = colorType;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, true);
	jpeg_start_compress(&cinfo, TRUE);

	// check if a read error occured
	destination.checkState();

	// write data		
	JSAMPROW data = image->getData<JSAMPLE>(mipmapIndex, imageIndex);
	int stride = width * channelCount;
	while (cinfo.next_scanline < cinfo.image_height)
	{
		jpeg_write_scanlines(&cinfo, &data, 1);
		data += stride;

		// check if a read error occured
		destination.checkState();
	}
	jpeg_finish_compress(&cinfo);

	// the source closes the jpeg image, also when an exception is thrown		
}

} // namespace digi
