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

void read_boot_cmdline(void) {
    if (cmdline_request.response== NULL) {
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
}
