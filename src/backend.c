/* Copyright © 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file backend.c
 *      @brief UI 'backend' function implementations relating to barcode generation
 *      @author Elijah Schutz
 *      @date 21/4/19
 */

#include "backend.h"

#include "barcode.h"
#include "error.h"
#include "gtk/gtk.h"

#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "util.h"

#include <errno.h>
#else
#include <unistd.h>
#endif

static FILE * bk_tempfile;
static char * bk_tempfile_path;

int bk_init(void) {

    jmp_buf env;
    int     status = SUCCESS;

    if (!setjmp(env)) {
        bk_tempfile_path = calloc(1, BK_TEMPFILE_TEMPLATE_SIZE);
        VERIFY_NULL_BC(bk_tempfile_path, BK_TEMPFILE_TEMPLATE_SIZE);

#ifdef _WIN32
        if (SUCCESS == (status = tmpnam_s(bk_tempfile_path, BK_TEMPFILE_TEMPLATE_SIZE))) {
            bk_tempfile = fopen(bk_tempfile_path, "w");
            if (NULL == bk_tempfile) {
                status = ERR_TEMPORARY_FILE_CREATION_FAILED;
                longjmp(env, status);
            }
        } else {
            status = ERR_TEMPORARY_FILE_CREATION_FAILED;
            longjmp(env, status);
        }

#else
        // -1 to account for null terminator
        strncpy((char *) bk_tempfile_path, BK_TEMPFILE_TEMPLATE, BK_TEMPFILE_TEMPLATE_SIZE - 1);

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
    char **barcodes,
    int *quantities,
    int num_barcodes,
    PSProperties * props,
    Layout * layout,
    char ** ps_name_ptr
) {

    // clang-format on

    char *     postscript_dest;
    Code128 ** barcode_structs;
    // There are multiple failure points so we define some memory allocation flags
    // Safe error handling is provided by setjmp() and longjmp()
    bool postscript_allocated = false;
    int  num_allocated        = 0;

    jmp_buf env;
    int     status = SUCCESS;

    if (!setjmp(env)) {
        /* Calculate total number of barcodes that will be generated, which is used to calculate the
           size of the barcode buffer */
        int    total_barcodes       = 0;
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
        /* Loop over each barcode, then encode that barcode the corresponding number of times
           specified in quantities[]. This creates an array of internal barcode representations
           which we later use to generate PostScript */
        for (int barcode_no = 0; barcode_no < num_barcodes; barcode_no++) {
            for (int barcode_idx = 0; barcode_idx < quantities[barcode_no]; barcode_idx++) {
                status = c128_encode((uchar *) barcodes[barcode_no],
                                     strlen(barcodes[barcode_no]),
                                     &barcode_structs[num_allocated]);

                // __LINE__, __func__, barcode_no, barcode_idx);
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

        // wipe file completely
        if (NULL == freopen(bk_tempfile_path, "w", bk_tempfile)) {
            status = ERR_FILE_RESET_FAILED;
            longjmp(env, status);
        }

        // fprintf returns negative when an error occurs
        if (fprintf(bk_tempfile, "%s", postscript_dest) < 0) {
            status = ERR_FILE_WRITE_FAILED;
            longjmp(env, status);
        }

        // ensure everything is written to file since we're keeping it open
        if (fflush(bk_tempfile) != SUCCESS) {
            status = ERR_FLUSH;
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

/**
 *      @brief Print a file to a specific printer - abstraction from platform-specific APIs
 *      @param file File to print
 *      @param printer Destination printer
 *      @return SUCCESS, TODO: fill out other return values
 */
int bk_print(char * filename, char * printer) {

    int status = SUCCESS;

#ifdef _WIN32
    // 1 for null terminator
    size_t initial_print_cmd_length = sizeof(char) * (strlen(BK_WIN_PRINT_CMD) + strlen(printer) + 1);
    // 4 extra bytes: one for whitespace, two for quotation marks, one for null terminator
    size_t final_print_cmd_length = sizeof(char) * (strlen(filename) + 4);
    size_t print_cmd_length = initial_print_cmd_length + final_print_cmd_length;

    char * print_cmd = calloc(1, print_cmd_length);
    VERIFY_NULL_BC(print_cmd, print_cmd_length);

    // Firstly format the printer name into the printing command string
    // This works differently from the Unix version, see BK_WIN_PRINT_CMD (backend.h)
    // for details.
    if ((snprintf(print_cmd, initial_print_cmd_length, BK_WIN_PRINT_CMD, printer) > 0)) {
        char * filename_buffer = calloc(1, final_print_cmd_length);
        VERIFY_NULL_BC(filename_buffer, final_print_cmd_length);

        if (snprintf(filename_buffer, final_print_cmd_length, " \"%s\"", filename) > 0) {
            strncat(print_cmd, filename_buffer, final_print_cmd_length);
            
            fprintf(stderr, "Print command: %s\n", print_cmd);
            fprintf(stderr, "Print exec: %s\n", BK_WIN_PRINT_CMD);
            fprintf(stderr, "Printer: %s\n", printer);
            fprintf(stderr, "Filename: %s\n", filename);
            for (int i = 0; i < print_cmd_length; i++) {
                fprintf(stderr, "%d, ", (int) print_cmd[i]);
            }
            if (SUCCESS != system(print_cmd)) {
                fprintf(stderr, "ERROR: could not start printing subprocess\n");
                status = ERR_SYSTEM;
            }
        } else {
            fprintf(stderr, "ERROR: could not format filename for printing\n");
            status = ERR_INVALID_STRING;
        }
        free(filename_buffer);
    } else {
        fprintf(stderr, "ERROR: could not format printing command\n");
        status = ERR_INVALID_STRING;
    }
    free(print_cmd);
#else
    int pid = fork();
    if (pid == 0) {
        execlp("lp", "lp", "-d", printer, "-t", filename, filename, NULL);
    } else if (pid == -1) {
        fprintf(stderr, "ERROR: could not start printing subprocess\n");
        status = ERR_FORK;
    }
#endif

    return status;
}

/**
 *      @detail Uses @c wmic (?) on Windows and @c lpstat otherwise
 */
int bk_get_printers(char *** printers, int * num_printers) {
    int     status = SUCCESS;
    char *  printer_addrs[BK_MAX_PRINTERS];
    FILE *  output_stream;
    int     outputlen;
    jmp_buf env;
    char *  output = calloc(1, BK_EXEC_BUFSIZE);
    VERIFY_NULL_BC(output, BK_EXEC_BUFSIZE);


    /*
     * Utilising popen() to grab output from lpstat -e or wmic
     */

    // Get a list of printers
    if (!setjmp(env)) {
        if ((output_stream = popen(BK_GET_PRINTER_CMD, BK_POPEN_MODE)) != NULL) {
            if ((outputlen = fread(output, sizeof *output, BK_EXEC_BUFSIZE - 1, output_stream)) >
                0) {
                output[outputlen + 1] = '\0';
            } else {
                fprintf(stderr, "ERROR: could not read from stream\n");
                status = ERR_FREAD;
                longjmp(env, status);
            }
            pclose(output_stream);
        } else {
            fprintf(stderr, "ERROR: could not get printers\n");
            status = ERR_POPEN;
            longjmp(env, status);
        }
    } else {
        free(output);
        return status;
    }
    *num_printers = 0;

    // Iterate over newline-delimited printer names from lpstat -e
    char * sep_output;
    sep_output = output;


#ifdef _WIN32
    char * context;
    char * current_printer;
    if (NULL != strtok_s(sep_output, "\n", &context)) {
        // Run again to remove 'Name' heading on Windows wmic output
        while (NULL != (sep_output = strtok_s(NULL, "\r\n", &context))) {
            current_printer = strstrip(sep_output); // This must be freed later
            if (0 != strcmp(sep_output, "")) {
                // Output is modified in-place to point to the next line
                printer_addrs[*num_printers] = current_printer;
                (*num_printers)++;
            }
        }
    } else {
        fprintf(stderr, "ERROR: Invalid printer list string\n");
        status = ERR_PRINTER_LIST;
        
        free(output);
        
        return status;
    }
#else
    char * old_addr = sep_output;
    while (strcmp(sep_output, "") != 0 && strsep(&sep_output, "\n") != NULL) {
        printer_addrs[*num_printers] = old_addr;
        old_addr = sep_output; // Output is modified in-place to point to the next line
        (*num_printers)++;
    }
#endif


    if (*num_printers != 0) {
        int printers_size = sizeof **printers * *num_printers;
        
        *printers         = calloc(1, printers_size);
        VERIFY_NULL_BC(*printers, printers_size);

        for (int i = 0; i < *num_printers; i++) {
            int len        = strlen(printer_addrs[i]) + 1; // +1 for null terminator
            int str_size   = sizeof(*printers)[i] * len;

            (*printers)[i] = calloc(1, str_size);
            VERIFY_NULL_BC((*printers)[i], str_size);

            strncpy((*printers)[i], printer_addrs[i], len);
            
            free(printer_addrs[i]); // Allocated from current_printer above
        }
    } else {
        status = ERR_NO_PRINTERS;
    }

    free(output);


    return status;
}
