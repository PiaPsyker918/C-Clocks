#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

unsigned int CLOCK_COLOR_HEX = 0xa200ff; // e.x. 0xFF5733

volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t window_changed = 1;

void handle_sigint(int sig) { keep_running = 0; }
void handle_winch(int sig) { window_changed = 1; }

void set_hex_color(unsigned int hex) {
    int r = (hex >> 16) & 0xFF;
    int g = (hex >> 8) & 0xFF;
    int b = hex & 0xFF;
    printf("\033[38;2;%d;%d;%dm", r, g, b);
}

const char *font[11][5] = {
    {"██████", "██  ██", "██  ██", "██  ██", "██████"}, // 0 █
    {"    ██", "    ██", "    ██", "    ██", "    ██"}, // 1
    {"██████", "    ██", "██████", "██    ", "██████"}, // 2
    {"██████", "    ██", "██████", "    ██", "██████"}, // 3
    {"██  ██", "██  ██", "██████", "    ██", "    ██"}, // 4
    {"██████", "██    ", "██████", "    ██", "██████"}, // 5
    {"██████", "██    ", "██████", "██  ██", "██████"}, // 6
    {"██████", "    ██", "    ██", "    ██", "    ██"}, // 7
    {"██████", "██  ██", "██████", "██  ██", "██████"}, // 8
    {"██████", "██  ██", "██████", "    ██", "██████"}, // 9
    {"      ", "  ██  ", "      ", "  ██  ", "      "}  // :
};

int main(int argc, char *argv[]) {
    if (argc > 1) {
        sscanf(argv[1], "%x", &CLOCK_COLOR_HEX);
    }

    signal(SIGINT, handle_sigint);
    signal(SIGWINCH, handle_winch);

    printf("\033[?25l\033[2J");

    while (keep_running) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        if (window_changed) {
            printf("\033[2J");
            window_changed = 0;
        }

        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        int digits[8] = {
            t->tm_hour / 10, t->tm_hour % 10, 10,
            t->tm_min / 10,  t->tm_min % 10,  10,
            t->tm_sec / 10,  t->tm_sec % 10
        };

        int clock_h = 5;
        int clock_w = (8 * 6) + 7;
        
        int start_y = (w.ws_row - clock_h) / 2;
        int start_x = (w.ws_col - clock_w) / 2;

        set_hex_color(CLOCK_COLOR_HEX);

        for (int row = 0; row < clock_h; row++) {
            printf("\033[%d;%dH", start_y + row, start_x);
            for (int i = 0; i < 8; i++) {
                printf("%s ", font[digits[i]][row]);
            }
        }

        printf("\033[0m");
        fflush(stdout);
        usleep(200000);
    }

    printf("\033[?25h\033[2J\033[1;1HProgramm ended.\n");
    return 0;
}
