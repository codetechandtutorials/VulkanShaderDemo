// derived from https://github.com/KhronosGroup/Vulkan-Samples

#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
namespace VKSHADER {

std::vector<std::string> split(const std::string &input, char delim)
{
	std::vector<std::string> tokens;

	std::stringstream sstream(input);
	std::string       token;
	while (std::getline(sstream, token, delim))
	{
		tokens.push_back(token);
	}

	return tokens;
}

std::string read_text_file(const std::string &filename)
{
	std::vector<std::string> data;

	std::ifstream file;

	file.open(filename, std::ios::in);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}

	return std::string{(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>())};
}

std::string read_shader(const std::string &filename)
{
	return read_text_file(filename);
}

inline std::vector<std::string> precompile(const std::string& source) {
	std::vector<std::string> final_file;

	auto lines = split(source, '\n');

	for (auto &line : lines)
	{
		if (line.find("#include \"") == 0)
		{
			// Include paths are relative to the base shader directory
			std::string include_path = line.substr(10);
			size_t      last_quote   = include_path.find("\"");
			if (!include_path.empty() && last_quote != std::string::npos)
			{
				include_path = include_path.substr(0, last_quote);
			}

			auto include_file = precompile(read_shader(include_path));
			for (auto &include_file_line : include_file)
			{
				final_file.push_back(include_file_line);
			}
		}
		else
		{
			final_file.push_back(line);
		}
	}

	return final_file;
}

}