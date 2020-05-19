#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include "filesystem.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/program_options.hpp>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include "tags.h"
#include "database.h"
#include "progressbar.h"
#include "io/cli.h"

// TODO: check if we only need to update when we see the FIRST file.

namespace po = boost::program_options;
namespace fs = boost::filesystem;


std::string BuildQuery(const std::vector<DcmTagKey> &use_these_tags, std::map<DcmTagKey, std::string> &dicom_keys_map,
                       std::map<DcmTagKey, std::string> &dicom_metadata, std::string table_name,
                       std::string type_string) {

    std::vector<std::string> query_vector_names;
    std::vector<std::string> query_vector_values;

    std::string query;
    int counter = 0;

    std::string query_names;
    std::string query_values;

    for (auto const &dicom_tag : use_these_tags) {
        query_vector_names.push_back(dicom_keys_map[dicom_tag]);
        std::string curr_tag_value = dicom_metadata[dicom_tag];
        std::string query_value;
        if (curr_tag_value != "")
        {
            boost::replace_all(curr_tag_value, "'", "''");
            query_value = "'" + curr_tag_value + "'";
        } else
            query_value = "NULL";

        query_vector_values.push_back(query_value);

        if (type_string == "UPDATE") {
            // first value is the key itself
            if (counter != 0) {
                query += dicom_keys_map[dicom_tag];
                query += " = COALESCE(";
                query += dicom_keys_map[dicom_tag];
                query += ", ";
                query += query_value;
                query += ")";
                if (counter < use_these_tags.size() - 1) {
                    query += ",";
                }
            }
        }
        counter += 1;
    }


    // TODO: No need to use boost for this.
    if (type_string == "INSERT") {
        query_names = boost::algorithm::join(query_vector_names, ",");
        query_values = boost::algorithm::join(query_vector_values, ",");
        query = "INSERT INTO ";
        query += table_name;
        query += " (";
        query += query_names;
        query += ") VALUES (";
        query += query_values;
        query += ");";
    }

    return query;
}


int ParseDicom(const char *filename, std::map<DcmTagKey, std::string> &dicom_metadata) {
    DcmFileFormat file_format;


    OFCondition status = file_format.loadFile(filename);
    if (status.bad()) {
        std::cerr << "FATAL_DICOM: " + std::string(status.text()) + ": Path(" << filename << ")" << std::endl;
        return 1;
    }

    DcmDataset *dataset = file_format.getDataset();

    for (const auto &dicom_tag_group : dicom_tags) {
        for (const auto &dicom_tag : dicom_tag_group) {
            OFCondition condition;
            OFString value;
            condition = dataset->findAndGetOFStringArray(dicom_tag, value);
            if (condition.good()) {
                dicom_metadata[dicom_tag] = value.c_str();
            } else {
                dicom_metadata[dicom_tag] = "";
            }
        }
    }
    return 0;
}


int main(int argc, const char *argv[]) {
    fs::path input_dir;
    fs::path use_file_for_paths;
    fs::path sqlite_database;
    
    bool dump_filenames;
    bool verbose;
    ProcessCommandLine(input_dir, sqlite_database, use_file_for_paths, dump_filenames, verbose, argc, argv);

    freopen("error.log", "w", stderr);

    std::map<DcmTagKey, std::string> dicom_keys_map;
    for (const auto &dicom_tag_group : dicom_tags) {
        for (const auto &dicom_tag : dicom_tag_group) {
            dicom_keys_map[dicom_tag] = std::string(DcmTag(dicom_tag).getTagName());
        }
    }

    bool db_exists = fs::exists(sqlite_database);
    SQLite3 db(sqlite_database.string());
    if (!db_exists) {
        db.exec(patient_sql);
        db.exec(studies_sql);
        db.exec(series_sql);
        db.exec(images_sql);
//        db.exec(indices);
        std::cout << "Database " << sqlite_database.string() << " created." << std::endl;
    }

    std::cout << "Parsing DICOM headers..." << std::endl;
    const auto start = std::chrono::steady_clock::now();

    int exists_counter = 0;
    int imported_counter = 0;
    int error_counter = 0;

    std::vector<std::string> modalities_seen;

    std::vector<std::string> file_list;
    int has_counter = 0;
    if (!use_file_for_paths.empty())
    {
        std::cout << "Using filename for files: " << use_file_for_paths.string() << std::endl;

        // Open the File
        std::ifstream input_file(use_file_for_paths.c_str());

        // Check if object is valid
        if(!input_file)
        {
            std::cerr << "Cannot open: " << use_file_for_paths.string() << std::endl;
            return 0;
        }

        std::string str;
        while (std::getline(input_file, str))
        {
            if(!str.empty()) {
                    file_list.push_back(str);
                }
            }
        int original_size = file_list.size();
        std::cout << "Read list with " << original_size << " files." << std::endl;
        // Find if it was there yet.
        std::string query = "SELECT filename FROM Images;";

        std::vector<SQLite3::Row> result_table;
        result_table = db.exec(query);
        std::vector<std::string> file_list_new;

        if (result_table.size() > 0) {
            std::unordered_set<std::string> in_database_strings;
            std::cout << "Getting files already in database..." << std::endl;
            for (const auto &row : result_table) {
                for (const auto &col : row) {
                    in_database_strings.insert(*col.second);
                    has_counter += 1;
                }
            }
            std::cout << "Found " << has_counter << " files in database." << std::endl;


            std::copy_if (file_list.begin(), file_list.end(),
                    std::back_inserter(file_list_new),
                    [&in_database_strings](std::string i){return (in_database_strings.find(i) == in_database_strings.end());} );
            std::cout << "Filtered, length is now: " << file_list_new.size() << std::endl;

            file_list.clear();
            file_list = file_list_new;

        }
        else{
        }
        input_file.close();
        std::cout << "Need to process " << file_list.size() <<
        " files. Will not check if these are DICOM." << std::endl;
    }
    else {
        std::cout << "Parsing directory: " << input_dir.string() << std::endl;
        RecursiveFindDicom(input_dir, file_list, true);

        if (dump_filenames) {
            std::cout << "Dumping filenames. " << std::endl;
            std::ofstream output_filename("filenames.txt");
            for (const auto &e : file_list) output_filename << e << "\n";
            output_filename.close();
        }
    }

    progressbar bar(file_list.size());

    #pragma omp parallel for
    for(int i=0; i<file_list.size(); i++) {
        db.exec("BEGIN TRANSACTION");
        std::string filename = file_list[i];
        if(verbose)
        {
            std::cout << "Working on: " << filename << std::endl;
        }
        // dicom_metadata holds the metadata for our dicom file.
        std::map<DcmTagKey, std::string> dicom_metadata;
        int ret = ParseDicom(filename.c_str(), dicom_metadata);

        if (ret == 1) {
            continue;
        }

        std::string modality = dicom_metadata[DCM_Modality];

        // Check if we saw this Modality yet
        auto it = std::find(modalities_seen.begin(), modalities_seen.end(), modality);
        if (it == modalities_seen.end()) {
            modalities_seen.push_back(modality);
            if (modality == "MR" || modality == "MROT")
                db.exec(mri_modality_sql);
        }


        std::string query;
        // First find if this has not yet been inserted
        query = "SELECT SOPInstanceUID,filename FROM Images WHERE SOPInstanceUID = '";
        query += dicom_metadata[DCM_SOPInstanceUID];
        query += "';";

        std::vector<SQLite3::Row> result_table;
        result_table = db.exec(query);

        if (result_table.size() == 1) { // There is a match (necessarily unique)
            std::string existing_filename = *result_table[0][1].second;
            if (existing_filename == filename) exists_counter += 1;
            else {
                std::cerr
                        << "SOP_EXISTS: " << "Path(" << existing_filename
                        << ") - Path(" << filename << ")" << std::endl;
                error_counter += 1;
            }
        } else {
            query = BuildQuery(dicom_tags_image, dicom_keys_map, dicom_metadata,
                    "Images", "INSERT");
            db.exec(query);

            query = "UPDATE Images SET filename = '";
            query += filename;
            query += "' WHERE SOPInstanceUID = '";
            query += dicom_metadata[DCM_SOPInstanceUID];
            query += "';";

            db.exec(query);
        }

        // Import patient
        query = "SELECT * FROM Patients WHERE PatientID = '";
        query += dicom_metadata[DCM_PatientID];
        query += "';";
        result_table = db.exec(query);

        if (result_table.size() == 1) { // There is a match (necessarily unique)
            //                // Patient exists, might be possible some fields were missing earlier, add these.
            query = "UPDATE Patients SET ";
            query += BuildQuery(dicom_tags_patient, dicom_keys_map, dicom_metadata,
                    "Patients", "UPDATE");
            query += " WHERE PatientID = ";
            query += "'";
            query += dicom_metadata[DCM_PatientID];
            query += "';";
            db.exec(query);
        } else {
            query = BuildQuery(dicom_tags_patient, dicom_keys_map, dicom_metadata,
                    "Patients", "INSERT");
            db.exec(query);
        }

        // Import study
        query = "SELECT * FROM Studies WHERE StudyInstanceUID = '";
        query += dicom_metadata[DCM_StudyInstanceUID];
        query += "';";
        result_table = db.exec(query);

        if (result_table.size() == 1) { // There is a match (necessarily unique)
            //                // Patient exists, might be possible some fields were missing earlier, add these.
            query = "UPDATE Studies SET ";
            query += BuildQuery(dicom_tags_study, dicom_keys_map,
                    dicom_metadata, "Studies", "UPDATE");
            query += " WHERE StudyInstanceUID = ";
            query += "'";
            query += dicom_metadata[DCM_StudyInstanceUID];
            query += "';";
            db.exec(query);
        } else {
            query = BuildQuery(dicom_tags_study, dicom_keys_map,
                    dicom_metadata, "Studies", "INSERT");

            db.exec(query);
        }


        // Import series
        query = "SELECT * FROM Series WHERE SeriesInstanceUID = '";
        query += dicom_metadata[DCM_SeriesInstanceUID];
        query += "';";
        result_table = db.exec(query);

        if (result_table.size() == 1) { // There is a match (necessarily unique)
            //                // Patient exists, might be possible some fields were missing earlier, add these.
            query = "UPDATE Series SET ";
            query += BuildQuery(dicom_tags_series, dicom_keys_map,
                    dicom_metadata, "Series", "UPDATE");
            query += " WHERE SeriesInstanceUID = ";
            query += "'";
            query += dicom_metadata[DCM_SeriesInstanceUID];
            query += "';";
            db.exec(query);
        } else {
            query = BuildQuery(dicom_tags_series, dicom_keys_map, dicom_metadata, "Series", "INSERT");
            db.exec(query);
        }

        // We add modality specific tables here
        if (modality == "MR" || modality == "MROT")
        {
            query = "SELECT * FROM MRIImages WHERE SOPInstanceUID = '";
            query += dicom_metadata[DCM_SOPInstanceUID];
            query += "';";
            result_table = db.exec(query);

            if (result_table.size() == 1) { // There is a match (necessarily unique)
                //                // Patient exists, might be possible some fields were missing earlier, add these.
                query = "UPDATE MRIImages SET ";
                query += BuildQuery(dicom_mri_modality_tags, dicom_keys_map,
                        dicom_metadata, "MRIImages", "UPDATE");
                query += " WHERE SOPInstanceUID = ";
                query += "'";
                query += dicom_metadata[DCM_SOPInstanceUID];
                query += "';";
                db.exec(query);

            } else {
                query = BuildQuery(
                        dicom_mri_modality_tags, dicom_keys_map,
                        dicom_metadata, "MRIImages", "INSERT");
                db.exec(query);
            }
        }
        db.exec("END TRANSACTION");
        imported_counter += 1;
        if (!verbose) {
            bar.update();
        }
    }

    std::cout << "\n" << exists_counter << " were already in the database. " << std::endl;
    std::cout << error_counter << " errors encountered. Logged to errors.log";
    const auto dur = std::chrono::steady_clock::now() - start;
    std::cout << "\nProcess completed in " << std::chrono::duration_cast<std::chrono::seconds>(dur).count()
              << " seconds." << std::endl;
}
