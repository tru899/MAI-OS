#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

int main() {
    char buf[4096];
    ssize_t n;
    size_t pos = 0;

    while ((n = read(STDIN_FILENO, buf + pos, sizeof(buf) - pos - 1)) > 0) {
        pos += n;
        buf[pos] = '\0';

        char *cur_line = buf;
        char *tmp_line;

        while ((tmp_line = strchr(cur_line, '\n'))) {
            *tmp_line = '\0';

            int result = 0;
            int div_seen = 0;
            int count = 0;
            int division_by_zero = 0;
            int invalid_input = 0;
            
            //parsing int
            char *p = cur_line;
            while (*p) {
                while (*p && isspace((unsigned char)*p)) {
                    p++;
                }
                if (!*p) {
                    break;
                }
                int is_neg = 1;
                if (*p == '-') {
                    is_neg = -1;
                    p++;
                }
                if (!isdigit((unsigned char)*p)) {
                    invalid_input = 1;
                    break;
                }

                int num = 0;
                int digit_found = 0;
                while (*p && isdigit((unsigned char)*p)) {
                    num = num * 10 + (*p - '0');
                    p++;
                    digit_found = 1;
                }
                if (!digit_found) {
                    while (*p && !isspace((unsigned char)*p)) p++;
                    continue;
                }
                num *= is_neg;
                count++;

                if (!div_seen) {
                    result = num;
                    div_seen = 1;
                } else {
                    if (num == 0) {
                        division_by_zero = 1;
                        break;
                    }
                    result /= num;
                }
                while (*p && !isspace((unsigned char)*p)) {
                    invalid_input = 1;
                    break;
                }
                if (invalid_input) {
                    break;
                }
            }

            if (invalid_input) {
                const char msg[] = "error: invalid input\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
            } else if (division_by_zero) {
                const char msg[] = "error: division by zero\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
            } else if (count < 3) {
                const char msg[] = "error: not enough arguments\n";
                write(STDERR_FILENO, msg, sizeof(msg) - 1);
            } else {
                char out[64];
                int len = snprintf(out, sizeof(out), "%d\n", result);
                write(STDOUT_FILENO, out, len);
            }

            cur_line = tmp_line + 1;
        }

        pos = strlen(cur_line);
        memmove(buf, cur_line, pos);
    }

    return 0;
}
