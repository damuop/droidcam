#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

#include "common.h"
#include "settings.h"

static inline FILE *GetFile(const char* mode) {
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/.config/droidcam", getenv("HOME"));
    return fopen(buf, mode);
}

void LoadSettings(struct settings* settings) {
    char buf[512];
    int version = 0;
    FILE * fp = GetFile("r");

    // set defaults
    memset(settings, 0, sizeof(struct settings));
    settings->video = 1;
    settings->port = 4747;
    settings->connection = CB_RADIO_WIFI;

    if (!fp) {
        return;
    }

    if(fgets(buf, sizeof(buf), fp)){
        sscanf(buf, "v%d", &version);
    }

    if (version == 1) {
        if (fgets(buf, sizeof(buf), fp)){
            buf[strlen(buf)-1] = '\0';
            strncpy(settings->ip, buf, sizeof(settings->ip));
        }

        if (fgets(buf, sizeof(buf), fp)) {
            buf[strlen(buf)-1] = '\0';
            settings->port = atoi(buf);
        }
    }
    else if (version == 2 || version == 3) {
        if (fgets(buf, sizeof(buf), fp)){
            buf[strlen(buf)-1] = '\0';
            strncpy(settings->ip, buf, sizeof(settings->ip));
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->port);
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->audio);
        }
        if (version == 3) {
            if (fgets(buf, sizeof(buf), fp)) {
                sscanf(buf, "%d", &settings->video);
            }
        }
        if (fgets(buf, sizeof(buf), fp)) {
            sscanf(buf, "%d", &settings->connection);
        }
    }
    else if (version == 4) {
        int arg1, arg2;
        while (fgets(buf, sizeof(buf), fp)) {
            if (1 == sscanf(buf, "ip=%16s\n", settings->ip))    continue;
            if (1 == sscanf(buf, "port=%d\n", &settings->port)) continue;

            if (1 == sscanf(buf, "audio=%d\n", &settings->audio)) continue;
            if (1 == sscanf(buf, "video=%d\n", &settings->video)) continue;

            if (2 == sscanf(buf, "size=%dx%d\n", &arg1, &arg2)) {
                settings->v4l2_width = arg1;
                settings->v4l2_height = arg2;
                continue;
            }

            if (1 == sscanf(buf, "type=%d\n",&settings->connection)) continue;
        }
    }

    fclose(fp);
    dbgprint(
        "settings: ip=%s\n"
        "settings: port=%d\n"
        "settings: audio=%d\n"
        "settings: video=%d\n"
        "settings: size=%dx%d\n"
        "settings: connection=%d\n"
        ,
        settings->ip,
        settings->port,
        settings->audio,
        settings->video,
        settings->v4l2_width, settings->v4l2_height,
        settings->connection);
}

void SaveSettings(struct settings* settings) {
    int version = 4;
    FILE * fp = GetFile("w");
    if (!fp) return;

    fprintf(fp,
        "v%d\n"
        "ip=%s\n"
        "port=%d\n"
        "audio=%d\n"
        "video=%d\n"
        "size=%dx%d\n"
        "type=%d\n"
        ,
        version,
        settings->ip,
        settings->port,
        settings->audio,
        settings->video,
        settings->v4l2_width, settings->v4l2_height,
        settings->connection);
    fclose(fp);
}
