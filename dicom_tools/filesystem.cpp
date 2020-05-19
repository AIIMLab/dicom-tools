//
// Created by Jonas Teuwen on 04/04/2020.
//
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <boost/filesystem.hpp>
#include "filesystem.h"

namespace fs = boost::filesystem;


int RecursiveFindDicom(const fs::path &input_dir, std::vector<std::string> &file_list, bool check_if_dicom_10) {
    const auto start = std::chrono::steady_clock::now();

    auto begin = fs::recursive_directory_iterator(input_dir);
    const auto end = fs::recursive_directory_iterator();
    unsigned long long cnt = 0;

    for (; begin != end; ++begin) {
        const fs::path path = begin->path();
        if (!fs::is_directory(path)) {
            if (!check_if_dicom_10 || IsDicom(path)) {
                ++cnt;
                file_list.push_back(path.string());
                std::cout << "\r" << cnt << " DICOM files found..." << std::flush;
            }
        }
    }
    const auto dur = std::chrono::steady_clock::now() - start;
    std::cout << " Process completed in " << std::chrono::duration_cast<std::chrono::seconds>(dur).count()
              << " seconds." << std::endl;

    return 0;
}

bool IsDicom(const fs::path &filename)
/**
 * Input file is checked for the presence of the DICOM "magic word" 'DICM' at byte position 128.
 * @param filename
 * @return
 */
{
    bool is_dicom_10;
    FILE *f = nullptr;

    if (strlen(filename.string().c_str()) > 0) f = fopen(filename.string().c_str(), "rb");

    if (f == nullptr) {
        is_dicom_10 = false;
    } else {
        char signature[4];
        if ((fseek(f, DCM_PreambleLen, SEEK_SET) < 0) ||
            (fread(signature, 1, DCM_MagicLen, f) != DCM_MagicLen)) {
            is_dicom_10 = false;
        } else is_dicom_10 = strncmp(signature, DCM_Magic, DCM_MagicLen) == 0;
        fclose(f);
    }

    return is_dicom_10;
}