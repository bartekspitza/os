#include <stdint.h>

/**
 * Base-10 str to int conversion
 */
int atoi(const char* str) {
    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

/**
 * The following is a direct copy of a ChatGPT implementation, I have NOT
 * written this myself
 */
static char digit36(unsigned int v) {
    return (v < 10u) ? (char)('0' + v) : (char)('a' + (v - 10u));
}
char* itoa(int value, unsigned int base, char *buf, size_t buf_size) {
    if (!buf || buf_size == 0) return NULL;
    if (base < 2u || base > 36u) return NULL;

    size_t i = 0;

    unsigned int u;
    int negative = 0;

    // Common convention: only show '-' for base 10.
    if (base == 10u && value < 0) {
        negative = 1;
        u = (unsigned int)(-(value + 1)) + 1u; // avoid INT_MIN overflow
    } else {
        u = (unsigned int)value;
    }

    do {
        if (i + 1 >= buf_size) return NULL;
        buf[i++] = digit36(u % base);
        u /= base;
    } while (u != 0u);

    if (negative) {
        if (i + 1 >= buf_size) return NULL;
        buf[i++] = '-';
    }

    if (i >= buf_size) return NULL;
    buf[i] = '\0';

    for (size_t a = 0, b = i - 1; a < b; ++a, --b) {
        char tmp = buf[a];
        buf[a] = buf[b];
        buf[b] = tmp;
    }

    return buf;
}