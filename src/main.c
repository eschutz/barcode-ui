/* Copyright © 2019 Elijah Schutz */

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
#include <stdlib.h>

/**
 *      @brief Cleaning up APIs
 */
void cleanup(void);

void startup_msg(void);

void license_msg(void);

void help_msg(void);

void quiet_msg(void);

const char CMD_LINE_OPTS[NUM_CMD_LINE_OPTS][CMD_LINE_OPTS_LENGTH] = { "--help", "--license", "--startup"};
const void (* CMD_LINE_OPTS_F[NUM_CMD_LINE_OPTS])(void) = { help_msg, license_msg, startup_msg};

int main(int argc, char ** argv) {
    // Process command line options
    if (!(argc > 1 && strcmp(argv[1], "--quiet") == 0)) {
        startup_msg();
        if (argc > 1) {
            for (int i = 0; i < NUM_CMD_LINE_OPTS; i++) {
                if (strncmp(argv[1], CMD_LINE_OPTS[i], CMD_LINE_OPTS_LENGTH) == 0) {
                    (*CMD_LINE_OPTS_F[i])();
                    exit(EXIT_SUCCESS);
                }
            }
            fprintf(stderr, "Error: invalid command line option\n");
            exit(EXIT_FAILURE);
        }
    }
    
    atexit(cleanup);

    int status = bk_init();

    if (ERR_TEMPORARY_FILE_CREATION_FAILED == status) {
        fprintf(stderr, "FATAL: Could not create temporary file, exiting.\n");
        exit(EXIT_FAILURE);
    } else if (SUCCESS != status) {
        fprintf(stderr, "FATAL: Received invalid error code from bk_init()\n");
        exit(EXIT_FAILURE);
    }

    // Ignore all other command line options by setting passing argc as 1
    return g_application_run(G_APPLICATION(barcode_app_new()), 1, argv);
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

void startup_msg(void) {
    printf(
        "-- Barcode User Interface --\
       \nCopyright © 2019 Elijah Schutz\
       \nThis program is subject to the terms of the Mozilla Public License, v. 2.0.\
       \nPlease see LICENSE in the installation directory for a copy of the MPL.\
       \nThe source code for this program is available at\
       \nhttps://github.com/eschutz/barcode-ui\
       \n\
       \nSeveral third-party libraries are bundled with this program under various\
       \nlicenses. Please see LICENSE-THIRD-PARTY for copyright notices and license\
       \ncopies, or run this program from the command line with --license for a\
       \n summary.\
       \nRun this program with --help for usage information.\
       \n"
        );
}

void license_msg(void) {
    printf(
        "-- Third Party Licenses and Copyright Notices --\
       \nBelow is contained a summary of the third party software distributed with this\
       \nprogram, the applicable license, and any copyright notices.\
       \nPlease see LICENSE-THIRD-PARTY in the installation directory for full details\
       \nand license copies.\
       \n\
       \nATK is licensed under the GNU Lesser General Public License version 2.1.\
       \n(LGPL 2.1).\
       \nCairo is licensed under the LGPL 2.1.\
       \nGdkPixbuf is licensed under the LGPL 2.1.\
       \nGLib is licensed under the LGPL 2.1.\
       \nGTK is licensed under the LGPL 2.1.\
       \ngettext is licensed under the LGPL 2.1.\
       \n\
       \nlibiconv is licensed under the GNU Library General Public License version 2.\
       \n(LGPL 2).\
       \nPango is licensed under the LGPL 2.\
       \n\
       \nEpoxy is licensed under the MIT License.\
       \nCopyright © 2013-2014 Intel Corporation\
       \n\
       \nExpat is licensed under the MIT License.\
       \nCopyright (c) 1998-2000 Thai Open Source Software Center Ltd and Clark Cooper\
       \nCopyright (c) 2001-2017 Expat maintainers\
       \n\
       \nFontconfig is licensed under the MIT License.\
       \nCopyright © 2000,2001,2002,2003,2004,2006,2007 Keith Packard\
       \nCopyright © 2005 Patrick Lam\
       \nCopyright © 2009 Roozbeh Pournader\
       \nCopyright © 2008,2009 Red Hat, Inc.\
       \nCopyright © 2008 Danilo Šegan\
       \nCopyright © 2012 Google, Inc.\
       \n\
       \nHarfBuzz is licensed under the so-called \"Old MIT\" license.\
       \nCopyright © 2010,2011,2012,2013,2014,2015,2016,2017,2018,2019  Google, Inc.\
       \nCopyright © 2019  Facebook, Inc.\
       \nCopyright © 2012  Mozilla Foundation\
       \nCopyright © 2011  Codethink Limited\
       \nCopyright © 2008,2010  Nokia Corporation and/or its subsidiary(-ies)\
       \nCopyright © 2009  Keith Stribley\
       \nCopyright © 2009  Martin Hosken and SIL International\
       \nCopyright © 2007  Chris Wilson\
       \nCopyright © 2006  Behdad Esfahbod\
       \nCopyright © 2005  David Turner\
       \nCopyright © 2004,2007,2008,2009,2010  Red Hat, Inc.\
       \nCopyright © 1998-2004  David Turner and Werner Lemberg\
       \n\
       \n-- Other Copyright Notices --\
       \nPortions of this software are copyright © 2019 The FreeType\
       \n    Project (www.freetype.org).  All rights reserved.\
       \n"
        );
}

void help_msg(void) {
    printf(
        "Usage: barcode.exe [ --help | --license | --startup ]\
       \n    --help      Display this help dialogue and exit\
       \n    --license   Display third-party copyright and license notices and exit\
       \n    --startup   Display the startup message and exit\
       \n    --quiet     Do not display the startup message and run the program as\
       \n                normal\
       \n"
        );
}

void quiet_msg(void) {}
