/*
 * Inotify usage example which watches for the file modify and metadata
 * changes.
 *
 * Some editors only trigger IN_MODIFY when saving file but some editors
 * will delete original file and replace it with the swap file instead.
 * This is how vim works for example. In this situation IN_IGNORED event
 * is triggered instead. IN_IGNORED is triggered because watch is removed
 * by the kernel when the file was deleted by the editor. In this situation
 * program simply adds inotify watch again.
 */
#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>


const char* PATH = "../sample";
const int WATCH_MASK = IN_MODIFY | IN_DONT_FOLLOW | IN_ATTRIB;
const int MAX_BUF_SIZE = sizeof(struct inotify_event);
static int running = 1;

void sigint_handler(int signalId) {
  running = 0;
}

int main(int argc, char **argv) {
    const struct inotify_event *event;
    int fd;
    int wd;
    ssize_t len;
    char buf[MAX_BUF_SIZE] __attribute__ ((aligned(__alignof__(struct inotify_event))));

    signal(SIGINT, sigint_handler);
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }
    wd = inotify_add_watch(fd, PATH, WATCH_MASK);
    if (wd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    printf("watching sample file for changes\n");
    while (running == 1) {
        len = read(fd, buf, MAX_BUF_SIZE);
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (len > 0) {
            event = (const struct inotify_event *)buf;
            printf("mask: %d\n", event->mask);
            if (event->mask & IN_IGNORED) {
                printf("watch removed, add it again\n");
                // Watch was removed by the system because file was deleted.
                // This can happen with vim for example which replaces
                // original file with swap file.
                wd = inotify_add_watch(fd, PATH, WATCH_MASK);
                if (wd == -1) {
                    perror("inotify_add_watch");
                    exit(EXIT_FAILURE);
                }
            }
            if (event->mask & IN_MODIFY) {
                printf("file modified\n");
            }
            if (event->mask & IN_ATTRIB) {
                printf("metadata changed\n");
            }
        }
        sleep(1);
    };
    exit(EXIT_SUCCESS);
}
