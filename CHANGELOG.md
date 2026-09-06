# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-09-06

### Added
- **Initial Release** of `rz_convert_image`.
- Complete C++23 CLI tool for batch image conversion to AVIF and WebP formats.
- Generates responsive widths for the web: 480, 680, 800, 1024, 1280.
- Multi-threaded processing leveraging `std::execution::par`.
- Safe responsive generation logic that prevents upscaling smaller images.
- Incremental execution: Skips already converted files unless the `--force` flag is provided.
- Added a `--strip` argument to remove EXIF metadata for web optimization.
- Modern C++23 features: `std::expected` for robust error handling and `std::ranges` & `std::views` for directory filtering.
