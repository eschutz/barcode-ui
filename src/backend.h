/* Copyright © 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file backend.h
 *      @brief UI 'backend' function declarations relating to barcode generation
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef BACKEND_H
#define BACKEND_H

#include "barcode.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif
/**
 *      @defgroup BackendProperties Backend properties relating to print preview generation
 */
/*@{*/
#define BK_BARCODE_LENGTH C128_MAX_STRING_LEN
#ifdef _WIN32
#define BUILD_TARGET "x86"
#define BK_TEMPFILE_TEMPLATE_SIZE L_tmpnam_s
#define BK_GET_PRINTER_CMD "wmic printer get name"
#define BK_POPEN_MODE "rt"
#define BK_WIN_PRINT_CMD "bin\\" BUILD_TARGET "\\gswin32.exe -dBATCH -dNOPAUSE -sDEVICE=mswinpr2 -sOutputFile=\"%%printer%%%s\""
#define popen _popen
#define pclose _pclose
#else
#define BK_TEMPFILE_TEMPLATE P_tmpdir "/barcodeXXXXXX"
#define BK_GET_PRINTER_CMD "lpstat -e"
#define BK_POPEN_MODE "r"
#define BK_TEMPFILE_TEMPLATE_SIZE sizeof(BK_TEMPFILE_TEMPLATE) + 1
#endif
#define BK_EXEC_BUFSIZE 1024 // Hopefully 1 KB is enough to hold printer info
/* #define BK_PRINTER_LENGTH                   127  // Enough for 8 printers, allowing for newlines
 */
#define BK_MAX_PRINTERS 8
/*@}*/

int bk_init(void);

int bk_exit(void);

/**
 *      @brief Generates PostScript the given barcodes and properties
 *      @param barcodes A list of barcode strings to be encoded
 *      @param quantities A list of barcode quantities (@c barcode[n] is drawn @c quantities[n]
 *                        times)
 *      @param num_barcodes The length of @c barcodes
 *      @param props The PostScript properties to be used when generating the PostScript and image
 *      @param layout The arrangement of rows and columns used to lay out the barcodes
 *      @param postscript_dest The destination pointer for generated PostScript
 *      @return SUCCESS,
                ERR_INVALID_LAYOUT,
                ERR_DATA_LENGTH,
                ERR_CHAR_INVALID,
                ERR_INVALID_CODE_SET,
                ERR_ARGUMENT,
                ERR_FILE_POSITION_RESET_FAILED,
                ERR_FILE_WRITE_FAILED,
                ERR_FLUSH,
                ERR_PRINTER_LIST (Windows only)
 */
int bk_generate(char **, int *, int, PSProperties *, Layout *, char **);

/**
 *      @brief Print a file to a specific printer - abstraction from platform-specific APIs
 *      @param filename File to print
 *      @param printer Destination printer
 *      @return SUCCESS, ERR_FORK
 */
int bk_print(char *, char *);

/**
 *      @brief Get a list of available printing destinations for use in bk_print()
 *      @param printers Unallocated triple pointer to char - is allocated within the function
 *      @param num_printers Destination pointer for the number of printers - the length of @c
 * printers
 *      @return SUCCESS, ERR_POPEN, ERR_FREAD
 */
int bk_get_printers(char ***, int *);

#endif
