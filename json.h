

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <memory>
#include "my_util.h"

class JsonObject;

enum ValueType // for identifying which value has been initialized
{
    boolean,
    integer,
    string,
    array,
    object,
    unknown, // unknown type
};

struct Value
{
    // Ponential values for Json data. Only one of those will be used
    bool _boolValue;
    int _intValue;
    std::string _stringValue;
    std::vector<std::shared_ptr<Value>> _values; // For Json arrays
    std::shared_ptr<JsonObject> _p_object;
    ValueType _valueType;

    Value() : _valueType{ValueType::unknown} {}
    Value(bool booleanValue) : _boolValue{booleanValue}, _valueType{ValueType::boolean} {}
    Value(int intValue) : _intValue{intValue}, _valueType{ValueType::integer} {}
    Value(std::string stringValue) : _stringValue{stringValue}, _valueType{ValueType::string} {}
    Value(std::vector<std::shared_ptr<Value>> values) : _values{values}, _valueType{ValueType::array} {}
    Value(std::shared_ptr<JsonObject> p_object) : _p_object{p_object}, _valueType{ValueType::object} {}
};

struct JsonData
{
    std::string _name;
    Value _value;

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
            _datas.push_back(jd);
        } while (ojbectStr.length() != 0);
    }

    int numbers() // how many int values
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::object)
                n += data._value._p_object->numbers();
            else if (data._value._valueType == ValueType::integer)
                ++n;
        }
        return n;
    }
    int texts() // how many text values
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::object)
                n += data._value._p_object->texts();
            else if (data._value._valueType == ValueType::string)
                ++n;
        }
        return n;
    }
    int booleans() // how many boolean values
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::object)
                n += data._value._p_object->booleans();
            else if (data._value._valueType == ValueType::boolean)
                ++n;
        }
        return n;
    }
    int lists() // how many lists
    {
        int n{0};
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::object)
                n += data._value._p_object->lists();
            else if (data._value._valueType == ValueType::array)
                ++n;
        }
        return n;
    }

    int getNumberValue(std::string valueName) // -> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::integer && data._name == valueName)
                return data._value._intValue;
            if (data._value._valueType == ValueType::object)
                return data._value._p_object->getNumberValue(valueName);
        }
        return -1;
    }
    std::string getTextValue(std::string valueName) //-> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::string && data._name == valueName)
                return data._value._stringValue;
            if (data._value._valueType == ValueType::object)
                return data._value._p_object->getTextValue(valueName);
        }
        return "NO CORRESPONDING TEXT!";
    }
    bool getBooleanValue(std::string valueName) //-> Palauttaa arvon, jonka nimi on sama kuin valueName muuttujan
    {
        for (JsonData data : _datas)
        {
            if (data._value._valueType == ValueType::boolean && data._name == valueName)
                return data._value._boolValue;
            if (data._value._valueType == ValueType::object)
                return data._value._p_object->getBooleanValue(valueName);
        }
        return false;
    }

private:
    std::vector<JsonData> _datas;
    enum IstreamState : char
    {
        string = '"',
        inObject = '{',
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
            case IstreamState::inObject:
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
            return Value{int_val};
            ;
        }
        else if (c == IstreamState::string) // if a string value
        {
            int i_str_end = findNthOccur(dataStr, IstreamState::string, 2);
            std::string str_value = dataStr.substr(i + 1, (i_str_end - i) - 1);
            return Value{str_value};
        }
        else if (dataStr[i] == 't' && dataStr[++i] == 'r' && dataStr[++i] == 'u' && dataStr[++i] == 'e') // if a boolean value true
        {
            return Value(true);
        }
        else if (dataStr[i] == 'f' && dataStr[++i] == 'a' && dataStr[++i] == 'l' && dataStr[++i] == 's' && dataStr[++i] == 'e') // if a boolean value false
        {
            return Value(false);
        }
        else if (c == IstreamState::inObject) // if an Json object value
        {
            std::shared_ptr<JsonObject> p_jo(new JsonObject(dataStr));
            return Value(p_jo);
        }
        else if (c == IstreamState::inArray) // if an array value
        {
            // std::vector<std::shared_ptr<Value>> arr;
            // std::string arrStr = dataStr.substr(i);
            // do
            // {
            //     dataStr = nextData(arrStr);
            //     std::cout << dataStr << std::endl;
            //     Value value = extractValue(dataStr);
            //     arr.push_back(std::make_shared<Value>(value));
            // } while (arrStr.length() != 0);
            // return arr;
            
            // FOR THE SAKE OF SIMPLICITY WE INGNORE ARRAYS AT THIS POINT! Use just dymmy array.
            std::shared_ptr<Value> p_value0(new Value{});
            std::shared_ptr<Value> p_value1(new Value{});
            std::shared_ptr<Value> p_value2(new Value{});
            std::vector<std::shared_ptr<Value>> dummyVector{p_value0, p_value1, p_value2};
            return Value{dummyVector};
        }
        else
        {
            return Value{};
        }
    }
};
