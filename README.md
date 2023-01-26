# ParseCommands

Evaluate commands entered over serial or as string and call a defined function.

* Use serial stream as input.
* Use string as input.
* Config commands in a struct.
* Config EOL of the command.
* Max input buffer size and argument count configurable.
* Error detection.

## INSTALLATION & DOWNLOAD
Install via the Arduino Library Manager, the PlatformIO Library Manager or download from [github](https://github.com/Gfy63/ParseCommands.git)

## BASIC USAGE

### INSTANTIATE

```cpp
#include <ParseCommands>

// Define the commands and hear callback functions
struct ParseCommands::command_t commandList[] = {
// command, callback function
    "test", CmdTest,
    "test2", CmdTest2,
    NULL, NULL              // END OF LIST (NEEDED)
};

ParseCommands pCmd( commandList );         // Constructor.

// Create all in the struct defined callback functions
void test( int argc, char *argv[] )
{
    Serial.println( "test()");
}
```

### SETUP

The commands and callback functions are defind in a struct.
The commands are case sensitive.

```cpp
// Define the commands and hear callback functions
struct ParseCommands::command_t commandList[] = {
// command, callback function
    "test", CmdTest,
    "test2", CmdTest2,
    NULL, NULL              // END OF LIST (NEEDED)
};
```

The last entry in the command list must by ```NULL, NULL```, this terminate the list.

### LOOP

```cpp
// Pass a serial input to the parser.
if( Serial.available() ) {err = pCmd.read( Serial.read() ); }
```
The CR finishes a input stream and start the parser. The LF is ignored.

## Constructors
Constructor get command list, input buffer size and max argument count.
```cpp
ParseCommands( command_t *c, size_t bufferSize, size_t argCnt );
```
Constructor get command list, input buffer size.
```cpp
ParseCommands( command_t *c, size_t bufferSize );
```
Constructor get command list.
```cpp
ParseCommands( command_t *c );
```
Default buffer size is 16 char. Default argCnt is 3.
The buffer size is only limited to the memory size in the device.
If the size exceed the device possibilities, you get an error, and no input is possible.

## Setup method
The command ends with the ```EOL```.
```setEOL( int eol );``` - set the EOL. Allowd are:
    CRLF (default)
    CR
    LF
    LFCR

## Loop method
This is the only method to be call from the loop.
```void read()``` read a char from any stream and complete the command. 
After the ```EOL``` the command is parsed and fires the appropriate callback function.

## Call by string
A command can also pass as string. (exp.: fired from a button push)
```cpp
// Pass a string command with 3 arguments.
pCmd.doCommand( "test 1 2 3" );
```

## Callback function

After the command is parsed, the callback function is called.

He has 2 arguments. (exp. ```void test( int argc, char *argv[]) ```)
    ```argc``` is the argument count. 0 for no arguments passed.
    ```argv``` is the list of the arguments as string.

## Errors

The ```read()``` or ```doCommand()``` return false if a error accoutred.

Use ```getError()``` to get the error code.

     1  No error.
    -1  Memory allocation problem.
    -2  Empty line.
    -3  Too many char input.
    -4  Input to long.
    -5  Command not found.
    -6  Too many arguments.
    