#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    std::cout << "[INFO] Building track library from config..." << std::endl;
    for(size_t i = 0; i< library_tracks.size(); ++i){
        AudioTrack* track = nullptr;
        if(library_tracks[i].type == "MP3"){
            track= new MP3Track(library_tracks[i].title, library_tracks[i].artists, library_tracks[i].duration_seconds,
            library_tracks[i].bpm, library_tracks[i].extra_param1, library_tracks[i].extra_param2);
            std::cout << "MP3: MP3Track created:" << library_tracks[i].extra_param1 << "kbps" << std::endl;
        }
        else if(library_tracks[i].type == "WAV"){
            track = new WAVTrack(library_tracks[i].title, library_tracks[i].artists, library_tracks[i].duration_seconds,
            library_tracks[i].bpm, library_tracks[i].extra_param1, library_tracks[i].extra_param2);
             std::cout << "WAV: WavTrack created:" << library_tracks[i].extra_param1 << "Hz/" <<library_tracks[i].extra_param2<< "bit" << std::endl;
        }
        if (track != nullptr){
            library.push_back(track);
        }
    }
    std::cout << "[INFO] Track library built: " << library_tracks.size() << "track loaded" << std::endl;
}

DJLibraryService::~DJLibraryService() {
    for(AudioTrack* at : library) {
        delete at;
    }
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return playlist.find_track(track_title);
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    Playlist new_playlist(playlist_name);
    int loaded_count = 0;
    for (size_t i = 0; i < track_indices.size(); ++i) {
        int index= track_indices[i];
        if (index < 1 || index > library.size()){
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }
        AudioTrack* track= library[index-1];
        AudioTrack* cloned_track = track->clone().release();
        if(cloned_track == nullptr){
             std::cout << "[ERROR] Track is null " << std::endl;
            continue;
        }
        cloned_track->load();
        cloned_track->analyze_beatgrid();
        new_playlist.add_track(cloned_track);
        loaded_count = loaded_count +1;
        std::cout << "Added '" << cloned_track->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
        
        }

    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << loaded_count << " tracks)" << std::endl;
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles;
     const std::vector<AudioTrack*>& tracks= playlist.getTracks();
     for(size_t i =0; i <tracks.size(); ++i){
        if (tracks[i] != nullptr){
            titles.push_back(tracks[i]-> get_title());
        }
    }
    
    return titles; 
}
