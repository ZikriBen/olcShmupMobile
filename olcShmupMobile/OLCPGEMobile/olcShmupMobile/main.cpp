//// olcShmup.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
#include <memory>
#define OLC_PGEX_MINIAUDIO          // Johnngy63: Added to support cool sounds
#include "olcPGEX_MiniAudio.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine_Mobile.h"
#include "Screen.h"


enum class GameState {
    START,
    MENU,
    INTRO,
    GAME,
    GAME_OVER,
    EXIT
};

class Shmup : public olc::PixelGameEngine {
public:
    Shmup() {
        sAppName = "Shmup";
    }

    std::unique_ptr<StartScreen> startScreen;
    std::unique_ptr<MenuScreen> menuScreen;
    std::unique_ptr<IntroScreen> introScreen;
    std::unique_ptr<GameScreen> gameScreen;
    std::unique_ptr<GameOverScreen> gameOverScreen;
    Screen* currentScreen;
    std::string currentScreenStr;
    std::unordered_map<std::string, Screen*> screenMap;
    GameState gameState;


public:
    /* Vectors */
    std::vector<std::string> vecMessages;
    /* END Vectors*/

    int nFrameCount = 0;
    int nStep = 20;

    /* Sprites */
    olc::Sprite* sprTouchTester = nullptr;
    olc::Sprite* sprOLCPGEMobLogo = nullptr;
    /* END Sprites*/

    /* Decals */
    olc::Decal* decTouchTester = nullptr;
    olc::Decal* decOLCPGEMobLogo = nullptr;
    /* End Decals */


    /* Sensors */
    std::vector<olc::SensorInformation> vecSensorInfos;
    /*End Sensors*/

    // To keep track of our sample ID
    // Ensure that all sound IDs are set to -1 stop memory leak when Android/iOS takes
    // the app out of focus
    int32_t song1 = -1;

    // For demonstration controls, with sensible default values
    float pan = 0.0f;
    float pitch = 1.0f;
    float seek = 0.0f;
    float volume = 1.0f;

    // The instance of the audio engine, no fancy config required.
    olc::MiniAudio ma;

    //Example Save State Struct and Vector for when your app is paused
    struct MySaveState {
        std::string key;
        int value;
    };

    std::vector<MySaveState> vecLastState;

    std::string sampleAFullPath; // Holds the full path to sampleA.wav

public:
    bool OnUserCreate() override {



        //NOTE: To access the features with your phone device use:
#if defined(__ANDROID__)
        // Access android directly
        //android_app* pMyAndroid = this->pOsEngine.app;

        // USE OF SOUND olcPGE_MiniAudio
        /*
         * For Android you cannot play the sounds directly from the assets as you would
         * on a Windows/Mac/Linux system. Android compress your assets in a compress file to
         * save on valuable phone storage. AndroidAudio (AAudio), miniAudio, and most others need
         * to be able to stream the music data, in short they are not very good at streaming for
         * a compress file.
         * Therefore you will need to extract these sound files to internal storage so the sounds
         * can be played.
         *
         * In short, as I know you didn't read the above, you cannot stream from an asset in Android
         *
         */

        std::string songFullPath = (std::string)app_GetInternalAppStorage() + "/sounds/song1.mp3";
        olc::rcode fileRes = olc::filehandler->ExtractFileFromAssets("sounds/song1.mp3", songFullPath);

        switch (fileRes) {

        case olc::rcode::NO_FILE:
        { break; }
        case olc::rcode::FAIL:
        { break; }
        case olc::rcode::OK:
        {
            // only load the song if it is not already loaded
            song1 = ma.LoadSound(songFullPath);

            break;
        }
        }

        sampleAFullPath = (std::string)app_GetInternalAppStorage() + "/sounds/SampleA.wav";
        olc::rcode result = olc::filehandler->ExtractFileFromAssets("sounds/SampleA.wav", sampleAFullPath);


#endif

#if defined(__APPLE__)
        // Access iOS directly
        //apple_app* pMyApple = this->pOsEngine.app;

        // USE OF SOUND olcPGE_MiniAudio

        std::string songFullPath = (std::string)app_GetInternalAppStorage() + "/sounds/song1.mp3";
        olc::rcode fileRes = olc::filehandler->ExtractFileFromAssets("sounds/song1.mp3", songFullPath);

        switch (fileRes) {

        case olc::rcode::NO_FILE:
        { break; }
        case olc::rcode::FAIL:
        { break; }
        case olc::rcode::OK:
        {
            if (song1 < 0)
            {
                song1 = ma.LoadSound(songFullPath);
            }

            break;
        }
        }

        sampleAFullPath = (std::string)app_GetInternalAppStorage() + "/sounds/SampleA.wav";
        olc::rcode result = olc::filehandler->ExtractFileFromAssets("sounds/SampleA.wav", sampleAFullPath);

#endif

        sprTouchTester = new olc::Sprite("images/north_south_east_west_logo.png");
        decTouchTester = new olc::Decal(sprTouchTester);

        sprOLCPGEMobLogo = new olc::Sprite("images/olcpgemobilelogo.png");
        decOLCPGEMobLogo = new olc::Decal(sprOLCPGEMobLogo);









        startScreen = std::make_unique<StartScreen>(*this);
        startScreen->Create();
        menuScreen = std::make_unique<MenuScreen>(*this);
        menuScreen->Create();
        introScreen = std::make_unique<IntroScreen>(*this);
        introScreen->Create();
        gameScreen = std::make_unique<GameScreen>(*this);
        gameScreen->Create();
        gameOverScreen = std::make_unique<GameOverScreen>(*this);
        gameOverScreen->Create();

        screenMap["start"] = startScreen.get();
        screenMap["menu"] = menuScreen.get();
        screenMap["intro"] = introScreen.get();
        screenMap["game"] = gameScreen.get();
        screenMap["game_over"] = gameOverScreen.get();

        gameState = GameState::GAME;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        currentScreen = screenMap[EnumStateToString(gameState)];

        bool keepState = currentScreen->Run(fElapsedTime);

        if (!keepState)
            stateTransition(gameState);

        return (gameState == GameState::EXIT) ? false : true;
    }

    std::string EnumStateToString(GameState state) {
        std::string screen = "";
        switch (gameState) {
        case GameState::START:
            screen = "start";
            break;
        case GameState::MENU:
            screen = "menu";
            break;
        case GameState::INTRO:
            screen = "intro";
            break;
        case GameState::GAME:
            screen = "game";
            break;
        case GameState::GAME_OVER:
            screen = "game_over";
            break;
        case GameState::EXIT:
            break;
        }

        return screen;
    }

    void stateTransition(GameState state) {
        // Handle screen transition logic here
        switch (gameState) {
        case GameState::START:
            gameState = GameState::MENU;
            menuScreen->Reset();
            startScreen->Destroy(); // Only Destroy, as no need for start screen anymore
            break;
        case GameState::MENU:
            if (menuScreen->currentSelection == 0) {
                gameState = GameState::INTRO;
            }
            else if (menuScreen->currentSelection == menuScreen->lines.size() - 1) {
                gameState = GameState::EXIT;
            }
            introScreen->Reset();
            break;
        case GameState::INTRO:
            gameScreen->Reset();
            gameScreen->CreateSpawns();
            gameState = GameState::GAME;
            break;
        case GameState::GAME:
            gameOverScreen->Reset();
            gameState = GameState::GAME_OVER;
            gameScreen->Destroy();
            break;
        case GameState::GAME_OVER:
            menuScreen->Reset();
            gameState = GameState::MENU;
            break;
        case GameState::EXIT:
            break;
        }
    }

    
    
    
    
    
    
    
    
    
    
    bool OnUserDestroy() override {
        return true;
    }
    
    void OnSaveStateRequested() override
    {
        // Fires when the OS is about to put your game into pause mode
        // You have, at best 30 Seconds before your game will be fully shutdown
        // It depends on why the OS is pausing your game tho, Phone call, etc
        // It is best to save a simple Struct of your settings, i.e. current level, player position etc
        // NOTE: The OS can terminate all of your data, pointers, sprites, layers can be freed
        // Therefore do not save sprites, pointers etc 

        // Example 1: vector
        vecLastState.clear();
        vecLastState.push_back({ "MouseX", 55 });
        vecLastState.push_back({ "MouseY", 25 });
        vecLastState.push_back({ "GameLevel", 5 });

#if defined(__ANDROID__)
        // You can save files in the android Internal app storage
        const char* internalPath = app_GetInternalAppStorage(); //Android protected storage
#endif
#if defined(__APPLE__)
        // For iOS the internal app storage is read only, therefore we use External App Storage
        const char* internalPath = app_GetExternalAppStorage(); // iOS protected storage AKA /Library
#endif

        std::string dataPath(internalPath);

        // internalDataPath points directly to the files/ directory                                  
        std::string lastStateFile = dataPath + "/lastStateFile.bin";

        std::ofstream file(lastStateFile, std::ios::out | std::ios::binary);

        if (file)
        {
            float fVecSize = vecLastState.size();
            file.write((char*)&fVecSize, sizeof(long));
            for (auto& vSS : vecLastState)
            {
                file.write((char*)&vSS, sizeof(MySaveState));
            }

            file.close();
        }


    }

    void OnRestoreStateRequested() override
    {
        // This will fire every time your game launches 
        // OnUserCreate will be fired again as the OS may have terminated all your data

#if defined(__ANDROID__)
        // You can save files in the android Internal app storage
        const char* internalPath = app_GetInternalAppStorage(); //Android protected storage
#endif
#if defined(__APPLE__)
        // For iOS the internal app storage is read only, therefore we use External App Storage
        const char* internalPath = app_GetExternalAppStorage(); // iOS protected storage AKA /Library
#endif

        std::string dataPath(internalPath);
        std::string lastStateFile = dataPath + "/lastStateFile.bin";

        vecLastState.clear();

        std::ifstream file(lastStateFile, std::ios::in | std::ios::binary);

        MySaveState saveState;

        if (file)
        {
            float fVecSize = 0.0f;
            file.read((char*)&fVecSize, sizeof(long));
            for (long i = 0; i < fVecSize; i++)
            {
                file.read((char*)&saveState, sizeof(MySaveState));
                vecLastState.push_back(saveState);
            }

            file.close();
            // Note this is a temp file, we must delete it
            std::remove(lastStateFile.c_str());

        }


    }

};


int main()
{
    Shmup game;
    if (game.Construct(640, 480, 2, 2))
        game.Start();

    return 0;
}

// 1. Add HUD
// 2. Fix all memory leaks
// 3. replace sounds


void android_main(struct android_app* initialstate) {

    /*
        initalstate allows you to make some more edits
        to your app before the PGE Engine starts
        Recommended just to leave it at its defaults
        but change it at your own risk
        to access the Android/iOS directly in your code
        android_app* pMyAndroid = this->pOsEngine.app;;

    */

    Shmup demo;

    /*
        Note it is best to use HD(1280, 720, ? X ? pixel, Fullscreen = true) the engine can scale this best for all screen sizes,
        without affecting performance... well it will have a very small affect, it will depend on your pixel size
        Note: cohesion is currently not working
    */
    demo.Construct(640, 480, 2, 2, true, false, false);

    demo.Start(); // Lets get the party started


}