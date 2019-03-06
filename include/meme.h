#include <string>

#ifndef MEME_H_
#define MEME_H_

//Never thought I'd see the day I'd develop a Meme class, but here we are.
//This is a simple wrapper for the data that comprises a meme.
class Meme {
  public:
  	Meme(string img_path, string top_text, string bottom_text, int id);
  	string getImgPath();
  	string getTopText();
  	string getBottomText();
    int getID();
  	
  private:
  	string img_path_;
  	string top_text_;
  	string bottom_text_;
    int id_;
}

#endif //MEME_H_