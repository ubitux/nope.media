#include <stdio.h>
#include <stdlib.h>

#include <nopemd.h>

int main(int ac, char **av)
{
    if (ac < 2) {
        fprintf(stderr, "Usage: %s <media> [<use_pkt_duration>]\n", av[0]);
        return -1;
    }

    const char *filename = av[1];
    const int use_pkt_duration = ac > 2 ? atoi(av[2]) : 0;

    struct nmd_ctx *ctx = nmd_create();
    if (!ctx)
        return -1;

    struct nmd_media *s = nmd_add_media(ctx, filename);
    if (!s)
        return -1;

    struct nmd_frame *f;

    nmd_set_option(s, "auto_hwaccel", 0);
    nmd_set_option(s, "use_pkt_duration", use_pkt_duration);
    nmd_seek(s, 12.7);
    nmd_seek(s, 21.0);
    nmd_seek(s, 5.3);
    nmd_start(s);
    nmd_start(s);
    nmd_seek(s, 15.3);
    nmd_stop(s);
    nmd_start(s);
    nmd_stop(s);
    nmd_start(s);
    nmd_seek(s, 7.2);
    nmd_start(s);
    nmd_stop(s);
    nmd_seek(s, 82.9);
    f = nmd_get_frame(s, 83.5);
    if (!f) {
        nmd_free(&ctx);
        return -1;
    }
    nmd_release_frame(f);
    nmd_stop(s);
    f = nmd_get_frame(s, 83.5);
    if (!f) {
        nmd_free(&ctx);
        return -1;
    }
    nmd_free(&ctx);
    nmd_release_frame(f);
    return 0;
}
