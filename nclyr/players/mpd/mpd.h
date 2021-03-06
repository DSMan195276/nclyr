#ifndef PLAYERS_MPD_H
#define PLAYERS_MPD_H

#include <pthread.h>
#include <mpd/client.h>

#include "config.h"
#include "player.h"
#include "song.h"

struct mpd_player {
    struct player player;
    pthread_t mpd_thread;
    int stop_fd[2];
    int ctrl_fd[2];

    struct mpd_connection *conn;

    struct player_state_full state;

    /* seek_offset represents the amount of time that we had already played
     * before 'playing_started' */
    int    seek_offset;
    time_t playing_started;
};

extern struct mpd_player mpd_player;

enum {
    PLAYER_CONFIG_MPD_SERVER,
    PLAYER_CONFIG_MPD_PORT,
    PLAYER_CONFIG_MPD_TOTAL
};

extern struct config_item mpd_config[];

#endif
