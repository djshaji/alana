#include "engine.h"

std::vector <Plugin *> *Engine::activePlugins = nullptr;

bool Engine::addPlugin(char* library, int pluginIndex, SharedLibrary::PluginType _type) {
    SharedLibrary * sharedLibrary = new SharedLibrary (library, _type);

    sharedLibrary ->setLibraryPath(std::string ("libs"));
    sharedLibrary->load();

    if (sharedLibrary->descriptors.size() == 0) {
        LOGE("Unable to load shared library!") ;
        return false;
    }

    LOGD("loaded shared library [ok] ... now trying to load plugin\n");
    Plugin * plugin = new Plugin (sharedLibrary->descriptors.at(pluginIndex), (long) sampleRate, _type);
    plugin->sharedLibrary = sharedLibrary;
    if (_type != SharedLibrary::LADSPA) {
        plugin -> load () ;
    }

    if (activePlugins == nullptr) {
        LOGD ("first run\n");
        activePlugins = new std::vector <Plugin *> () ;
    }
    activePlugins ->push_back(plugin);
    LOGD ("adding plugin to active chain %d\n", activePlugins->size ());
    // todo

    buildPluginChain();
    return true ;
}

bool Engine::openAudio () {
    driver = new AudioDriver (processor);
    return driver->open ();
}

Engine::Engine () {
    processor = new Processor () ;
    openAudio () ;
}

void Engine::buildPluginChain () {
    IN
    LOGD("building chain for %d plugins", activePlugins->size());
    
    processor->activePlugins = 0;
    for (int i = 0 ; i < activePlugins->size () ; i ++) {
        Plugin *p = activePlugins->at (i);
        
        if (!p->active)
            continue;
        processor->inputPorts [processor->activePlugins] = p->inputPort ;
        processor->outputPorts [processor->activePlugins] = p->outputPort ;
        processor->inputPorts2 [processor->activePlugins] = p->inputPort2 ;
        processor->outputPorts2 [processor->activePlugins] = p->outputPort2 ;
        if (p->type == SharedLibrary::LADSPA) {
            processor->connect_port [processor->activePlugins] = p->descriptor->connect_port ;
            processor->run [processor->activePlugins] = p->descriptor->run ;
            processor->run_adding [processor->activePlugins] = p->descriptor->run_adding ;
            processor->set_run_adding_gain [processor->activePlugins] = p->descriptor->set_run_adding_gain ;
            processor->descriptor [processor->activePlugins] = p->descriptor;
        } else /*if (p->type == SharedLibrary::LV2) */{
            processor->connect_port [processor->activePlugins] = reinterpret_cast<void (*)(
                    LADSPA_Handle, unsigned long, LADSPA_Data *)>(p->lv2Descriptor->connect_port);
            processor->run [processor->activePlugins] = reinterpret_cast<void (*)(
                    LADSPA_Handle, unsigned long)>(p->lv2Descriptor->run);
            processor->descriptor [processor->activePlugins] = reinterpret_cast<const LADSPA_Descriptor *>(p->lv2Descriptor);
        }

        processor->run_adding_gain [processor->activePlugins] = p->run_adding_gain ;
        processor->handle [processor->activePlugins] = p->handle ;
        processor->activePlugins ++ ;
    }

    OUT
}
