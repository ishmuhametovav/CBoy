#pragma once

#include <mutex>
#include <array>
#include <optional>

constexpr size_t BUFFER_SIZE = 4096;

template<typename T> class ring_buffer
{
	size_t read_index;
	size_t write_index;
	std::array<T, BUFFER_SIZE> buffer;

	std::mutex buffer_mutex;
public:
	ring_buffer() : read_index(0), write_index(0), buffer{}, buffer_mutex{} {}

	void add(const T value)
	{
		std::lock_guard<std::mutex> lock(buffer_mutex);

		buffer[write_index] = value;
		write_index = (write_index + 1) % BUFFER_SIZE;
		/*if write index == read index we have to rewrite buffer(it is overflowed)*/
		if (write_index == read_index) read_index = (read_index + 1) % BUFFER_SIZE;
	}

	std::optional<T> get()
	{
		std::lock_guard<std::mutex> lock(buffer_mutex);

		/*if read index == write index then we stop reading*/
		if (read_index == write_index) return std::nullopt;

		T value = buffer[read_index];
		read_index = (read_index + 1) % BUFFER_SIZE;

		return value;
	}

	void reset()
	{
		std::lock_guard<std::mutex> lock(buffer_mutex);
		buffer.fill(0);
		read_index = write_index = 0;
	}
};