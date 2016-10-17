#include "file_system_io.h"
#include <cassert>
#include <sstream>
#include <algorithm>
#include <regex>

namespace fs {

	//void* fs::read(stream* ss, size_t offset, size_t len, char * buffer)
	//{
	//	if (ss->file->type == file_type::file)
	//	{
	//		auto req_to = ss->r_pos + len;
	//		auto l = req_to < ss->file->data.size() - 1 ? len - 1 : ss->file->data.size() - ss->r_pos;
	//		if (l > 0)
	//		{
	//			memcpy(buffer, &ss->file->data[ss->r_pos], l);
	//			ss->r_pos += l;
	//			buffer[l] = '\0';
	//			return buffer;
	//		}
	//		else return nullptr; //eof
	//	}
	//	else if (ss->file->type == file_type::directory)
	//	{
	//		auto req_to = ss->r_pos;
	//		if (ss->r_pos < ss->file->children.size())
	//		{
	//			auto name_len = ss->file->children[ss->r_pos].first.size();
	//			auto l = len - 1 < name_len ? len - 1 : name_len;
	//			memcpy(buffer, &ss->file->children[ss->r_pos].first[0], l);
	//			ss->r_pos += 1;
	//			buffer[l] = '\0';
	//			return buffer;
	//		}
	//		else return nullptr; //eof	
	//	}
	//	else return nullptr; //error
	//}
	
	//int fs::write(stream* ss, size_t offset, size_t len, char * data)
	//{
	///*	if (ss->file->data.capacity() < ss->file->length + len + 1)
	//		ss->file->data.reserve(ss->file->data.size() + len + 1);
	//	memcpy(&ss->file->data[ss->w_pos], data, len);*/
	//	ss->file->data.replace(ss->w_pos, len, data);
	//	//ss->file->data.insert(ss->file->data.begin() + ss->w_pos, &data[0], &data[len]);
	//	ss->file->data[ss->w_pos + len] = '\0';
	//	ss->w_pos += len;
	//	//ss->file->length = ss->file->length < ss->w_pos ? ss->w_pos : ss->file->length;
	//	return 0;
	//}

	bool is_name_valid(const std::string& str)
	{
		if (str.size() < 2) return false;
		if (str[0] == '.') return false;
		return std::regex_match(str, std::regex("^[A-Za-z0-9_. ]+$"));
	}

	bool is_name_valid(file_t * file, const std::string& str)
	{
		if (file->type != file_type::directory) return false;
		auto res = std::find_if(file->children.begin(), file->children.end(), [&](const dir_entry_t& in) { return in.first == ".."; });
		dir_entry_t parent;
		if (res != file->children.end())
			parent = *res;
		else return false;

		res = std::find_if(parent.second->children.begin(), parent.second->children.end(), [&str](const dir_entry_t& in) { return in.first == str; });
		if (res != file->children.end())  return false;;
		return is_name_valid(str);
	}

	void * read(os::file_stream_t & stream, file_t * file, size_t len, char * buffer, char delim)
	{
		if (file->type == file_type::file)
		{
			if (stream.r_pos >= file->data.size())  return nullptr; //eof

			auto req_to = stream.r_pos + len;
			auto l = req_to < file->data.size() - 1 ? len - 1 : file->data.size() - stream.r_pos;
			int delim_found = 0;
			for (size_t i = 0; i < l; i++)
			{
				if (file->data[stream.r_pos + i] == delim)
				{
					l = i;
					delim_found = 1;
					break;
				}
			}

			memcpy(buffer, &file->data[stream.r_pos], l);
			stream.r_pos += (l + delim_found);
			buffer[l] = '\0';

			return buffer;
		}
		else if (file->type == file_type::directory)
		{
			auto req_to = stream.r_pos;
			if (stream.r_pos < file->children.size())
			{
				auto name_len = file->children[stream.r_pos].first.size();
				auto l = len - 1 < name_len ? len - 1 : name_len;
				memcpy(buffer, &file->children[stream.r_pos].first[0], l);
				stream.r_pos += 1;
				buffer[l] = '\0';
				return buffer;
			}
			else return nullptr; //eof	
		}
		else return nullptr; //error
	}

	int write(os::file_stream_t & stream, file_t * file, size_t offset, size_t len, char * data)
	{
		file->data.replace(stream.w_pos, len, data);
		file->data[stream.w_pos + len] = '\0';//!!!!
		stream.w_pos += len;
		file->timestamp = time(0);
		return 0;
	}

	file_t* open(file_t * file, file_type type, char * path, os::open_mode mode)
	{
		std::stringstream ss(path);
		std::string s;
		file_t* f = file;
		while (getline(ss, s, '\\'))
		{
			auto res = std::find_if(f->children.begin(), f->children.end(), [&s](const dir_entry_t& in) { return in.first == s; });
			if (res == f->children.end())
			{
				if (!(mode & FLAG_CREATE) || !is_name_valid(s)) return nullptr;
				auto nf = new file_t(s.c_str(), ss.eof() ? type : file_type::directory, time(0), f);
				f->children.push_back(dir_entry_t(s.c_str(), nf));
				f = nf;
			}
			else f = res->second;
		}
		//if (mode & FLAG_APPEND) f->data.seekp(0, std::ios::end);
		//stream* fs = new stream { 0, (mode & FLAG_APPEND) ? f->data.size() : 0, f, mode };
		return f;
	}

	char* get_path(file_t * file, size_t len, char * buffer)
	{
		auto f = file;
		if (len == 0) return nullptr;
		size_t pos = len;

		while (f)
		{
			auto size = f->file_name.size();
			if (pos - size - 2 < len)
			{
				buffer[pos - 1] = '\\';
				pos -= (size + 1);
				memcpy(&buffer[pos], f->file_name.c_str() , size);	
			}
			else break;

			auto res = std::find_if(f->children.begin(), f->children.end(), [&](const dir_entry_t& in) { return in.first == ".."; });
			f = (res != f->children.end()) ? res->second : nullptr;
		}
		memcpy(&buffer[0], &buffer[pos], (len - pos));

		buffer[(len - pos)] = '\0';
		return buffer;
	}
}