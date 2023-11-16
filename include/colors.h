#ifndef COLORS_H
#define COLORS_H

#ifndef HTML
#define RED_ESCAPE_SEQUENCE "\033[1;31m"

#define GREEN_ESCAPE_SEQUENCE "\033[1;32m"

#define YELLOW_ESCAPE_SEQUENCE "\033[1;33m"

#define PURPLE_ESCAPE_SEQUENCE "\033[1;35m"

#define BLUE_ESCAPE_SEQUENCE "\033[1;94m"

#define RESET_ESCAPE_SEQUENCE "\033[0m"

#define WHITE_ESCAPE_SEQUENCE RESET_ESCAPE_SEQUENCE

#else
#define RED_ESCAPE_SEQUENCE "<font color=\"red\">"

#define GREEN_ESCAPE_SEQUENCE "<font color=\"green\">"

#define YELLOW_ESCAPE_SEQUENCE "<font color=\"yellow\">"

#define PURPLE_ESCAPE_SEQUENCE "<font color=\"purple\">"

#define BLUE_ESCAPE_SEQUENCE "<font color=\"blue\">"

#define WHITE_ESCAPE_SEQUENCE "<font color=\"white\">"

#define RESET_ESCAPE_SEQUENCE "</font>"
#endif

#define color_red(str) RED_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#define color_green(str) GREEN_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#define color_blue(str) BLUE_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#define color_yellow(str) YELLOW_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#define color_white(str) WHITE_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#define color_purple(str) PURPLE_ESCAPE_SEQUENCE str RESET_ESCAPE_SEQUENCE

#endif //COLORS_H