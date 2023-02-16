#include "args.hxx"
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> ReadFile(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
        throw std::runtime_error("Cannot open file " + path);

    std::vector<std::string> result;

    while (input)
    {
        std::string line;
        std::getline(input, line);

        if (input)
            result.push_back(line);
    }

    return result;
}

//< Test
//< test
//< 
//< bla
//
//< bla

void WriteFile(const std::string& path, const std::vector<std::string>& contents)
{
    std::ofstream output(path);
    if (!output)
        throw std::runtime_error("Cannot open file " + path);

    for (std::string line : contents)
        output << line << std::endl;
}

std::string TrimLeft(const std::string& s)
{
    auto index = s.find_first_not_of(" \t\r\n");
    if (index != std::string::npos)
        return s.substr(index);
    else
        return {};
}

std::vector<std::string> ExtractContents(const std::vector<std::string>& lines)
{
    std::vector<std::string> result;

    for (auto& line : lines)
    {
        auto index = line.find("//");
        if (index != std::string::npos)
        {
            auto commentedLine = line.substr(index + 2);
            if (!commentedLine.empty() && commentedLine.front() == '<')
                result.push_back(TrimLeft(commentedLine.substr(1)));
        }
    }

    return result;
}

void Extract(args::Subparser& p)
{
    args::Group options(p, "Options:");
    args::ValueFlag<std::string> inputFile(options, "InputFile", "Input file name.", { 'i', "input" });
    args::ValueFlag<std::string> outputFile(options, "OutputFile", "Output file name.", { 'o', "output" });

    p.Parse();

    if (inputFile.Get().empty())
        throw std::runtime_error("No input file is provided.");

    if (outputFile.Get().empty())
        throw std::runtime_error("No output file is provided.");

    WriteFile(outputFile.Get(), ExtractContents(ReadFile(inputFile.Get())));
}

int main(int argc, const char* argv[])
{
    std::string toolname = std::string(argv[0]).substr(std::string(argv[0]).find_last_of("\\") + 1);
    args::ArgumentParser parser("\"" + toolname + "\"" + " is a tool used to extract files (containing e.g. documentation) from C/C++ source files.");

    args::Group commands(parser, "Commands:");
    args::Command extractCommand(commands, "extract", "Extract contents from a file.", [&](args::Subparser &p) { Extract(p); });

    args::Group arguments(parser, "Optional arguments:");
    args::HelpFlag help(arguments, "help", "Display this help menu.", { 'h', "help" });

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Completion& e)
    {
        std::cout << e.what();
        return EXIT_FAILURE;
    }
    catch (const args::Help&)
    {
        std::cout << parser;
        return EXIT_SUCCESS;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return EXIT_FAILURE;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//< bla
