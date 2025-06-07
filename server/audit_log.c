#include "audit_log.h"
#include <stdio.h>
#include <time.h>

void audit_log(const char *user, const char *action, const char *target) {
    FILE *fp = fopen("audit.log", "a");
    if (!fp) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "[%s] USER=%s ACTION=%s TARGET=%s\n", timebuf, user ? user : "-", action ? action : "-", target ? target : "-");
    fclose(fp);
}

