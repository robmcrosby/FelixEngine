//
//  EventHandler.cpp
//  FelixEngine
//
//  Created by Robert Crosby on 9/3/17.
//  Copyright © 2017 Robert Crosby. All rights reserved.
//

#include "EventHandler.h"

namespace fx {
  struct EventListener {
    EventHandler *handler;
    int count;
    
    EventListener(EventHandler *handler = nullptr): handler(handler), count(1) {}
    void setEmpty() {
      handler = nullptr;
      release();
    }
    void retain() {++count;}
    void release() {
      if (--count <= 0)
        delete this;
    }
    bool isEmpty() const {return handler == nullptr;}
    void handle(const Event &event) {
      if (handler != nullptr && handler->eventFlags() & event.catagory)
        handler->handle(event);
    }
  };
}


using namespace std;
using namespace fx;

EventHandler::EventHandler() {
  _listener = new EventListener(this);
  _eventFlags = EVENT_ALL;
}

EventHandler::EventHandler(const EventHandler &other) {
  _listener = new EventListener(this);
  *this = other;
}

EventHandler::~EventHandler() {
  clearListeners();
  _listener->setEmpty();
}

EventHandler& EventHandler::operator=(const EventHandler &other) {
  _eventFlags = other._eventFlags;
  return *this;
}

void EventHandler::handle(const Event &event) {
  
}

void EventHandler::notify(Event event) {
  event.sender = this;
  EventListeners::iterator itr = _listeners.begin();
  while (itr != _listeners.end()) {
    EventListener *listener = *itr;
    if (listener->isEmpty()) {
      itr = _listeners.erase(itr);
      listener->release();
    }
    else {
      listener->handle(event);
      ++itr;
    }
  }
}

void EventHandler::addListener(EventHandler *handler) {
  if (handler == this)
    return;
  for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr) {
    if (*itr == handler->_listener)
      return;
  }
  handler->_listener->retain();
  _listeners.push_back(handler->_listener);
}

void EventHandler::removeListener(EventHandler *handler) {
  for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr) {
    EventListener *listner = *itr;
    if (listner == handler->_listener) {
      _listeners.erase(itr);
      listner->release();
      return;
    }
  }
}

void EventHandler::clearListeners() {
  for (EventListeners::iterator itr = _listeners.begin(); itr != _listeners.end(); ++itr)
    (*itr)->release();
  _listeners.clear();
}

