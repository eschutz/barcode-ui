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
#define BK_BARCODE_LENGTH                   C128_MAX_STRING_LEN
/*@}*/

/**
 *      @brief Generates PostScript the given barcodes and properties
 *      @param barcodes A list of barcode strings to be encoded
 *      @param quantities A list of barcode quantities (@c barcode[n] is drawn @c quantities[n]
 *                        times)
 *      @param num_barcodes The length of @c barcodes
 *      @param props The PostScript properties to be used when generating the PostScript and image
 *      @param layout The arrangement of rows and columns used to lay out the barcodes
 *      @param postscript_dest The destination pointer for generated PostScript
 *      @return SUCCESS
 */
int bk_generate(char[][C128_MAX_STRING_LEN], int[], int, PSProperties *, Layout *, char **);

#endif
