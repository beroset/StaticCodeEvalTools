#include <iostream>
#include <fstream>
#include <sstream>
#include "StatisticsCollector.h"
#include "FileProcessor.h"
#include "FileStatistics.h"
#include "ReportWriter.h"

static constexpr std::size_t InputBufferSize = 8 * 1024;

static void processLoop(std::ifstream& inStream,
	FileStatistics& statistics) noexcept
{
	StatisticsCollector fileAnalyzer(statistics);

	std::stringstream inputBuffer;
	inputBuffer << inStream.rdbuf();
	fileAnalyzer.analyzeBuffer(inputBuffer.str());
}

/*
 * Processing a file includes reading the file, analyzing the input to collect
 * the statistics and then printing the statistics.
 */
static std::string processFile(const ProgramOptions& options, std::string fileName,
	FileStatistics& totalStats)
{
	std::string fileResults;

	try {
		if (fileName.empty())
		{
			std::string eMsg(
				"Programmer Error: File name is empty in "
				"FileProcessor Constructor!");
			std::runtime_error programmerError(eMsg);
			throw programmerError;
		}

		std::ifstream inStream(fileName);
		if (!inStream.is_open())
		{
			std::string eMsg("Runtime error:  Can't open file " + fileName +
				" for input.");
			std::runtime_error FileInputError(eMsg);
			throw FileInputError;
		}

		FileStatistics statistics(fileName);
		processLoop(inStream, statistics);

		inStream.close();

		ReportWriter ReportFileStatistics(options);
		fileResults = ReportFileStatistics.getResultText(statistics);
		statistics.addTotals(totalStats);
	}

	catch (const std::exception& ex)
	{
		std::cerr <<
			"Error: unable to complete processing file statistics for "
			<< fileName << " Error: " << ex.what() << std::endl;
		fileResults.clear();
	}

	return fileResults;
}

std::string processAllFiles(const std::vector<std::string>& fileNames,
                             const ProgramOptions& progOptions)
{
	ReportWriter TotalsReporter(progOptions);
	FileStatistics allFiles;

	std::string resultsToDisplay(TotalsReporter.getColumnHeadingAsOneString());

	for (const auto& currentFile : fileNames)
	{
		try
		{
			std::string fileResults = processFile(progOptions, currentFile, allFiles);
			if (!fileResults.empty())
			{
				resultsToDisplay += fileResults;
			}
		}
		catch (const std::runtime_error& re)
		{
			std::cerr << re.what() << "\n";
		}
	}

	resultsToDisplay += TotalsReporter.getColumnHeadingAsOneString();
	resultsToDisplay += TotalsReporter.getResultText(allFiles);

	return resultsToDisplay;
}
