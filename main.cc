#include "src/acs/acd_writer.hpp"
#include "src/acs/header.hpp"
#include "src/acs/bmp_writer.hpp"
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <iostream>

static std::string filenameForIndex(size_t i, std::string const& extension) {
    return banana::acs::acd_writer::itoa(i) + "." + extension;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.acs> <output/>"
            << std::endl;
        return EXIT_FAILURE;
    }

    banana::acs::header header {};
    header.load(argv[1]);

    std::error_code err;
    std::filesystem::path out_path { argv[2] };
    auto audio_path = out_path / "Audio";
    auto image_path = out_path / "Images";
    if (!std::filesystem::create_directory(out_path, err) ||
        !std::filesystem::create_directory(audio_path, err) ||
        !std::filesystem::create_directory(image_path, err))
    {
        std::cerr << "couldn't create output directories" << std::endl;
        return EXIT_FAILURE;
    }

    // Write sounds
    for (size_t i=0; i<header.sounds.size(); ++i) {
        std::ofstream out;
        auto &sound = header.sounds[i];
        out.open(audio_path / filenameForIndex(i, "wav"),
            std::ios::binary | std::ios::out);
        out.write((const char *)sound.data.data(), sound.data.size());
        out.close();
    }

    // Write images
    banana::acs::bmp_writer bmp_writer;
    bmp_writer.set_palette(header.character_info.color_table);
    for (size_t i=0; i<header.images.size(); ++i) {
        std::ofstream out;
        auto &image = header.images[i];
        out.open(image_path / filenameForIndex(i, "bmp"),
            std::ios::binary | std::ios::out);
        auto decompressed = image.image_data.decompress();
        bmp_writer.write_bmp(out, image.width,
            image.height, decompressed);
        out.close();
    }

    // Write system tray icon
    if (header.character_info.tray_icon.has_value()) {
        static const uint8_t ico_header[] = { 0x42, 0x4D, 0x70, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x36, 0x00, 0x00, 0x00 };
        
        std::ofstream out;
        auto &color = header.character_info.tray_icon->color_bitmap;
        out.open(image_path / "SystemTray.ico", std::ios::binary | std::ios::out);
        out.write((const char *)ico_header, sizeof(ico_header));
        out.write((const char *)color.data(), color.size());
        out.close();

        auto &mono = header.character_info.tray_icon->monochrome_bitmap;
        out.open(image_path / "SystemTrayMono.ico", std::ios::binary | std::ios::out);
        out.write((const char *)ico_header, sizeof(ico_header));
        out.write((const char *)mono.data(), mono.size());
        out.close();
    }

    auto acd_path = out_path / (out_path.filename().string() + ".acd");
    {
        std::ofstream out;
        out.open(acd_path);
        banana::acs::acd_writer acd_writer;
        header.write_to(acd_writer);
        out << acd_writer.str() << std::endl;
        out.close();
    }

    return EXIT_SUCCESS;
}