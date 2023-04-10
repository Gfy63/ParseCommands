/**
 * Example using ParseCommands library.
*/
#include <Arduino.h>

#include <ParseCommands.h>

// Declare functions.
void CmdTest( int argc, char *argv[] );
void CmdTest2( int argc, char *argv[] );

void display_freeram();
int freeRam();


// List of commands with callback to functions.
struct ParseCommands::command_t commandList[] = {
	// command, callback function
	"test", CmdTest,
	"test2", CmdTest2,
	NULL, NULL				// END OF LIST (NEEDED)
};

/**
 * Instatiate an ParseCommands.
*/
// ParseCommands pCmd( commandList );
ParseCommands pCmd( commandList, 32 );
// ParseCommands pCmd( commandList, 48, 5 );

void setup()
{
  // put your setup code here, to run once:

	Serial.begin( 9600 );

    delay( 500 );
    Serial.println( "ParseCommands Exemple" );

    // Test string commands.
	pCmd.doCommand( "test 1 2 3" );

	char cmd[20];
	sprintf( cmd, "test %i %i done", 5, 9);
	pCmd.doCommand( cmd );
}

void loop()
{
 
	bool err = true;
	if( Serial.available() ) {err = pCmd.read( Serial.read() ); }

	if( !err)
	{	
		Serial.print( "Error code: ");
		Serial.println( pCmd.getError() );
		Serial.println(pCmd.getErrorText() );

		err = true;
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