/***********************************
 * @file ParseCommands.cpp
 * @author Gfy63 (mrgoofy@gmx.net)
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * @copyright 2023-24
 **********************************/

#include "ParseCommands.h"

/**
 * --- PUBLIC FUNCTIONS ---
*/

//--- Constructor ---

ParseCommands::ParseCommands( pcmd_command_t *c, size_t bufferSize, size_t argCnt )
{
	begin( c, bufferSize, argCnt );
}

ParseCommands::ParseCommands( pcmd_command_t *c, size_t bufferSize )
{
	begin( c, bufferSize );
}

ParseCommands::ParseCommands( pcmd_command_t *c )
{
	begin( c );
}

ParseCommands::ParseCommands() {}			// Empty constructor.

////////////////////////////////////

void ParseCommands::begin( pcmd_command_t *c, size_t bufferSize, size_t argCnt )
{
	_cmds = c;
	_memOK = AllocateMemory( bufferSize, argCnt );
}

void ParseCommands::begin( pcmd_command_t *c, size_t bufferSize )
{
	begin( c, bufferSize, _argcMax );
}

void ParseCommands::begin( pcmd_command_t *c )
{
   begin( c, _cmdBufferSize, _argcMax );
}

////////////////////////////////////

bool ParseCommands::read( char data )
{
	_lastChar = data;

	DoEventCall( PCMD_INPUT_CHAR_EVT );

	if( !isMemoryOK() ) return false;

	int index = strlen(_cmdBuffer);         // Index to the end of the used _cmdBuffer.
	static bool maxLenReached = false;      // Max command len reached if true.

	// Add char to command buffer.
	_cmdBuffer[index] = data;
	index++;
	_cmdBuffer[index] = '\0';

	// EoL found. Command complete.
	char* eolFound = strstr( _cmdBuffer, _eolArry[_eol] );
	if( eolFound )
	{
		eolFound[0] = '\0';     // Cutoff EOL.

		if( strlen( _cmdBuffer ) == 0 )
		{
			// Empty line.
			_err = PCMD_EMPLY_LINE_ERR;
			DoEventCall( PCMD_ERROR_EVT );

			return false;
		}

		if( maxLenReached )
		{
			// Ignore input while to long.
			_cmdBuffer[0] = '\0';       // Clear input. Ready for next command.
			maxLenReached = false;      // Ready for next input.

			_err = PCMD_INPUT_TO_LONG_ERR;
			DoEventCall( PCMD_ERROR_EVT );

		   return false;
		}

		// Command complete to parse.
		strcpy( _lastCommand, _cmdBuffer );   // Copy of command.

		DoEventCall( PCMD_READ_COMMAND_EVT );

		bool ret = parse();         		// False if error.
		if( ret ) _err = PCMD_COMMAND_OK;	// Clear error code.
		_cmdBuffer[0] = '\0';       		// Clear input. Ready for next command.

		return ret;

	} // EOL found
	
	// Code must be behind EOL detection.
	if( index>=_cmdBufferSize )
	{
		// Max len detected.
		maxLenReached = true;

		_err = PCMD_TOO_MANY_CHAR_ERR;
		DoEventCall( PCMD_ERROR_EVT );

		return false;
	}

	return true;

} // Read()

////////////////////////////////////

bool ParseCommands::doCommand( const char *c )
{
	if( !isMemoryOK() ) return false;

	if( !isBufferLenOK(c) ) return false;

	strcpy( _cmdBuffer, c);
	strcpy( _lastCommand, c );
	
	DoEventCall( PCMD_DO_COMMAND_EVT );

	bool ret = parse();         		// False if error.
	if( ret ) _err = PCMD_COMMAND_OK;	// Clear error code.

	_cmdBuffer[0] = '\0';       		// Clear input. Ready for next command.

	return ret;

} // doCommand()

////////////////////////////////////

void ParseCommands::eventHandler( PCMD_EventCallbackFunction cb ) { _eventCB = cb; }

////////////////////////////////////


void ParseCommands::setEOL( int eol ) { if( eol>=0 || eol<=EOLCNT) _eol = eol; }

////////////////////////////////////

char ParseCommands::getLastCharRead( void ) { return _lastChar; }

////////////////////////////////////

char * ParseCommands::getLastCommand( void ) { return _lastCommand; }

////////////////////////////////////

int ParseCommands::getError( void ) { return _err; }

////////////////////////////////////

const __FlashStringHelper * ParseCommands::getErrorText( void )
{
	switch( _err )
	{
		case PCMD_MEM_ALLOCATION_ERR:
			return F("Memory allocation problem.");
		
		case PCMD_EMPLY_LINE_ERR:
			return F("Empty line.");

		case PCMD_TOO_MANY_CHAR_ERR:
			return F("Too many char input.");

		case PCMD_INPUT_TO_LONG_ERR:
			return F("Input to long.");

		case PCMD_CMD_NOT_FOUND_ERR:
			return F("Command not found.");

		case PCMD_TOO_MANY_ARGUMENTS_ERR:
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

	split = strtok_c( _cmdBuffer );

	if( split != NULL )
	{
		// First part is command.
		_cmd = split;
		split = strtok_c( NULL );	// Next split.
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
			_err = PCMD_TOO_MANY_ARGUMENTS_ERR;
			return false;       
		}
		split = strtok_c( NULL );        // Next split.
	}

	// Search for command in list.
	int index = 0;
	bool cmdFound = false;
	PCMD_CallbackFunction cb = NULL;

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
		_err = PCMD_CMD_NOT_FOUND_ERR;
		DoEventCall( PCMD_ERROR_EVT );
	}

	return cmdFound;

} // Parse()

////////////////////////////////////

char * ParseCommands::strtok_c( char * s )
{
	static char *input = NULL;		// Pointer in input.
	char *search, *tok=NULL;
	int offset=0;
	bool isQuote= false;
	bool found, eol;

	if( s != NULL )
		input = s;			// Restart with new text else use old.

    if( *input == '\0' )	// Empty input.
		return NULL;

	search = input;

	// Find start.
	found = false;
	while( !found )
	{
		switch( *search )
		{
			case ' ':		// Space
				input = search +1;		// Delete leading space.
				break;

			case '\"':		// Quote
				// Quote, start string.
				isQuote = true;
				input = search +1;		// Delete Quote.
				found = true;
				break;

			case '\0':      // EOL
				return NULL;
		        break;
		        
			default:
				// Start without quotes.
				found = true;
				break;
		}

		search++;
	}
    
    tok = input;        // Copy start pos for return.

	found = eol = false;
	offset = 0;
	
	// Find end.
	while( !found && !eol )
	{
		switch( *search )
		{
			case ' ':		// Space
				if( !isQuote )
					found = true;	// End of part.
				break;
			
			case '\"':		// Quote
				if( isQuote )
					found = true; // End of quoted string.
				break;

			case '\\':		// Escape char.
				// Start escape char.
				switch ( *(search+1) )
				{
					// Escape char?
					case '\\':
					case '\"':
						offset++;
						search++;
						break;
				}
				break;
				
			case '\0':      // EOL
		        eol = true;
		        break;
		}

		*(search-offset) = *search;		// Move char do to escape chars.
		search++;			// Next char.
	}

	*(search-offset-1) = '\0';

	if( eol ) search--;		// Still on EOL.

	input = search;			// For next iteration.

	return tok;

} // strtok_c()

////////////////////////////////////

void ParseCommands::DoEventCall( int event )
{  
	if( _eventCB != NULL ) _eventCB( event );

} // DoEventCallback()

////////////////////////////////////

bool ParseCommands::AllocateMemory( size_t bs, size_t argc )
{
	if( bs<=0 || argc<=0 ) return false;		// Wrong arguments.

	_cmdBuffer = (char *) malloc( (bs+1) * sizeof( char ) );
	_lastCommand = (char *) malloc( (bs+1) * sizeof( char ) );
	_argv = (char **) malloc( (argc+1) * sizeof( char* ) );

	if( _cmdBuffer==NULL || _lastCommand==NULL || _argv==NULL ) return false;   // malloc() faild.

	_cmdBufferSize = bs;
	_argcMax = argc;
	_cmdBuffer[0] = '\0';       // Clear input buffer.

	return true;

} // AllocateMemory()

////////////////////////////////////

bool ParseCommands::isMemoryOK()
{
	if( _memOK ) return true;

	// Memory allocation faild.
	_err = PCMD_MEM_ALLOCATION_ERR;
	DoEventCall( PCMD_ERROR_EVT );

	return false;

} // isMemoryOK()

////////////////////////////////////

bool ParseCommands::isBufferLenOK( const char *c )
{
	if( (int)strlen(c) <= _cmdBufferSize ) return true;

	// To many input char.
	_err = PCMD_TOO_MANY_CHAR_ERR;
	DoEventCall( PCMD_ERROR_EVT );

	return false;

} // isBufferLenOK()

// End of 'ParseCommands.cpp'.