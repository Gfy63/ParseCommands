/***********************************
 * @file ParseCommands.h
 * @author Gfy63 (mrgoofy@gmx.net)
 * @brief Collect command char inputs and parse it, CR ends the command. LF is ignored.
 *        Default input length is 16, default parameter count is 5.
 *        If the command is found in the command list,
 *        than there callback function is called.
 * @version 1.2.5
 * @date 2023-04-11
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * @copyright 2023
 **********************************/

#ifndef ParseCommands_h
#define ParseCommands_h

#include <Arduino.h>
#include <string.h>

class ParseCommands
{
    protected:
        typedef void( *CallbackFunction)(int argc, char *argv[]);

    public:
        /**
         * List of allowed EOL values. (CRLF (default), CR, LF, LFCR 
        */
        enum EOL { CRLF=0, CR, LF, LFCR };

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
         * @param c Command list. Last entry is 'NULL, NULL'.
         * @param bufferLen Len of the input buffer. (default=16)
         * @param argCnt Number of arguments allowed. (default=3)
         */
        ParseCommands( command_t *c, size_t bufferLen, size_t argCnt );

        ParseCommands( command_t *c, size_t bufferLen );
        
        ParseCommands( command_t *c );

        /**
         * @brief Read char of incoming command & parameter.
         * @param data Incoming char.
         * @return true     Data read ok.
         *         false    Data read error. See getError() for more details.  
         */
        bool read( char data);

        /**
         * @brief Do parse a command string.
         * @param c Command to parse.
         * @return true     Command ok
         *         false    Command not correct. See getError() for more details.  
         */
        bool doCommand( const char *c );

        /**
         * @brief Select EOL.
         * @param eol CRLF (default), CR, LF, LFCR
         */
        void setEOL( int eol );

        /**
         * @brief Get error code if false is returned.
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

        /**
         * @brief Return error as text.
         * @return  Error text.
        */
        const __FlashStringHelper * getErrorText( void );

    private:

        // Command list.
        command_t *_cmds;

        #define EOLCNT 4
        char const *_eolArry[EOLCNT] = {
            "\r\n",     // CR LF
            "\r",       // CR
            "\n",       // LF
            "\n\r"      // LF CR
        };
        int _eol = CRLF;    // Default.

        int _cmdBufferSize=16;      // Max command buffer size.
        char *_cmdBuffer;           // Command buffer. +1 for \0

        // Read parameter of command.
        char *_cmd;                 // Command.
        int _argcMax = 3;           // Max count of arguments.
        char **_argv;               // List of arguments.
        int _argc;                  // Count of arguments.

        bool _memOK = false;        // Allocation memory for _bmdBuffer & _argv, ok if true.
        int _err = 1;               // Error code from read(), doCommand() and parse().

        /**
         * @brief Command complete read. (CR or LF read)
         *        - Split _cmdBuffer to cmd and parameters.
         *        - Search Callback and call it.
         * @return true     command found.
         *         false    command not found.
         */
        bool parse( void );

        /**
         * @brief Allocate memory for _cmdBuffer & _argv.
         * @param bs Input Buffer size.
         * @param argc Max arguments count.
         * @return true     Memory allocation ok
         *         false    Error. No memory allocated. 
         */
        bool AllocateMemory( size_t bs, size_t argc );

};

#endif


