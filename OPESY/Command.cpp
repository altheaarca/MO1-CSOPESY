//Barebones process instructions
#include "Command.h"

//TODO: Support basic process instructions, akin to programming language instructions.
/*
	* NOTES
	* Process instructions are pre-determined and not typed by the user. E.g., randomized via scheduler-start command.
	* Variables are stored in memory and will not be released until the process finishes.
	* uint16 variables are clamped between (0, max(uint16)).
	* Unless specified in the test case, the “msg” in the PRINT function should always be “Hello world from <process_name>!”
	* For loops can be nested up to 3 times.
 */

void Command::executeCommand()
{
	// depending on command type execute this...
}

//void Command::executeCommand2(Process& process)
//{
//	// for logging print commands in processes
//}
