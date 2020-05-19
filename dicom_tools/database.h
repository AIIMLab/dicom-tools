//
// Created by Jonas Teuwen on 06/04/2020.
//
#include <iostream>
#include <sqlite3.h>


#ifndef DICOM_TOOLS_DATABASE_H
#define DICOM_TOOLS_DATABASE_H

#endif //DICOM_TOOLS_DATABASE_H


const std::string patient_sql = "CREATE TABLE Patients("
                                "PatientID                      VARCHAR(64)     PRIMARY KEY     NOT NULL, "
                                "PatientName                    VARCHAR(64),                              "
                                "PatientBirthDate               CHAR(8),                                  "
                                "PatientAge                     CHAR(4),                                  "
                                "PatientSex                     VARCHAR(16));";

const std::string studies_sql = "CREATE TABLE Studies("
                                "StudyInstanceUID               VARCHAR(64)     PRIMARY KEY     NOT NULL, "
                                "StudyDate                      CHAR(12),                                 "
                                "StudyTime                      VARCHAR(16),                              "
                                "StudyID                        VARCHAR(16),                              "
                                "StudyDescription               VARCHAR(64),                              "
                                "AccessionNumber                VARCHAR(16),                              "
                                "ModalitiesInStudy              VARCHAR(64),                              "
                                "PatientID                      VARCHAR(64),                              "
                                "CONSTRAINT fk                  FOREIGN KEY (PatientID)                   "
                                "REFERENCES Patients(PatientID) ON DELETE CASCADE);";


const std::string series_sql = "CREATE TABLE Series("
                               "SeriesInstanceUID               VARCHAR(64)     PRIMARY KEY     NOT NULL, "
                               "SeriesNumber                    VARCHAR(12),                              "
                               "SeriesDate                      CHAR(12),                                 "
                               "SeriesTime                      VARCHAR(16),                              "
                               "SeriesDescription               VARCHAR(64),                              "
                               "Modality                        VARCHAR(16),                              "
                               "PatientPosition                 VARCHAR(16),                              "
                               "ContrastBolusAgent              VARCHAR(64),                              "
                               "Manufacturer                    VARCHAR(64),                              "
                               "ManufacturerModelName           VARCHAR(64),                              "
                               "BodyPartExamined                VARCHAR(64),                              "
                               "ProtocolName                    VARCHAR(64),                              "
                               "InstitutionName                 VARCHAR(64),                              "
                               "FrameOfReferenceUID             VARCHAR(64),                              "
                               "StudyInstanceUID                VARCHAR(64),                              "
                               "CONSTRAINT fk                   FOREIGN KEY (StudyInstanceUID)             "
                               "REFERENCES Studies(StudyInstanceUID) ON DELETE CASCADE);";


const std::string images_sql = "CREATE TABLE Images("
                               "SOPInstanceUID                  VARCHAR(64)     PRIMARY KEY     NOT NULL, "
                               "SOPClassUID                     VARCHAR(12),                              "
                               "ImageID                         VARCHAR(12),                              "
                               "InstanceNumber                  VARCHAR(12),                              "
                               "ContentDate                     CHAR(12),                                 "
                               "ContentTime                     VARCHAR(16),                              "
                               "NumberOfFrames                  VARCHAR(12),                              "
                               "AcquisitionNumber               VARCHAR(12),                              "
                               "AcquisitionDate                 CHAR(12),                                 "
                               "AcquisitionTime                 VARCHAR(16),                              "
                               "ReceiveCoilName                 VARCHAR(12),                              "
                               "SliceLocation                   VARCHAR(16),                              "
                               "Rows                            VARCHAR(5),                               "
                               "Columns                         VARCHAR(5),                               "
                               "SamplesPerPixel                 VARCHAR(16),                              "
                               "PhotometricInterpretation       VARCHAR(16),                              "
                               "BitsStored                      VARCHAR(5),                               "
                               "ImageType                       VARCHAR(128),                             "
                               "filename                        VARCHAR(128),                             "
                               "SeriesInstanceUID               VARCHAR(64),                              "
                               "CONSTRAINT fk                   FOREIGN KEY (SeriesInstanceUID)            "
                               "REFERENCES Series(SeriesInstanceUID) ON DELETE CASCADE);";

// C.8.3.1 MR Image Module
const std::string mri_modality_sql = "CREATE TABLE IF NOT EXISTS MRIImages("
                                     "SOPInstanceUID                  VARCHAR(64)     PRIMARY KEY     NOT NULL, "
                                     "ScanningSequence                VARCHAR(64),                              "
                                     "SequenceVariant                 VARCHAR(64),                              "
                                     "AcquisitionType                 VARCHAR(16),                              "
                                     "EchoTime                        VARCHAR(16),                              "
                                     "EchoTrainLength                 VARCHAR(64),                              "
                                     "EchoNumbers                     VARCHAR(64),                              "
                                     "InversionTime                   VARCHAR(16),                              "
                                     "TriggerTime                     VARCHAR(16),                              "
                                     "RepetitionTime                  VARCHAR(16),                              "
                                     "SequenceName                    VARCHAR(16),                              "
                                     "SeriesTime                      VARCHAR(16),                              "
                                     "MRAcquisitionType               VARCHAR(16),                              "
                                     "MagneticFieldStrength           VARCHAR(16),                              "
                                     "FlipAngle                       VARCHAR(64),                              "
                                     "NumberOfTemporalPositions       VARCHAR(64),                              "
                                     "TemporalPositionIdentifier      VARCHAR(64),                              "
                                     "TemporalResolution              VARCHAR(64),                              "
                                     "CONSTRAINT fk                   FOREIGN KEY (SOPInstanceUID)              "
                                     "REFERENCES Image(SOPInstanceUID) ON DELETE CASCADE);";


const std::string indices = "CREATE UNIQUE INDEX filename on Images (filename); "
                            "CREATE UNIQUE INDEX SOPInstanceUID on Images (SOPInstanceUID); "
                            "CREATE UNIQUE INDEX PatientID on Patients (PatientID); "
                            "CREATE UNIQUE INDEX StudyInstanceUID on Studies (StudyInstanceUID); "
                            "CREATE UNIQUE INDEX SeriesInstanceUID on Series (SeriesInstanceUID); ";



struct SQLite3 {
    typedef std::shared_ptr<std::string> Value;
    typedef std::vector<std::pair<std::string, Value>> Row;
    sqlite3 *db;

    SQLite3(const std::string &database_name);

    ~SQLite3();

    std::vector<Row> exec(const std::string &sql);
};