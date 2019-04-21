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
#include "util.h"
#include <stdio.h>

int main(int argc, char **argv) {
    // GhostScript error messages are such that failure is < 0 (SUCCESS)
    if (ghostscript_init() < SUCCESS) {
        fprintf(stderr, "FATAL: Could not initialise GhostScript, exiting.");
        exit(ERR_GENERIC);
    }

    return g_application_run(G_APPLICATION(barcode_app_new()), argc, argv);
}
