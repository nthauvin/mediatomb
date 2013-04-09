/*MT*
    
    MediaTomb - http://www.mediatomb.cc/
    
    youtube_video_url.cc - this file is part of MediaTomb.
    
    Copyright (C) 2005 Gena Batyan <bgeradz@mediatomb.cc>,
                       Sergey 'Jin' Bostandzhyan <jin@mediatomb.cc>
    
    Copyright (C) 2006-2010 Gena Batyan <bgeradz@mediatomb.cc>,
                            Sergey 'Jin' Bostandzhyan <jin@mediatomb.cc>,
                            Leonhard Wimmer <leo@mediatomb.cc>
    
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.
    
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    version 2 along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
    
    $Id: youtube_video_url.cc 2094 2010-04-03 20:22:31Z jin_eld $
*/

/// \file youtube_video_url.cc
/// \brief Definitions of the Transcoding classes. 

#ifdef HAVE_CONFIG_H
    #include "autoconfig.h"
#endif

#ifdef YOUTUBE

#include <pthread.h>

#include "youtube_video_url.h"
#include "tools.h"
#include "url.h"

using namespace zmm;

#define YOUTUBE_URL_PARAMS_REGEXP   "var swfHTML.*\\;"
#define YOUTUBE_URL_LOCATION_REGEXP "\nLocation: (http://[^\n]+)\n"
#define YOUTUBE_URL_WATCH           "http://www.youtube.com/watch?v="
#define YOUTUBE_URL_GET             "http://www.youtube.com/get_video?" 
#define YOUTUBE_URL_PARAM_VIDEO_ID  "video_id"
#define YOUTUBE_URL_PARAM_T_REGEXP  ".*&t=([^&]+)&"
#define YOUTUBE_URL_PARAM_T         "t"
#define YOUTUBE_IS_HD_AVAILABLE_REGEXP  "IS_HD_AVAILABLE[^:]*: *([^,]*)"
YouTubeVideoURL::YouTubeVideoURL()
{
    curl_handle = curl_easy_init();
    if (!curl_handle)
        throw _Exception(_("failed to initialize curl!\n"));

    reVideoURLParams = Ref<RExp>(new RExp());
    reVideoURLParams->compile(_(YOUTUBE_URL_PARAMS_REGEXP));
    redirectLocation = Ref<RExp>(new RExp());
    redirectLocation->compile(_(YOUTUBE_URL_LOCATION_REGEXP));
    param_t = Ref<RExp>(new RExp());
    param_t->compile(_(YOUTUBE_URL_PARAM_T_REGEXP));

    HD = Ref<RExp>(new RExp());
    HD->compile(_(YOUTUBE_IS_HD_AVAILABLE_REGEXP));

    // this is a safeguard to ensure that this class is not called from
    // multiple threads - it is not allowed to use the same curl handle
    // from multiple threads
    pid = pthread_self();
}
    
YouTubeVideoURL::~YouTubeVideoURL()
{
    if (curl_handle)
        curl_easy_cleanup(curl_handle);
}

String YouTubeVideoURL::getVideoURL(String video_id, bool mp4, bool hd)
{
    long retcode; 
    String flv_location;
    String watch, command, url;
#ifdef TOMBDEBUG
    bool verbose = true;
#else
    bool verbose = false;
#endif
    FILE *fp;
    char command_char[1035];
    char final_url[1035];

    if (!string_ok(video_id))
        throw _Exception(_("No video ID specified!"));
    watch = _(YOUTUBE_URL_WATCH) + video_id;
    command = _("youtube-dl -g --max-quality 35 ") + watch;
    fp = popen(command.c_str(), "r");
    if (fp == NULL) {
      log_debug("Failed to run youtube-dl\n" );
      exit;
    }
    fgets(final_url, sizeof(final_url)-1, fp);
    url = String(final_url);
    return (url.substring(0, url.length()-1));
}

#endif//YOUTUBE
