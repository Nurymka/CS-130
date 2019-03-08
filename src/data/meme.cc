#include <string>
#include <sstream>
#include "meme.h"

Meme::Meme(string img_path, string top_text, string bottom_text, string id) {
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

string Meme::getID() {
	return this->id_;
}

string Meme::toJSON() {
	stringstream meme_stream;
	meme_stream << "{ ";
	meme_stream << "\"img_path\" : \"" << this->img_path_ << "\", ";
	meme_stream << "\"top_text\" : \"" << this->top_text_ << "\", ";
	meme_stream << "\"bottom_text\" : \"" << this->bottom_text_ << "\", ";
	meme_stream << "\"id\" : \"" << this->id_ << "\"";
	meme_stream << " }";
	return meme_stream.str();
}