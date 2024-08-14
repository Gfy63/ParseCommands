/***********************************
 * @file ParseCommands.h
 * @author Gfy63 (mrgoofy@gmx.net)
 * @brief Collect command char inputs and parse it, CR ends the command. LF is ignored.
 *        Default input length is 16, default parameter count is 3.
 *        If the command is found in the command list,
 *        than there callback function is called.
 * @version 1.5.0
 * @date 2023-08-10
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * @copyright 2023-24
 **********************************/

#ifndef ParseCommands_h
#define ParseCommands_h

#include <Arduino.h>
#include <string.h>

/**
 * --- CONSTANT'S ---
 */

/**
 * List of error codes.
*/
enum pcmd_error { PCMD_COMMAND_OK=1, 
				// ! Negative count upwards.
				PCMD_TOO_MANY_ARGUMENTS_ERR=-6, PCMD_CMD_NOT_FOUND_ERR, 
				PCMD_INPUT_TO_LONG_ERR, PCMD_TOO_MANY_CHAR_ERR, PCMD_EMPLY_LINE_ERR,
				PCMD_MEM_ALLOCATION_ERR };

/**
 * List of events for eventHandler().
*/
enum pcmd_events { PCMD_INPUT_CHAR_EVT=0, PCMD_READ_COMMAND_EVT, PCMD_DO_COMMAND_EVT, PCMD_ERROR_EVT };

/**
 * List of allowed EOL values. (CRLF (default), CR, LF, LFCR 
*/
enum EOL { CRLF=0, CR, LF, LFCR };

/**
 * --- STRUCT & TYPEDEF ---
*/

/**
 * Callback typedef.
 */
typedef void( *PCMD_CallbackFunction)(int argc, char *argv[]);
typedef void( *PCMD_EventCallbackFunction)( int event );

/**
 * List of commands follow by the callback function.
 * The list ends with 'NULL, NULL'
*/
struct pcmd_command_t{
	const char *cmd;
	PCMD_CallbackFunction cb;
};

/**
 * --- CLASS ---
 */

class ParseCommands
{
	protected:
		// typedef void( *CallbackFunction)(int argc, char *argv[]);
		// typedef void( *EventCallbackFunction)( int event );

	public:
		/**
		 * --- #DEFINE ---
		*/

		/**
		 * --- STRUCT & TYPEDEF ---
		*/

		/**
		 * --- CONSTRUCTOR & BEGIN ---
		*/

		/**
		 * @brief Constructors.
		 * @param c Command list. Last entry is 'NULL, NULL'.
		 * @param bufferLen Len of the input buffer. (default=16)
		 * @param argCnt Number of arguments allowed. (default=3)
		 */
		ParseCommands( pcmd_command_t *c, size_t bufferLen, size_t argCnt );
		ParseCommands( pcmd_command_t *c, size_t bufferLen );
		ParseCommands( pcmd_command_t *c );
		ParseCommands();

		/**
		 * @brief Init of ParseCommands, if use empty constructor.
		 * @param c Command list. Last entry is 'NULL, NULL'.
		 * @param bufferLen Len of the input buffer. (default=16)
		 * @param argCnt Number of arguments allowed. (default=3)
		 */
		void begin( pcmd_command_t *c, size_t bufferLen, size_t argCnt );
		void begin( pcmd_command_t *c, size_t bufferLen );
		void begin( pcmd_command_t *c );

		/**
		 * --- PUBLIC FUNCTIONS ---
		*/

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
		 * @brief Set the event callback function. This is called on every interaction.
		 * 		  It can be used for debuging, echo, log and more purpotion.
		 * @param cb Callback function.
		 */
		void eventHandler( PCMD_EventCallbackFunction cb );

		/**
		 * @brief Select EOL.
		 * @param eol CRLF (default), CR, LF, LFCR
		 */
		void setEOL( int eol );

		/**
		 * @brief Return the last char read.
		 * @return	The last char.
		 */
		char getLastCharRead( void );

		/**
		 * @brief Return the last command.
		 * @return	The last command.
		 */
		char *getLastCommand( void );

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
		pcmd_command_t *_cmds;

		#define EOLCNT 4
		char const *_eolArry[EOLCNT] = {
			"\r\n",     // CR LF
			"\r",       // CR
			"\n",       // LF
			"\n\r"      // LF CR
		};
		int _eol = CRLF;    // Default.

		int _cmdBufferSize=16;			// Max command buffer size.
		char *_cmdBuffer;				// Command buffer. +1 for \0

		char *_lastCommand;				// Copy of buffer for getLastCommand().
		char _lastChar;					// Copy of last cahr read.

		// Read parameter of command.
		char *_cmd;						// Command.
		int _argcMax = 3;				// Max count of arguments.
		char **_argv;					// List of arguments.
		int _argc;						// Count of arguments.

		PCMD_EventCallbackFunction _eventCB = NULL;		// Callback function for debug events.
		bool _memOK = false;			// Allocation memory for _cmdBuffer, _lastCommand & _argv, ok if true.
		int _err = 1;					// Error code from read(), doCommand() and parse().

		/**
		 * @brief Command complete read. (CR or LF read)
		 *        - Split _cmdBuffer to cmd and parameters.
		 *        - Search Callback and call it.
		 * @return true     command found.
		 *         false    command not found.
		 */
		bool parse( void );

		/**
		 * @brief A sequence of calls split s into tokens, which are sequences of contiguous characters separated by space.
		 * 		  Simulat to strtok() but a token can be put in quotes (").
		 * @param	s String to splt. If NULL use the rest of the previouse call.
		 * @return	Begin of the tken found.
		 */
		char * strtok_c( char * s );

		/**
		 * @brief Call event_callback.
		 * @param event Event to send with event_callback.
		 */
		void DoEventCall( int event );

		/**
		 * @brief Allocate memory for _cmdBuffer & _argv.
		 * @param bs Input Buffer size.
		 * @param argc Max arguments count.
		 * @return true     Memory allocation ok
		 *         false    Error. No memory allocated. 
		 */
		bool AllocateMemory( size_t bs, size_t argc );

		/**
		 * @brief Test if memory allocation is ok.
		 * @return true		Memory allocation ok
		 * 		   false	Error. No memory allocated. (generat error code)
		 */
		bool isMemoryOK( void );

		/**
		 * @brief Test if buffer len is not exceeded.
		 * @return true		Buffer len ok.
		 * 		   false	Error. Buffer len exceeded. (generat error code)
		 */
		bool isBufferLenOK( const char *c );

};

#endif

// End of 'ParseCommands.h.
