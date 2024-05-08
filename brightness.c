#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_BRIGHTNESS_FILE "/sys/class/backlight/intel_backlight/max_brightness"
#define BRIGHTNESS_FILE "/sys/class/backlight/intel_backlight/brightness"
#define KBD_MAX_BRIGHTNESS_FILE "/sys/class/leds/tpacpi::kbd_backlight/max_brightness"
#define KBD_BRIGHTNESS_FILE "/sys/class/leds/tpacpi::kbd_backlight/brightness"

/// @brief given a file like /sys/class/backlight/intel_backlight/max_brightness returns the integer value
/// @param fname the file name
/// @return the value read from the file or -1 if the file could not be opened or read
int read_int_from_file(const char *fname)
{
    FILE *file;
    char buffer[100];
    int number;

    file = fopen(fname, "r");
    if (file == NULL)
    {
        return -1;
    }

    if (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        number = atoi(buffer);
    }
    else
    {
        return -1;
    }

    fclose(file);
    return number;
}

void write_int_to_file(const char *fname, int value)
{
    FILE *file;

    file = fopen(fname, "w");
    if (file == NULL)
    {
        return;
    }

    fprintf(file, "%d", value);
    fclose(file);
}

int backlight_to_step(int backlight, int min, int max, int steps)
{
    double xmin = log10(min);
    double xmax = log10(max);

    return round(log10(backlight) / (xmax - xmin) * steps);
}

int step_to_backlight(int step, int min, int max, int steps)
{
    double xmin = log10(min);
    double xmax = log10(max);
    double x = ((double)step / (double)steps) * (xmax - xmin);
    double x10 = pow(10, x);
    double backlight;

    if (x10 > max)
    {
        backlight = max;
    }
    else if (x10 < min)
    {
        backlight = min;
    }
    else
    {
        backlight = x10;
    }

    return round(backlight);
}

void change_backlight(int d)
{
    int min = 2;
    int max = read_int_from_file(MAX_BRIGHTNESS_FILE);
    int steps = 20;
    int current_backlight = read_int_from_file(BRIGHTNESS_FILE);
    int current_steps = backlight_to_step(current_backlight, min, max, steps);
    int next_steps = current_steps + d;
    int next_backlight = step_to_backlight(next_steps, min, max, steps);
    write_int_to_file(BRIGHTNESS_FILE, next_backlight);
}

void change_kbd_backlight()
{
    int min = 0;
    int max = read_int_from_file(KBD_MAX_BRIGHTNESS_FILE);

    int current_backlight = read_int_from_file(KBD_BRIGHTNESS_FILE);

    if (current_backlight == max)
    {
        write_int_to_file(KBD_BRIGHTNESS_FILE, min);
    }
    else
    {
        write_int_to_file(KBD_BRIGHTNESS_FILE, current_backlight + 1);
    }
}

int main(int argc, char **argv)
{
    return 0;
}
