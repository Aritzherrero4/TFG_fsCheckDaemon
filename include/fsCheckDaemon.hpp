#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>
#include <systemd/sd-bus.h>

#include "../include/utils.hpp"
#include "../include/mtree.hpp"


//Define sd-daemon logging prefixes

#define SD_EMERG   "<0>"  /* system is unusable */
#define SD_ALERT   "<1>"  /* action must be taken immediately */
#define SD_CRIT    "<2>"  /* critical conditions */
#define SD_ERR     "<3>"  /* error conditions */
#define SD_WARNING "<4>"  /* warning conditions */
#define SD_NOTICE  "<5>"  /* normal but significant condition */
#define SD_INFO    "<6>"  /* informational */
#define SD_DEBUG   "<7>"  /* debug-level messages */

static FILE *log_file;
static sd_bus_slot *slot = NULL;
static sd_bus *bus = NULL;
static Mtree *tree;

static int method_getUpdate(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
void file_added(fs::path path);
void file_deleted(fs::path path);
void file_updated(fs::path path);

/* Definition of the bus object and method*/
static const sd_bus_vtable fscheck_vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_METHOD("getUpdate", "is", "i", method_getUpdate, SD_BUS_VTABLE_UNPRIVILEGED | SD_BUS_VTABLE_METHOD_NO_REPLY),
        SD_BUS_VTABLE_END
};
