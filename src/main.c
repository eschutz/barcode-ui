/* Copyright (C) 2019 Elijah Schutz */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/*
 *      @file main.c
 *      @brief Main executable code
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "backend.h"
#include "error.h"
#include "ui.h"
#include "util.h"

#include <stdio.h>

/**
 *      @brief Cleaning up APIs
 */
void cleanup(void);

int main(int argc, char ** argv) {
    fprintf(stderr, "Successfully reached %s:%d %s()\n", __FILE__, __LINE__, __func__);
    atexit(cleanup);

    int status = bk_init();

    if (ERR_TEMPORARY_FILE_CREATION_FAILED == status) {
        fprintf(stderr, "FATAL: Could not create temporary file, exiting.\n");
        exit(ERR_GENERIC);
    } else if (SUCCESS != status) {
        fprintf(stderr, "FATAL: Received invalid error code from bk_init()\n");
    }

    return g_application_run(G_APPLICATION(barcode_app_new()), argc, argv);
}

void cleanup(void) {
    int status = bk_exit();

    switch (status) {
        case SUCCESS:
            break;
        case ERR_FILE_CLOSE_FAILED:
            fprintf(stderr, "WARNING: Could not close file.\n");
            break;
        case ERR_FILE_REMOVE_FAILED:
            fprintf(stderr, "ERROR: Could not remove files; files not cleaned up.\n");
            break;
        default:
            fprintf(stderr, "ERROR: Received invalid error code from bk_exit()\n");
    }
}
