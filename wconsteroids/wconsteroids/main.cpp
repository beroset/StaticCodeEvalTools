#include <iostream>
#include <string>
#include "Executionctrlvalues.h"
#include "CommandLineParser.h"
#include "FileStatistics.h"
#include "FileProcessor.h"
#include "ProgramOptions.h"
#include "ReportWriter.h"

#ifdef _DEBUG
#include <cstring>
static void debugMainLoop(ExecutionCtrlValues& executionCtrl)
{
	ProgramOptions debugOptions = executionCtrl.options;
	debugOptions.debugPrint();

	std::cout << "\nFile specifications:\n";
	for (auto fileSpec : executionCtrl.fileSpecTypes)
	{
		std::cout << "\t" << fileSpec << "\n";
	}

	std::cout << "\nFile names:\n";
	for (auto fileName : executionCtrl.filesToProcess)
	{
		std::cout << "\t" << fileName << "\n";
	}
}

static char** makeFakeArgs(int *FakeArgc)
{
	std::vector<std::string> args =
	{
		"-Llcw",
		"-R",
		"*.cpp",
		"*.h",
	};
	*FakeArgc = static_cast<int>(args.size());
	char** fakeArgs = new char* [*FakeArgc];

	for (size_t i = 0; i < args.size(); i++)
	{
		fakeArgs[i] = nullptr;
		fakeArgs[i] = new char[args[i].length() + 1];
		strcpy(fakeArgs[i], args[i].c_str());
	}

	return fakeArgs;
}
#endif

static void mainLoop(ExecutionCtrlValues& executionCtrl)
{
	FileStatistics allFiles;

#ifdef _DEBUG
	debugMainLoop(executionCtrl);
#endif

	ProgramOptions& options = executionCtrl.options;
	std::vector<std::string> fileToProcess = executionCtrl.filesToProcess;

	ReportWriter TotalReporter(options);
	TotalReporter.printColumnHeadings();

	for (auto currentFile : fileToProcess)
	{
		try
		{
			FileProcessor fileProcessor(currentFile, options);
			if (fileProcessor.processFile())
			{
				fileProcessor.mergeStatistics(allFiles);
			}
		}
		catch (std::runtime_error re)
		{
			std::cerr << re.what() << "\n";
		}
	}
	
	TotalReporter.printColumnHeadings();
	TotalReporter.printResult(allFiles);
}

int main(int argc, char* argv[])
{
	int exit_status = EXIT_SUCCESS;
	ExecutionCtrlValues executionCtrl;
	std::string versionString("1.0.0");

#ifdef _DEBUG
	int FAKEARGC = 0;
	char** FAKEARGV = makeFakeArgs(&FAKEARGC);
	CommandLineParser cmdLineParser(FAKEARGC, FAKEARGV, versionString);
#else
	CommandLineParser cmdLineParser(argc, argv, versionString);
#endif

	try
	{
		executionCtrl.initFromEnvironmentVariables();
		if (cmdLineParser.parse(executionCtrl))
		{
			mainLoop(executionCtrl);
		}
	}

	catch (HelpMe helpMe)
	{
		cmdLineParser.printHelpMessage();
	}
	catch (showVersions sv)
	{
		cmdLineParser.printVersion();
	}
	catch (std::exception ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}