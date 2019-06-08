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
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>

static FILE * bk_tempfile;
static char * bk_tempfile_path;

int bk_init(void) {
    jmp_buf env;
    int status = SUCCESS;

    if (!setjmp(env)) {
        bk_tempfile_path = calloc(1, BK_TEMPFILE_TEMPLATE_SIZE);
        VERIFY_NULL_BC(bk_tempfile_path, BK_TEMPFILE_TEMPLATE_SIZE);

        // -1 to account for null terminator
        strncpy((char*) bk_tempfile_path, BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_SIZE - 1);

        #ifdef _WIN32
        if (SUCCESS = _mktemp_s(bk_tempfile_path, BK_TEMPFILE_TEMPLATE_SIZE)) {
            bk_tempfile = fopen(bk_tempfile_path, "w");
            tempfile_assigned = true;
            if (NULL == bk_tempfile) {
                status = ERR_TEMPORARY_FILE_CREATION_FAILED;
                longjmp(env, status);
            }
        } else {
            status = ERR_TEMPORARY_FILE_CREATION_FAILED;
            longjmp(env, status);
        }
        #else
        int temp_fd = mkstemp(bk_tempfile_path);
        if (-1 == temp_fd) {
            status = ERR_TEMPORARY_FILE_CREATION_FAILED;
            longjmp(env, status);
        }

        bk_tempfile = fdopen(temp_fd, "w");
        if (NULL == bk_tempfile) {
            status = ERR_TEMPORARY_FILE_CREATION_FAILED;
            longjmp(env, status);
        }
        #endif
    } else {
        free(bk_tempfile_path);
    }

    return status;
}

int bk_exit(void) {
    int status = SUCCESS;

    if (EOF == fclose(bk_tempfile)) {
        status = ERR_FILE_CLOSE_FAILED;
        // non-fatal error
    }

    if (SUCCESS != remove(bk_tempfile_path)) {
        status = ERR_FILE_REMOVE_FAILED;
        // non-fatal error
    }

    free(bk_tempfile_path);

    return status;
}

/**
 *      @detail bk_generate_png() generates a temporary po file from a list of barcodes and
 *              property structures and fills the destination pointer with the image path.
 */

// clang-format off
int bk_generate(
    char barcodes[][BK_BARCODE_LENGTH],
    int quantities[],
    int num_barcodes,
    PSProperties * props,
    Layout * layout,
    char ** ps_name_ptr
) {
    // clang-format on

    char *postscript_dest;
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
        VERIFY_NULL_BC(barcode_structs, barcode_structs_size);


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
        status = c128_ps_layout(barcode_structs, total_barcodes, &postscript_dest, props, layout);

        if (status != SUCCESS) {
            longjmp(env, status);
        }

        postscript_allocated = true;

        // reset file position to 0
        if (SUCCESS != fseek(bk_tempfile, 0, SEEK_SET)) {
            status = ERR_FILE_POSITION_RESET_FAILED;
            longjmp(env, status);
        }

        // fprintf returns negative when an error occurs
        if (fprintf(bk_tempfile, "%s", postscript_dest) < 0) {
            status = ERR_FILE_WRITE_FAILED;
            longjmp(env, status);
        }

        // allocate and copy file destination to the given pointer
        *ps_name_ptr = calloc(1, BK_TEMPFILE_TEMPLATE_SIZE);
        VERIFY_NULL_BC(*ps_name_ptr, BK_TEMPFILE_TEMPLATE_SIZE);

        strncpy(*ps_name_ptr, bk_tempfile_path, BK_TEMPFILE_TEMPLATE_SIZE);

    }

    if (postscript_allocated) {
        free(postscript_dest);
    }

    // Clean up barcode_structs
    for (int i = 0; i < num_allocated; i++) {
        free(barcode_structs[i]);
    }
    free(barcode_structs);


    return status;
}
