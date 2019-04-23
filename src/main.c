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
 *      @file main.c
 *      @brief Main executable code
 *      @author Elijah Schutz
 *      @date 3/3/19
 */

#include "error.h"
#include "ui.h"
#include "backend.h"
#include "util.h"
#include <stdio.h>

/**
 *      @brief Cleaning up APIs
 */
void cleanup(void) {
    int status = bk_exit();

    switch (status) {
        case SUCCESS:
            break;
        case ERR_CHDIR_FAILED:
            fprintf(stderr, "ERROR: Could not change into temporary directory; files not cleaned up.\n");
            break;
        case ERR_FTS_ERROR:
            fprintf(stderr, "WARNING: FTS raised an error on close.\n");
            break;
        default:
            fprintf(stderr, "ERROR: Received invalid error code from bk_exit()");
    }

    if (ghostscript_exit(gs_instance) < SUCCESS) {
        fprintf(stderr, "ERROR: Error on exiting Ghostscript!\n");
    }
}

int main(int argc, char **argv) {
    atexit(cleanup);

    // GhostScript error messages are such that failure is < 0 (SUCCESS)
    if (ghostscript_init() < SUCCESS) {
        fprintf(stderr, "FATAL: Could not initialise GhostScript, exiting.\n");
        exit(ERR_GENERIC);
    }

    int status = bk_init();

    if (ERR_TEMPORARY_DIR_CREATION_FAILED == status) {
        fprintf(stderr, "FATAL: Could not create temporary directory, exiting.\n");
        exit(ERR_GENERIC);
    } else if (SUCCESS != status) {
        fprintf(stderr, "FATAL: Received invalid error code from bk_init()\n");
    }

    return g_application_run(G_APPLICATION(barcode_app_new()), argc, argv);
}
