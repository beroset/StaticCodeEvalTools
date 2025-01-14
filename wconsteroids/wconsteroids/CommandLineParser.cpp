#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "CommandLineParser.h"
#include "CmdLineFileExtractor.h"
#include "Executionctrlvalues.h"
#include "UtilityTimer.h"

static std::string_view simplify_name(char *path)
{
	return std::filesystem::path{path ? path : "wconsteroids"}.filename().string();
}

/*
 * Begin public interface.
 */
CommandLineParser::CommandLineParser(int argc, char* argv[],
	std::string progVersion)
	: program_name{ simplify_name(argv[0]) },
	args(argv + 1, argv + argc),
	version{ std::move(progVersion) },
	programName{ argv[0] }
{

}

bool CommandLineParser::parse(ExecutionCtrlValues& execVars)
{
	UtilityTimer stopWatch;

	extractAllArguments();
	if (useDefaultFlags)
	{
		SetDefaultOptionsWhenNoFlags();
	}

	findAllFilesToProcess(execVars);
	execVars.options = options;

	if (options.enableExecutionTime)
	{
		stopWatch.stopTimerAndReport("command line parsing at ");
	}

	return execVars.filesToProcess.size() != 0;
}

void CommandLineParser::printHelpMessage() const
{
	std::cout << "\n" << programName <<
		" file name or file type specification (*.ext)\n"
		"Options:\n"
		"\t-c, --bytes print the byte counts\n"
		"\t-m, --chars print the character counts\n"
		"\t-l, --lines print the newline counts\n"
		"\t-t, --time-execution print the execution time of the program\n"
		"\t-L, --max-line-length print the length of the longest line\n"
		"\t-w, --words print the word counts\n"
		"\t--help display this help and exit\n"
		"\t--version output version information and exit\n"
		"\t-R, --subdirectories all files in the"
		" directory as well as sub directories\n"
		"\tBy default the -c -l and -w flags are set, setting any"
		" flag requires all flags you want to be set.\n";
	printVersion();
}

void CommandLineParser::printVersion() const noexcept
{
	std::cout << programName << ": version: " << version << "\n"
		"Packaged by Chernick Consulting\n"
		"License GPLv3+: GNU GPL version 3 or later"
		" <http://gnu.org/licenses/gpl.html>.\n"
		"This is free software : you are free to change and redistribute it.\n"
		"\tThere is NO WARRANTY, to the extent permitted by law.\n"
		"\nWritten by Paul A. Chernick, Toby Speight and Edward Beroset\n";
	// flush the buffer to make sure the entire message is visible
	std::cout << std::flush;
	exit(EXIT_SUCCESS);
}

void CommandLineParser::findAllFilesToProcess(ExecutionCtrlValues& execVars)
{
	bool searchSubDirs = options.recurseSubDirectories;
	CmdLineFileExtractor fileExtractor(NotFlagsArgs, searchSubDirs);
	fileExtractor.findAllRequiredFiles();
	execVars.filesToProcess = fileExtractor.getFileList();
}

void CommandLineParser::extractAllArguments()
{
	// Since argv[0] is removed in the constructor we
	// can perform a ranged for loop.
	for (auto arg: args)
	{
		if (arg[0] == '-')
		{
			if (arg[1] == '-')
			{
				processDoubleDashOptions(arg);
			}
			else
			{
				processSingleDashOptions(arg);
			}
		}
		else
		{
			NotFlagsArgs.push_back(std::move(arg));
		}
	}
}

/*
 * Flags starting with -- are full strings that need to be processed
 * as strings.
 */
void CommandLineParser::processDoubleDashOptions(std::string_view currentArg)
{
	auto flag = doubleDashArgs.find(currentArg);
	if (flag != doubleDashArgs.end())
	{
		(*flag).second = true;
		useDefaultFlags = false;
		return;
	}

	// The following switches require alternate handling
	if (currentArg == "--subdirectories")
	{
		// Since this is not a column switch it does not affect the default 
		options.recurseSubDirectories = true;
		return;
	}

	if (currentArg == "--time-execution")
	{
		// Since this is not a column switch it does not affect the default 
		options.enableExecutionTime = true;
		return;
	}

	if (currentArg == "--help")
	{
		// calls printVersion.
		printHelpMessage();
	}

	if (currentArg == "--version")
	{
		// calls exit after printing version
		printVersion();
	}

	std::cerr << "Unknown flag: " << currentArg << "\n";
}

/*
 * Each character needs to be processed independently.
 */
void CommandLineParser::processSingleDashOptions(std::string_view currentArg)
{
	for (std::size_t i = 1; i < currentArg.size(); i++)
	{
		auto thisOption = singleDashArgs.find(currentArg[i]);
		if (thisOption != singleDashArgs.end())
		{
			(*thisOption).second = true;
			useDefaultFlags = false;
		}
		else
		{
			switch (currentArg[i])
			{
			case 'R':
				// Since this is not a column switch it does not affect the
				// default  
				options.recurseSubDirectories = true;
				continue;
			case 't':
				// Since this is not a column switch it does not affect the
				// default  
				options.enableExecutionTime = true;
				continue;
			default:
				std::cerr << "Unknown flag: " << currentArg[i] << "\n";
				continue;
			}
		}
	}
}

void CommandLineParser::SetDefaultOptionsWhenNoFlags()
{
	// Based on the default functionality of the wc program.
	options.byteCount = true;
	options.wordCount = true;
	options.lineCount = true;
}

