#include <string>
#include <boost/format.hpp>
#include <vector>
#include "meme_db.h"

using namespace std;

// adapted from https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm

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
    return -1;
  }
}

vector<Meme> MemeDB::findAll() {
  vector<Meme> memes;
  const char* sql = "SELECT * FROM Meme;";
  sqlite3_stmt* stmt;
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  
  int rc = sqlite3_step(stmt);
  if(rc = SQLITE_DONE) {
    // Adapted from:
    // https://stackoverflow.com/questions/11931978/c-sqlite-select-statement-and-store-as-vector
    vector<vector<string>> meme_strings;
    int m_index = 0;
    while(sqlite3_column_text(stmt, 0)) {

      for(int i = 0; i < 4; i++) {
        string attribute = string((char * ) sqlite3_column_text(stmt, i));
        meme_strings[i].push_back(attribute);
      }
      string img_path = meme_strings[0][m_index];
      string top_text = meme_strings[1][m_index];
      string bottom_text = meme_strings[2][m_index];
      string id = meme_strings[3][m_index];
      m_index++;
      Meme k = Meme(img_path, top_text, bottom_text, id);
      memes.push_back(k);
    }
  }

  return memes;
}

unique_ptr<Meme> MemeDB::findByID(string id) {
  unique_ptr<Meme> meme;
  sqlite3_stmt* stmt;
  const char* sql = "SELECT img_path, top_text, bottom_text FROM Meme WHERE id = ?;";
  sqlite3_prepare(db_, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, id.c_str(), id.length(), SQLITE_TRANSIENT);
  
  int rc = sqlite3_step(stmt);
  if(rc == SQLITE_DONE) {
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
 
  return meme;
}



