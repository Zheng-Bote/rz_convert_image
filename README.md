# rz_convert_image

A modern, multi-threaded C++23 CLI tool for batch converting arbitrary images into optimized, responsive web formats (AVIF or WebP) at standard breakpoints (480w, 680w, 800w, 1024w, 1280w).

## Features

- **Batch Processing**: Point it at a directory, and it will recursively find and process all supported images.
- **Multithreading**: Uses standard C++ execution policies (`std::execution::par`) to process multiple images in parallel, fully utilizing all available CPU cores.
- **Anti-Upscaling**: Intelligently prevents upscaling images. It only generates responsive sizes that are smaller than or equal to the original image's width, saving storage and preserving quality.
- **Incremental Builds**: Safely skip already converted images to save massive amounts of time on subsequent runs. Use the `--force` flag to overwrite.
- **Metadata Stripping**: Optional `--strip` flag to remove bulky EXIF data and private metadata, reducing the final file footprint for web deployment.
- **Modern C++23**: Built with modern paradigms like `std::expected` for robust error handling and `std::views` for lazy evaluation.

## Prerequisites

- **Compiler**: A C++23 compatible compiler (e.g., GCC 15+, Clang 18+, MSVC)
- **CMake**: Version 3.25 or newer
- **ImageMagick (Magick++)**: Version 7+ (Version 6 is also supported as fallback)

### Installing Dependencies

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install cmake g++ libmagick++-dev pkg-config
```

**macOS (Homebrew):**
```bash
brew install cmake imagemagick pkg-config
```

## Build

```bash
mkdir -p build
cd build
cmake ..
make
```

## Usage

```bash
# Convert a single image (outputs to ./web by default in AVIF)
./rz_convert_image my_photo.jpg

# Convert an entire directory, stripping metadata, saving as WebP
./rz_convert_image /path/to/images -o ./optimized_web -f webp --strip

# Force re-conversion of all images (overwrite existing)
./rz_convert_image /path/to/images --force
```

### CLI Options

```
Usage: rz_convert_image [OPTIONS] [inputs...]

Positionals:
  inputs TEXT ...             Input image files or directory. If a directory is provided, all images in it are processed.

Options:
  -h,--help                   Print this help message and exit
  -o,--output TEXT            Output directory [default: ./web]
  -f,--format TEXT            Output format: avif or webp [default: avif]
  -q,--quality UINT           Output quality 1–100 [default: 80]
  --force                     Overwrite existing files in output directory
  --strip                     Strip EXIF and other metadata to reduce file size
  -v                          Output short version
  --version                   Output long version
```

## License

This project is licensed under the MIT License.
