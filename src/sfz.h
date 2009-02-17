/* -*- Mode: C++ ; c-basic-offset: 8 -*- */
#ifndef LIBSFZ_SFZ_H
#define LIBSFZ_SFZ_H

// SFZ 1.0
// Copyright (c) 2008, Anders Dahnielson
//
// Contact: anders@dahnielson.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include <fstream>
#include <vector>
#include <string>

#define TRIGGER_ATTACK  ((unsigned char) (1 << 0)) // 0x01
#define TRIGGER_RELEASE ((unsigned char) (1 << 1)) // 0x02
#define TRIGGER_FIRST   ((unsigned char) (1 << 2)) // 0x04
#define TRIGGER_LEGATO  ((unsigned char) (1 << 3)) // 0x08

namespace sfz 
{

	// Forward declarations
	class Group;
	class Region;
	class Instrument;
	class File;

	// Enumerations
	enum sw_vel_t   { VEL_CURRENT, VEL_PREVIOUS };
	enum off_mode_t { OFF_FAST, OFF_NORMAL };

	typedef unsigned char trigger_t;

	/////////////////////////////////////////////////////////////
	// class Region

	/// Defines Region information of an Instrument
	class Region
	{
	public:
		Region(Group* parent);
		virtual ~Region();

		/// Returns the group Region belongs to
		Group* get_group();

		// sample definition
		std::string sample;

		// input controls
		int   lochan;    int   hichan;
		int   lokey;     int   hikey;
		int   lovel;     int   hivel;
		//int   loccN;     int   hiccN; //fixme: implement
		int   lobend;    int   hibend;
		int   lochanaft; int   hichanaft;
		int   lopolyaft; int   hipolyaft;
		float lorand;    float hirand;
		int   lobpm;     int   hibpm;

		int seq_length;  
		int seq_position;

		int sw_lokey;    int sw_hikey;  
		int sw_last;
		int sw_down;     
		int sw_up;
		int sw_previous; 
		sw_vel_t sw_vel;

		trigger_t trigger;

		int group;
		int off_by;
		off_mode_t off_mode;

	protected:
		/// Return true if Region is triggered by note
		bool triggered(int chan, int note, int vel, float rand, trigger_t trig);

		friend class Instrument;

	private:
		/// Pointer to the Group this region belongs to
		Group* _group;

		int _seq_position_counter;
		int _last_sw_key;
	};

	/////////////////////////////////////////////////////////////
	// class Group

	/// Defines Group information of an Instrument
	class Group
	{
	public:
		Group(Instrument* parent);
		virtual ~Group();

		/// Add a new region to group
		Region* add_region();
		/// Returns the instrument the Group belongs to
		Instrument* get_instrument();
		/// Returns the first Region of this Group
		Region* get_first_region();
		/// Returns the next Region of this Group
		Region* get_next_region();

		// sample definition
		std::string sample;

		// input controls
		int   lochan;    int   hichan;
		int   lokey;     int   hikey;
		int   lovel;     int   hivel;
		//int   loccN;     int   hiccN; //fixme: implement
		int   lobend;    int   hibend;
		int   lochanaft; int   hichanaft;
		int   lopolyaft; int   hipolyaft;
		float lorand;    float hirand;
		int   lobpm;     int   hibpm;

		int seq_length;  
		int seq_position;

		int sw_lokey;    int sw_hikey;  
		int sw_last;
		int sw_down;     
		int sw_up;
		int sw_previous; 
		sw_vel_t sw_vel;

		trigger_t trigger;

		int group;
		int off_by;
		off_mode_t off_mode;

	private:
		/// Pointer to the Instrument this group belongs to
		Instrument* _instrument;
		/// List of Regions belonging to this Group
		std::vector<Region*> _regions;
	};

	/////////////////////////////////////////////////////////////
	// class Instrument

	/// Provides all neccessary information for the synthesis of an Instrument
	class Instrument
	{
	public:
		Instrument();
		virtual ~Instrument();

		/// Adds a new group to the instrument
		Group* add_group();
		/// Returns the first Group of this Instrument
		Group* get_first_group();
		/// Return the next Group of this Instrument
		Group* get_next_group();

		/// Pitch bend handler
		void pitch_bend(const int value);
		/// Channel pressure handler 
		void channel_pressure(const int value);
		/// Aftertouch handler
		void aftertouch(const int value);

		/// Set the tempo
		void set_bpm(const int value);

		/// Handler returns Regions for a triggred continous controller event
		std::vector<Region*> continous_controller(const int controller, const int value);
		/// Handler returns Regions for a triggered note on event
		std::vector<Region*> note_on(const int channel, const int note, const int velocity);
		/// Handler returns Regions for a triggered note off event
		std::vector<Region*> note_off(const int channel, const int note);

	protected:
		int _controller[128];
		bool _key[128];
		int _vel[128];

		int _keys_pressed;

		int _last_key;
		int _last_vel;

		int _bend;
		int _chanaft;
		int _polyaft;
		int _bpm;

		friend class Region;

	private:
		/// List of Groups belonging to this Instrument
		std::vector<Group*> _groups;
	};

	/////////////////////////////////////////////////////////////
	// class File

	/// Parses SFZ files and provides abstract access to the data
	class File
	{
	public:
		/// Load an existing SFZ file
		File(std::ifstream file);
		virtual ~File();

		/// Returns a pointer to the instrument object
		Instrument* get_instrument();

	private:
		void push_header(std::string token);
		void push_opcode(std::string token);

		/// Pointer to the Instrument belonging to this file
		Instrument* _instrument;

		// state variables
		enum section_t { UNKNOWN, GROUP, REGION };
		section_t _current_section;
		Region* _current_region;
		Group* _current_group;
	};

} // !namespace sfz

#endif // !LIBSFZ_SFZ_H
