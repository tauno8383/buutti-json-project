
#ifndef MY_UTIL_CPP
#define MY_UTIL_CPP

#include <string>

std::string readFile(std::string filename) {
    std::string wholeJsonText;
    std::ifstream MyReadFile(filename);
    for (std::string line; std::getline(MyReadFile, line);)
    {
        wholeJsonText.append(line);
    }
    MyReadFile.close();
    return wholeJsonText;
}

int findNthOccur(std::string str, char ch, int N)
{
    int occur = 0;
    // Loop to find the Nth
    // occurrence of the character
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == ch)
            occur += 1;
        if (occur == N)
            return i;
    }
    return -1;
}

#endif