#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

//in text file
string access_token = "";
//stores response body
string body = "";

struct Playback {
  string songName, artistName, albumName;
  bool status = false;  //true if recent http request succeeded
};

//function prototypes
void setAccessToken();
void getCurrentState(Playback &user);
void pauseOrResume(Playback &user, string url);
void nextOrPrev(Playback &user, string url);
size_t setBody(char *buffer, size_t itemsize, size_t nitems, void *ignore);
void song(Playback &user);
void resume(Playback &user);
void pause(Playback &user);
void next(Playback &user);
void prev(Playback &user);

int main() {
  //set access token stored in text file
  setAccessToken();
  Playback user;  //one shared user object
  string commands = "song, pause, resume, next, prev, exit";
  cout << "commands: " << commands << endl;
  
  while (true) {
    cout << "spotifycli$> ";
    string input;
    cin >> input;
    cout << endl;

    if (input == "song") {
      song(user);
    }
    else if (input == "pause") {
      pause(user);
    }
    else if (input == "resume") {
      resume(user);
    }
    else if (input == "next") {
      next(user);
    }
    else if (input == "prev") {
      prev(user);
    }
    else if (input == "exit") {
      cout << "exiting..." << endl;
      exit(0);
    }
    else {
      cout << "commands: " << commands << endl;
    }

    cout << endl;
  }

  return 0;
}

//scopes: user-read-playback-state, user-modify-playback-state
void setAccessToken() {
  ifstream fin;
  fin.open("access_token.txt");
  string token;
  while (fin >> token) {
    access_token = token;
  }
  fin.close();
}

void song(Playback &user) {
  getCurrentState(user);
  //if request was a success then print it
  if (user.status) {
    cout << "Song: " << user.songName << endl;
    cout << "Artist: " << user.artistName << endl;
    cout << "Album: " << user.albumName << endl;
  }
}

void pause(Playback &user) {
  pauseOrResume(user, "https://api.spotify.com/v1/me/player/pause");
  if (user.status) {
    //get current song to print it
    getCurrentState(user);
    if (user.status) {
      cout << user.songName << " by " << user.artistName << " paused" << endl;
    }
  }
}

void resume(Playback &user) {
  pauseOrResume(user, "https://api.spotify.com/v1/me/player/play");
  if (user.status) {
    //get current song to print it
    getCurrentState(user);
    if (user.status) {
      cout << user.songName << " by " << user.artistName << " resumed" << endl;
    }
  }
}

void next(Playback &user) {
  nextOrPrev(user, "https://api.spotify.com/v1/me/player/next");
  if (user.status) {
    //get current song to print it
    getCurrentState(user);
    if (user.status) {
      cout << ">> Now playing " << user.songName << " by " << user.artistName << endl;
    }
  }
}

void prev(Playback &user) {
  nextOrPrev(user, "https://api.spotify.com/v1/me/player/previous");
  if (user.status) {
    //get current song to print it
    getCurrentState(user);
    if (user.status) {
      cout << "<< Now playing " << user.songName << " by " << user.artistName << endl;
    }
  }
}

size_t setBody(char *buffer, size_t itemsize, size_t nitems, void *ignore) {
  //append each byte in buffer to body
  for (int i = 0; i < nitems; i++) {
    body = body + buffer[i];
  }

  //done with all the bytes in the buffer
  return nitems;
}

//post req
void nextOrPrev(Playback &user, string url) {
  CURL *curl;
  CURLcode result;
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl) {
    //POST request options
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    //send no data in POST request
    const char *noData = "";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, noData);

    //set header
    struct curl_slist *list = NULL;
    string header = "Authorization: Bearer " + access_token;
    list = curl_slist_append(list, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    //pass chunks of response to setBody function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, setBody);

    //perform action
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
      cout << "curl_easy_perform() failed. error: " << curl_easy_strerror(result) << endl;
    }

    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();

  //if there is a body then there was an error
  if (body != "") {
    cout << body << endl;
    json j = j.parse(body);
    json error = j.at("error");
    cout << error.at("status") << ": " << error.at("message") << endl;
    user.status = false;

    //clear body
    body = "";
    return;
  }
  user.status = true;
}

//scopes: user-modify-playback-state
//put req
void pauseOrResume(Playback &user, string url) {
  CURL *curl;
  CURLcode result;
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl) {
    //PUT request
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    //send no data in PUT request
    const char *noData = "";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, noData);

    //set header
    struct curl_slist *list = NULL;
    string header = "Authorization: Bearer " + access_token;
    list = curl_slist_append(list, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    //pass chunks of response to setBody function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, setBody);

    //perform action
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
      cout << "curl_easy_perform() failed. error: " << curl_easy_strerror(result) << endl;
    }

    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  //if there is a body then there was an error
  if (body != "") {
    json j = j.parse(body);
    json error = j.at("error");
    cout << error.at("status") << ": " << error.at("message") << endl;
    user.status = false;

    //clear body
    body = "";
    return;
  }
  user.status = true;
}

//scopes: user-read-playback-state
//get req
void getCurrentState(Playback &user) {
  CURL *curl;
  CURLcode result;
  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl) {
    //set options
    string url = "https://api.spotify.com/v1/me/player";

    //GET request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    //set header
    struct curl_slist *list = NULL;
    string header = "Authorization: Bearer " + access_token;
    list = curl_slist_append(list, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    //pass chunks of response to setBody function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, setBody);

    //perform action
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
      cout << "curl_easy_perform() failed. error: " << curl_easy_strerror(result) << endl;
    }

    curl_slist_free_all(list);
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  //convert string body to json
  json j = j.parse(body);

  //clear body
  body = "";

  //if has key named "error" then API call failed
  if (j.contains("error")) {
    //error object contains key-value pair with key "message"
    json error = j.at("error");
    cout << error.at("status") << ": " << error.at("message") << endl;
    user.status = false;
    return;
  }
 
  //get item object, album object, and artists array from json body
  json item = j.at("item");
  json artists = item.at("artists");
  json album = item.at("album");

  //store in playback object to keep track of user's current state
  user.songName = item.at("name");
  user.artistName = artists[0].at("name");  //artists is array of objects
  user.albumName = album.at("name");
  user.status = true;
}