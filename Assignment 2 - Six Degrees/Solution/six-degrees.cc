#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

const int MAX_LENGTH = 5; //Maximum length of path
/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
	 << "Please try again." << endl;
  }
}

/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

bool generateShortestPath(string first, string second, const imdb& db);
int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    return 1;
  }
  
  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      // replace the following line by a call to your generateShortestPath routine... 
      if(generateShortestPath(source, target, db)){
        //Nothing
      } else {
        cout << endl << "No path between those two people could be found." << endl << endl;
      }
    }
  }
  
  cout << "Thanks for playing!" << endl;
  return 0;
}


bool generateShortestPath(string first, string second, const imdb& db){
  list<path> partialPaths; // functions as a queue
  set<string> previouslySeenActors;
  set<film> previouslySeenFilms;

  path startPath(first); //create a partial path around the start actor;
  partialPaths.push_back(startPath); //add this partial path to the queue of partial paths;
  while(!partialPaths.empty() && partialPaths.front().getLength() <= MAX_LENGTH){
    path frontPath = partialPaths.front(); //Get front path
    partialPaths.pop_front(); //Delete from front path from list
   
    vector<film> films; //In films vector we will keep all the films played by lastplayer
    string lastPlayer = frontPath.getLastPlayer(); 
    db.getCredits(lastPlayer, films); //look up last actor’s movies
    
    for(int i = 0; i < films.size(); i++){
      film currentFilm = films[i];
      if(previouslySeenFilms.count(currentFilm) != 1){ //If we haven't seen current film yet
        previouslySeenFilms.insert(currentFilm); //Add curent film in visited
        vector<string> cast;  
        db.getCast(currentFilm, cast); //look up movie’s cast
        //For each memeber of cast we haven't seen befor
        for(int j = 0; j < cast.size(); j++){
          string currentActor = cast[j];
          if(previouslySeenActors.count(currentActor) != 1){
            previouslySeenActors.insert(currentActor); //add cast member to set of those previously seen
            path clonePath = frontPath; //clone the partial path
            clonePath.addConnection(currentFilm, currentActor); //add the movie/costar connection to the clone
            //Check if currentActor is target
            if(currentActor == second){
              cout << clonePath << endl;
              return true; 
            }
            partialPaths.push_back(clonePath); //add this new partial path to the end of the queue
          }
        }
      }
    }
  }
  return false;
}