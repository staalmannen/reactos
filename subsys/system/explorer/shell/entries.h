/*
 * Copyright 2003 Martin Fuchs
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


 //
 // Explorer clone
 //
 // entries.h
 //
 // Martin Fuchs, 23.07.2003
 //


enum ENTRY_TYPE {
	ET_WINDOWS,
#ifdef __linux__
	ET_UNIX,
#endif
	ET_SHELL
};

enum SORT_ORDER {
	SORT_NAME,
	SORT_EXT,
	SORT_SIZE,
	SORT_DATE
};

struct Entry
{
protected:
	Entry(ENTRY_TYPE etype);
	Entry(Entry* parent);

public:
	~Entry();

	Entry*		_next;
	Entry*		_down;
	Entry*		_up;

	bool		_expanded;
	bool		_scanned;
	int 		_level;

	WIN32_FIND_DATA _data;

	BY_HANDLE_FILE_INFORMATION _bhfi;
	bool		_bhfi_valid;

	SFGAOF		_shell_attribs;

	ENTRY_TYPE	_etype;
	HICON		_hicon;

	void	free_subentries();

	void	read_directory(SORT_ORDER sortOrder);
	Entry*	read_tree(const void* path, SORT_ORDER sortOrder);
	void	sort_directory(SORT_ORDER sortOrder);

	virtual void read_directory() {}
	virtual const void* get_next_path_component(const void*) {return NULL;}
	virtual Entry* find_entry(const void*) {return NULL;}
	virtual void get_path(PTSTR path) = 0;
	virtual BOOL launch_entry(HWND hwnd, UINT nCmdShow);
};

struct Directory {
protected:
	Directory(void* path) : _path(path) {}
	~Directory() {free(_path);}

	void*	_path;
};


struct Root {
	Root();
	~Root();

	Entry*	_entry;
	TCHAR	_path[MAX_PATH];
	TCHAR	_volname[_MAX_FNAME];
	TCHAR	_fs[_MAX_DIR];
	DWORD	_drive_type;
	DWORD	_fs_flags;
};

