#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_BRIGHTNESS_FILE "/sys/class/backlight/intel_backlight/max_brightness"
#define BRIGHTNESS_FILE "/sys/class/backlight/intel_backlight/brightness"
#define KBD_MAX_BRIGHTNESS_FILE "/sys/class/leds/tpacpi::kbd_backlight/max_brightness"
#define KBD_BRIGHTNESS_FILE "/sys/class/leds/tpacpi::kbd_backlight/brightness"
#define LED_MUTE_FILE "/sys/class/leds/tpacpi::mute/brightness"
#define LED_MIC_MUTE_FILE "/sys/class/leds/tpacpi::micmute/brightness"
#define LED_CAPSLOCK_FILE "/sys/class/leds/tpacpi::capslock/brightness"

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

/// @brief writes an integer value to a file
/// @param fname the file name
/// @param value the value to write
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

/// @brief function to increase backlight in logarithmic steps
/// this is useful because the human eye perceives light in a logarithmic way
/// this function converts the backlight value to a step in a logarithmic scale
/// @param backlight the current backlight value
/// @param min the minimum backlight value
/// @param max the maximum backlight value
/// @param steps the number of steps to use
/// @return the step in the logarithmic scale
int backlight_to_step(int backlight, int min, int max, int steps)
{
    double xmin = log10(min);
    double xmax = log10(max);

    return round(log10(backlight) / (xmax - xmin) * steps);
}

/// @brief function to convert a step in a logarithmic scale to a backlight value
/// this is useful because the human eye perceives light in a logarithmic way
/// this function converts a step in a logarithmic scale to a backlight value
/// @param step a step in the logarithmic scale
/// @param min the minimum backlight value
/// @param max the maximum backlight value
/// @param steps the number of steps to use
/// @return the backlight value which can be sent to the driver
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

/// @brief changes the backlight value by a given delta
/// the delta can be positive or negative
/// this function reads the current backlight value, converts it to a step in a logarithmic scale
/// then adds the delta to the step and converts it back to a backlight value
/// @param d the delta to add to the current backlight value
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

/// @brief the function changes an led light value
/// each call will increase the light value by a step
/// if the max value is reached, the brightness will be reset to zero
/// @param max_brightness_file the file containing the maximum brightness value
/// @param brightness_file the file containing the current brightness value
void change_led_light(const char *max_brightness_file, const char *brightness_file)
{
    int min = 0;
    int max = read_int_from_file(max_brightness_file);

    int current_backlight = read_int_from_file(brightness_file);

    if (current_backlight == max)
    {
        write_int_to_file(brightness_file, min);
    }
    else
    {
        write_int_to_file(brightness_file, max);
    }
}

void change_kbd_backlight()
{
    change_led_light(KBD_MAX_BRIGHTNESS_FILE, KBD_BRIGHTNESS_FILE);
}

void change_mute_led()
{
    change_led_light(LED_MUTE_FILE, LED_MUTE_FILE);
}

void change_mic_mute_led()
{
    change_led_light(LED_MIC_MUTE_FILE, LED_MIC_MUTE_FILE);
}

int main(int argc, char **argv)
{
    return 0;
}
