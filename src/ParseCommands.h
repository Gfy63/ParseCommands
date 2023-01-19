/***********************************
 * @file ParseCommands.h
 * @author Gfy (mrgoofy@gmx.net)
 * @brief Collect command char inputs and parse it, CR ends the command. LF is ignored.
 *        Default input length is 16, default parameter count is 5.
 *        If the command is found in the command list,
 *        than there callback function is called.
 * @version 1.0.0
 * @date 2023-01-18
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * @copyright 2023
 **********************************/

#ifndef ParseCommands_h
#define ParseCommands_h

#include <Arduino.h>
#include <string.h>

#define PC_DEBUG

#ifdef PC_DEBUG
    #define DPRINT(...)         Serial.print(__VA_ARGS__)
    #define DPRINTLN(...)       Serial.println(__VA_ARGS__)
    #define DDELAY(...)         delay(__VA_ARGS__)
    #define VAR_PRINT(...)      Serial.print(F(#__VA_ARGS__" = ")); Serial.print(__VA_ARGS__); Serial.print(F(" "))
    #define VAR_PRINTLN(...)    VAR_PRINT(__VA_ARGS__); Serial.println()
#else
    #define DPRINT(...)
    #define DPRINTLN(...)
    #define DDELAY(...)
    #define VAR_PRINT(...)
    #define VAR_PRINTLN(...)
#endif

class ParseCommands
{
    protected:
        typedef void( *CallbackFunction)(int argc, char *argv[]);

    public:
        /**
         * List of commands follow by the callback function.
         * The list ends with 'NULL, NULL'
        */
        struct command_t{
            const char *cmd;
            CallbackFunction cb;
        };

        /**
         * @brief Constructors.
         * 
         * @param c Command list. Last entry is 'NULL, NULL'.
         * @param bufferLen Len of the input buffer. (default=16)
         * @param argCnt Number of arguments allowed. (default=3)
         */
        ParseCommands( command_t *c, size_t bufferLen, size_t argCnt );

        ParseCommands( command_t *c, size_t bufferLen );
        
        ParseCommands( command_t *c );

        /**
         * @brief Read char of incoming command & parameter.
         * 
         * @param data Incoming char.
         * 
         * @return true     Data read ok.
         *         false    Data read error. See getError() for more details.  
         */
        bool read( char data);

        /**
         * @brief Do parse a command string.
         * 
         * @param c Command to parse.
         * 
         * @return true     Command ok
         *         false    Command not correct. See getError() for more details.  
         */
       bool doCommand( const char *c );

        /**
         * @brief Get error code if false is returned.
         * 
         * @return Error code:
         *           1  No error.
         *          -1  Memory allocation problem.
         *          -2  Empty line.
         *          -3  Too many char input.
         *          -4  Input to long.
         *          -5  Command not found.
         *          -6  Too many arguments.
         */
        int getError( void );

    private:

        // Command list.
        command_t *_cmds;

        // Data in.
        #define sCR 0x0D            // Carriage Return  (13)    \r
        #define sLF 0x0A            // Line Feed        (10)    \n
        int _cmdBufferSize=16;      // Max command buffer size.
        char *_cmdBuffer;           // Command buffer. +1 for \0

        // Read parameter of command.
        char *_cmd;                 // Command.
        int _argcMax = 3;           // Max count of arguments.
        char **_argv;               // List of arguments.
        int _argc;                  // Count of arguments.

        bool _memOK = false;        // Allocation memory for _bmdBuffer & _argv ok if true.
        int _err = 1;               // Error code from read() and pars().

        /**
         * @brief Command complete read. (CR or LF read)
         *        - Split _cmdBuffer to cmd and parameters.
         *        - Search Callback and call it.
         * 
         * @return true     command found.
         *         false    command not found.
         */
        bool parse( void );

        /**
         * @brief Allocate memory for _cmdBuffer & _argv.
         * 
         * @param bs Input Buffer size.
         * @param argc Max arguments count.
         * 
         * @return true     Memory allocation ok
         *         false    Error. No memory allocated. 
         */
        bool AllocateMemory( size_t bs, size_t argc );

};

#endif


