#include <string>
#include <vector>
#include <boost/thread/shared_mutex.hpp>
#include "sqlite3.h"
#include "meme.h"

#ifndef MEME_DB_H_
#define MEME_DB_H_

using namespace std;

const char MEME_DB_PATH[] = "meme.db";

// sqlite3 has mutex of its own to be used in a multithreaded environment.
// However, this involves dealing with SQLITE_BUSY and deadlock could occur
// during concurrent requests. An external reader/writer lock is used in this
// wrapper object to simplify concurrent requests. 
class MemeDB {
 public:
  static MemeDB& getMemeDB();
  MemeDB();
  explicit MemeDB(string db_path);
  ~MemeDB();

  // Adds a new entry to Meme table.
  // Returns the id of the created entry upon success, -1 upon error.
  int add(string img_path, string top_text, string bottom_text);
  unique_ptr<Meme> findByID(string id);
  vector<Meme> findAll();
  void clear();

 private:
  void init();
  boost::shared_mutex db_mutex_;
  sqlite3* db_;
  string db_path_;
};

#endif  // MEME_DB_H_
