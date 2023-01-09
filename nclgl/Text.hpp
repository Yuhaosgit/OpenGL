#pragma once
#include <string>

class Text {
public:
	Text() = default;

	template<typename T>
	void SetContent(const T& in_content) {
		content = std::to_string(in_content);
	}

	template<>
	void SetContent<std::string>(const std::string& in_content) {
		content = in_content;
	}

	std::string GetContent() {
		return content;
	}
private:
	std::string content;
};