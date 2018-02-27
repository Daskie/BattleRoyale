// allows program to be run on dedicated graphics processor for laptops with
// both integrated and dedicated graphics using Nvidia Optimus
#ifdef _WIN32
extern "C" {
    _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
}
#endif

#include <iostream>

#include "glm/gtx/transform.hpp"

#include "EngineApp/EngineApp.hpp"
#include "LevelBuilder/FileReader.hpp"

void printUsage() {
    std::cout << "Valid arguments: " << std::endl;

    std::cout << "\t-h\n\t\tPrint help" << std::endl;
    
    std::cout << "\t-v\n\t\tSet verbose nature logging" << std::endl;

    std::cout << "\t-r <resource_dir>" << std::endl;
    std::cout << "\t\tSet the resource directory" << std::endl;

    std::cout << "\t-n <application_name>" << std::endl;
    std::cout << "\t\tSet the application name" << std::endl;
}

int parseArgs(int argc, char **argv) {
    /* Process cmd line args */
    for (int i = 0; i < argc; i++) {
        /* Help */
        if (!strcmp(argv[i], "-h")) {
            printUsage();
            return 1;
        }
        /* Verbose */
        if (!strcmp(argv[i], "-v")) {
            EngineApp::verbose = true;
        }
        /* Set resource dir */
        if (!strcmp(argv[i], "-r")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::RESOURCE_DIR = argv[i + 1];
        }
        /* Set application name */
        if (!strcmp(argv[i], "-n")) {
            if (i + 1 >= argc) {
                printUsage();
                return 1;
            }
            EngineApp::APP_NAME = argv[i + 1];
        }
    }
    return 0;
}

GameObject & createProjectile(const glm::vec3 & initPos, const glm::vec3 & initVel, bool gravity) {
    GameObject & obj(Scene::createGameObject());
    SpatialComponent & spat(Scene::addComponent<SpatialComponent>(obj));
    spat.setPosition(initPos);
    BounderComponent & bounder(Scene::addComponentAs<SphereBounderComponent, BounderComponent>(obj, 1, Sphere(glm::vec3(), 0.1f)));
    NewtonianComponent & newt(Scene::addComponent<NewtonianComponent>(obj));
    if (gravity) Scene::addComponentAs<GravityComponent, AcceleratorComponent>(obj);
    newt.addVelocity(initVel);
    return obj;
}

int main(int argc, char **argv) {
    if (parseArgs(argc, argv) || EngineApp::init()) {
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }

    Window::setCursorEnabled(false);

    GameObject & imguiGO(Scene::createGameObject());

    /* Create diffuse shader */
    glm::vec3 lightPos(100.f, 100.f, 100.f);
    if (!RenderSystem::createShader<DiffuseShader>(
            "diffuse_vert.glsl",    /* Vertex shader file       */
            "diffuse_frag.glsl",    /* Fragment shader file     */
            lightPos                /* Shader-specific uniforms */
        )) {
        std::cerr << "Failed to add diffuse shader" << std::endl;
        std::cin.get(); // don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Diffuse Shader ImGui Pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Diffuse Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<DiffuseShader>()->toggleEnabled();
            }
            if (ImGui::Button("Wireframe")) {
                RenderSystem::getShader<DiffuseShader>()->toggleWireFrame();
            }
            if (ImGui::Button("Toon")) {
                RenderSystem::getShader<DiffuseShader>()->toggleToon();
            }
            if (RenderSystem::getShader<DiffuseShader>()->isToon()) {
                float angle = RenderSystem::getShader<DiffuseShader>()->getSilAngle();
                ImGui::SliderFloat("Silhouette Angle", &angle, 0.f, 1.f);
                RenderSystem::getShader<DiffuseShader>()->setSilAngle(angle);
                
                int cells = int(RenderSystem::getShader<DiffuseShader>()->getCells());
                ImGui::SliderInt("Cells", &cells, 0, 15);
                RenderSystem::getShader<DiffuseShader>()->setCells(float(cells));
            }
        }
    );

    // Create collider
    // alternate method using unique_ptr and new
    if (!RenderSystem::createShader<BounderShader>("bounder_vert.glsl", "bounder_frag.glsl")) {
        std::cerr << "Failed to add collider shader" << std::endl;
        std::cin.get(); //don't immediately close the console
        return EXIT_FAILURE;
    }
    /* Collider ImGui pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Bounder Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<BounderShader>()->toggleEnabled();
            }
        }
    );
    
    // Ray shader (for testing)
    if (!RenderSystem::createShader<RayShader>("ray_vert.glsl", "ray_frag.glsl")) {
        std::cerr << "Failed to add ray shader" << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    }
    // Ray shader toggle
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Ray Shader",
        [&]() {
            if (ImGui::Button("Active")) {
                RenderSystem::getShader<RayShader>()->toggleEnabled();
            }
        }
    );

    /* Set Gravity */
    SpatialSystem::setGravity(glm::vec3(0.0f, -10.0f, 0.0f));

    /* Setup Player */
    float playerFOV(45.0f);
    float playerNear(0.1f);
    float playerFar(300.0f);
    float playerHeight(1.75f);
    float playerWidth(playerHeight / 4.0f);
    glm::vec3 playerPos(0.0f, 6.0f, 0.0f);
    float playerLookSpeed(0.005f);
    float playerMoveSpeed(5.0f);
    float playerJumpSpeed(5.0f);
    GameObject & player(Scene::createGameObject());
    SpatialComponent & playerSpatComp(Scene::addComponent<SpatialComponent>(player));
    playerSpatComp.setPosition(playerPos);
    NewtonianComponent & playerNewtComp(Scene::addComponent<NewtonianComponent>(player));
    GravityComponent & playerGravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(player));
    GroundComponent & playerGroundComp(Scene::addComponent<GroundComponent>(player));
    Capsule playerCap(glm::vec3(), playerHeight - 2.0f * playerWidth, playerWidth);
    CapsuleBounderComponent & playerBoundComp(Scene::addComponentAs<CapsuleBounderComponent, BounderComponent>(player, 5, playerCap));
    CameraComponent & playerCamComp(Scene::addComponent<CameraComponent>(player, playerFOV, playerNear, playerFar));
    PlayerControllerComponent & playerContComp(Scene::addComponent<PlayerControllerComponent>(player, playerLookSpeed, playerMoveSpeed, playerJumpSpeed));

    /* Setup Camera */
    float freeCamFOV(playerFOV);
    float freeCamNear(playerNear);
    float freeCamFar(playerFar);
    float freeCamLookSpeed(playerLookSpeed);
    float freeCamMoveSpeed(playerMoveSpeed);
    GameObject & freeCam(Scene::createGameObject());
    SpatialComponent & freeCamSpatComp(Scene::addComponent<SpatialComponent>(freeCam));
    CameraComponent & freeCamCamComp(Scene::addComponent<CameraComponent>(freeCam, freeCamFOV, freeCamNear, freeCamFar));
    CameraControllerComponent & freeCamContComp(Scene::addComponent<CameraControllerComponent>(freeCam, freeCamLookSpeed, freeCamMoveSpeed));
    freeCamContComp.setEnabled(false);

    RenderSystem::setCamera(&playerCamComp);

    // Toggle free camera (ctrl-tab)
    auto camSwitchCallback([&](const Message & msg_) {
        static bool free = false;

        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_TAB && msg.action == GLFW_PRESS && msg.mods & GLFW_MOD_CONTROL) {
            if (free) {
                // disable camera controller
                freeCamContComp.setEnabled(false);
                // enable player controller
                playerContComp.setEnabled(true);
                RenderSystem::setCamera(&playerCamComp);
            }
            else {
                // disable player controller
                playerContComp.setEnabled(false);
                // enable camera object
                freeCamContComp.setEnabled(true);
                // set camera object camera to player camera
                freeCamSpatComp.setPosition(playerSpatComp.position());
                freeCamSpatComp.setUVW(playerSpatComp.u(), playerSpatComp.v(), playerSpatComp.w());
                freeCamCamComp.lookInDir(playerCamComp.getLookDir());
                RenderSystem::setCamera(&freeCamCamComp);
            }
            free = !free;
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, camSwitchCallback);

    /* VSync ImGui Pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "VSync",
        [&]() {
            if (ImGui::Button("VSync")) {
                Window::toggleVSync();
            }
        }
    );

    /*Parse and load json level*/
    FileReader fileReader;
    const char *levelPath = "../resources/GameLevel_02.json";
    fileReader.loadLevel(*levelPath);

    /* Create bunny */
    Mesh * bunnyMesh(Loader::getMesh("bunny.obj"));
    for (int i(0); i < 0; ++i) {
        GameObject & bunny(Scene::createGameObject());
        SpatialComponent & bunnySpatComp(Scene::addComponent<SpatialComponent>(
            bunny,
            glm::vec3(-10.0f, 5.0, i), // position
            glm::vec3(0.25f), // scale
            glm::mat3() // rotation
        ));
        NewtonianComponent & bunnyNewtComp(Scene::addComponent<NewtonianComponent>(bunny));
        GravityComponent & bunnyGravComp(Scene::addComponentAs<GravityComponent, AcceleratorComponent>(bunny));
        BounderComponent & bunnyBoundComp(CollisionSystem::addBounderFromMesh(bunny, 5, *bunnyMesh, false, true, false));
        DiffuseRenderComponent & bunnyDiffuse = Scene::addComponent<DiffuseRenderComponent>(
            bunny,
            RenderSystem::getShader<DiffuseShader>()->pid,
            *bunnyMesh,
            ModelTexture(0.3f, glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f)), 
            true);
        PathfindingComponent & bunnyPathComp(Scene::addComponent<PathfindingComponent>(bunny, player, 1.0f));
    }

    /* Game stats pane */
    Scene::addComponent<ImGuiComponent>(
        imguiGO,
        "Stats",
        [&]() {
            ImGui::Text("FPS: %d", EngineApp::fps);
            ImGui::Text("dt: %f", EngineApp::timeStep);
            ImGui::Text("Player Pos: %f %f %f", playerSpatComp.position().x, playerSpatComp.position().y, playerSpatComp.position().z);
        }
    );

    // Demo ray picking and intersection (cntrl-click)
    int rayDepth(100);
    Vector<glm::vec3> rayPositions;
    auto rayPickCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            rayPositions.clear();
            rayPositions.push_back(playerSpatComp.position());
            glm::vec3 dir(playerCamComp.getLookDir());
            for (int i(0); i < rayDepth; ++i) {
                auto pair(CollisionSystem::pick(Ray(rayPositions.back(), dir), &player));
                if (!pair.second.is) {
                    break;
                }
                rayPositions.push_back(pair.second.pos);
                dir = glm::reflect(dir, pair.second.norm);
            }
            RenderSystem::getShader<RayShader>()->setPositions(rayPositions);
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, rayPickCallback);

    // Swap gravity (ctrl-g)
    auto gravSwapCallback([&](const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_G && msg.action == GLFW_PRESS && msg.mods == GLFW_MOD_CONTROL) {
            SpatialSystem::setGravity(-SpatialSystem::gravity());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, gravSwapCallback);

    // Fire projectile (click)
    float projectileSpeed(50.0f);
    Vector<GameObject *> projectiles;
    auto fireCallback([&](const Message & msg_) {
        const MouseMessage & msg(static_cast<const MouseMessage &>(msg_));
        if (msg.button == GLFW_MOUSE_BUTTON_1 && !msg.mods && msg.action == GLFW_PRESS) {
            projectiles.push_back(&createProjectile(
                playerSpatComp.position() + playerCamComp.getLookDir() * 2.0f,
                playerCamComp.getLookDir() * projectileSpeed,
                true
            ));
        }
        if (msg.button == GLFW_MOUSE_BUTTON_2 && msg.action == GLFW_PRESS) {
            for (auto & proj : projectiles) {
                Scene::destroyGameObject(*proj);
            }
            projectiles.clear();
        }
    });
    Scene::addReceiver<MouseMessage>(nullptr, fireCallback);

    // Delete scene element (delete) or delete all but (ctrl-delete)
    auto deleteCallback([&] (const Message & msg_) {
        const KeyMessage & msg(static_cast<const KeyMessage &>(msg_));
        if (msg.key == GLFW_KEY_DELETE && msg.mods & GLFW_MOD_CONTROL && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(playerSpatComp.position(), playerCamComp.getLookDir()), &player));
            if (pair.first) {
                for (GameObject * obj : Scene::getGameObjects()) {
                    if (obj != &pair.first->gameObject() && obj != &player && obj != &freeCam) {
                        Scene::destroyGameObject(*obj);
                    }
                }
            }
        }
        else if (msg.key == GLFW_KEY_DELETE && msg.action == GLFW_PRESS) {
            auto pair(CollisionSystem::pick(Ray(playerSpatComp.position(), playerCamComp.getLookDir()), &player));
            if (pair.first) Scene::destroyGameObject(pair.first->gameObject());
        }
    });
    Scene::addReceiver<KeyMessage>(nullptr, deleteCallback);

    /* Main loop */
    EngineApp::run();

    return EXIT_SUCCESS;
}


