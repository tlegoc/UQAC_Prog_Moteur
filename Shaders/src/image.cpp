#include <simplege/pch/precomp.h>

#include <simplege/simplege.h>

#include <png.h>

namespace SimpleGE
{
  static void ReadPngData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
  {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    Ensures(io_ptr);

    auto* reader = std::bit_cast<std::vector<unsigned char>::const_iterator*>(io_ptr);
    std::copy(*reader, *reader + byteCountToRead, outBytes);
    *reader = *reader + byteCountToRead;
  }

  Image::Image(const std::vector<std::byte>& content)
  {
    Expects(png_sig_cmp((unsigned char*) content.data(), 0, 8) == 0);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    Ensures(png_ptr);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    Ensures(info_ptr);

    auto reader = content.begin();
    png_set_read_fn(png_ptr, &reader, ReadPngData);

    png_read_info(png_ptr, info_ptr);

    int bitDepth;
    int colorType;
    auto retVal =
        png_get_IHDR(png_ptr, info_ptr, &size.width, &size.height, &bitDepth, &colorType, nullptr, nullptr, nullptr);
    Ensures(retVal != -1);

    if (bitDepth == 16)
    {
      png_set_strip_16(png_ptr);
    }

    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_palette_to_rgb(png_ptr);
    }

    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
    {
      png_set_expand_gray_1_2_4_to_8(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0)
    {
      png_set_tRNS_to_alpha(png_ptr);
    }

    if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }

    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
      png_set_gray_to_rgb(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    std::size_t bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    m_Content.resize(size.height * bytesPerRow / sizeof(GFXAPI::ColorU8));
    std::vector<GFXAPI::ColorU8*> rowPointers(size.height);
    for (int y = 0; y < size.height; y++)
    {
      rowPointers[y] = &m_Content[y * bytesPerRow / sizeof(GFXAPI::ColorU8)];
    }

    png_read_image(png_ptr, (png_bytepp) rowPointers.data());

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  }
} // namespace SimpleGE