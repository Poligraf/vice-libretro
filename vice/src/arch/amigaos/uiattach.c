/*
 * uiattach.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"
#include "private.h"

#include <stdio.h>

#include "attach.h"
#include "autostart.h"
#include "lib.h"
#include "tape.h"
#include "ui.h"
#include "uiattach.h"
#include "uilib.h"
#include "uires.h"

#define SuspendFullscreenModeKeep(...) /* FIXME */
#define ResumeFullscreenModeKeep(...) /* FIXME */

static void uiattach_disk_dialog(video_canvas_t *canvas, int idm)
{
    char *name;
    char *resource;
    int unit = 8;
    int autostart_index = -1;

    SuspendFullscreenModeKeep(hwnd);
    switch (idm) {
      case IDM_ATTACH_8:
        unit = 8;
        break;
      case IDM_ATTACH_9:
        unit = 9;
        break;
      case IDM_ATTACH_10:
        unit = 10;
        break;
      case IDM_ATTACH_11:
        unit = 11;
        break;
    }
    resource = lib_msprintf("AttachDevice%dReadonly", unit);
    if ((name = uilib_select_file_autostart("Attach disk image",
        UILIB_FILTER_DISK | UILIB_FILTER_ZIP | UILIB_FILTER_ALL,
        UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_DISK,
        &autostart_index, resource)) != NULL) {

        if (autostart_index >= 0) {
            if (autostart_autodetect(name, NULL, autostart_index,
                AUTOSTART_MODE_RUN) < 0)
                ui_error("Cannot autostart specified file.");
        } else {
            if (file_system_attach_disk(unit, name) < 0)
                ui_error("Cannot attach specified file");
        }
        lib_free(name);
    }
    ResumeFullscreenModeKeep(hwnd);
    lib_free(resource);
}

static void uiattach_tape_dialog(video_canvas_t *canvas)
{
    char *name;
    int autostart_index = -1;

    SuspendFullscreenModeKeep(hwnd);
    if ((name = uilib_select_file_autostart("Attach tape image",
        UILIB_FILTER_TAPE | UILIB_FILTER_ZIP | UILIB_FILTER_ALL,
        UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_TAPE,
        &autostart_index, NULL)) != NULL) {

        if (autostart_index >= 0) {
            if (autostart_autodetect(name, NULL, autostart_index,
                AUTOSTART_MODE_RUN) < 0)
                ui_error("Cannot autostart specified file.");
        } else {
            if (tape_image_attach(1, name) < 0)
                ui_error("Cannot attach specified file");
        }
        lib_free(name);
    }
    ResumeFullscreenModeKeep(hwnd);
}

static void uiattach_autostart_dialog(video_canvas_t *canvas)
{
    char *name;
    int autostart_index = 0;

    SuspendFullscreenModeKeep(hwnd);
    if ((name = uilib_select_file_autostart("Autostart disk/tape image",
        UILIB_FILTER_DISK | UILIB_FILTER_TAPE | UILIB_FILTER_ZIP
        | UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_LOAD,
        UILIB_SELECTOR_STYLE_DISK_AND_TAPE, &autostart_index, NULL)) != NULL) {

        if (autostart_autodetect(name, NULL, autostart_index,
            AUTOSTART_MODE_RUN) < 0)
            ui_error("Cannot autostart specified file.");
        lib_free(name);
    }
    ResumeFullscreenModeKeep(hwnd);
}

void uiattach_command(video_canvas_t *canvas, int idm)
{
    switch (idm) {
      case IDM_ATTACH_8:
      case IDM_ATTACH_9:
      case IDM_ATTACH_10:
      case IDM_ATTACH_11:
        uiattach_disk_dialog(canvas, idm);
        break;
      case IDM_DETACH_8:
        file_system_detach_disk(8);
        break;
      case IDM_DETACH_9:
        file_system_detach_disk(9);
        break;
      case IDM_DETACH_10:
        file_system_detach_disk(10);
        break;
      case IDM_DETACH_11:
        file_system_detach_disk(11);
        break;
      case IDM_DETACH_ALL:
        file_system_detach_disk(8);
        file_system_detach_disk(9);
        file_system_detach_disk(10);
        file_system_detach_disk(11);
        break;
      case IDM_ATTACH_TAPE:
        uiattach_tape_dialog(canvas);
        break;
      case IDM_DETACH_TAPE:
        tape_image_detach(1);
        break;
      case IDM_AUTOSTART:
        uiattach_autostart_dialog(canvas);
        break;
    }
}

