#include <string>
#include <vector>
#include "sqlite3.h"
#include "meme.h"

#ifndef MEME_DB_H_
#define MEME_DB_H_

using namespace std;

const char MEME_DB_PATH[] = "meme.db";

class MemeDB {
 public:
  MemeDB();
  explicit MemeDB(string db_path);
  ~MemeDB();

  // Adds a new entry to Meme table.
  // Returns the id of the created entry upon success, -1 upon error.
  int add(string img_path, string top_text, string bottom_text);
  unique_ptr<Meme> findByID(string id);
  vector<Meme> findAll();

 private:
  void init();
  sqlite3* db_;
  string db_path_;
};

#endif  // MEME_DB_H_
