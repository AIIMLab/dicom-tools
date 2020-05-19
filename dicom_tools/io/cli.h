//
// Created by Jonas Teuwen on 10/04/2020.
//

#ifndef DICOM_TOOLS_CLI_H
#define DICOM_TOOLS_CLI_H

#endif //DICOM_TOOLS_CLI_H

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;


bool ProcessCommandLine(
        fs::path& input_dir, fs::path& sqlite_database, fs::path& use_file_for_paths,
        bool& dump_filenames, bool& verbose, int& argc, const char *argv[])
{
    bool do_not_dump_filenames;
    try
    {
        po::options_description description("dicom-tools help");
        description.add_options()
                ("help,h", "Display this help message")
                ("input-dir", po::value<fs::path>(), "Input directory")
                ("use-file", po::value<fs::path>(),
                 "Instead of scrolling over the dataset uses this list of files.")
                ("sql-database", po::value<fs::path>()->default_value("dicomtosql.db"), "Path to sqlite3 database")
                ("do-not-dump-filenames", po::bool_switch(&do_not_dump_filenames), "If set no filenames will be saved.")
                ("verbose", po::bool_switch(&verbose), "Verbose output.")
                ("version,v", "Display the version number");

        po::positional_options_description p;
        p.add("input-dir",-1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << description;
            return 0;
        }

        if (vm.count("version")) {
            std::cout << "dicom-tools version 0.1" << std::endl;

            return 0;
        }

        if (vm.count("input-dir")) {
            input_dir = vm["input-dir"].as<fs::path>();
            // TODO: Check empty directory.
            if (!fs::is_directory(input_dir)) {
                std::cout << input_dir.string() << " is not a directory." << std::endl;
                return 0;
            }
        }

        if (vm.count("use-file")) {
            use_file_for_paths = vm["use-file"].as<fs::path>();
            if (!fs::is_regular_file(use_file_for_paths)) {
                std::cout << use_file_for_paths.string() << " is not a file or does not exist." << std::endl;
                return 0;
            }
        }

        if (vm.count("do-not-dump-filenames")) {
            do_not_dump_filenames = vm["do-not-dump-filenames"].as<bool>();
            dump_filenames = ~do_not_dump_filenames;
        }

        if (vm.count("sql-database")) {
            sqlite_database = vm["sql-database"].as<fs::path>();
        }

    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return -1;
    }
    catch(...)
    {
        std::cerr << "Unhandled error." << "\n";
        return -1;
    }
}