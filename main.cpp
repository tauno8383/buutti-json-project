
#include <iostream>
#include <fstream>
#include <vector>

#include "my_util.h"
#include "json.h"



int main()
{
    std::cout << "-----------Json-------------" << std::endl;

    std::string jsonStringObject = readFile("test_json.json");

    //std::cout << jsonStringObject << std::endl;

    JsonObject jo{jsonStringObject};

    return 0;
}
