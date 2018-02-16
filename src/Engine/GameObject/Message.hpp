/* Message struct used to communicate data between components */
#pragma once
#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_



#include "glm/glm.hpp"
// This file shouldn't contain any functionality, and it will be included all
// over the place. Forward declare, don't include (if possible).



//==============================================================================
// README !!!
//------------------------------------------------------------------------------ 
// A message can be sent from anywhere using Scene's sendMessage method, which
// looks like...
//
//     Scene::get().sendMessage<MessageType>([nullptr | gameobject], message args);
//
// This message is sent out to all receivers of the specified message type. If
// gameobject is not null, the message is ALSO sent to any receiver of that
// game object and message type. This is for efficient inter-component
// communication.
//
// The scene keeps a list of messages, and what type they were, until it's time
// to relay the messages. This happens before and after every system update.
// The system will only relay messages to any receivers of the corresponding
// type. A receiver is a std::function<void (const Message &)> . To add a
// receiver, do...
//
//     Scene::get().addReceiver<MessageType>([nullptr | gameobject], receiver);
//
// Here, if gameobject is null, the receiver will receive all messages of the
// specified type. If gameobject is not null, the receiver will only receive
// messages of the specified type that have been sent to that object. This is
// how you do efficient inter-component communication.
//
// For receiver, can pass either a function pointer or a lambda. Note, you can
// technically bind a method to a std::function, but it's ugly. For adding
// receivers from objects that can reference that object, I reccommend using a
// lambda, like so...
//    
//    auto receiver = [&](const Message & msg_) {
//        const MessageIWantType & msg(static_cast<const MessageIWantType &>(msg_));
//        ...
//    };
//    Scene::get().addReceiver<MessageIWantType>(receiver);
//
// There is also the idea of a Tag, which is simply a way for messages of
// different types but sharing the same data to be accessed. For an example of
// this, take a look in CollisionSystem's init method. Tags must not be used as
// message types in sendMessage or addReceiver! They are only helpers.
//------------------------------------------------------------------------------



class GameObject;
class SpatialComponent;
class BounderComponent;



struct Message {};



// a spatial was transformed in some way
// tag for the following six messages
// Don't use this as a message type!!
struct SpatialTransformTag : public Message {
    const SpatialComponent & spatial;
    SpatialTransformTag(const SpatialComponent & spatial) : spatial(spatial) {}
};

// a spatial's location was set
struct SpatialPositionSetMessage :  public SpatialTransformTag {
    SpatialPositionSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was moved
struct SpatialMovedMessage :  public SpatialTransformTag {
    SpatialMovedMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial's scale was set
struct SpatialScaleSetMessage :  public SpatialTransformTag {
    SpatialScaleSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was scaled
struct SpatialScaledMessage :  public SpatialTransformTag {
    SpatialScaledMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial's rotation was set
struct SpatialRotationSetMessage :  public SpatialTransformTag {
    SpatialRotationSetMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};

// a spatial was rotated
struct SpatialRotatedMessage :  public SpatialTransformTag {
    SpatialRotatedMessage(const SpatialComponent & spatial) : SpatialTransformTag(spatial) {}
};



// a collision occurred between the two bounders
// there may not necessarily have been any adjustment
// two messages are sent per pair of bounders, where the order of bounders is swapped
struct CollisionMessage : public Message {
    const BounderComponent & bounder1, & bounder2;
    CollisionMessage(const BounderComponent & bounder1, const BounderComponent & bounder2) : bounder1(bounder1), bounder2(bounder2) {}
};

// a collision occurred and the game object's position was adjusted
struct CollisionAdjustMessage : public Message {
    const GameObject & gameObject;
    CollisionAdjustMessage(const GameObject & gameObject) : gameObject(gameObject) {}
};



// the window was resized
struct WindowSizeMessage : public Message {
    int width, height;
    WindowSizeMessage(int width, int height) : width(width), height(height) {}
};



// the near/far planes were changed
struct NearFarMessage : public Message {
    float near, far;
    NearFarMessage(float near, float far) : near(near), far(far) {}
};



// key input
struct KeyMessage : public Message {
    int key, action, mods;
    KeyMessage(int key, int action, int mods) : key(key), action(action), mods(mods) {}
};



#endif