/****************************************************************************
 * Copyright (C) 2019  Elijah Schutz
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ****************************************************************************/
/*
 *      @file backend.h
 *      @brief UI 'backend' function declarations relating to barcode generation
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef BACKEND_H
#define BACKEND_H

#include "barcode.h"
#include "util.h"
#include <stdio.h>

/**
 *      @defgroup BackendProperties Backend properties relating to print preview generation
 */
/*@{*/
// clang-format off
#define BK_BARCODE_LENGTH                   C128_MAX_STRING_LEN
#define BK_TEMPDIR_TEMPLATE                 P_tmpdir "barcode.dirXXXXXX"
#define BK_TEMPDIR_TEMPLATE_LENGTH          STRLEN(BK_TEMPDIR_TEMPLATE)
#define BK_TEMPDIR_LENGTH                   BK_TEMPDIR_TEMPLATE_LENGTH
#define BK_TEMPFILE_TEMPLATE_NAME           "barcodeXXXXXX"
/* We don't add -1 (to account for null-terminator double-up) as there is an extra character
   (UTIL_FSEP) connecting the directory and the file name */
#define BK_TEMPFILE_TEMPLATE_NAME_LENGTH   BK_TEMPDIR_TEMPLATE_LENGTH + STRLEN(BK_TEMPFILE_TEMPLATE_NAME)
#define BK_TEMPFILE_TEMPLATE_LENGTH        BK_TEMPFILE_TEMPLATE_NAME_LENGTH
#define BK_GHOSTSCRIPT_CMD                  "gs -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dGraphicsAlphaBits=4 -sOutputFile=%s %s > errfile.txt"
#define BK_GHOSTSCRIPT_CMD_LENGTH           256
#define BK_GHOSTSCRIPT_ARGC                 7
#define BK_OUT_FILE_IDX                     6
#define BK_OUT_FILE_FMT                     "-sOutputFile=%s"
#define BK_OUT_FILE_FMT_LEN                 STRLEN(BK_OUT_FILE_FMT)
#define BK_OUT_FILE_BUF_LEN                 BK_OUT_FILE_FMT_LEN + BK_TEMPFILE_TEMPLATE_LENGTH - 1
#define BK_GS_USER_ERRORS                   0
// clang-format on

const extern char *BK_GHOSTSCRIPT_ARGV[];
const extern char *BK_TEMPDIR;
const extern char *BK_TEMPFILE_TEMPLATE;
/*@}*/

/**
 *      @brief Backend initialisation code
 *      @return SUCCESS or ERR_TEMPORARY_DIR_CREATION_FAILED
 *      @warning ALWAYS call this before using any other backend functions
 */
int bk_init(void);

/**
 *      @brief Backend deinitialisation code
 *      @return SUCCESS, ERR_CHDIR_FAILED, ERR_FTS_ERROR
 *      @warning ALWAYS call this before the end of the program
 */
int bk_exit(void);

// bk_generate_png(char[][C128_MAX_STRING_LEN] barcodes, int[] quantities, int num_barcodes, ...);

/**
 *      @brief Generates a PNG print preview from the given barcodes and properties
 *      @param barcodes A list of barcode strings to be encoded
 *      @param quantities A list of barcode quantities (@c barcode[n] is drawn @c quantities[n]
 *                        times)
 *      @param num_barcodes The length of @c barcodes
 *      @param props The PostScript properties to be used when generating the PostScript and image
 *      @param layout The arrangement of rows and columns used to lay out the barcodes
 *      @param png_name The destination pointer for the PNG name
 *      @return SUCCESS
 */
int bk_generate_png(char[][C128_MAX_STRING_LEN], int[], int, PSProperties *, Layout *, char *);

#endif
