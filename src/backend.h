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
 *      @file backend.c
 *      @brief UI 'backend' function declarations relating to barcode generation
 *      @author Elijah Schutz
 *      @date 23/3/19
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include "barcode.h"

#define BK_BARCODE_LENGTH C128_MAX_STRING_LEN
#define BK_TEMPFILE_TEMPLATE "barcodeXXXXXX"
#define BK_TEMPFILE_TEMPLATE_LENGTH 14
#define BK_GHOSTSCRIPT_CMD "gs -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dGraphicsAlphaBits=4 -sOutputFile=%s %s > errfile.txt"
#define BK_GHOSTSCRIPT_CMD_LENGTH 256
#define BK_GHOSTSCRIPT_ARGC 7
#define BK_OUT_FILE_IDX 6
#define BK_OUT_FILE_FMT "-sOutputFile=%s"
#define BK_OUT_FILE_FMT_LEN 16
#define BK_OUT_FILE_BUF_LEN BK_OUT_FILE_FMT_LEN + BK_TEMPFILE_TEMPLATE_LENGTH - 1
#define BK_GS_USER_ERRORS 0

const extern char *BK_GHOSTSCRIPT_ARGV[];

// bk_generate_png(char[][C128_MAX_STRING_LEN] barcodes, int[] quantities, int num_barcodes, ...);
char* bk_generate_png(char[][C128_MAX_STRING_LEN], int[], int, PSProperties *, Layout *);

#endif
