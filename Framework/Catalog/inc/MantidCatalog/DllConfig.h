// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2012 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_CATALOG_DLLCONFIG_H_
#define MANTID_CATALOG_DLLCONFIG_H_

/**
 * This file contains the DLLExport/DLLImport linkage configuration for the
 * Catalog library
 */
#include "MantidKernel/System.h"

#ifdef IN_MANTID_CATALOG
#define MANTID_CATALOG_DLL DLLExport
#define EXTERN_MANTID_CATALOG
#else
#define MANTID_CATALOG_DLL DLLImport
#define EXTERN_MANTID_CATALOG EXTERN_IMPORT
#endif /* IN_MANTID_CATALOG */

#endif // MANTID_CATALOG_DLLCONFIG_H_
