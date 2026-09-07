#include "miyabi.h"

void module_init(void) {
    printf("Hello, Miyabi!", 0xFFFFFF);
}

void module_cleanup(void) {
    printf("Goodbye, Miyabi!", 0xFFFFFF);
}
