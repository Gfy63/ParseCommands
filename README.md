# ParseCommands

Evaluate commands entered over serial or as string and call a defined function.

* Use serial stream as input.
* Use string as input.
* Config commands in a struct.
* Config EOL of the command.
* Max input buffer size and argument count configurable.
* Event callback for debugging, echo, log and more.
* Error detection.

> [!CAUTION]
> Since version 1.5.0 the struct `ParseCommand::command_t` is change to `pcmd_command_t`

## DOWNLOAD
Download from [github](https://github.com/Gfy63/ParseCommands.git).

## BASIC USAGE

### INSTANTIATE

```cpp
#include <ParseCommands.h>

ParseCommands pCmd;         // Constructor.

// Define the commands and hear callback functions
struct pcmd_command_t commandList[] = {
// command, callback function
    "test", CmdTest,
    "test2", CmdTest2,
    NULL, NULL              // END OF LIST (NEEDED)
};

void setup()
{
    pCmd.begin( commandList );
}

// Create all in the struct defined callback functions
void CmdTest( int argc, char *argv[] )
{
    Serial.println( "test()");
}
```

### SETUP

The commands and callback functions are defind in a struct.
The commands are case sensitive.

```cpp
// Define the commands and hear callback functions
struct pcmd_command_t commandList[] = {
// command, callback function
    "test", CmdTest,
    "test2", CmdTest2,
    NULL, NULL              // END OF LIST (NEEDED)
};
```

The last entry in the command list must by `NULL, NULL`, this terminate the list.

### LOOP

```cpp
// Pass a serial input to the parser.
if( Serial.available() ) {pCmd.read( Serial.read() ); }
```
The CR finishes a input stream and start the parser. The LF is ignored.

## Constructors
`Constructor` or `begin()` get command list, optional input buffer size and optional max argument count.
```cpp
ParseCommands( pcmd_command_t *c, size_t bufferSize, size_t argCnt );
or
begin( pcmd_command_t *c, size_t bufferSize, size_t argCnt );
```
Constructor get command list, input buffer size.
```cpp
ParseCommands( pcmd_command_t *c, size_t bufferSize );
or
begin( pcmd_command_t *c, size_t bufferSize );
```
Constructor get command list.
```cpp
ParseCommands( pcmd_command_t *c );
or
begin( pcmd_command_t *c );
```

> Default `bufferSize` is 16 char. <br>
> Default `argCnt` is 3.

> [!NOTE]
> The buffer size is only limited to the memory size in the device. If the size exceed the device possibilities, you get an error, and no input is possible.

## Setup method
The command ends with the `EOL`.

`setEOL( int eol );` - set the EOL. 

Allowd are:

    CRLF (default)
    CR
    LF
    LFCR

## Loop method
This is the only method to be call from the loop.
`void read()` read a char from any stream and complete the command. 
After the `EOL` the command is parsed and fires the appropriate callback function.

## Arguments
The command can be followed by serval arguments. All arguments are separated by a space.
If an argument need to contain a space, put it between quotes (`"`). You can use the escape character `\` to put a quote in this string.

`"Test \"aaa\""` ==> Test "aaa"

## Call by string
A command can also pass as string. (exp.: fired from a button push)
```cpp
// Pass a string command with 3 arguments.
pCmd.doCommand( "test 1 2 3" );
```

## Debugging event

The `eventHandler()` define a function that can be use for debugging, echo, log and more.

```cpp
void PCmd_EventHandler( int event );

pcmd.EventCallback( Pcmp_EventCallback );

void PCmd_EventCallback( int event )
{
    ...
}
```

Posible events are:
```
    PCMD_INPUT_CHAR_EVT
    PCMD_READ_COMMAND_EVT
    PCMD_DO_COMMAND_EVT
    PCMD_ERROR_EVT
```

Use `getLastCharRead()` and `getLastCommand()` to get the unfiltred input.

## Callback function

After the command is parsed, the callback function is called.

He has 2 arguments. (exp. `void test( int argc, char *argv[]) `)
- `argc` is the argument count. 0 for no arguments passed.
- `argv` is the list of the arguments as string.

## Errors

The `read()` or `doCommand()` return false if a error accoutred.

Use `getError()` to get the error code and `getErrorText()` to get the error text.

```cpp
    PCMD_COMMAND_OK 
	PCMD_TOO_MANY_ARGUMENTS_ERR
    PCMD_CMD_NOT_FOUND_ERR
	PCMD_INPUT_TO_LONG_ERR
    PCMD_TOO_MANY_CHAR_ERR
    PCMD_EMPLY_LINE_ERR
	PCMD_MEM_ALLOCATION_ERR
```
    