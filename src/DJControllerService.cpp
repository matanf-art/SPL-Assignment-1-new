#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    if(cache.contains(track.get_title())){
        cache.get(track.get_title());
        return 1;
    } 
    AudioTrack* clone_track = track.clone().release();
        if(clone_track == nullptr){
            std::cout << "error" << std::endl;
            return 0;
        }
    clone_track->load();
    clone_track->analyze_beatgrid();
    PointerWrapper<AudioTrack> temp_wrapper(clone_track);
    bool evicted = cache.put(std::move(temp_wrapper));
    if(evicted){
        return -1;
    }
    return 0;
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    AudioTrack* result_ptr = cache.get(track_title);
    if (result_ptr == nullptr) {
        return nullptr;
    }
    return result_ptr;
}
