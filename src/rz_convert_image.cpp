/**
 * SPDX-FileComment: rz_convert_image — Convert images to AVIF or WebP in multiple sizes.
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: MIT
 *
 * @file   rz_convert_image.cpp
 * @brief  CLI tool: images → avif/webp (480, 680, 800, 1024, 1280)
 * @std    C++23
 * @deps   Magick++ (ImageMagick ≥ 7), CLI11
 */

#include <CLI/CLI.hpp>
#include <Magick++.h>

#include <algorithm>
#include <array>
#include <execution>
#include <expected>
#include <filesystem>
#include <format>
#include <mutex>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "rz_config.hpp"

namespace fs = std::filesystem;

// ─────────────────────────────────────────────
//  Types & Config
// ─────────────────────────────────────────────

struct AppConfig {
  fs::path output_dir;
  std::string format;
  unsigned int quality;
  bool force;
  bool strip_meta;
};

// Use a mutex to synchronize console output in multithreaded context
std::mutex console_mutex;

// ─────────────────────────────────────────────
//  Worker functions
// ─────────────────────────────────────────────

[[nodiscard]]
std::expected<void, std::string> process_image(const fs::path &file, const AppConfig &cfg) {
  try {
    Magick::Image img;
    img.read(file.string());
    img.autoOrient();

    // 4. Strip metadata if requested
    if (cfg.strip_meta) {
      img.strip();
    }

    std::size_t original_width = img.columns();
    std::string stem = file.stem().string();

    static constexpr std::array<std::size_t, 5> kSizes = {480, 680, 800, 1024, 1280};
    
    // 2. Determine which sizes to generate to prevent upscaling
    std::vector<std::size_t> sizes_to_generate;
    for (std::size_t size : kSizes) {
      if (size <= original_width) {
        sizes_to_generate.push_back(size);
      }
    }
    // If the image is smaller than our smallest target (480), just output the original size
    if (sizes_to_generate.empty()) {
      sizes_to_generate.push_back(original_width);
    }

    for (std::size_t target_width : sizes_to_generate) {
      fs::path out_path = cfg.output_dir / std::format("{}_{}w.{}", stem, target_width, cfg.format);

      // 3. Check if file exists to skip unless --force is used
      if (!cfg.force && fs::exists(out_path)) {
        std::scoped_lock lock(console_mutex);
        std::println("  [SKIP] {} (already exists)", out_path.string());
        continue;
      }

      Magick::Image resized = img;
      
      if (target_width != original_width) {
        std::string geom_str = std::format("{}x", target_width);
        Magick::Geometry geom(geom_str);
        resized.resize(geom);
      }

      std::string format_upper = cfg.format;
      std::ranges::transform(format_upper, format_upper.begin(),
                             [](unsigned char c) { return std::toupper(c); });

      resized.magick(format_upper);
      resized.quality(cfg.quality);
      resized.write(out_path.string());

      {
        std::scoped_lock lock(console_mutex);
        std::println("  [OK]   {}", out_path.string());
      }
    }

    return {};
  } catch (const Magick::Exception &e) {
    return std::unexpected(std::format("ImageMagick error for {}: {}", file.string(), e.what()));
  } catch (const std::exception &e) {
    return std::unexpected(std::format("Error processing {}: {}", file.string(), e.what()));
  }
}

// ─────────────────────────────────────────────
//  Entry point
// ─────────────────────────────────────────────
int main(int argc, char **argv) {
  Magick::InitializeMagick(*argv);

  CLI::App app{"rz_convert_image — Convert arbitrary images to AVIF or WebP in multiple sizes\n"
               "Requires Magick++ (ImageMagick ≥ 7) to be installed."};

  std::vector<fs::path> input_paths;
  AppConfig cfg{
    .output_dir = fs::current_path() / "web",
    .format = "avif",
    .quality = 80,
    .force = false,
    .strip_meta = false
  };

  app.add_option("inputs", input_paths,
                 "Input image files or directory. If a directory is provided, all images in it are processed.");

  app.add_option("-o,--output", cfg.output_dir,
                 "Output directory [default: ./web]");

  app.add_option("-f,--format", cfg.format,
                 "Output format: avif or webp [default: avif]")
      ->check(CLI::IsMember({"avif", "webp", "AVIF", "WEBP"}));

  app.add_option("-q,--quality", cfg.quality,
                 "Output quality 1–100 [default: 80]")
      ->check(CLI::Range(1u, 100u));

  app.add_flag("--force", cfg.force, "Overwrite existing files in output directory");
  app.add_flag("--strip", cfg.strip_meta, "Strip EXIF and other metadata to reduce file size");

  app.add_flag_callback(
      "-v",
      []() {
        std::println("v{}", PROJECT_VERSION);
        std::exit(0);
      },
      "Output short version");

  app.add_flag_callback(
      "--version",
      []() {
        std::println("{} v{} {} {} {}", PROJECT_NAME, PROJECT_VERSION,
                     PROG_CREATED, PROG_ORGANIZATION_NAME,
                     PROJECT_HOMEPAGE_URL);
        std::exit(0);
      },
      "Output long version");

  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  // Normalize format string to lowercase
  std::ranges::transform(cfg.format, cfg.format.begin(),
                         [](unsigned char c) { return std::tolower(c); });

  if (input_paths.empty()) {
    input_paths.push_back(fs::current_path());
  }

  // ── Gather files using C++23 std::views (Point 5) ───────────────────────
  auto is_supported_image = [](const fs::path &p) {
    if (!fs::is_regular_file(p)) return false;
    std::string ext = p.extension().string();
    std::ranges::transform(ext, ext.begin(), [](unsigned char c) { return std::tolower(c); });
    return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".heic" ||
           ext == ".webp" || ext == ".avif" || ext == ".bmp" || ext == ".tiff";
  };

  std::vector<fs::path> files_to_process;
  for (const auto &p : input_paths) {
    if (fs::is_regular_file(p) && is_supported_image(p)) {
      files_to_process.push_back(p);
    } else if (fs::is_directory(p)) {
      auto dir_view = std::views::filter(fs::directory_iterator(p), [](const fs::directory_entry& e){ return e.is_regular_file(); })
                    | std::views::transform([](const fs::directory_entry& e) { return e.path(); })
                    | std::views::filter(is_supported_image);
      for (const auto &file_path : dir_view) {
        files_to_process.push_back(file_path);
      }
    } else {
      std::println(stderr, "Warning: Input '{}' does not exist or is not supported.", p.string());
    }
  }

  if (files_to_process.empty()) {
    std::println(stderr, "No image files found to process.");
    return 1;
  }

  if (!fs::exists(cfg.output_dir)) {
    std::println("Creating output directory: {}", cfg.output_dir.string());
    fs::create_directories(cfg.output_dir);
  }

  std::println("Found {} file(s) to process. Starting parallel execution...", files_to_process.size());

  // ── Parallel Execution (Point 1) ──────────────────────────────────────────
  std::for_each(std::execution::par, files_to_process.begin(), files_to_process.end(),
                [&cfg](const fs::path &file) {
                  {
                    std::scoped_lock lock(console_mutex);
                    std::println("Processing: {}", file.string());
                  }

                  if (auto result = process_image(file, cfg); !result.has_value()) {
                    std::scoped_lock lock(console_mutex);
                    std::println(stderr, "{}", result.error());
                  }
                });

  std::println("\nDone.");
  return 0;
}
