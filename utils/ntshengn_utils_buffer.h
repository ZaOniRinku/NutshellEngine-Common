#pragma once
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <vector>

namespace NtshEngn {

	class Buffer {
	public:
		Buffer();
		Buffer(const Buffer& other);
		Buffer(Buffer&&) noexcept = default;

		void write(const std::byte* dataToWrite, size_t size);
		size_t read(std::byte* dataToRead, size_t size);

		void setCursorPosition(size_t cursorPosition);

		const std::byte* getData() const;
		size_t getSize() const;

	private:
		std::vector<std::byte> m_data;

		size_t m_cursor;
	};

}