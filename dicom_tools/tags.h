//
// Created by Jonas Teuwen on 05/04/2020.
//
#include <vector>
#include <map>
#include <dcmtk/dcmdata/dctk.h>


#ifndef DICOM_TOOLS_TAGS_H
#define DICOM_TOOLS_TAGS_H

#endif //DICOM_TOOLS_TAGS_H

const std::vector<DcmTagKey> dicom_tags_patient = {
        DCM_PatientID, // Begin patient specific tags
        DCM_PatientName,
        DCM_PatientAge,
        DCM_PatientBirthDate,
        DCM_PatientSex, // End patient specific tags
};

const std::vector<DcmTagKey> dicom_tags_study = {
        DCM_StudyInstanceUID, // Begin study specific tags
        DCM_StudyDate,
        DCM_StudyTime,
        DCM_StudyID,
        DCM_StudyDescription,
        DCM_AccessionNumber,
        DCM_ModalitiesInStudy, // End study specific tags
        DCM_PatientID, // For cross referencing to Patient table
};

const std::vector<DcmTagKey> dicom_tags_series = {
        DCM_SeriesInstanceUID, // Begin series specific tags
        DCM_SeriesNumber,
        DCM_SeriesDate,
        DCM_SeriesTime,
        DCM_SeriesDescription,
        DCM_Modality,
        DCM_PatientPosition,
        DCM_ContrastBolusAgent,
        DCM_Manufacturer,
        DCM_ManufacturerModelName,
        DCM_BodyPartExamined,
        DCM_ProtocolName,
        DCM_InstitutionName,
        DCM_FrameOfReferenceUID, // End series specific tags
        DCM_StudyInstanceUID, // For cross referencing to Study table

};

const std::vector<DcmTagKey> dicom_tags_image = {
        DCM_SOPInstanceUID, // Begin image specific tags
        DCM_SOPClassUID,
        DCM_ImageID,
        DCM_InstanceNumber,
        DCM_ContentDate,
        DCM_ContentTime,
        DCM_NumberOfFrames,
        DCM_AcquisitionNumber,
        DCM_AcquisitionDate,
        DCM_AcquisitionTime,
        DCM_ReceiveCoilName,
        DCM_SliceLocation,
        DCM_Rows,
        DCM_Columns,
        DCM_SamplesPerPixel,
        DCM_PhotometricInterpretation,
        DCM_BitsStored,
        DCM_ImageType, // End image specific tags
        DCM_SeriesInstanceUID, // For cross referencing to Series table
};

const std::vector<DcmTagKey> dicom_mri_modality_tags = {
        DCM_ScanningSequence,
        DCM_SequenceVariant,
        DCM_AcquisitionType,
        DCM_RepetitionTime,
        DCM_EchoNumbers,
        DCM_EchoTime,
        DCM_EchoTrainLength,
        DCM_InversionTime,
        DCM_TriggerTime,
        DCM_SequenceName,
        DCM_MagneticFieldStrength,
        DCM_FlipAngle,
        DCM_SeriesTime,
        DCM_MRAcquisitionType,
        DCM_TemporalPositionIdentifier,
        DCM_NumberOfTemporalPositions,
        DCM_TemporalResolution,
        DCM_SOPInstanceUID, // For cross referencing to Images table. Unfortunately SeriesInstanceUID
        // is not enforced to be different in different timepoints.
};


const std::vector<std::vector<DcmTagKey>> dicom_tags = {
        dicom_tags_patient, dicom_tags_study, dicom_tags_series, dicom_tags_image, dicom_mri_modality_tags};
