//
// Created by Bob
//
#include <random>

#ifndef GAMEAUDIOENGINE_RANDOMNUMBERGEN_H
#define GAMEAUDIOENGINE_RANDOMNUMBERGEN_H

// Probably I should include in here functions for random int (in declared range or not)
// Also a (template) function that can accept float or double as parameters (in range or not)
class RandomNumberGen{
public:
    RandomNumberGen (const RandomNumberGen&) = delete; // delete copy constructor
    const RandomNumberGen& operator=(const RandomNumberGen&) = delete; // delete assignment operator

    static std::mt19937& getGen() {return getInst().getGenImpl();} // retrieve the generator instance as alias/address
                                                                    // OR retrieve a pointer to the instance of the generator

private:
    RandomNumberGen() = default;

    static RandomNumberGen& getInst() {
        static RandomNumberGen instance;
        return instance;
    }

    static std::mt19937& getGenImpl() {
        static std::random_device rd; // obtain a random number from hardware
        static std::mt19937 gen(rd()); // seed the generator/engine
//        static std::mt19937 gen((std::random_device() () )); // alternative
        return gen;
    }
};

#endif //GAMEAUDIOENGINE_RANDOMNUMBERGEN_H
