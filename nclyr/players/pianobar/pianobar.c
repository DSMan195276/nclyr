
#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/inotify.h>
#include <pthread.h>

#include "player.h"
#include "song.h"
#include "pianobar.h"
#include "debug.h"

const static char *piano_bar_nowplaying = "/home/dsman195276/.config/pianobar/nowplaying";

static struct song_info *pianobar_get_cur_song(void)
{
    struct song_info *sng;
    char buffer[500];
    char *cur, *start;
    const char *title = NULL;
    const char *artist = NULL;
    const char *album = NULL;
    int fd;

    fd = open(piano_bar_nowplaying, O_RDONLY | O_NONBLOCK);

    memset(buffer, 0, sizeof(buffer));
    read(fd, buffer, sizeof(buffer));

    for (cur = start = buffer; *cur; cur++) {
        if (*cur == '|') {
            *cur = '\0';
            if (!artist) {
                artist = start;
            } else if (!title) {
                title = start;
            }
            start = cur + 1;
        }
    }

    album = start;

    if (!artist || !title)
        return NULL;

    sng = malloc(sizeof(*sng));

    song_init(sng);

    sng->artist = strdup(artist);
    sng->title = strdup(title);
    sng->album = strdup(album);

    return sng;
}

static void *pianobar_inotify_thread(void *player)
{
    struct pianobar_player *pianobar = player;
    char buffer[2048];
    int inotify, exit_flag = 0;
    struct player_notification notif;
    struct song_info *song;
    struct pollfd fds[2];
    inotify = inotify_init1(O_NONBLOCK);

    inotify_add_watch(inotify, piano_bar_nowplaying, IN_MODIFY);

    memset(&notif, 0, sizeof(struct player_notification));
    notif.type = PLAYER_NO_SONG;
    write(pianobar->player.notify_fd, &notif, sizeof(notif));

    fds[0].fd = inotify;
    fds[0].events = POLLIN;

    fds[1].fd = pianobar->stop_pipe[0];
    fds[1].events = POLLIN;

    do {
        poll(fds, sizeof(fds)/sizeof(fds[0]), -1);

        if (fds[1].revents & POLLIN)
            exit_flag = 1;

        if (fds[0].revents & POLLIN) {
            while (read(inotify, buffer, sizeof(buffer)) != -1)
                ;

            song = pianobar_get_cur_song();

            if (!song)
                continue ;

            if (!song->artist || !song->title || !song->album) {
                song_free(song);
                continue ;
            }


            if (song_equal(song, pianobar->current_song)) {
                song_free(song);
                continue ;
            }

            DEBUG_PRINTF("New song: %s by %s on %s\n", song->title, song->artist, song->album);

            song_free(pianobar->current_song);
            pianobar->current_song = song;

            player_send_cur_song(&pianobar->player, song_copy(pianobar->current_song));
        }

    } while (!exit_flag);

    song_free(pianobar->current_song);

    close(inotify);
    return NULL;
}

static void pianobar_start_thread(struct player *player)
{
    struct pianobar_player *pianobar = container_of(player, struct pianobar_player, player);

    memset(&pianobar->notif_thread, 0, sizeof(pianobar->notif_thread));

    pipe(pianobar->stop_pipe);

    pthread_create(&pianobar->notif_thread, NULL, pianobar_inotify_thread, pianobar);

    return ;
}

static void pianobar_stop_thread(struct player *player)
{
    struct pianobar_player *pianobar = container_of(player, struct pianobar_player, player);
    int tmp = 2;

    write(pianobar->stop_pipe[1], &tmp, sizeof(tmp));
    pthread_join(pianobar->notif_thread, NULL);
}

struct pianobar_player pianobar_player = {
    .player = {
        .name = "pianobar",
        .start_thread = pianobar_start_thread,
        .stop_thread = pianobar_stop_thread,
        .player_windows = (const struct nclyr_win *[]) { NULL }
    },
    .stop_pipe = { 0, 0},
};

