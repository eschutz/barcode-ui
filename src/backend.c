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
#include "iapi.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fts.h>
#include <setjmp.h>

const char *BK_TEMPDIR;
const char *BK_TEMPFILE_TEMPLATE;

/**
 *      @detail Initialises BK_TEMPDIR and BK_TEMPFILE_TEMPLATE, preparing the application to
 *              generate image previews as temporary files.
 */
int bk_init(void) {
    jmp_buf env;
    int status = SUCCESS;

    if (!setjmp(env)) {
        #ifdef _WIN32
        #else
            size_t bk_tempdir_size = sizeof(char) * BK_TEMPDIR_LENGTH;
            BK_TEMPDIR = calloc(1, bk_tempdir_size);
            VERIFY_NULL_G(BK_TEMPDIR, bk_tempdir_size);

            // Copy the temporary directory template to BK_TEMPDIR
            strncpy((char*) BK_TEMPDIR, BK_TEMPDIR_TEMPLATE, BK_TEMPDIR_TEMPLATE_LENGTH);

            /* mkdtemp() modifies the argument in place, but its return value determines whether it was
               successful – if not, it returns a null pointer */
            const char result[BK_TEMPDIR_LENGTH];
            strncpy((char*) result, mkdtemp((char*) BK_TEMPDIR), BK_TEMPDIR_LENGTH);

            if (NULL == (char*) result) {
                status = ERR_TEMPORARY_DIR_CREATION_FAILED;
                longjmp(env, status);
            }

            size_t bk_tempfile_size = sizeof(char) * BK_TEMPFILE_TEMPLATE_LENGTH;
            BK_TEMPFILE_TEMPLATE = calloc(1, bk_tempfile_size);
            VERIFY_NULL_G(BK_TEMPFILE_TEMPLATE, bk_tempfile_size);


            /* Using the newly created temporary director, create a template for temporary files in that
               directory, to be used as destination files for print previews: e.g.
               '/tmp/barcode.dir123abc/barcodeXXXXXX' */
            strncpy((char*) BK_TEMPFILE_TEMPLATE, BK_TEMPDIR, BK_TEMPDIR_LENGTH);
            strncat((char*) BK_TEMPFILE_TEMPLATE, UTIL_FSEP, FSEP_LEN);
            strncat((char*) BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_NAME, BK_TEMPFILE_TEMPLATE_NAME_LENGTH);
        #endif
    } else {
        // If we're here, an error creating a temporary directory has occurred
        free((void*) BK_TEMPDIR);
    }

    return status;
}

/**
 *      @detail Removes temporary files and frees references created in the lifetime of the program.
 */
int bk_exit(void) {
    jmp_buf env;
    int status = SUCCESS;

    // setjmp() and longjmp() are used to free memory cleanly from nested error conditions
    if (!setjmp(env)) {
        #ifdef _WIN32
        #else
            // FTS argv format
            char * const dirs[BK_TEMPFILE_TEMPLATE_LENGTH] = { (char * const) BK_TEMPDIR, NULL };

            // Creates an FTS (File Traversal System?) handle
            // Note it has a depth of 1, as BK_TEMPDIR is only one directory
            FTS *fts_handle = fts_open((char * const *) dirs, FTS_LOGICAL | FTS_NOCHDIR, NULL);
            // Creates a linked list of children of the directory
            FTSENT *file_list = fts_children(fts_handle, FTS_NAMEONLY);

            if (chdir(BK_TEMPDIR) != 0) {
                status = ERR_CHDIR_FAILED;
                longjmp(env, status);
            }

            // Iterate through the linked list and delete files as we go
            while (NULL != file_list) {
                remove(file_list->fts_name);
                file_list = file_list->fts_link;
            }

            if (fts_close(fts_handle) != 0) {
                status = ERR_FTS_ERROR;
                longjmp(env, status);
            }
        #endif
    }

    free((void *) BK_TEMPDIR);
    free((void *) BK_TEMPFILE_TEMPLATE);

    return status;
}

/**
 *      @detail bk_generate_png() generates a temporary PNG file from a list of barcodes and
 *              property structures and fills the destination pointer with the image path.
 */

// clang-format off
int bk_generate_png(
    char barcodes[][BK_BARCODE_LENGTH],
    int quantities[],
    int num_barcodes,
    PSProperties * props,
    Layout * layout,
    char ** png_name_ptr
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
        *png_name_ptr = calloc(1, BK_TEMPFILE_TEMPLATE_LENGTH);
        VERIFY_NULL_G(*png_name_ptr, BK_TEMPFILE_TEMPLATE_LENGTH);

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
          (i) Generate temporary file name for destination PNG
          (ii) Generate barcodes from input text
          (iii) Generate PostScript from the given barcodes and properties and store in a string buffer
          (iv) Generate PNG from the generated PostScript and write to the temporary file
          (v) Return the file name **/

        /* (i) */
        /* Windows _mktemp() returns a string, whereas Unix mktemp() modifies the template string
           in-place */
        #ifdef _WIN32
            strncpy(*png_name_ptr, _mktemp(BK_TEMPFILE_TEMPLATE), BK_TEMPFILE_TEMPLATE_LENGTH);
        #else
            strncpy(*png_name_ptr, BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_LENGTH);
            mktemp(*png_name_ptr);
        #endif

        fprintf(stderr, "PNG NAME: '%s'\n", *png_name_ptr);

        /* (ii) */
        /* Loop over each barcode, then encode that barcode the corresponding number of times specified
           in quantities[]. This creates an array of internal barcode representations which we later use
           to generate PostScript */
        for (int barcode_no = 0; barcode_no < num_barcodes; barcode_no++) {
            for (int barcode_idx = 0; barcode_idx < quantities[barcode_no]; barcode_idx++) {
                status = c128_encode((uchar *) barcodes[barcode_no], strlen(barcodes[barcode_no]), &barcode_structs[barcode_no + barcode_idx]);
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

        fprintf(stderr, "%s", postscript_dest);
        postscript_allocated = true;
    }

    // Clean up barcode_structs
    for (int i = 0; i < num_allocated; i++) {
        free(barcode_structs[i]);
    }
    free(barcode_structs);

    if (postscript_allocated) {
        free(postscript_dest);
    }

    return status;
}






















































// const char *BK_GHOSTSCRIPT_ARGV[] = { "gs", "-dSAFER", "-dBATCH", "-dNOPAUSE", "-sDEVICE=png16m",
// "-dGraphicsAlphaBits=4" };

    /*
    Code128 **barcode_structs;
    int total_barcodes = 0;
    size_t element_size = sizeof *barcode_structs;

    for (int i = 0; i < num_barcodes; i++) {
        total_barcodes += quantities[i];
    }
    int size = element_size * total_barcodes;

    barcode_structs = calloc(1, size);
    if (NULL == barcode_structs) {
        fprintf(stderr, "FATAL: Failed to allocate %d bytes, terminating...\n", size);
        exit(1);
    }

    for (int i = 0; i < num_barcodes; i++) {
        char barcode[BK_BARCODE_LENGTH];
        strncpy(barcode, barcodes[i], BK_BARCODE_LENGTH);
        for (int j = 0; j < quantities[i]; j++) {
            c128_encode((uchar *) barcode, strlen(barcode), &barcode_structs[i]);
        }
    }

    char *ps_code;
    int status = c128_ps_layout(barcode_structs, total_barcodes, &ps_code, props, layout);
    if (SUCCESS != status) {
        fprintf(stderr, "FATAL: Barcode PostScript generation failed with code %d\n", status);
        exit(1);
    }

    // char *ps_name = calloc(1, BK_TEMPFILE_TEMPLATE_LENGTH);
    char *png_name = calloc(1, BK_TEMPFILE_TEMPLATE_LENGTH);

    #ifdef _WIN32
        // strncpy(ps_name, _mktemp(BK_TEMPFILE_TEMPLATE), BK_TEMPFILE_TEMPLATE_LENGTH);
        strncpy(png_name, _mktemp(BK_TEMPFILE_TEMPLATE), BK_TEMPFILE_TEMPLATE_LENGTH);
        // file = fopen(ps_name, "w");
    #else
        // strncpy(ps_name, BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_LENGTH);
        strncpy(png_name, BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_LENGTH);
        // int fd = mkstemp(ps_name);
        // if (fd == -1) {
        //     fprintf(stderr, "FATAL: Could not create temporary file, terminating...\n");
        //     exit(1);
        // }
        // file = fdopen(fd, "w");
        mktemp(png_name);
        fprintf(stderr, png_name);
    #endif

    int argv_len = sizeof BK_GHOSTSCRIPT_ARGV;
    char **argv = calloc(1, sizeof(**argv) * (argv_len + BK_OUT_FILE_BUF_LEN));
    memcpy(argv, BK_GHOSTSCRIPT_ARGV, argv_len);
    // argv[BK_OUT_FILE_IDX] = "";

    char buf[BK_OUT_FILE_BUF_LEN];

    snprintf(buf, BK_OUT_FILE_BUF_LEN, BK_OUT_FILE_FMT, png_name);

    argv[BK_OUT_FILE_IDX] = buf;
    // strncpy(BK_GHOSTSCRIPT_ARGV[BK_OUT_FILE_IDX], out_file, BK_OUT_FILE_BUF_LEN);
    // snprintf(BK_GHOSTSCRIPT_ARGV[BK_IN_FILE_IDX], BK_TEMPFILE_TEMPLATE_LENGTH, BK_IN_FILE_FMT, ps_name);

    int exit_code;
    if (!gs_initialised && ghostscript_instance(&gs_instance, NULL, BK_GHOSTSCRIPT_ARGC, argv) < SUCCESS) {
        fprintf(stderr, "Fatal error: Creating Ghostscript instance failed\n");
        exit(-1);
    } else {
        if (gsapi_run_string(gs_instance, "erasepage", BK_GS_USER_ERRORS, &exit_code) < SUCCESS) {
            fprintf(stderr, "ERROR: Ghostscript interpretation raised a fatal error, exiting Ghostscript.\n");
            ghostscript_exit(gs_instance);
        } else if (exit_code < SUCCESS) {
            fprintf(stderr, "ERROR: Ghostscript interpretation failed.\n");
        }
    }


    if (gsapi_run_string(gs_instance, ps_code, BK_GS_USER_ERRORS, &exit_code) < SUCCESS) {
        fprintf(stderr, "ERROR: Ghostscript interpretation raised a fatal error, exiting Ghostscript.\n");
        ghostscript_exit(gs_instance);
    } else if (exit_code < SUCCESS) {
        fprintf(stderr, "ERROR: Ghostscript interpretation failed.\n");
    }

    // if (EOF == fputs(dest, file)) {
    //     fprintf(stderr, "FATAL: Could not write to file, terminating...\n");
    //     exit(1);
    // }
    // fclose(file);
    free(ps_code);

    for (int i = 0; i < num_barcodes; i++) {
        free(barcode_structs[i]);
    }
    free(barcode_structs);

    return png_name;
    */
