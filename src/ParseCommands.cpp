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

ParseCommands::ParseCommands( command_t *c, size_t bufferSize, size_t argCnt )
{
    _cmds = c;
    _memOK = AllocateMemory( bufferSize, argCnt );
}

ParseCommands::ParseCommands( command_t *c, size_t bufferSize )
{
    _cmds = c;
    _memOK = AllocateMemory( bufferSize, _argcMax );
}

ParseCommands::ParseCommands( command_t *c )
{
    _cmds = c;
    _memOK = AllocateMemory( _cmdBufferSize, _argcMax );
}

bool ParseCommands::read( char data )
{
    if( !_memOK )       // Memory allocation problems.
    {
        _err = -1;
        return false;
    }

    int index = strlen(_cmdBuffer);         // Index to the end of the used _cmdBuffer.
    static bool maxLenReached = false;      // Max command len reached if true.

    if(index>=_cmdBufferSize)
    {
        // Max len detected.
        maxLenReached = true;

        _err = -3;
        return false;
    }

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

        if( !maxLenReached )
        {
            // Command complete to parse.
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
    }
    return true;

} // Read()

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

void ParseCommands::setEOL( int eol ) { if( eol>=0 || eol<=EOLCNT) _eol = eol; }

int ParseCommands::getError( void ) { return _err; }

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

bool ParseCommands::AllocateMemory( size_t bs, size_t argc )
{
    bool noErr = true;

    if( bs<=0 || argc<=0 ) noErr = false;       // Wrong size.
    else
    {
        _cmdBuffer = (char *) malloc( (bs+1) * sizeof(char) );
        _argv = (char **) malloc( (argc+1) * sizeof( char* ) );

        if( _cmdBuffer==NULL || _argv==NULL ) noErr = false;;   // malloc faild.
    }

    if( noErr )
    {
        _cmdBufferSize = bs;
        _argcMax = argc;
        _cmdBuffer[0] = '\0';       // Clear input buffer.
    }

    return noErr;

} // AllocateMemory()