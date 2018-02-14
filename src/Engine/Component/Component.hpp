#pragma once
#ifndef _COMPONENT_HPP_
#define _COMPONENT_HPP_

#include "GameObject/GameObject.hpp"
#include "GameObject/Message.hpp"

class Scene;

class Component {

    friend Scene;

    protected: // only scene can create components

        Component() : gameObject(nullptr) {};

        // TODO: potentially add move support
        Component(const Component & other) = default;
        Component & operator=(const Component & other) = default;

    public:

        /* virtual destructor necessary for polymorphic destruction */
        virtual ~Component() = default;

        virtual void init() {};
        virtual void update(float) {};

        GameObject * getGameObject() { return gameObject; }
        const GameObject * getGameObject() const { return gameObject; }

        void setGameObject(GameObject *go) { this->gameObject = go; }

        /* Receive a message sent by another component */
        //virtual void receiveMessage(Message &);



    //protected:
        /* Parent game object */
        GameObject* gameObject;
};

#endif