#ifndef FINALDAYONEARTH_ECS_H
#define FINALDAYONEARTH_ECS_H

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include "EntityComponents.h"

#define mapType(x, obj) static_cast<std::unordered_map<std::string, std::vector<std::vector<x>>>*>(obj);
#define AOSType(x, obj) static_cast<std::vector<PackedStruct<x>>*>(obj);

struct Future_Version_EntityComponentStorage {
    // Components that are used in more than one combination (including by itself)

    // SOA format - Indices are aligned across component vectors
    std::unordered_map<std::string, void*> compoundComponents = { // not necessary, but demonstrates structure
            {"positions", new std::unordered_map<std::string, std::vector<std::vector<glm::vec2>>>()},
            {"bounds", new std::unordered_map<std::string, std::vector<std::vector<EntityBoundsC>>>()},
    };

    // AOS format
    std::unordered_map<std::string,void*> compoundComponentsSqueezed;

    // Components that are only used in one combination (including by itself)
    std::vector<glm::vec2> entityVelocities;
    std::vector<EntityCasterC> entityCaster;
    std::vector<EntityExplosionC> entityExplosion;

    // Will break with duplicate names or inconsistent typing
    template <typename T>
    void addCompoundComponent(std::vector<std::string> comps, std::vector<T> data) {
        std::sort(comps.begin(), comps.end());
        std::string description = mergeVectorOfStrings(comps, "+");
        for(int i = 0; i < comps.size(); i++) {
            auto& map = *mapType(T, compoundComponents[comps[i]]);
            auto& vec = map[description];
            std::copy(vec.end(), data.begin(), data.end());

            // Add references to higher and lower level groups
            std::vector<std::vector<T>> pullUp;
            for(auto& [ex, v] : map){
                if(ex == description)
                    continue;
                std::vector<std::string> ec = split(ex, '+');
                if(containsAll(ec, comps)){
                    v.push_back(vec);
                } else if(containsAll(comps, ec)) {
                    pullUp.push_back(v);
                }
            }
            //Avoids duplication
            for(auto& v : pullUp){
                vec.push_back(v);
            }
        }
    }

    template<typename T>
    std::vector<std::vector<T>> getCompoundComponentSOA(std::string component, std::string groupComponents) {
        auto& map = mapType(T, compoundComponents[component]);
        return &map[groupComponents];
    }

    //WIP
    /*template<typename ...T>
    std::vector<int> getCompoundComponentAOS(std::string groupComponents) {
        auto& map = AOSType(...T, compoundComponentsSqueezed[groupComponents]);
        return &map;
    }

    template<typename ...T>
    void pullCompoundComponentAOS(std::string groupComponents) {
        auto& map = AOSType(...T, compoundComponents[groupComponents]);
        std::vector<std::string> comps = split(groupComponents, '+');


    }

    template<typename T>
    void pushCompoundComponentAOS(std::string groupComponents) {
        auto& map = AOSType(...T, compoundComponents[groupComponents]);
        return &map;
    } */

    bool containsAll(std::vector<std::string> base, std::vector<std::string> sub) {
        for(std::string s : sub) {
            int i = 0;
            for(;i < base.size(); i++){
                if(base[i] == s)
                    break;
            }
            if(i != base.size())
                return false;
        }
        return true;
    }

    std::string mergeVectorOfStrings(const std::vector<std::string>& strings, const std::string& separator = "") {
        std::string result = "";
        if (strings.empty()) {
            return result;
        }

        result += strings[0]; // Start with the first string

        for (size_t i = 1; i < strings.size(); ++i) {
            result += separator;
            result += strings[i];
        }
        return result;
    }

    std::vector<std::string> split(std::string string, char delimiter){
        std::istringstream tokenStream(string);
        std::string s;
        std::vector<std::string> out;
        while (std::getline(tokenStream, s, delimiter))
            out.push_back(s);
        return out;
    }

};

// COPIED
#include <tuple>
#include <cstddef>
#include <new>
#include <memory>
#include <type_traits>
#include <cstring>
/*
template<typename... T>
struct PackedStruct {
    alignas(std::max({alignof(T)...})) std::byte buffer[sizeof(T) + ...];

    std::tuple<T*...> ptrs;

    PackedStruct(T&&... args) {
        std::byte* current = buffer;
        ptrs = std::make_tuple(construct<T>(current, std::forward<T>(args))...);
    }

    ~PackedStruct() {
        std::apply([](auto*... ptrs) {
            (destroy(ptrs), ...);
        }, ptrs);
    }

private:
    template<typename U>
    static U* construct(std::byte*& pos, U&& value) {
        void* ptr = pos;
        U* obj = new (ptr) U(std::forward<U>(value));
        pos += sizeof(U);
        return obj;
    }

    template<typename U>
    static void destroy(U* ptr) {
        ptr->~U();
    }
};
*/
#endif //FINALDAYONEARTH_ECS_H
