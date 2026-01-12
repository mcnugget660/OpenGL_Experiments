#ifndef FINALDAYONEARTH_JSONREADER_H
#define FINALDAYONEARTH_JSONREADER_H

#include <map>
#include <string>

enum DATA_TYPE {
    STRING,
    BOOLEAN,
    INTEGER,
    DOUBLE,
    OBJECT
};

struct JSON_OBJECT {
    std::vector<std::pair<DATA_TYPE, void *>> fields;
    std::map<std::string, std::vector<std::string>> string_fields;
    std::map<std::string, std::vector<bool>> boolean_fields;
    std::map<std::string, std::vector<int>> integer_fields;
    std::map<std::string, std::vector<double>> double_fields;
    std::map<std::string, std::vector<JSON_OBJECT>> OBJECT_fields;
};

class JsonReader {
public:
    static JSON_OBJECT read(std::string path);
private:
    static JSON_OBJECT parseObject(std::string input);
    static void parseField(std::string input, std::string name, JSON_OBJECT &obj);
};


#endif //FINALDAYONEARTH_JSONREADER_H
