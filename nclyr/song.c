
#include "common.h"

#include <string.h>

#include "nstrdup.h"
#include "song.h"

void song_init(struct song_info *sng)
{
    memset(sng, 0, sizeof(*sng));
}

void song_clear(struct song_info *sng)
{
    if (!sng)
        return ;
    free(sng->title);
    free(sng->artist);
    free(sng->album);
}

void song_copy(struct song_info *dest, const struct song_info *src)
{
    dest->title = nstrdup(src->title);
    dest->artist = nstrdup(src->artist);
    dest->album = nstrdup(src->album);

    dest->duration = src->duration;
}

int song_equal(const struct song_info *src1, const struct song_info *src2)
{
    if (!src1 || !src2)
        return 0;
    if (!src1->title || !src2->title || !!strcmp(src1->title, src2->title))
        return 0;
    if (!src1->artist || !src2->artist || !!strcmp(src1->artist, src2->artist))
        return 0;
    if (!src1->album || !src2->album || !!strcmp(src1->album, src2->album))
        return 0;
    if (src1->duration != src2->duration)
        return 0;
    return 1;
}

