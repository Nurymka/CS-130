#include <string>
#include <boost/format.hpp>
#include <vector>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include "meme_db.h"

using namespace std;

// https://stackoverflow.com/questions/989795/example-for-boost-shared-mutex-multiple-reads-one-write
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2406.html#shared_lock_rationale
typedef boost::shared_lock<boost::shared_mutex> ReaderLock;
typedef boost::unique_lock<boost::shared_mutex> WriterLock;

// adapted from https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm

MemeDB& MemeDB::getMemeDB() {
  static MemeDB memeDB;
  return memeDB;
}

MemeDB::MemeDB() {
  db_path_ = MEME_DB_PATH;
  init();
}

MemeDB::MemeDB(string db_path) : db_path_(db_path) {
  init();
}

MemeDB::~MemeDB() {
  sqlite3_close(db_);
}

void MemeDB::init() {
  sqlite3_open(db_path_.c_str(), &db_);
  const char* sql = "CREATE TABLE IF NOT EXISTS Meme("  \
    "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
    "img_path TEXT NOT NULL," \
    "top_text TEXT NOT NULL," \
    "bottom_text TEXT NOT NULL);";
  sqlite3_exec(db_, sql, NULL, NULL, NULL);
}

int MemeDB::add(string img_path, string top_text, string bottom_text) {
  WriterLock lock(db_mutex_);
  // https://stackoverflow.com/questions/36815112/c-and-sqlite-how-to-execute-a-query-formed-by-user-input
  // uses prepare and binding to protect against sql injection
  sqlite3_stmt* stmt;
  const char* sql = "INSERT INTO Meme(img_path, top_text, bottom_text) VALUES (?,?,?);";
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, img_path.c_str(), img_path.length(), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, top_text.c_str(), top_text.length(), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, bottom_text.c_str(), bottom_text.length(), SQLITE_TRANSIENT);

  int rc = sqlite3_step(stmt);

  if (rc == SQLITE_DONE) {
    int inserted = sqlite3_last_insert_rowid(db_);
    sqlite3_finalize(stmt);
    return inserted;
  } else {
    sqlite3_finalize(stmt);
    return -1;
  }
}

vector<Meme> MemeDB::findAll() {
  return findAll(""); // passing an empty query will return all memes
}


vector<Meme> MemeDB::findAll(string query) {
  ReaderLock lock(db_mutex_);
  vector<Meme> memes;
  query = "%" + query + "%"; // surround with wildcard to match substring
  const char* sql = "SELECT img_path, top_text, bottom_text, id FROM Meme " \
                    "WHERE img_path LIKE ? OR top_text LIKE ? OR bottom_text LIKE ? " \
                    "ORDER BY id desc;";
  sqlite3_stmt* stmt;
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, query.c_str(), query.length(), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, query.c_str(), query.length(), SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, query.c_str(), query.length(), SQLITE_TRANSIENT);

  int rc = sqlite3_step(stmt);
  if(rc == SQLITE_ROW) {
    // Adapted from:
    // https://stackoverflow.com/questions/11931978/c-sqlite-select-statement-and-store-as-vector
    while(sqlite3_column_text(stmt, 0)) {
      vector<string> meme_attributes;
      for(int i = 0; i < 4; i++) {
        string attribute = string((char * ) sqlite3_column_text(stmt, i));
        meme_attributes.push_back(attribute);
      }
      string img_path = meme_attributes[0];
      string top_text = meme_attributes[1];
      string bottom_text = meme_attributes[2];
      string id = meme_attributes[3];
      memes.push_back(Meme(img_path, top_text, bottom_text, id));
      sqlite3_step(stmt);
    }
  }

  sqlite3_finalize(stmt);
  return memes;
}

unique_ptr<Meme> MemeDB::findByID(string id) {
  ReaderLock lock(db_mutex_);

  unique_ptr<Meme> meme;

  // id should only be numeric
  if (id.find_first_not_of("0123456789") != std::string::npos) {
    return meme;
  }
  sqlite3_stmt* stmt;
  const char* sql = "SELECT img_path, top_text, bottom_text FROM Meme WHERE id = ?;";
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  sqlite3_bind_int(stmt, 1, stoi(id)); 
  
  int rc = sqlite3_step(stmt);
  if(rc == SQLITE_ROW) {
    // Adapted from:
    // https://stackoverflow.com/questions/11931978/c-sqlite-select-statement-and-store-as-vector
    vector<string> meme_attributes;
    if(sqlite3_column_text(stmt, 0)) {
      for(int i = 0; i < 3; i++) {
        string attribute = string((char * ) sqlite3_column_text(stmt, i));
        meme_attributes.push_back(attribute);
      }
      string img_path = meme_attributes[0];
      string top_text = meme_attributes[1];
      string bottom_text = meme_attributes[2];
      meme = unique_ptr<Meme>(new Meme(img_path, top_text, bottom_text, id));
    } 
  }
 
  sqlite3_finalize(stmt);
  return meme;
}

void MemeDB::clear() {
  WriterLock lock(db_mutex_);
  const char* sql = "DELETE FROM Meme;";
  sqlite3_stmt* stmt;
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

