#ifndef __2DUNR_FILE_MANAGER_H__
#define __2DUNR_FILE_MANAGER_H__

#include <iostream>
#include <fstream>

typedef std::ofstream READ_FILE;
typedef std::ifstream WRITE_FILE;
typedef std::fstream RW_FILE;

// TODO: Resolution struct temporarily defined here, eventually move to better file 
typedef struct
{
    int x;
    int y;
} Resolution;

//-------------------------------------------------------------------------------------------
// CONFIG FILES MANAGEMENT
//-------------------------------------------------------------------------------------------

#define CONFIG_FILE_NAME "game_config.ini"

bool ConfigFileExists();                // Returns true if the config file exists
Resolution GetResolutionFromConfig();   // Gets the screen resolution from the config file

//-------------------------------------------------------------------------------------------
// SAVING AND LOADING
//-------------------------------------------------------------------------------------------

#define GAME_SAVE_SUCCESS 0
#define GAME_SAVE_OTHER_ERROR 255

#define GAME_LOAD_SUCCESS 0
#define GAME_LOAD_FILE_NOT_FOUND 1
#define GAME_LOAD_OTHER_ERROR 255
/*
int SaveGame(unsigned char);               // Saves current game to specified save file
int LoadGame(unsigned char);               // Loads a game from the specified file, returns true if loading succeeded, false otherwise
*/
#endif