#include "ntshengn_utils_buffer.h"

NtshEngn::Buffer::Buffer() : m_cursor(0) {}

NtshEngn::Buffer::Buffer(size_t size) : m_data(size), m_cursor(0) {}

NtshEngn::Buffer::Buffer(const std::byte* data, size_t size) : m_data(size), m_cursor(0) {
	write(data, size);
	setCursorPosition(0);
}

NtshEngn::Buffer::Buffer(const Buffer& other) : m_data(other.m_data), m_cursor(other.m_cursor) {}
		
NtshEngn::Buffer& NtshEngn::Buffer::operator=(const Buffer& other) {
	m_data = other.m_data;
	m_cursor = other.m_cursor;

	return *this;
}

void NtshEngn::Buffer::write(const std::byte* dataToWrite, size_t size) {
	if ((m_cursor + size) > m_data.size()) {
		m_data.resize(m_cursor + size);
	}
	std::copy(dataToWrite, dataToWrite + size, m_data.begin() + m_cursor);

	m_cursor += size;
}

size_t NtshEngn::Buffer::read(std::byte* dataToRead, size_t size) {
	size_t actualSize = (m_cursor + size) > m_data.size() ? (m_data.size() - m_cursor) : size;
	if (actualSize == 0) {
		return 0;
	}
	std::copy(m_data.begin() + m_cursor, m_data.begin() + m_cursor + actualSize, dataToRead);

	m_cursor += actualSize;
	
	return actualSize;
}

void NtshEngn::Buffer::setCursorPosition(size_t cursorPosition) {
	if (cursorPosition > m_data.size()) {
		throw std::out_of_range("setCursorPosition: cursor position (" + std::to_string(cursorPosition) + ") cannot be greater than DataBuffer size (" + std::to_string(m_data.size()) + ").");
	}
	m_cursor = cursorPosition;
}

const std::byte* NtshEngn::Buffer::getData() const {
	return m_data.data();
}

size_t NtshEngn::Buffer::getSize() const {
	return m_data.size();
}