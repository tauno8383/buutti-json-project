

#include <iostream>
#include <fstream>
#include <vector>
#include "my_util.h"

class JsonObject;

union Value
{
    // Ponential values for Json data. Only one of those will be used so thus "union".
    bool _boolValue;
    int _intValue;
    std::string *_stringValue;
    std::vector<Value> *_values; // For Json arrays
    JsonObject *_object;
};

struct JsonData
{
    std::string _name;
    Value _value;

    JsonData(std::string name) : _name{name} {}
    JsonData(std::string name, Value value) : _name{name}, _value{value} {};
};

class JsonObject
{
public:
    JsonObject(std::string ojbectStr)
    {
        std::string dataStr;
        do
        {
            dataStr = nextData(ojbectStr);
            std::cout << dataStr << std::endl;
            std::string name = extractName(dataStr);
            Value value = extractValue(dataStr);
            JsonData jd{name, value};
            _data.push_back(jd);
        } while (ojbectStr.length() != 0);
    }

private:
    std::vector<JsonData> _data;
    enum IstreamState : char
    {
        string = '"',
        inOjbect = '{',
        outObject = '}',
        inArray = '[',
        outArray = ']',
        valueForData = ':',
        nextField = ',',
    };

    std::string nextData(std::string &str)
    {
        int i_data_starts = str.find(IstreamState::string);
        str = str.substr(i_data_starts + 1);
        int i_data_ends;
        int i_in_array{0};  // i.e how deep in array, say [[[]]] is 3
        int i_in_object{0}; // i.e how deep in object, say {{{{}}}} is 4
        for (int i = 0; i < str.length(); ++i)
        {
            char c = str[i];
            if ((c == IstreamState::nextField || c == IstreamState::outObject) && i_in_array == 0 && i_in_object == 0)
            {
                i_data_ends = i;
                break;
            }
            switch (c)
            {
            case IstreamState::inOjbect:
                ++i_in_object;
                break;
            case IstreamState::outObject:
                --i_in_object;
                break;
            case IstreamState::inArray:
                ++i_in_array;
                break;
            case IstreamState::outArray:
                --i_in_array;
                break;
            }
        }
        std::string dataStr = str.substr(0, i_data_ends);
        str = str.substr(i_data_ends + 1);
        return dataStr;
    }

    std::string extractName(std::string &dataStr)
    {
        int i_name_ends = dataStr.find(IstreamState::string);
        std::string name = dataStr.substr(0, i_name_ends);
        dataStr = dataStr.substr(i_name_ends + 1);
        return name;
    }

    Value extractValue(std::string dataStr)
    {
        Value value;
        int i{0};
        char c;
        while (isspace(c = dataStr[i]) || c == IstreamState::valueForData) // skip spaces and ':' to reach the actual vale.
        {
            ++i;
        }
        if (isdigit(c)) // if a number value
        {
            int i_digit_end = i;
            while (isdigit(c = dataStr[++i_digit_end]))
                ;
            std::string digit_str = dataStr.substr(i, i_digit_end);
            int int_val = std::stoi(digit_str);
            value._intValue = int_val;
        }
        else if (c == IstreamState::string) // if a string value
        {
            int i_str_end = findNthOccur(dataStr, IstreamState::string, 2);
            std::string str_vale = dataStr.substr(i + 1, (i_str_end - i) - 1);
            value._stringValue = &str_vale;
        }
        else if (dataStr[i] == 't' && dataStr[++i] == 'r' && dataStr[++i] == 'u' && dataStr[++i] == 'e') // if a boolean value true
        {
            value._boolValue = true;
        }
        else if (dataStr[i] == 'f' && dataStr[++i] == 'a' && dataStr[++i] == 'l' && dataStr[++i] == 's' && dataStr[++i] == 'e') // if a boolean value false
        {
            value._boolValue = false;
        }
        else if (c == IstreamState::inOjbect) // if an Json object value
        {
            JsonObject jo{dataStr};
            value._object = &jo;
        }
        else if (c == IstreamState::inArray) // if an array value
        {
            // FOR THE SAKE OF SIMPLICITY WE INGNORE ARRAYS AT THIS POINT! Use just dymmy array.
            Value dummyVale1;
            dummyVale1._boolValue = false;
            Value dummyVale2;
            dummyVale2._boolValue = false;
            std::vector<Value> dummyVector{dummyVale1, dummyVale2};
            value._values = &dummyVector;
        }
        return value;
    }
};
