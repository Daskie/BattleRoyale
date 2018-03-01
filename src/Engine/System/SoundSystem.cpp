#include "SoundSystem.hpp"

const Vector<SoundComponent *> & SoundSystem::s_soundComponents(Scene::getComponents<SoundComponent>());
String SoundSystem::s_SOUND_DIR = EngineApp::RESOURCE_DIR + "/soundeffects/";

#ifdef HAVE_FMOD_LIBRARY
FMOD::System* SoundSystem::s_system = NULL;
Map<String, FMOD::Sound*> SoundSystem::s_soundLibrary = Map<String, FMOD::Sound*>();
CameraComponent* SoundSystem::s_camera = NULL;
#endif

float count = 0.f;

void SoundSystem::init() {
#ifdef HAVE_FMOD_LIBRARY
    FMOD_RESULT result;

    result = FMOD::System_Create(&s_system);
    if (result != FMOD_OK)
    {
        printf("failed to create system");
    }

    result = s_system->init(36, FMOD_INIT_NORMAL, NULL);
    if (result != FMOD_OK) {
        printf("failed to initialize system\n");
    }

    float dopplescale = 1.0f;
    float distancefactor = 1.0f;
    float rolloffscale = 1.0f;
    result = s_system->set3DSettings(dopplescale, distancefactor, rolloffscale);
    if (result != FMOD_OK) {
        printf("failed to set up 3D settings\n");
    }

    initSoundLibrary();

#endif
    
}

void SoundSystem::update(float dt) 
{
#ifdef HAVE_FMOD_LIBRARY
    updateListener();
    s_system->update();
#endif
}

#ifdef HAVE_FMOD_LIBRARY
void SoundSystem::initSoundLibrary()
{   
    Vector<String> sfn = getSoundFilenames("filesinfolder.txt");
    for (auto &s : sfn) {
        FMOD::Sound* tempSound = createSound(s, FMOD_DEFAULT);
        s_soundLibrary[s + "2D"] = tempSound;
        tempSound = createSound(s, FMOD_3D);
        s_soundLibrary[s + "3D"] = tempSound;
    }
}

void SoundSystem::setCamera(CameraComponent *camera) {
    s_camera = camera;
}

Vector<String> SoundSystem::getSoundFilenames(String listname) {
    Vector<String> soundfilenames = Vector<String>();
    String line;
    Vector<String> validextensions = {
        ".wav", ".mp3", "aiff", "asf", "asx", "dls", "flac",
        "it", "m3u", "midi", "mod", "mp2", "mp3", "pls",
        "s3m", "wma", "xm"
    };
    String name = s_SOUND_DIR + listname;
    std::ifstream infile(name.c_str());

    while (std::getline(infile, line)) {
        for (auto ext : validextensions) {
            if (line.compare(line.length() - 4, 4, ext) == 0) {
                printf((line + " loaded\n").c_str());
                soundfilenames.push_back(line);
                break;  
            }
        }

    }

    return soundfilenames;
}

void SoundSystem::updateListener() {
    if (s_camera != NULL) {
        SpatialComponent* s = s_camera->gameObject().getSpatial();
        s_system->set3DListenerAttributes(0,
            fVec(s->position()), 
            NULL, 
            fVec(s_camera->getLookDir()), 
            fVec(s_camera->v())
        );
    }
}

FMOD_VECTOR* SoundSystem::fVec(glm::vec3 v) {
    FMOD_VECTOR *fv =  new FMOD_VECTOR();
    fv->x = v.x;
    fv->y = v.y;
    fv->z = v.z;
    return fv;
}

FMOD::Sound* SoundSystem::createSound(String soundfilename, FMOD_MODE m) 
{
    String fullpath = s_SOUND_DIR + soundfilename;
    const char* path = fullpath.c_str();	

    FMOD::Sound *sound;
    FMOD_RESULT result = s_system->createSound(path, m, 0, &sound);
    if (result != FMOD_OK) {
	    printf("Failed to create sound.\n");
    }

    return sound;
}

//play sound from resources/soundeffects by filename
void  SoundSystem::playSound(String fileName) {
    FMOD::Sound *sound;
    if (s_soundLibrary.count(fileName + "2D")) {
        sound = s_soundLibrary[fileName + "2D"];
    }
    else {
        sound = createSound(fileName + "2D", FMOD_DEFAULT);
    }
    FMOD_RESULT result = s_system->playSound(sound, NULL, false, NULL);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}

void SoundSystem::playSound3D(String fileName, glm::vec3 pos) {
    FMOD::Sound *sound;
    FMOD::Channel *newChannel;
    FMOD_VECTOR *fPos;

    if (s_soundLibrary.count(fileName + "3D")) {
        sound = s_soundLibrary[fileName + "3D"];
    }
    else {
        sound = createSound(fileName + "3D", FMOD_3D);
    }

    fPos = fVec(pos);
    FMOD_RESULT result = s_system->playSound(sound, NULL, true, &newChannel);
    newChannel->set3DAttributes(fPos, NULL, NULL);
    newChannel->setPaused(false);
    if (result != FMOD_OK) {
        printf("playSound() done goofed!\n");
    }
}
#endif