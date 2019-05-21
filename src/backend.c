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
 *      @brief UI 'backend' function implementations relating to barcode generation
 *      @author Elijah Schutz
 *      @date 21/4/19
 */

#include "backend.h"
#include "error.h"
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>

/**
 *      @detail bk_generate_png() generates a temporary PNG file from a list of barcodes and
 *              property structures and fills the destination pointer with the image path.
 */

// clang-format off
int bk_generate(
    char barcodes[][BK_BARCODE_LENGTH],
    int quantities[],
    int num_barcodes,
    PSProperties * props,
    Layout * layout,
    char ** postscript_dest
) {
    // clang-format on
    Code128 **barcode_structs;
    // There are multiple failure points so we define some memory allocation flags
    // Safe error handling is provided by setjmp() and longjmp()
    bool postscript_allocated = false;
    int num_allocated = 0;

    jmp_buf env;
    int status = SUCCESS;

    if (!setjmp(env)) {
        /* Calculate total number of barcodes that will be generated, which is used to calculate the
           size of the barcode buffer */
        int total_barcodes = 0;
        size_t barcode_structs_size = 0;
        for (int barcode_no = 0; barcode_no < num_barcodes; barcode_no++) {
            for (int barcode_idx = 0; barcode_idx < quantities[barcode_no]; barcode_idx++) {
                total_barcodes++;
            }
        }
        barcode_structs_size = sizeof *barcode_structs * total_barcodes;

        barcode_structs = calloc(1, barcode_structs_size);
        VERIFY_NULL_G(barcode_structs, barcode_structs_size);


         /** Algorithm:
          (i) Generate barcodes from input text
          (ii) Generate PostScript from the given barcodes and properties and store in a supplied
               string buffer
         */

        /* (i) */
        /* Loop over each barcode, then encode that barcode the corresponding number of times specified
           in quantities[]. This creates an array of internal barcode representations which we later use
           to generate PostScript */
        for (int barcode_no = 0; barcode_no < num_barcodes; barcode_no++) {
            for (int barcode_idx = 0; barcode_idx < quantities[barcode_no]; barcode_idx++) {
                status = c128_encode((uchar *) barcodes[barcode_no], strlen(barcodes[barcode_no]), &barcode_structs[num_allocated]);
                if (status != SUCCESS) {
                    longjmp(env, status);
                }
                num_allocated++;
            }
        }

        /* (iii) */
        status = c128_ps_layout(barcode_structs, total_barcodes, postscript_dest, props, layout);

        if (status != SUCCESS) {
            longjmp(env, status);
        }

        postscript_allocated = true;
    } else {
        if (postscript_allocated) {
            free(postscript_dest);
        }
    }

    // Clean up barcode_structs
    for (int i = 0; i < num_allocated; i++) {
        free(barcode_structs[i]);
    }
    free(barcode_structs);


    return status;
}
