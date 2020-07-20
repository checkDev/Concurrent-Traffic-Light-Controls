#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */
#include <chrono>
#include <random>
#include <future>

class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const {
        return std::chrono::duration_cast<std::chrono::seconds>
        (clock_::now() - beg_).count(); }
    
private:
    typedef std::chrono::high_resolution_clock clock_;
   // typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

template <typename T>
T MessageQueue<T>::receive()
{

    std::unique_lock<std::mutex> uLock(_mutex);
    _cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable
    
    // remove last vector element from queue
    T msg = std::move(_messages.back());
    _messages.pop_back();
    
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    
    
        // simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);
        
        // add vector to queue
        std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
        _messages.push_back(std::move(msg));
        _cond.notify_one(); // notify client after pushing new Vehicle into vector
    
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    sp = std::make_shared<MessageQueue<TrafficLightPhase>>();
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
  
    while(true)
    {
        auto message = sp->receive();
        if(message == TrafficLightPhase::green)
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases ,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
   
    Timer timer;
    //std::vector<std::future<void>> futures;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<>distr(4,6); //4 to 6 seconds
    int sleepinseconds = distr(eng);
  
    while(true)
    {
         std::cout << "toggling lights to...sleepinseconds  " << sleepinseconds  << " time elapsed "<< timer.elapsed() << std::endl;
        
        if(timer.elapsed() < sleepinseconds)
            continue;
        timer.reset();
        _currentPhase = (_currentPhase == TrafficLightPhase::red ) ? TrafficLightPhase::green : TrafficLightPhase::red;
    
       
        distr(eng);
        std::cout << "toggling lights to... " << _currentPhase << std::endl;
       // auto  message = _currentPhase;
        /*25 May 2020*/
        //Ratul - understand the std::async (like thread spawning)-> MethodName , Object of the method to be passed like
        //queue->MessageQueue(..) and the method parameter to be passed to the MethodName.
        //futures.emplace_back(std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, sp, std::move(message)));
        sp->send(std::move(_currentPhase));
        sleepinseconds = distr(eng);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        
    }
}


