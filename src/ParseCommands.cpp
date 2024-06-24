/***********************************
 * @file ParseCommands.cpp
 * @author Gfy63 (mrgoofy@gmx.net)
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * @copyright 2023
 **********************************/

#include "ParseCommands.h"

/**
 * --- PUBLIC FUNCTIONS ---
*/

//--- Constructor ---

ParseCommands::ParseCommands( command_t *c, size_t bufferSize, size_t argCnt )
{
    _cmds = c;
    _memOK = AllocateMemory( bufferSize, argCnt );
}

ParseCommands::ParseCommands( command_t *c, size_t bufferSize )
{
    // ParseCommands( c, bufferSize, _argcMax );
    _cmds = c;
    _memOK = AllocateMemory( bufferSize, _argcMax );
}

ParseCommands::ParseCommands( command_t *c )
{
    // ParseCommands( c, _cmdBufferSize, _argcMax );
    _cmds = c;
    _memOK = AllocateMemory( _cmdBufferSize, _argcMax );
}

// ParseCommands::ParseCommands() {}			// Empty constructor.

////////////////////////////////////

// ParseCommands::begin( command_t *c, size_t bufferSize, size_t argCnt )
////////////////////////////////////

bool ParseCommands::read( char data )
{
    if( !_memOK )       // Memory allocation problems.
    {
        _err = -1;
        return false;
    }

    int index = strlen(_cmdBuffer);         // Index to the end of the used _cmdBuffer.
    static bool maxLenReached = false;      // Max command len reached if true.

    // Add char to command buffer.
    _cmdBuffer[index]=data;
    index++;
    _cmdBuffer[index]= '\0';
    _err = 1;       // Clear error code.

    // EoL found. Command complete.
    char* eolFound = strstr( _cmdBuffer, _eolArry[_eol] );
    if( eolFound )
    {
        eolFound[0] = '\0';     // Cutoff EOL.

        if( strlen( _cmdBuffer ) == 0 )
        {
            // Empty line.
            _err = -2;
            return false;
        }

        if( !maxLenReached )
        {
            // Command complete to parse.
            strcpy( _lastCommand, _cmdBuffer );   // Copy of command.

            bool ret = parse();         // False if error.
            if( ret ) _err = 1;         // Clear error code.
            _cmdBuffer[0] = '\0';       // Clear input. Ready for next command.
            return ret;
        }
        else
        {
            // Ignore input while to long.
            _cmdBuffer[0] = '\0';       // Clear input. Ready for next command.
            maxLenReached = false;      // Ready for next input.

            _err = -4;
            return false;
        }
    } // EOL found
    
    // Code must be behind EOL detection.
    if(index>=_cmdBufferSize)
    {
        // Max len detected.
        maxLenReached = true;

        _err = -3;
        return false;
    }

    return true;

} // Read()

////////////////////////////////////

bool ParseCommands::doCommand( const char *c )
{
    _err = 1;                   // Clear error code.

    if( !_memOK )       // Memory allocation problems.
    {
        _err = -1;
        return false;
    }

    if( (int)strlen(c) > _cmdBufferSize )
    {   
        _err = -3;
        return false;
    }

    strcpy( _cmdBuffer, c);
    bool ret = parse();         // False if error.
    if( ret ) _err = 1;         // Clear error code.

    _cmdBuffer[0] = '\0';       // Clear input. Ready for next command.
    return ret;

} // doCommand()

////////////////////////////////////

char * ParseCommands::getLastCommand( void ) { return _lastCommand; }

////////////////////////////////////

void ParseCommands::setEOL( int eol ) { if( eol>=0 || eol<=EOLCNT) _eol = eol; }

////////////////////////////////////

int ParseCommands::getError( void ) { return _err; }

////////////////////////////////////

const __FlashStringHelper * ParseCommands::getErrorText( void )
{
    switch( _err )
    {
        case -1:
            return F("Memory allocation problem.");
        
        case -2:
            return F("Empty line.");

        case -3:
            return F("Too many char input.");

        case -4:
            return F("Input to long.");

        case -5:
            return F("Command not found.");

        case -6:
            return F("Too many arguments.");

    }

    return F("No error");

} // getErrorText()

/**
 * --- PRIVATE FUNCTION ---
*/

bool ParseCommands::parse( void )
{
    char *split;        // Pointer to split position.

    _cmd = NULL;        // Clear command.

    split = strtok( _cmdBuffer, " " );

    // First part is command.
    if( split != NULL )
    {
        _cmd = split;
        split = strtok( NULL, " " );
    }

    // Read parameter if are.
    _argc = 0;
    while( split != NULL )
    {
        _argv[_argc] = split;
        _argc++;

        if( _argc > _argcMax )
        {
            // To many parameter.
            _err = -6;
            return false;       
        }
        split = strtok( NULL, " " );        // Next split.
    }

    // Search for command in list.
    int index = 0;
    bool cmdFound = false;
    CallbackFunction cb = NULL;

    while( _cmds[index].cmd )
    {
        if( strcmp( _cmd, _cmds[index].cmd) == 0 )
        {
            // Command found.
            cb = _cmds[index].cb;   // Get Callback.
            cmdFound = true;
            break;
        }
        index++;
    }

    if( cmdFound && cb!=NULL ) 
        cb( _argc, _argv );         // Call function.
    else
    {
        _err = -5;
    }

    return cmdFound;

} // Parse()

////////////////////////////////////

bool ParseCommands::AllocateMemory( size_t bs, size_t argc )
{
    bool noErr = true;

    if( bs<=0 || argc<=0 )
        noErr = false;       // Wrong size.
    else
    {
        _cmdBuffer = (char *) malloc( (bs+1) * sizeof( char ) );
        _lastCommand = (char *) malloc( (bs+1) * sizeof( char ) );
        _argv = (char **) malloc( (argc+1) * sizeof( char* ) );

        if( _cmdBuffer==NULL || _lastCommand==NULL || _argv==NULL ) noErr = false;   // malloc faild.
    }

    if( noErr )
    {
        _cmdBufferSize = bs;
        _argcMax = argc;
        _cmdBuffer[0] = '\0';       // Clear input buffer.
    }

    return noErr;

} // AllocateMemory()

// End of 'ParseCommands.cpp'.