#ifndef GAME_FILE_H
#define GAME_FILE_H

#include <stdint.h>

enum {
    FILE_LOAD_SUCCESS = 1,
    FILE_LOAD_DOES_NOT_EXIST = 0,
    FILE_LOAD_INCOMPATIBLE_VERSION = -1,
    FILE_LOAD_WRONG_FILE_FORMAT = -2
};

/**
 * Start scenario by name
 * @param scenario_name Name of the scenario without extension
 * @return Boolean true on success, false on failure
 */
int game_file_start_scenario_by_name(const uint8_t *scenario_name);

/**
 * Start scenario from filename
 * @param scenario_file File to load
 * @return Boolean true on success, false on failure
 */
int game_file_start_scenario(const char *scenario_file);

/**
 * Load saved game
 * @param filename File to load
 * @return 1, 0 for "file does not exist", -1 for "incompatible version", -2 for "invalid file format"
 */
int game_file_load_saved_game(const char *filename);

/**
 * Write saved game to disk
 * @param filename File to save to
 * @return Boolean true on success, false on failure
 */
int game_file_write_saved_game(const char *filename);

/**
 * Delete saved game
 * @param filename File to delete
 * @return Boolean true on success, false on failure
 */
int game_file_delete_saved_game(const char *filename);

/**
 * Write starting save for the current campaign mission
 */
void game_file_write_mission_saved_game(void);

#endif // GAME_FILE_H
