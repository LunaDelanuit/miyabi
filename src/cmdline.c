#include "cmdline.h"
#include "drivers/fb.h"

__attribute__((used, section(".requests")))
volatile struct limine_executable_cmdline_request cmdline_request = {
    .id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
    .revision = 0
};

static bool check_flag(const char *cmdline, const char *flag) {
    if (!cmdline || !flag) return false;

    size_t flag_len = 0;
    while (flag[flag_len] != '\0') flag_len++;

    const char *p = cmdline;
    while (*p != '\0') {
        if (*p == flag[0]) {
            bool match = true;
            for (size_t i= 0; i < flag_len; i++) {
                if (p[i] != flag[i]) {
                    match = false;
                    break;
                }
            }

            if (match && (p[flag_len] == '\0' || p[flag_len] == ' ')) {
                return true;
            }
        }
        p++;
    }
    return false;
}

bool DEBUG = false;
static char boot_modules[MAX_BOOT_MODULES][MAX_BOOT_MODULE_PATH];
static size_t boot_modules_count = 0;

static int is_module_argument(const char *arg, size_t length) {
    const char prefix[] = "--MOD=";

    if (length <= sizeof(prefix) - 1) return 0;

    for (size_t i = 0; i < sizeof(prefix) - 1; i++) {
        if (arg[i] != prefix[i]) return 0;
    }

    return 1;
}

static void collect_boot_modules(const char *cmdline) {
    const char *arg = cmdline;

    while (*arg) {
        while (*arg == ' ') arg++;
        if (!*arg) break;

        const char *end = arg;
        while (*end && *end != ' ') end++;

        size_t length = (size_t)(end - arg);
        if (is_module_argument(arg, length) && boot_modules_count < MAX_BOOT_MODULES) {
            const char *path = arg + 6;
            size_t path_length = length - 6;

            if (path_length < MAX_BOOT_MODULE_PATH) {
                for (size_t i = 0; i < path_length; i++) {
                    boot_modules[boot_modules_count][i] = path[i];
                }
                boot_modules[boot_modules_count][path_length] = '\0';
                boot_modules_count++;
            }
        }

        arg = end;
    }
}

void read_boot_cmdline(void) {
    if (cmdline_request.response == NULL) {
        printf("Cmdline requests not supported in this Limine version - or incorrectly processed.\n", 0xFF0000);
        return;
    }

    const char *cmdline = cmdline_request.response->cmdline;
    if (cmdline == NULL) {
        return;
    }

    printf("Boot options: %s\n", 0xFFFFFF, cmdline);

    if (check_flag(cmdline, "--debug")) {
        DEBUG = true;
    }

    collect_boot_modules(cmdline);
}

size_t boot_module_count(void) {
    return boot_modules_count;
}

const char *boot_module_path(size_t index) {
    if (index >= boot_modules_count) return NULL;
    return boot_modules[index];
}
