/**
 * Example using ParseCommands library.
*/
#include <Arduino.h>

#include <ParseCommands.h>

// Declare functions.
void CmdTest( int argc, char *argv[] );
void CmdTest2( int argc, char *argv[] );

void PCmd_EventCallback( int event );

void display_freeram();
int freeRam();

ParseCommands pCmd;

// List of commands with callback to functions.
struct pcmd_command_t commandList[] = {
	// command, callback function
	"test", CmdTest,
	"test2", CmdTest2,
	NULL, NULL				// END OF LIST (NEEDED)
};

void setup()
{
  // put your setup code here, to run once:

	Serial.begin( 9600 );

	/**
	 * Instatiate an ParseCommands.
	*/
	// pCmd.begin( commandList );
	pCmd.begin( commandList, 32 );
	// pCmd.begin( commandList, 48, 5 );

	pCmd.eventHandler( PCmd_EventCallback );

    delay( 500 );
    Serial.println( "ParseCommands Exemple" );

    // Test string commands.
	pCmd.doCommand( "test \"1 1\" 2 3" );

	char cmd[20];
	sprintf( cmd, "test %i %i done", 5, 9);
	pCmd.doCommand( cmd );
}

void loop()
{
 
	bool err = true;
	if( Serial.available() ) { err = pCmd.read( Serial.read() ); }

	if( !err)
	{	
		Serial.print( "Error code: ");
		Serial.println( pCmd.getError() );
		Serial.println(pCmd.getErrorText() );

		err = true;
	}
}

void PCmd_EventCallback( int event )
{
	switch( event )
	{

		case PCMD_INPUT_CHAR_EVT:
			Serial.print( pCmd.getLastCharRead() );		// Echo.
			break;

		case PCMD_READ_COMMAND_EVT:
			Serial.printf( "Read command: %s\n", pCmd.getLastCommand() );
			break;

		case PCMD_DO_COMMAND_EVT:
			Serial.printf( "Do command: %s\n", pCmd.getLastCommand() );
			break;

		case PCMD_ERROR_EVT:
			Serial.printf("ParseCommands - Error: %i %s", pCmd.getError(), pCmd.getErrorText() );
			break;
	}
}

/**
 * Callback for the command 'test'.
 */
 void CmdTest(int argc, char *argv[] )
{
	Serial.println( "CmdTest()");
	if( argc != 0 )
	{
		for( int i=0; i<argc; i++ )
		{
			Serial.print( "   Parameter " );
			Serial.print( i );
			Serial.print( " : ");
			Serial.println( argv[i] );
		}
	}

	if( pCmd.getError() != 1 )
	{
		Serial.print( "Error:" );
		Serial.println( pCmd.getError());
	}
}

/**
 * Callback for thr command 'test2'.
*/
void CmdTest2( int argc, char *argv[] )
{
	Serial.println( "CmdTest2()");
}