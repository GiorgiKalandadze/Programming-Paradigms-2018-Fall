using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#include <string.h>


const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}
const int START_YEAR = 1900;

struct Actor {
  char* name;
  void* file;
};
struct Movie {
  void* movie;
  void* file;
};
/* This method compares acotors. 
 *  actorOne points to object of struct Actor
 */
int actorCompare(const void *actorOne, const void *actorTwo) {
   Actor* actor = (Actor*) actorOne;
   void* actorFile = actor -> file;
   char* toCompare = actor -> name;
   char* compareWith = (char*) actorFile + *(int*)actorTwo;

   return strcmp(toCompare, compareWith);
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  //Variables for bsearch
  int* base = (int *)actorFile + 1;
  int numberOfElements = *(int*)actorFile;
  int sizeOfElement = sizeof(int);
  
  Actor actor;
  actor.name = (char*)player.c_str(); //SOS
  actor.file = (void*)actorFile;
  
  void* location = bsearch(&actor, base, numberOfElements, sizeOfElement, actorCompare);

  if(location == NULL) return false; 

  char* playerInfo = (char*) actorFile + *(int*) location;
  char* currentPointer = playerInfo;
  int currentOffset = 0;
 
  //Check size of player's name and move to location of number of movies.
  //Omit '\0' at the end of the string
  currentPointer += player.size() + 1;
  currentOffset += player.size() + 1;
  //Check if size of string is even and if string has extra '\0'
  if(player.size() % 2 == 0){ //It has extra '\0'
    currentPointer += 1;
    currentOffset += 1;
  } 

  short numberOfMovies = *(short * )currentPointer;

  //Move to location of movies.
    currentPointer += 2; //Number of bytes of numberOfMovies/short
    currentOffset += 2;

  if(currentOffset % 4 != 0){ //Two extra '\0'-s
    currentPointer += 2; 
  }
  
  for(short i = 0; i < numberOfMovies; i++){
    //Movie location in moviefile
    char* moviePointer = (char* )movieFile + *(int*)currentPointer;
    currentPointer += 4; //Go to next movie's location
    
    //Title of movie
    string movieTitle; 
    movieTitle = moviePointer;
    moviePointer += movieTitle.size() + 1; //+1 means omit last '\0'
    int movieYear = *moviePointer + START_YEAR; 
    film movie;
    movie.title = movieTitle;
    movie.year = movieYear;
    films.push_back(movie);
  }
  return true;
}

/* Compare method for movies
 * movieOne points to object of struct Movie
 */
int movieCompare(const void *movieOne, const void *movieTwo){
  Movie* first = (Movie*) movieOne;
  void* movieFile = first -> file;
  film* firstMovie = (film*)first -> movie;
  string firstTitle = firstMovie -> title;
  int firstYear = firstMovie -> year;

  char* compareWith = (char*) movieFile + *(int*)movieTwo;
  string secondTitle;
  secondTitle = compareWith;
  compareWith += secondTitle.size() + 1; //Move to year (+1 means omit last '\0')
  int secondYear = *compareWith + START_YEAR;
  //Make film structure for second movie accotding to it's title and year
  film secondMovie;
  secondMovie.title = secondTitle;
  secondMovie.year = secondYear;

  //Compare 
  if(*firstMovie < secondMovie){
    return -1;
  } else if(*firstMovie == secondMovie){
    return 0;
  } else {
    return 1;
  }
}
bool imdb::getCast(const film& movie, vector<string>& players) const { 
  //Variables for bsearch
  Movie movieKey;
  movieKey.movie = (void*) &movie;
  movieKey.file = (void*) movieFile;
  
  int* base = (int*)movieFile + 1;
  int numberOfElements = *(int*)movieFile;
  int sizeOfElements = sizeof(int);

  void* location = bsearch(&movieKey, base, numberOfElements, sizeOfElements, movieCompare);
  if(location == NULL) return false;

  char* movieInfo = (char*) movieFile + *(int*) location;
  char* currentPointer = movieInfo;
  int currentOffset = 0;

  currentPointer += movie.title.size() + 2;
  currentOffset += movie.title.size() + 2;
  //CurrentPointer points on year
  //Check if offset is odd then it has extra '\0'
  if(currentOffset % 2  == 1){
      currentPointer += 1;
      currentOffset += 1;
  }
  //CurrentPointer points on two-byte short stroing the number of actors
  short numberOfActors = *(short*)currentPointer;
  currentPointer += 2;
  currentOffset += 2;

  if(currentOffset % 4 != 0){
    currentPointer += 2;
  }
  //CurrentPointer points on integer offsets
  for(short i = 0; i < numberOfActors; i++){
    //Actor location in actorFile
    char* actorPointer = (char*) actorFile + *(int*) currentPointer;
    currentPointer += 4;  //Move to next actor's location
    string actorName;
    actorName = actorPointer;
    players.push_back(actorName);
  }
  return true; 
}


imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
