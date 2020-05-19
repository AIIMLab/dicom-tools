//
// Created by Jonas Teuwen on 04/04/2020.
//

#ifndef DICOM_TOOLS_FILESYSTEM_H
#define DICOM_TOOLS_FILESYSTEM_H

#endif //DICOM_TOOLS_FILESYSTEM_H

#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/ofstd/ofconapp.h"


namespace fs = boost::filesystem;

int RecursiveFindDicom(const fs::path &input_dir, std::vector<std::string> &file_list, bool check_if_dicom_10);

bool IsDicom(const fs::path &filename);