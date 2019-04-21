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
#include <string.h>
#include <unistd.h>

// const char *BK_GHOSTSCRIPT_ARGV[] = { "gs", "-dSAFER", "-dBATCH", "-dNOPAUSE", "-sDEVICE=png16m",
// "-dGraphicsAlphaBits=4" };

/**
 *      @detail bk_generate_png() generates a temporary PNG file from a list of barcodes and
 *              property structures and returns the image path.
 *      @param barcodes A list of barcode strings to be encoded
 *      @param quantities A list of barcode quantities (@c barcode[n] is drawn @c quantities[n]
 *                        times)
 *      @param num_barcodes The length of @c barcodes
 *      @param props The PostScript properties to be used when generating the PostScript and image
 *      @param layout The arrangement of rows and columns used to lay out the barcodes
 *      @return The file path to a generated PNG image
 */

// clang-format off
char* bk_generate_png(
    char barcodes[][BK_BARCODE_LENGTH],
    int quantities[],
    int num_barcodes,
    PSProperties * props,
    Layout * layout
) {
// clang-format off
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
    return "";
}
