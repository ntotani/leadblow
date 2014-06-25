#include "AppDelegate.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "Runtime.h"
#include "ConfigParser.h"
#include "PluginManager.h"
#include "ProtocolAnalytics.h"
#include "ProtocolAds.h"
#include "Values.h"

using namespace CocosDenshion;

USING_NS_CC;
using namespace std;
using namespace plugin;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

bool AppDelegate::applicationDidFinishLaunching()
{
#if (COCOS2D_DEBUG>0)
    initRuntime();
#endif
    
    if (!ConfigParser::getInstance()->isInit()) {
            ConfigParser::getInstance()->readConfig();
        }

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();    
    if(!glview) {
        Size viewSize = ConfigParser::getInstance()->getInitViewSize();
        string title = ConfigParser::getInstance()->getInitViewName();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        extern void createSimulator(const char* viewName, float width, float height,bool isLandscape = true, float frameZoomFactor = 1.0f);
        bool isLanscape = ConfigParser::getInstance()->isLanscape();
        createSimulator(title.c_str(),viewSize.width,viewSize.height,isLanscape);
#else
        glview = GLView::createWithRect(title.c_str(), Rect(0,0,viewSize.width,viewSize.height));
        director->setOpenGLView(glview);
#endif
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto flurryId = FLURRY_ID_IOS;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto flurryId = FLURRY_ID_ANDROID;
#endif
    if (strlen(flurryId) > 0) {
        auto flurry = dynamic_cast<ProtocolAnalytics*>(PluginManager::getInstance()->loadPlugin("AnalyticsFlurry"));
        if (flurry) {
#ifdef COCOS2D_DEBUG
            flurry->setDebugMode(true);
            flurry->setCaptureUncaughtException(false);
#else
            flurry->setDebugMode(false);
            flurry->setCaptureUncaughtException(true);
#endif
            flurry->startSession(flurryId);
        }
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto admobId = ADMOB_ID_IOS;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto admobId = ADMOB_ID_ANDROID;
#endif
    if (strlen(admobId) > 0) {
        auto admob = dynamic_cast<ProtocolAds*>(PluginManager::getInstance()->loadPlugin("AdsAdmob"));
        if (admob) {
#ifdef COCOS2D_DEBUG
            admob->setDebugMode(true);
#else
            admob->setDebugMode(false);
#endif
            TAdsDeveloperInfo devInfo;
            devInfo["AdmobID"] = admobId;
            admob->configDeveloperInfo(devInfo);
        }
    }

    auto amoad = dynamic_cast<ProtocolAds*>(PluginManager::getInstance()->loadPlugin("AdsAmoad"));
    if (amoad) {
#ifdef COCOS2D_DEBUG
        amoad->setDebugMode(true);
#endif
        TAdsDeveloperInfo info;
        info["sendUUID"] = "YES";
        amoad->configDeveloperInfo(info);
    }

    auto felloId = FELLO_ID;
    if (strlen(felloId) > 0) {
        auto fello = dynamic_cast<ProtocolAds*>(PluginManager::getInstance()->loadPlugin("AdsFello"));
        if (fello) {
            TAdsDeveloperInfo devInfo;
            devInfo["FelloID"] = felloId;
            fello->configDeveloperInfo(devInfo);
        }
    }

    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    
    //register custom function
    //LuaStack* stack = engine->getLuaStack();
    //register_custom_function(stack->getLuaState());
    
#if (COCOS2D_DEBUG>0)
    if (startRuntime())
        return true;
#endif

    engine->executeScriptFile(ConfigParser::getInstance()->getEntryFile().c_str());
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

