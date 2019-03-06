#include <string>
#include "meme.h"

Meme::Meme(string img_path, string top_text, string bottom_text, int id) {
	img_path_ = img_path;
	top_text_ = top_text;
	bottom_text_ = bottom_text;
	id_ = id;
}

string Meme::getImgPath() {
	return this->img_path_;
}

string Meme::getTopText() {
	return this->top_text_;
}

string Meme::getBottomText() {
	return this->bottom_text_;
}