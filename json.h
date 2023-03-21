

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
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

enum ValueType
{
    boolean,
    integer,
    string,
    array,
    object,
    unknown, // unknown type
};

struct JsonData
{
    std::string _name;
    Value _value;
    ValueType _valueType;

    JsonData(std::string name, Value value, ValueType valueType) : _name{name}, _value{value}, _valueType{valueType} {};
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
            std::pair<Value, ValueType> value = extractValue(dataStr);
            JsonData jd{name, value.first, value.second};
            _datas.push_back(jd);
        } while (ojbectStr.length() != 0);
    }

    int numbers() // kuinka monta numeroarvoa objekti sisältää
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::object)
                n += data._value._object->numbers();
            else if (data._valueType == ValueType::integer)
                ++n;
        }
        return n;
    }
    int texts() //  – || –
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::object)
                n += data._value._object->texts();
            else if (data._valueType == ValueType::string)
                ++n;
        }
        return n;
    }
    int booleans() // | – || –
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::object)
                n += data._value._object->booleans();
            else if (data._valueType == ValueType::boolean)
                ++n;
        }
        return n;
    }
    int lists() // – ||
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::object)
                n += data._value._object->lists();
            else if (data._valueType == ValueType::array)
                ++n;
        }
        return n;
    }

    int getNumberValue(std::string valueName) // -> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        int r_value;
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::integer && data._name == valueName)
                r_value = data._value._intValue;
            else if (data._valueType == ValueType::object)
                r_value = data._value._object->getNumberValue(valueName);
        }
        return r_value;
    }
    std::string getTextValue(std::string valueName) //-> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        std::string r_value;
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::string && data._name == valueName)
                r_value = *data._value._stringValue;
            else if (data._valueType == ValueType::object)
                r_value = data._value._object->getTextValue(valueName);
        }
        return r_value;
    }
    bool getBooleanValue(std::string valueName) //-> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        bool r_value;
        for (JsonData data : _datas)
        {
            if (data._valueType == ValueType::boolean && data._name == valueName)
                r_value = data._value._boolValue;
            else if (data._valueType == ValueType::object)
                r_value = data._value._object->getBooleanValue(valueName);
        }
        return r_value;
    }

private:
    std::vector<JsonData> _datas;
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
        int i_in_array{0};  // i.e how deep in array, say [[[]]] is 3 in the midle
        int i_in_object{0}; // i.e how deep in object, say {{{{}}}} is 4 in the midle
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

    std::pair<Value, ValueType> extractValue(std::string dataStr)
    {
        Value r_value;
        ValueType r_value_type;
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
            r_value._intValue = int_val;
            r_value_type = ValueType::integer;
        }
        else if (c == IstreamState::string) // if a string value
        {
            int i_str_end = findNthOccur(dataStr, IstreamState::string, 2);
            std::string str_vale = dataStr.substr(i + 1, (i_str_end - i) - 1);
            r_value._stringValue = &str_vale;
            r_value_type = ValueType::string;
        }
        else if (dataStr[i] == 't' && dataStr[++i] == 'r' && dataStr[++i] == 'u' && dataStr[++i] == 'e') // if a boolean value true
        {
            r_value._boolValue = true;
            r_value_type = ValueType::boolean;
        }
        else if (dataStr[i] == 'f' && dataStr[++i] == 'a' && dataStr[++i] == 'l' && dataStr[++i] == 's' && dataStr[++i] == 'e') // if a boolean value false
        {
            r_value._boolValue = false;
            r_value_type = ValueType::boolean;
        }
        else if (c == IstreamState::inOjbect) // if an Json object value
        {
            JsonObject jo{dataStr};
            r_value._object = &jo;
            r_value_type = ValueType::object;
        }
        else if (c == IstreamState::inArray) // if an array value
        {
            // FOR THE SAKE OF SIMPLICITY WE INGNORE ARRAYS AT THIS POINT! Use just dymmy array.
            Value dummyVale1;
            dummyVale1._boolValue = false;
            Value dummyVale2;
            dummyVale2._boolValue = false;
            std::vector<Value> dummyVector{dummyVale1, dummyVale2};
            r_value._values = &dummyVector;
            r_value_type = ValueType::array;
        }
        else
        {
            r_value_type = ValueType::unknown;
        }
        return std::pair<Value, ValueType>{r_value, r_value_type};
    }
};
