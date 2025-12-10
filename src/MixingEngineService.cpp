#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(1), auto_sync(false), bpm_tolerance(0)
{
    decks[0]=nullptr;
    decks[1]=nullptr;
    std::cout <<"[MixingEngineService] Initialized with 2 empty decks."<<std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    std:: cout << "[MixingEngineService] Cleaning up decks..." << std::endl;
   for (size_t i = 0; i < 2; ++i) { 
        if (decks[i] != nullptr) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}

MixingEngineService::MixingEngineService(const MixingEngineService& other) 
: decks(), active_deck(other.active_deck), auto_sync(other.auto_sync), bpm_tolerance(other.bpm_tolerance) {
    for (size_t i = 0; i < 2; ++i){
        if(other.decks[i] != nullptr){
            decks[i]= other.decks[i]->clone().release();
        }
        decks[i]= nullptr;
    }
}

MixingEngineService& MixingEngineService :: operator= (const MixingEngineService& other){
    if(this == &other){
        return *this;
    }
    for (size_t i = 0; i < 2; ++i){
        if(decks[i] != nullptr){
            delete decks[i];
            decks[i] = nullptr;
        }
    }
    this->active_deck = other.active_deck;
    this->auto_sync = other.auto_sync;
    this->bpm_tolerance = other.bpm_tolerance;
    for (size_t i = 0; i < 2; ++i){
        if(other.decks[i] != nullptr){
            decks[i]= other.decks[i]->clone().release();
        }
    }
    return *this;
}

/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
 std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> clone_track = track.clone();
    if(!clone_track){
        std::cerr << "[ERROR] Track:" <<track.get_title()<< " failed to clone " << std::endl;
        return -1;
    }
    if (decks[0]== nullptr && decks[1]== nullptr){
        decks[0] = clone_track.release();
        active_deck = 0;
        std::cerr << "[Load Complete] '" << decks[0]->get_title() << "' is now loaded on deck 0" << "\n";
        std::cerr << "[Active Deck] Switched to deck 0" << std::endl;
        return 0;
    }
    
    clone_track -> load();
    clone_track-> analyze_beatgrid();

    int target_deck = 1 - active_deck;
    
    std::cerr << "[Deck Switch] Target deck :" <<target_deck<< std::endl;
    if (decks[target_deck]!= nullptr){
        delete decks[target_deck];
        decks[target_deck]= nullptr;
    }
    
    if (decks[active_deck] != nullptr && auto_sync && !can_mix_tracks(clone_track)){ 
        sync_bpm(clone_track);
    }
        AudioTrack* raw_track= clone_track.release();
        decks[target_deck] = raw_track;
        if(decks[active_deck] != nullptr){
            std::cerr << "[Unload] Unloading previous deck " <<active_deck<< "('" <<decks[active_deck]->get_title()<< "')" "\n";
            delete decks[active_deck];
            decks[active_deck]=nullptr;
        }
        std::cerr << "[Load Complete]" <<raw_track->get_title()<< "is now loaded on deck" <<active_deck<< "\n";
        active_deck = target_deck;
        std::cerr<< "[Active Deck] swiched to deck" <<target_deck << std::endl;

    
    return target_deck;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    if(decks[active_deck] == nullptr){
        return false;
    }
    int curr_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track->get_bpm();
    double difference = curr_bpm - new_bpm;
    if (difference < 0) {
        difference = -difference;
    }
    if (difference <= bpm_tolerance) {
        return true; 
    } 
    return false; 
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    if (decks[active_deck] == nullptr || !track) {
        return; 
    }
    int original_bpm = decks[active_deck]->get_bpm();
    int new_bpm = track-> get_bpm();
    int avg_bpm = (original_bpm + new_bpm)/2;
    track->set_bpm(avg_bpm);
    std::cerr << "[Sync BPM] Syncing BPM from " <<original_bpm<< " to" <<avg_bpm<< std::endl;
}
