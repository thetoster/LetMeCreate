/*
 * On linux, GPIO's can be controlled via sysfs by reading/writing to some
 * specific files. These are not regular files and writing to them trigger
 * some operations.
 *
 * In /sys/class/gpio folder, two important files are present: export and
 * unexport. To make a GPIO accessible to the user, you must first export it.
 * This consists in writing its index to the export file. For instance, to
 * export GPIO 27, simply write string '27' to /sys/class/gpio/export.
 * Similarly, to release a GPIO, one needs to write string '27' to
 * /sys/class/gpio/unexport.
 *
 * Once a GPIO is exported, it can be controlled by reading/writing to files in
 * folder /sys/class/gpio/gpioN (where N is its index).
 * Hence, exporting GPIO 27 creates folder /sys/class/gpio/gpio27 and the
 * following files are present: active_low, direction, edge, value
 *
 * To set GPIO 27 as an input, write string 'in' to
 * /sys/class/gpio/gpio27/direction.
 *
 * Reading /sys/class/gpio/gpio27/value always return strings '0' or '1'. This
 * is used to find the current state of an input.
 *
 * To set GPIO 27 as an output, write string 'out' to
 * /sys/class/gpio/gpio27/direction. However, reading now
 * /sys/class/gpio/gpio27/value indicates if the output level is high or low.
 * If a GPIO is configured as an output, writing to /sys/class/gpio/gpioN/value
 * changes the level of the output.
 *
 */


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "letmecreate/core/gpio.h"
#include "letmecreate/core/common.h"

#define GPIO_DIR_BASE_PATH      "/sys/class/gpio/"
#define GPIO_PATH_FORMAT        "/sys/class/gpio/gpio%d/%s"


static bool check_pin(uint8_t pin)
{
    switch (pin) {
    case GPIO_14:
    case GPIO_21:
    case GPIO_22:
    case GPIO_23:
    case GPIO_24:
    case GPIO_25:
    case GPIO_27:
    case GPIO_31:
    case GPIO_73:
    case GPIO_74:
    case GPIO_75:
    case GPIO_88:
    case GPIO_89:
    case GPIO_72:
    case GPIO_80:
    case GPIO_81:
    case GPIO_82:
    case GPIO_83:
    case GPIO_84:
    case GPIO_85:
        return true;
    default:
        fprintf(stderr, "Invalid gpio pin.\n");
        return false;
    }
}

static bool create_gpio_path(char *path, uint8_t gpio_pin, const char *file_name)
{
    if (snprintf(path, MAX_STR_LENGTH, GPIO_PATH_FORMAT, gpio_pin, file_name) < 0) {
        fprintf(stderr, "gpio: Could not create path for accessing %s of gpio %d.\n", file_name, gpio_pin);
        return false;
    }

    return true;
}

static bool is_gpio_exported(uint8_t gpio_pin)
{
    DIR *dir = NULL;
    char path[MAX_STR_LENGTH];

    if (!create_gpio_path(path, gpio_pin, ""))
        return false;

    dir = opendir(path);
    if (!dir)
        return false;

    closedir(dir);

    return true;
}

static int write_str_gpio_file(uint8_t gpio_pin, const char *file_name, const char *value)
{
    char path[MAX_STR_LENGTH];

    if (!create_gpio_path(path, gpio_pin, file_name)) {
        fprintf(stderr, "gpio: Cannot create path to file %s.\n", file_name);
        return -1;
    }

    return write_str_file(path, value);
}

static int write_int_gpio_file(uint8_t gpio_pin, const char *file_name, uint32_t value)
{
    char str[MAX_STR_LENGTH];

    if (snprintf(str, MAX_STR_LENGTH, "%d", value) < 0) {
        fprintf(stderr, "gpio: Failed to convert integer %d to string.\n", value);
        return -1;
    }

    return write_str_gpio_file(gpio_pin, file_name, str);
}

static int read_str_gpio_file(uint8_t gpio_pin, const char *file_name, char *value, uint32_t max_str_length)
{
    char path[MAX_STR_LENGTH];

    if (!create_gpio_path(path, gpio_pin, file_name))
        return -1;

    return read_str_file(path, value, max_str_length);
}

static int read_int_gpio_file(uint8_t gpio_pin, const char *file_name, uint8_t *value)
{
    char path[MAX_STR_LENGTH];
    uint32_t tmp;

    if (!create_gpio_path(path, gpio_pin, file_name))
        return -1;

    if (read_int_file(path, &tmp) < 0)
        return -1;

    *value = tmp;
    return 0;
}

int gpio_init(uint8_t gpio_pin)
{
    if (!check_pin(gpio_pin))
        return -1;

    if (is_gpio_exported(gpio_pin))
        return 0;

    if (export_pin(GPIO_DIR_BASE_PATH, gpio_pin) < 0)
        return -1;

    return gpio_set_direction(gpio_pin, GPIO_INPUT);
}

int gpio_set_direction(uint8_t gpio_pin, uint8_t dir)
{
    char str[4];

    if (!check_pin(gpio_pin)) {
        fprintf(stderr, "Cannot set direction to invalid pin %d\n", gpio_pin);
        return -1;
    }

    if (dir == GPIO_OUTPUT) {
        strcpy(str, "out");
    } else if (dir == GPIO_INPUT) {
        strcpy(str, "in");
    } else {
        fprintf(stderr, "gpio: Cannot set gpio %d to invalid direction.\n", gpio_pin);
        return -1;
    }

    if (!is_gpio_exported(gpio_pin)) {
        fprintf(stderr, "gpio: Cannot set direction of uninitialised gpio %d\n", gpio_pin);
        return -1;
    }

    return write_str_gpio_file(gpio_pin, "direction", str);
}

int gpio_get_direction(uint8_t gpio_pin, uint8_t *dir)
{
    char value[MAX_STR_LENGTH];

    if (!check_pin(gpio_pin))
        return -1;

    if (dir == NULL) {
        fprintf(stderr, "gpio: Cannot store direction of pin %d to null variable.\n", gpio_pin);
        return -1;
    }

    if (!is_gpio_exported(gpio_pin))
        return 0;

    if (read_str_gpio_file(gpio_pin, "direction", value, MAX_STR_LENGTH) < 0)
        return -1;

    if (strncmp(value, "out", 3) == 0) {
        *dir = GPIO_OUTPUT;
    } else if (strncmp(value, "in", 2) == 0) {
        *dir = GPIO_INPUT;
    } else {
        fprintf(stderr, "gpio: Invalid direction read from gpio %d.\n", gpio_pin);
        return -1;
    }

    return 0;
}

int gpio_set_value(uint8_t gpio_pin, uint8_t value)
{
    uint8_t dir;

    if (!check_pin(gpio_pin))
        return -1;

    if (!is_gpio_exported(gpio_pin))
        return 0;

    if (gpio_get_direction(gpio_pin, &dir) < 0)
        return -1;

    if (dir == GPIO_INPUT) {
        fprintf(stderr, "gpio: Cannot set value to an input.\n");
        return -1;
    }

    return write_int_gpio_file(gpio_pin, "value", value == 0 ? 0 : 1);
}

int gpio_get_value(uint8_t gpio_pin, uint8_t *value)
{
    if (!check_pin(gpio_pin))
        return -1;

    if (value == NULL) {
        fprintf(stderr, "gpio: Cannot store value of pin %d to null variable.\n", gpio_pin);
        return -1;
    }

    if (!is_gpio_exported(gpio_pin))
        return 0;

    return read_int_gpio_file(gpio_pin, "value", value);
}

int gpio_release(uint8_t gpio_pin)
{
    if (!check_pin(gpio_pin))
        return -1;

    if (!is_gpio_exported(gpio_pin))
        return 0;

    return unexport_pin(GPIO_DIR_BASE_PATH, gpio_pin);
}

