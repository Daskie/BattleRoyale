#pragma once
#ifndef _GAME_SYSTEM_HPP_
#define _GAME_SYSTEM_HPP_



#include "System.hpp"
#include "Model/Material.hpp"
#include "Component/Components.hpp"



struct GameInterface;



// static class
class GameSystem {

    friend Scene;
    friend GameInterface;

    public:

    enum class Culture { none, american, asian, italian };

    private:

    //--------------------------------------------------------------------------
    // Lighting

    struct Lighting {

        static const float k_defAmbience;
        static const glm::vec3 k_defLightDir;

    };

    //--------------------------------------------------------------------------
    // Player

    struct Player {

        static const glm::vec3 k_position;
        static const float k_height;
        static const float k_width;
        static const unsigned int k_weight;
        static const float k_lookSpeed;
        static const float k_moveSpeed;
        static const float k_sprintSpeed;
        static const float k_jumpSpeed;
        static const float k_fov;
        static const float k_near;
        static const float k_far;
        static const float k_maxHP;
        static const glm::vec3 k_playerPosition;
        static const glm::vec3 k_mainHandPosition;

        static GameObject * gameObject;
        static SpatialComponent * bodySpatial;
        static SpatialComponent * headSpatial;
        static NewtonianComponent * newtonian;
        static CapsuleBounderComponent * bounder;
        static CameraComponent * camera;
        static PlayerControllerComponent * controller;
        static PlayerComponent * playerComp;
        static HealthComponent * health;
        static AmmoComponent * ammo;
        static SpatialComponent * handSpatial;
        static DiffuseRenderComponent * handDiffuse;

        static void init();

        static void restore();

    };

    //--------------------------------------------------------------------------
    // Enemies

    struct Enemies {

        struct Basic {

            static const String k_bodyMeshName;
            static const String k_headMeshName;
            static const String k_textureName;
            static const glm::vec3 k_headPosition;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_moveSpeed;
            static const float k_maxHP;
            static const float k_meleeDamage;

            static void create(const glm::vec3 & position, const float speed, const float hp);

            static void spawn();

        };

        static void killAll();

        static void enablePathfinding();

        static void disablePathfinding();

    };

    //--------------------------------------------------------------------------
    // Waves 

    struct Wave {

        static const Vector<glm::vec3> k_spawnPoints;
        static int waveNumber;
        static float spawnCooldown;
        static float spawnPeriod;
        static int enemiesLeft;
        static float enemyHealth;
        static float enemySpeed;

        static glm::vec3 randomSpawnPoint();
        static int computeEnemiesInWave();
        static float computeEnemyHealth();
        static float computeEnemySpeed();
        static float computeSpawnTimerMax();

        static void next();

        static void spawnAny(float dt);

        static void end();

        static bool finished();

    };

    //--------------------------------------------------------------------------
    // Weapons

    struct Weapons {

        struct PizzaSlice {

            static const String k_meshName;
            static const String k_texName;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_speed; 
            static const float k_damage;
            static const int k_ammo;

            static GameObject * fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel, const glm::quat & orient);
            static GameObject * fireFromPlayer();

            static void equip();
            
            static void unequip();

        };

        struct SodaGrenade {

            static const String k_meshName;
            static const String k_texName;
            static const bool k_isToon;
            static const glm::vec3 k_scale;
            static const unsigned int k_weight;
            static const float k_speed; 
            static const float k_damage;
            static const float k_radius;
            static const int k_ammo;

            static GameObject * fire(const glm::vec3 & initPos, const glm::vec3 & initDir, const glm::vec3 & srcVel);
            static GameObject * fireFromPlayer();

            static void equip();
            
            static void unequip();

        };

        struct SrirachaBottle {

            static const float k_damage;
            static const float k_radius;
            static const float k_ammo;

            static GameObject * playerSriracha;

            static GameObject * start(const SpatialComponent & hostSpatial, const glm::vec3 & offset = glm::vec3());

            static void toggleForPlayer();

            static void updatePlayers(float dt);

            static void equip();
            
            static void unequip();

        };

        static void destroyAllWeapons();

    };

    //--------------------------------------------------------------------------
    // Shops

    struct Shops {

        struct American {

            static BounderComponent * bounder;
            static bool isOpen;

            static void init();

            static void open();

            static void close();

        };

        struct Asian {

            static BounderComponent * bounder;
            static bool isOpen;

            static void init();

            static void open();

            static void close();

        };

        struct Italian {

            static BounderComponent * bounder;
            static bool isOpen;

            static void init();

            static void open();

            static void close();

        };

        static const float k_rotationPeriod;

        static float rotationCooldown;

        static void init();

        static void update(float dt);

        static void openAll();

        static void openRandom();

        static int numOpen();
        static int numClosed();
        static int numAbleToOpen();

        static bool isOpen(Culture shop);

        static void close(Culture shop);

    };

    //--------------------------------------------------------------------------
    // Freecam

    struct Freecam {

        static const float k_minSpeed;
        static const float k_maxSpeed;

        static GameObject * gameObject;
        static SpatialComponent * spatialComp;
        static CameraComponent * cameraComp;
        static CameraControllerComponent * controllerComp;

        static void init();

    };

    //--------------------------------------------------------------------------
    // Music

    struct Music {

        static const String k_defMusic;
        static const String k_american, k_asian, k_italian;

        static bool s_playing;

        static void stop();

        static void start();

        static void toggle();

        static void set();

    };
    
    //--------------------------------------------------------------------------

    public:

    static void init();

    static void update(float dt);

    private:

    static void updateGame(float dt);

    static void setCulture(Culture culture);

    static void equipWeapon();
    static void unequipWeapon();

    static void startWave();
    static void endWave();

    static void setupMessageCallbacks();

    static void setupImGui();

    private:

    static const Vector<CameraComponent *> & s_cameraComponents;
    static const Vector<CameraControllerComponent *> & s_cameraControllerComponents;
    static const Vector<PlayerControllerComponent *> & s_playerControllers;
    static const Vector<ImGuiComponent *> & s_imguiComponents;
    static const Vector<PlayerComponent *> & s_playerComponents;
    static const Vector<EnemyComponent *> & s_enemyComponents;
    static const Vector<ProjectileComponent *> & s_projectileComponents;
    static const Vector<BlastComponent *> & s_blastComponents;
    static const Vector<MeleeComponent *> & s_meleeComponents;

    static const glm::vec3 k_defGravity;
    static const float k_restTime;

    static Culture s_culture;
    static bool s_changeCulture;
    static Culture s_newCulture;
    static bool s_useWeapon;
    static bool s_unuseWeapon;
    static Culture s_shopVisited;
    static bool s_resting;
    static float s_restCooldown;

};

#endif