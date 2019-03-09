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

// https://stackoverflow.com/questions/7724448/simple-json-string-escape-for-c
std::string escape_json(const std::string& input) {
    std::ostringstream ss;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
    //C++98/03:
    //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

string Meme::toJSON() {
	stringstream meme_stream;
	meme_stream << "{ ";
	meme_stream << "\"img_path\" : \"" << escape_json(this->img_path_) << "\", ";
	meme_stream << "\"top_text\" : \"" << escape_json(this->top_text_) << "\", ";
	meme_stream << "\"bottom_text\" : \"" << escape_json(this->bottom_text_) << "\", ";
	meme_stream << "\"id\" : \"" << this->id_ << "\"";
	meme_stream << " }";
	return meme_stream.str();
}
