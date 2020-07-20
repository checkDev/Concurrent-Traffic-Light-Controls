#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);
    
private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _messages;
};



enum TrafficLightPhase{
    red,
    green
};

class TrafficLight :public TrafficObject
{
public:
    // constructor / desctructor

    // getters / setters
    TrafficLight();

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    TrafficLightPhase _currentPhase;
    void cycleThroughPhases();

    
    std::shared_ptr<MessageQueue<TrafficLightPhase>> sp;//(new MessageQueue<TrafficLightPhase>);
    
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif
