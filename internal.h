/*
 * This file is part of sxplayer.
 *
 * Copyright (c) 2015 Stupeflix
 *
 * sxplayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * sxplayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with sxplayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SXPLAYER_INTERNAL_H
#define SXPLAYER_INTERNAL_H

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>

#include "sxplayer.h"
#include "decoders.h"
#include "async.h"

#define ENABLE_INFO 0
#define ENABLE_DBG 0
#define ENABLE_TIMINGS 0

void do_log(void *log_ctx, int log_level, const char *fn, const char *fmt, ...);

#define DO_LOG(log_ctx, log_level, ...) do_log(log_ctx, log_level, __PRETTY_FUNCTION__, __VA_ARGS__)

#define INFO(log_ctx, ...)  DO_LOG(log_ctx, AV_LOG_INFO, __VA_ARGS__)

#if ENABLE_DBG
#define TRACE(log_ctx, ...) do { DO_LOG(log_ctx, AV_LOG_INFO, __VA_ARGS__); fflush(stdout); } while (0)
#else
/* Note: this could be replaced by a "while(0)" but it wouldn't test the
 * compilation of the printf format, so we use this more complex form. */
#define TRACE(log_ctx, ...) do { if (0) DO_LOG(log_ctx, AV_LOG_INFO, __VA_ARGS__); } while (0)
#endif

enum AVPixelFormat pix_fmts_sx2ff(enum sxplayer_pixel_format pix_fmt);
enum sxplayer_pixel_format pix_fmts_ff2sx(enum AVPixelFormat pix_fmt);
void set_thread_name(const char *name);

#define TIME2INT64(d) llrint((d) * av_q2d(av_inv_q(AV_TIME_BASE_Q)))
#define PTS2TIMESTR(t64) av_ts2timestr(t64, &AV_TIME_BASE_Q)

struct sxplayer_ctx {
    const AVClass *class;                   // necessary for the AVOption mechanism
    char *filename;                         // input filename
    char *logname;

    int context_configured;                 // set if options are pre-processed, file is opened, ...

    /* configurable options */
    int avselect;                           // select audio or video
    double skip;                            // see public header
    double trim_duration;                   // see public header
    double dist_time_seek_trigger;          // distance time triggering a seek
    int max_nb_frames;                      // maximum number of frames in the queue
    int max_nb_packets;                     // maximum number of packets in the queue
    char *filters;                          // user filter graph string
    int sw_pix_fmt;                         // sx pixel format to use for software decoding
    int autorotate;                         // switch for automatically rotate in software decoding
    int auto_hwaccel;                       // attempt to enable hardware acceleration
    int export_mvs;                         // export motion vectors into frame->mvs
    int pkt_skip_mod;                       // skip packet if module pkt_skip_mod (and not a key pkt)

    int64_t skip64;
    int64_t trim_duration64;
    int64_t dist_time_seek_trigger64;

    /* misc general fields */
    enum AVMediaType media_type;            // AVMEDIA_TYPE_{VIDEO,AUDIO} according to avselect
    const char *media_type_string;          // "audio" or "video" according to avselect

    /* ... */
    struct async_context *actx;
    struct async_reader *reader;
    struct async_decoder *adec;
    struct async_filterer *afilterer;
    AVFrame *cached_frame;
    int64_t pkt_count;

    int64_t last_pushed_frame_ts;           // ts value of the latest pushed frame (it acts as a UID)
    int64_t first_ts;

    /* fields specific to decoding thread */
    AVFormatContext *fmt_ctx;               // demuxing context
    struct decoder_ctx *dec_ctx;            // decoder context
    AVCodec *dec;
    AVStream *stream;                       // selected stream
    int stream_idx;                         // selected stream index
};

#endif
