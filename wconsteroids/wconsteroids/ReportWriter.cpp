#include <iostream>
#include <string>
#include <vector>
#include "ReportWriter.h"

void ReportWriter::printResult(FileStatistics& resultsForOutput)
{
	std::cout << getResultText(resultsForOutput) << "\n";
}

/*
 * Maintain the order between this function and getColumneHeadingsText().  
 */
std::string ReportWriter::getResultText(FileStatistics& resultsForOutput)
{
	std::string outString;
	ProgramOptions options = executionCtrl->options;

	if (options.lineCount)
	{
		outString += std::to_string(resultsForOutput.getTotalLines()) + "\t";
	}

	if (options.wordCount)
	{
		outString += std::to_string(resultsForOutput.getWords()) + "\t";
	}

	if (options.byteCount)
	{
		outString += std::to_string(resultsForOutput.getCharacters()) + "\t";
	}

	if (options.charCount)
	{
		outString += std::to_string(resultsForOutput.getCharacters()) + "\t";
	}

	if (options.maxLineWidth)
	{
		outString += std::to_string(resultsForOutput.getWidestLine()) + "\t";
	}

	// End of backwards compatability with wc utility.

	if (options.codeCount)
	{
		outString += std::to_string(resultsForOutput.getCodeLines()) + "\t";
	}

	if (options.commentCount)
	{
		outString += std::to_string(resultsForOutput.getCommentLines()) + "\t";
	}

	if (options.percentages)
	{
		outString +=
			std::to_string(resultsForOutput.getPerecentageOfCode()) + "\t";
	}

	if (options.whitespaceCount)
	{
		outString += std::to_string(resultsForOutput.getWhitespace()) + "\t";
	}
	if (options.blankLineCount)
	{
		outString += std::to_string(resultsForOutput.getBlankLines()) + "\t";
	}

	outString += "\t\t" + resultsForOutput.getFileName();

	return outString;
}

/*
 * Maintain the order between this function and getResultText().
 */
std::vector<std::string> ReportWriter::getColumneHeadingsText()
{
	std::string firstLine;
	std::string secondline;
	ProgramOptions options = executionCtrl->options;

	if (options.lineCount)
	{
		firstLine += "Lines\t";
		secondline += "of Text\t";
	}

	if (options.wordCount)
	{
		firstLine += "Words\t";
		secondline += "\t";
	}

	if (options.byteCount)
	{
		firstLine += "Bytes\t";
		secondline += "\t";
	}

	if (options.charCount)
	{
		firstLine += "Characters\t";
		secondline += "\t\t";
	}

	if (options.maxLineWidth)
	{
		firstLine += "Length of\t";
		secondline += "Longest Line\t";
	}

	// End of backwards compatability with wc utility.

	if (options.codeCount)
	{
		firstLine += "Lines\t";
		secondline += "of Code\t";
	}

	if (options.commentCount)
	{
		firstLine += "Lines of\t";
		secondline += "Comments\t";
	}

	if (options.percentages)
	{
		firstLine += "Percentage of\t";
		secondline += "Lines of Code\t";
	}

	if (options.whitespaceCount)
	{
		firstLine += "Whitespace\t";
		secondline += "Characters\t";
	}
	if (options.blankLineCount)
	{
		firstLine += "Blank\t";
		secondline += "Lines\t";
	}


	std::vector<std::string> headerRows = { firstLine, secondline };
	return headerRows;
}

void ReportWriter::printColumnHedings()
{
	std::vector<std::string> headerRows = getColumneHeadingsText();
	for (auto line : headerRows)
	{
		std::cout << line << "\n";
	}
}
