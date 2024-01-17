#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UNIX_HOST
#include "../picoc/picoc.h"
#define PICOC_STACK_SIZE (128000*4)

int main(void) {
    Picoc pc;
    PicocInitialize(&pc, PICOC_STACK_SIZE);

    PicocPlatformScanFile(&pc, "game.c");

    for (int i = 0; i < 50000; i++) {
        PicocParse(
            &pc,
            "game.c",
            "frame();", sizeof("frame();"),
            true,  /* run it! */
            true,  /*    do free Tokens afterwards */
            false, /* don't free Source string afterwards */
            false  /* not debugging */
        );
    }

    PicocCleanup(&pc);

    return 0;
}
