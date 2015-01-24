
#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <pthread.h>
#include <mpd/client.h>

#include "player.h"
#include "song.h"
#include "mpd.h"
#include "debug.h"

static void mpd_song_to_song_info(struct mpd_song *msong, struct song_info *isong)
{
    isong->artist = strdup(mpd_song_get_tag(msong, MPD_TAG_ARTIST, 0));
    isong->title = strdup(mpd_song_get_tag(msong, MPD_TAG_TITLE, 0));
    isong->album = strdup(mpd_song_get_tag(msong, MPD_TAG_ALBUM, 0));
}

static void get_cur_song(struct mpd_connection *conn, struct song_info *song)
{
    struct mpd_song *msong;

    DEBUG_PRINTF("Asking mpd for current song\n");

    mpd_send_current_song(conn);

    msong = mpd_recv_song(conn);

    DEBUG_PRINTF("Recieved song\n");

    mpd_response_finish(conn);

    mpd_song_to_song_info(msong, song);
    mpd_song_free(msong);
}

static void get_and_send_cur_song(struct mpd_connection *conn, int notify_fd)
{
    struct song_info song;
    struct player_notification notif;

    get_cur_song(conn, &song);
    memset(&notif, 0, sizeof(notif));
    notif.type = PLAYER_SONG;
    notif.u.song = song;
    write(notify_fd, &notif, sizeof(notif));

    return ;
}

static void *mpd_thread(void *p)
{
    int stop_flag = 0;
    struct mpd_player *player = p;
    struct pollfd fds[3] = { { 0 } };
    enum mpd_idle idle;
    struct player_notification notif;

    DEBUG_PRINTF("Connecting to mpd...\n");
    player->conn = mpd_connection_new("127.0.0.1", 6600, 0);

    if (mpd_connection_get_error(player->conn) != MPD_ERROR_SUCCESS) {
        mpd_connection_free(player->conn);
        return NULL;
    }

    DEBUG_PRINTF("Connection sucessfull\n");

    memset(&notif, 0, sizeof(notif));
    notif.type = PLAYER_IS_UP;
    write(player->notify_fd, &notif, sizeof(notif));

    get_and_send_cur_song(player->conn, player->notify_fd);

    fds[0].fd = mpd_connection_get_fd(player->conn);
    fds[0].events = POLLIN;

    fds[1].fd = player->ctrl_fd[0];
    fds[1].events = POLLIN;

    fds[2].fd = player->stop_fd[0];
    fds[2].events = POLLIN;

    do {
        int handle_idle = 0;

        mpd_send_idle(player->conn);
        poll(fds, sizeof(fds)/sizeof(*fds), -1);

        if (fds[2].revents & POLLIN) {
            stop_flag = 1;
            continue;
        }

        if (fds[1].revents & POLLIN) {
            mpd_send_noidle(player->conn);
            handle_idle = 1;
        }

        if (fds[0].revents & POLLIN || handle_idle) {
            DEBUG_PRINTF("Recieved idle info from mpd!\n");
            idle = mpd_recv_idle(player->conn, false);
            if (idle & MPD_IDLE_PLAYER)
                get_and_send_cur_song(player->conn, player->notify_fd);
        }

        if (fds[1].revents & POLLIN) {
            struct player_ctrl_msg msg;
            DEBUG_PRINTF("Got control message\n");
            read(fds[1].fd, &msg, sizeof(msg));
            DEBUG_PRINTF("Read control message\n");

            switch (msg.type) {
            case PLAYER_CTRL_PLAY:
                mpd_run_play(player->conn);
                break;

            case PLAYER_CTRL_PAUSE:
                mpd_run_pause(player->conn, msg.u.pause);
                break;

            case PLAYER_CTRL_TOGGLE_PAUSE:
                mpd_run_toggle_pause(player->conn);
                break;

            case PLAYER_CTRL_NEXT:
                mpd_run_next(player->conn);
                break;

            case PLAYER_CTRL_PREV:
                mpd_run_previous(player->conn);
                break;

            case PLAYER_CTRL_SET_VOLUME:
                mpd_run_set_volume(player->conn, msg.u.volume);
                break;

            case PLAYER_CTRL_CHANGE_VOLUME:
                mpd_run_change_volume(player->conn, msg.u.vol_change);
                break;

            case PLAYER_CTRL_SEEK:
            case PLAYER_CTRL_SHUFFLE:
                break;

            }
        }

    } while (!stop_flag);

    mpd_connection_free(player->conn);

    return NULL;
}

static void mpd_start_thread(struct player *p, int pipfd)
{
    struct mpd_player *player = container_of(p, struct mpd_player, player);

    player->notify_fd = pipfd;
    pipe(player->stop_fd);
    pipe(player->ctrl_fd);

    pthread_create(&player->mpd_thread, NULL, mpd_thread, player);
    return ;
}

static void mpd_stop_thread(struct player *p)
{
    struct mpd_player *player = container_of(p, struct mpd_player, player);
    int tmp = 0;

    write(player->stop_fd[1], &tmp, sizeof(tmp));
    DEBUG_PRINTF("Joining mpd thread...\n");
    pthread_join(player->mpd_thread, NULL);
    DEBUG_PRINTF("mpd thread closed.\n");

    close(player->stop_fd[0]);
    close(player->stop_fd[1]);

    close(player->ctrl_fd[0]);
    close(player->ctrl_fd[1]);

    return ;
}

static void mpd_send_ctrl_msg (struct player *p, const struct player_ctrl_msg *msg)
{
    struct mpd_player *player = container_of(p, struct mpd_player, player);
    write(player->ctrl_fd[1], msg, sizeof(*msg));
}

struct mpd_player mpd_player = {
    .player = {
        .name = "mpd",
        .start_thread = mpd_start_thread,
        .stop_thread = mpd_stop_thread,
        .ctrls = {
            .ctrl = mpd_send_ctrl_msg,
            .has_pause = 1,
            .has_toggle_pause = 1,
            .has_play = 1,
            .has_next = 1,
            .has_prev = 1,
            .has_seek = 0,
            .has_shuffle = 0,
            .has_set_volume = 1,
            .has_change_volume = 1
        }
    }
};

