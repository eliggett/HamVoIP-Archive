/*
    ltdl.c - ltdl emulation for android
    Copyright (C) 2012 Ladislav Vaiz <ok1zia@nagano.cz>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

*/

#include <config.h>
#include <ltdl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#ifdef ANDROID
#include <android/log.h>
#endif

#define APREFIX "lib"
#define ASUFFIX ".so"
#define AMAXSTR 1024
#define ASONAME "libhamlib.so"

// path to application's libraries with trailing slash
char *libpath = NULL;

char *getlibpath(void){
    char s[AMAXSTR];
    FILE *f;

    if (libpath != NULL) return libpath;

    f = fopen("/proc/self/maps", "rt");
    if (!f) return "./";

    while (fgets(s, AMAXSTR - 1, f)){
        char *c;

        s[AMAXSTR - 1] = '\0';
        c = strstr(s, ASONAME);
        if (!c) continue;

        // s is like "4a8a5000-4a8a6000 r--p 00018000 1f:01 743        /data/data/cz.nagano.tucnak/lib/libhamlib.so\n"
        *c = '\0';
        c = strchr(s, '/');
        if (!c) continue;
        libpath = malloc(strlen(c) + 1);
        strcpy(libpath, c);
        break;
    }
    fclose(f);
    return libpath;
}

int lt_dlinit(void){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlinit");
    return 0;
}

// not called from hamlib
int lt_dlexit(void){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlexit");

    if (libpath != NULL){
        free(libpath);
        libpath = NULL;
    }
    return 0;
}

int lt_dladdsearchdir(const char *search_dir){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dladdsearchdir");
    return 0;
}

lt_dlhandle adlopen(const char *filename){
    char *c;
    lt_dlhandle *ret;

//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "adlopen('%s')", filename);
    getlibpath();
    if (libpath == NULL || filename == NULL) return NULL;

    c = malloc(strlen(libpath) + strlen(APREFIX) + strlen(filename) + strlen(ASUFFIX) + 1);
    strcpy(c, libpath);
    strcat(c, APREFIX);
    strcat(c, filename);
    strcat(c, ASUFFIX);

    ret = dlopen(c, 0);
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "adlopen('%s')=%p", c, ret);
    free(c);
    return ret;
}

lt_dlhandle lt_dlopen(const char *filename){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlopen(%s)", filename);
    return adlopen(filename);
}

lt_dlhandle lt_dlopenext(const char *filename){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlopenext(%s)", filename);
    return adlopen(filename);
}

int lt_dlclose(lt_dlhandle handle){
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlclose");
    return dlclose(handle);
}

void *lt_dlsym(lt_dlhandle handle, const char *name){
    void *ret = dlsym(handle, name);
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlsym(%s)=%p", name, ret);
    return ret;
}

const char *lt_dlerror(void){
    const char *ret = dlerror();
//    __android_log_print(ANDROID_LOG_DEBUG, PACKAGE_NAME, "lt_dlerror=%s", ret);
    return ret;
}
