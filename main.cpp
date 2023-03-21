
#include <iostream>
#include <fstream>
#include <vector>

#include "my_util.h"
#include "json.h"

struct JsonCount
{
    int numbers;
    int texts;
    int booleans;
    int lists;
};

JsonCount analyzeObject(JsonObject object)
{
    JsonCount outJsonConut{0, 0, 0, 0};
    outJsonConut.numbers = object.numbers();
    outJsonConut.texts = object.texts();
    outJsonConut.booleans = object.booleans();
    outJsonConut.lists = object.lists();
    return outJsonConut;
}

int main()
{
    std::cout << "-----------Json-------------" << std::endl;

    std::string jsonStringObject = readFile("test_json2.json");

    // std::cout << jsonStringObject << std::endl;

    JsonObject jo{jsonStringObject};

    JsonCount jc = analyzeObject(jo);

    // display results
    std::cout << "---------------------\n";
    std::cout << "numbers: " << jc.numbers << std::endl;
    std::cout << "texts: " << jc.texts << std::endl;
    std::cout << "booleans: " << jc.booleans << std::endl;
    std::cout << "lists: " << jc.lists << std::endl;
    std::cout << "---------------------\n";
    std::cout << "number33 value is " << jo.getNumberValue("number33") << std::endl;
    //std::cout << "descriptiveText has value " << jo.getTextValue("descriptiveText") << std::endl;
    std::cout << "important has value " << jo.getBooleanValue("important") << std::endl;
    std::cout << "relevancyCheck has value " << jo.getBooleanValue("relevancyCheck") << std::endl;

    return 0;
}
