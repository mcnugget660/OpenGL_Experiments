#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>
#include <iostream>

#include "JsonReader.h"

JSON_OBJECT JsonReader::read(std::string path) {
    // !! COPIED
    auto size = std::filesystem::file_size(path);
    std::string content(size, '\0');
    std::ifstream in(path);
    in.read(&content[0], size);
    in.close();
    // !!
    content = std::regex_replace(content, std::regex("\\s+|\n"), "");
    return parseObject(content);
}

void JsonReader::parseField(std::string input, std::string name, JSON_OBJECT &obj) {
    bool isArray = input[0] == '[';
    int offset = isArray;

    DATA_TYPE type;
    if(input[offset] == '\"')
        type = DATA_TYPE::STRING;
    else if(input[offset] == '{')
        type = DATA_TYPE::OBJECT;
    else if(!input.compare(offset, 4, "true") || !input.compare(offset, 5, "false")){
        type = DATA_TYPE::BOOLEAN;
    } else {
        std::string possibleNumber = input.substr(offset, input.find(offset,'\n') - offset);
        try{
            std::stoi(possibleNumber);
            type = DATA_TYPE::INTEGER;
        } catch (const std::invalid_argument& e){
            try {
                std::stod(possibleNumber);
                type = DATA_TYPE::DOUBLE;
            } catch (const std::invalid_argument& e) {
                std::cout << "Error: " << input << " is not a valid field 3" << std::endl;
                return;
            }
        }
    }
    if(isArray) {
        std::vector<std::string> a1;
        std::vector<JSON_OBJECT> a2;
        std::vector<bool> a3;
        std::vector<int> a4;
        std::vector<double> a5;

        std::vector<char> stack;
        int start = offset;
        for(;offset < input.size() - 1; offset++) {
            if (input[offset] == '{'||  ((stack.size() == 0 || stack.back() != '\"') && input[offset] == '\"')) {
                if(stack.size() == 0)
                    start = offset;
                stack.push_back(input[offset]);
            } else if (input[offset] == '}' || input[offset] == '\"') {
                stack.pop_back();
                if(stack.size() == 0) {
                    if(type == DATA_TYPE::STRING)
                        a1.emplace_back(std::string(input.substr(start + 1, offset - start - 1)));
                    else if(type == DATA_TYPE::OBJECT)
                        a2.emplace_back(parseObject(input.substr(start, offset - start + 1)));
                    else if(type == DATA_TYPE::BOOLEAN)
                        a3.emplace_back(input.substr(start, offset - start + 1) == "true");
                    else if(type == DATA_TYPE::INTEGER)
                        a4.emplace_back(std::stoi(input.substr(start, offset - start + 1)));
                    else if(type == DATA_TYPE::DOUBLE)
                        a5.emplace_back(std::stod(input.substr(start, offset - start + 1)));
                }
            }
        }
        if(type == DATA_TYPE::STRING)
            obj.string_fields[name] = a1;
        else if(type == DATA_TYPE::OBJECT)
            obj.OBJECT_fields[name] = a2;
        else if(type == DATA_TYPE::BOOLEAN)
            obj.boolean_fields[name] = a3;
        else if(type == DATA_TYPE::INTEGER)
            obj.integer_fields[name] = a4;
        else if(type == DATA_TYPE::DOUBLE)
            obj.double_fields[name] = a5;

    } else if(type == DATA_TYPE::STRING){
        obj.string_fields[name] = std::vector(1, std::string(input.substr(1, input.size() - 2)));
    } else if(type == DATA_TYPE::OBJECT) {
        obj.OBJECT_fields[name] = std::vector(1, JSON_OBJECT(parseObject(input)));
    } else if(type == DATA_TYPE::BOOLEAN) {
        obj.boolean_fields[name] = std::vector(1, std::string(input.substr(0, input.size())) == "true");
    } else if(type == DATA_TYPE::INTEGER) {
        obj.integer_fields[name] = std::vector(1, std::stoi(std::string(input.substr(0, input.size()))));
    } else if(type == DATA_TYPE::DOUBLE) {
        obj.double_fields[name] = std::vector(1, std::stod(std::string(input.substr(0, input.size()))));
    }
}

//input includes {}
JSON_OBJECT JsonReader::parseObject(std::string input) {
    JSON_OBJECT obj;
    for(int i = 0; i < input.size() && i > -1; i = input.find("\"", i)) {
        i = input.find("\"", i);
       int end = input.find("\"", i + 1);
       std::string name = input.substr(i + 1, end - i - 1);
       if(input[end + 1] != ':') {
           std::cout << "Error: " << name << " is not a valid field 1" << std::endl;
       }
       int start = end + 2;
       std::vector<char> stack;
       int j = start;
       for(; j < input.size() - 1; j++) {
           if(input[j] == '{'|| input[j] == '[' || ((stack.size() == 0 ||  stack.back() != '\"') && input[j] == '\"')) {
               stack.push_back(input[j]);
           } else if(input[j] == '}' || input[j] == ']' || input[j] == '\"') {
               stack.pop_back();
               if(stack.size() == 0) {
                   parseField(input.substr(start, j - start + 1), name, obj);
                   i = j;
                   break;
               }
           } else if(input[j] == ',' || j == input.size() - 2) {
               if(stack.size() == 0) {
                   parseField(input.substr(start, j - start + (j == input.size() - 2)), name, obj); // Don't parse the comma
                   i = j;
                   break;
               }
           }
       }
       if(j != i)
       std::cout << "Error: " << name << " is not a valid field 2" << std::endl;
       i++;
    }
    return obj;
}
