#pragma once
#include <fstream>
#include <string>

using namespace std;

namespace Augiwne {
	static string ReadFile(const string& filePath)
	{
		ifstream ifs((filePath).c_str());
		string content(istreambuf_iterator<char>(ifs.rdbuf()),
			istreambuf_iterator<char>());
		return content;
	}
}