/* -*- Mode: C++ ; c-basic-offset: 8 -*- */
#ifndef LIBSFZ_SFZ_H
#define LIBSFZ_SFZ_H

// SFZ 1.0
// Copyright (c) 2008-2009, Anders Dahnielson
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
	class Articulation;
	class Region;
	class Group;
	class Instrument;
	class File;

	// Enumerations
	enum sw_vel_t    { VEL_CURRENT, VEL_PREVIOUS };
	enum off_mode_t  { OFF_FAST, OFF_NORMAL };
	enum loop_mode_t { NO_LOOP, ONE_SHOT, LOOP_CONTINOUS, LOOP_SUSTAIN };
	enum curve_t     { GAIN, POWER };

	typedef unsigned char trigger_t;
	typedef unsigned char uint8_t;

	/////////////////////////////////////////////////////////////
	// class Articulation

	class Articulation
	{
	public:
		Articulation();
		virtual ~Articulation();
	};

	/////////////////////////////////////////////////////////////
	// class Region

	/// Defines Region information of an Instrument
	class Region
	{
	public:
		Region();
		virtual ~Region();

		/// Return true if region is triggered by key
		bool OnKey(uint8_t chan, uint8_t key, uint8_t vel,
			   int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
			   uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
			   float timer, uint8_t seq, bool* sw, uint8_t last_sw_key, uint8_t prev_sw_key);

		/// Return true if region is triggered by control change
		bool OnControl(uint8_t chan, uint8_t cont, uint8_t val,
			       int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
			       uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
			       float timer, uint8_t seq, bool* sw, uint8_t last_sw_key, uint8_t prev_sw_key);

		/// Return an articulation for the current state
 		Articulation* GetArticulation(int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft, uint8_t* cc);

		// unique region id
		int id;

		// sample definition
		std::string sample;

		// input controls
		int   lochan;    int   hichan;
		int   lokey;     int   hikey;
		int   lovel;     int   hivel;
		int   locc[128]; int   hicc[128];
		int   lobend;    int   hibend;
		int   lobpm;     int   hibpm;
		int   lochanaft; int   hichanaft;
		int   lopolyaft; int   hipolyaft;
		int   loprog;    int   hiprog;
		float lorand;    float hirand;
		float lotimer;   float hitimer;

		int seq_length;  
		int seq_position;
		
		int start_locc[128]; int start_hicc[128];
		int stop_locc[128];  int stop_hicc[128];

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

		int on_locc[128]; int on_hicc[128];

		// sample player
		int count;
		float delay; float delay_random; float delay_oncc[128];
		int delay_beats; int stop_beats;
		int delay_samples; int delay_samples_oncc[128];
		int end;
		int loop_crossfade;
		int offset; int offset_random; int offset_oncc[128];
		loop_mode_t loop_mode;
		int loop_start; int loop_end;
		float sync_beats;
		float sync_offset;
		
		// amplifier
		float volume;
		float pan;
		float width;
		float position;
		float amp_keytrack; int amp_keycenter; float amp_veltrack; float amp_velcurve_[128]; float amp_random;
		float rt_decay;
		float gain_oncc[128];
		int xfin_lokey; int xfin_hikey;
		int xfout_lokey; int xfout_hikey;
		curve_t xf_keycurve;
		int xfin_lovel; int xfin_hivel;
		int xfout_lovel; int xfout_hivel;
		curve_t xf_velcurve;
		int xfin_locc[128]; int xfin_hicc[128];
		int xfout_locc[128]; int xfout_hicc[128];
		curve_t xf_cccurve;

		// pitch
		int transpose;
		int tune;
		int pitch_keycenter; int pitch_keytrack; int pitch_veltrack; int pitch_random;
		int bend_up; int bend_down; int bend_step;
	};

	/////////////////////////////////////////////////////////////
	// class Instrument

	/// Provides all neccessary information for the synthesis of an Instrument
	class Instrument
	{
	public:
		Instrument();
		virtual ~Instrument();

		/// List of Regions belonging to this Instrument
		std::vector<Region*> regions;
	};

	/////////////////////////////////////////////////////////////
	// class Group

	/// A Group act just as a template containing Region default values
	class Group
	{
	public:
		Group();
		virtual ~Group();

		/// Reset Group to default values
		void Reset();

		/// Create a new Region
		Region* RegionFactory();

		// id counter
		int id;

		// sample definition
		std::string sample;

		// input controls
		int   lochan;    int   hichan;
		int   lokey;     int   hikey;
		int   lovel;     int   hivel;
		int   locc[128]; int   hicc[128];
		int   lobend;    int   hibend;
		int   lobpm;     int   hibpm;
		int   lochanaft; int   hichanaft;
		int   lopolyaft; int   hipolyaft;
		int   loprog;    int   hiprog;
		float lorand;    float hirand;
		float lotimer;   float hitimer;

		int seq_length;  
		int seq_position;

		int start_locc[128]; int start_hicc[128];
		int stop_locc[128];  int stop_hicc[128];

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

		int on_locc[128]; int on_hicc[128];

		// sample player
		int count;
		float delay; float delay_random; float delay_oncc[128];
		int delay_beats; int stop_beats;
		int delay_samples; int delay_samples_oncc[128];
		int end;
		int loop_crossfade;
		int offset; int offset_random; int offset_oncc[128];
		loop_mode_t loop_mode;
		int loop_start; int loop_end;
		float sync_beats;
		float sync_offset;
		
		// amplifier
		float volume;
		float pan;
		float width;
		float position;
		float amp_keytrack; int amp_keycenter; float amp_veltrack; float amp_velcurve_[128]; float amp_random;
		float rt_decay;
		float gain_oncc[128];
		int xfin_lokey; int xfin_hikey;
		int xfout_lokey; int xfout_hikey;
		curve_t xf_keycurve;
		int xfin_lovel; int xfin_hivel;
		int xfout_lovel; int xfout_hivel;
		curve_t xf_velcurve;
		int xfin_locc[128]; int xfin_hicc[128];
		int xfout_locc[128]; int xfout_hicc[128];
		curve_t xf_cccurve;

		// pitch
		int transpose;
		int tune;
		int pitch_keycenter; int pitch_keytrack; int pitch_veltrack; int pitch_random;
		int bend_up; int bend_down; int bend_step;
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
		Instrument* GetInstrument();

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
