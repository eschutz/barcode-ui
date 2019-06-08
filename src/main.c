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
void cleanup(void);

int main(int argc, char **argv) {
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
