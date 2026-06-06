#include <string>
#include <format>
#include "filemanager.h"

//-------------------------------------------------------------------------------------------
// SAVING AND LOADING
//-------------------------------------------------------------------------------------------
/*
inline std::string GetSaveName(unsigned char index)
{
    return std::format("Save{}.sav", (short)index); // Cast to short so that it doesn't just put a character there...
}

int LoadGame(unsigned char index)
{
    std::string SaveName = GetSaveName(index);
    READ_FILE save(SaveName);
    
    if (!save.is_open())
    {
        // I suppose that, if a file isn't open after we try to open it,
        // then it doesn't exist?
        return GAME_LOAD_FILE_NOT_FOUND;
    }
    
    // TODO: Add save reading here
    
    save.close();
}

int SaveGame(unsigned char index)
{
    std::string SaveName = GetSaveName(index);
    WRITE_FILE save(SaveName);
    
    if (!save.is_open())
    {
        return GAME_SAVE_OTHER_ERROR;
    }
    
    // TODO: Add save logic here...
    
    save.close();
}*/