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
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include <boost/array.hpp>

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
	enum filter_t    { LPF_1P, HPF_1P, BPF_1P, BRF_1P, APF_1P, 
			   LPF_2P, HPF_2P, BPF_2P, BRF_2P, PKF_2P, 
			   LPF_4P, HPF_4P, 
			   LPF_6P, HPF_6P };

	typedef unsigned char trigger_t;
	typedef unsigned char uint8_t;

	/////////////////////////////////////////////////////////////
	// class Exception

	class Exception : 
		public std::runtime_error 
	{
        public:
		Exception(const std::string& msg) : 
			runtime_error(msg) 
		{
		}
		
		std::string Message() 
		{
			return what();
		}
		
		void PrintMessage() 
		{
			std::cerr << what() << std::endl << std::flush;
		}
	};

	/////////////////////////////////////////////////////////////
	// class optional

	//  Handy class nicked from LinuxSampler...
	//  Copyright (C) 2003, 2004 by Benno Senoner and Christian Schoenebeck 
	//  Copyright (C) 2005, 2006 Christian Schoenebeck  

	class optional_base 
	{
        public:
		class nothing_t { public: nothing_t() {} };
		static const nothing_t nothing;
	};

	template<class T>
	class optional : 
		public optional_base 
	{
        public:
		optional() 
		{
			initialized = false;
		}

		optional(T data) 
		{
			this->data  = data;
			initialized = true;
		}
		
		optional(nothing_t) 
		{
			initialized = false;
		}
		
		template <class T_inner>
		optional(T_inner data) 
		{
			this->data  = T(data);
			initialized = true;
		}
		
		const T& get() const throw (Exception) 
		{
			if (!initialized) throw Exception("optional variable not initialized");
			return data;
		}
		
		T& get() throw (Exception) 
		{
			if (!initialized) throw Exception("optional variable not initialized");
			return data;
		}

		void unset()
		{
			initialized = false;
		}
		
		optional& operator =(const optional& arg) throw (Exception) 
		{
			if (!arg.initialized) throw Exception("optional variable not initialized");
			this->data  = arg.data;
			initialized = true;
			return *this;
		}
		
		optional& operator =(const T& arg) 
		{
			this->data  = arg;
			initialized = true;
			return *this;
		}
		
		const T& operator *() const throw (Exception) { return get(); }
		T&       operator *()       throw (Exception) { return get(); }
		
		const T* operator ->() const throw (Exception) 
		{
			if (!initialized) throw Exception("optional variable not initialized");
			return &data;
		}
		
		T* operator ->() throw (Exception) 
		{
			if (!initialized) throw Exception("optional variable not initialized");
			return &data;
		}
		
		operator bool()   const { return initialized; }
		bool operator !() const { return !initialized; }
		
        protected:
		T    data;
		bool initialized;
	};

	/////////////////////////////////////////////////////////////
	// class Articulation

	// Articulation containing all performance parameters for synthesis
	class Articulation
	{
	public:
		Articulation();
		virtual ~Articulation();
	};

	/////////////////////////////////////////////////////////////
	// class Definition

	// Base definition used by groups and regions
	class Definition
	{
	public:
		Definition();
		virtual ~Definition();

		// sample definition
		std::string sample;

		// input controls
		int   lochan;    int   hichan;
		int   lokey;     int   hikey;
		int   lovel;     int   hivel;
		boost::array<int, 128> locc; boost::array<int, 128> hicc;
		int   lobend;    int   hibend;
		int   lobpm;     int   hibpm;
		int   lochanaft; int   hichanaft;
		int   lopolyaft; int   hipolyaft;
		int   loprog;    int   hiprog;
		float lorand;    float hirand;
		float lotimer;   float hitimer;

		int seq_length;  
		int seq_position;

		boost::array<int, 128> start_locc; boost::array<int, 128> start_hicc;
		boost::array<int, 128> stop_locc;  boost::array<int, 128> stop_hicc;

		int sw_lokey;    int sw_hikey;  
		int sw_last;
		int sw_down;     
		int sw_up;
		int sw_previous; 
		sw_vel_t sw_vel;

		trigger_t trigger;

		optional<int> group;
		optional<int> off_by;
		off_mode_t off_mode;

		boost::array<int, 128> on_locc; boost::array<int, 128> on_hicc;

		// sample player
		optional<int> count;
		optional<float> delay; optional<float> delay_random; boost::array<optional<float>, 128> delay_oncc;
		optional<int> delay_beats; optional<int> stop_beats;
		optional<int> delay_samples; boost::array<optional<int>, 128> delay_samples_oncc;
		optional<int> end;
		optional<int> loop_crossfade;
		optional<int> offset; optional<int> offset_random; boost::array<optional<int>, 128> offset_oncc;
		loop_mode_t loop_mode;
		optional<int> loop_start; optional<int> loop_end;
		optional<float> sync_beats;
		optional<float> sync_offset;
		
		// amplifier
		float volume;
		float pan;
		float width;
		float position;
		float amp_keytrack; optional<int> amp_keycenter; float amp_veltrack; boost::array<optional<float>, 128> amp_velcurve_; float amp_random;
		float rt_decay;
		boost::array<float, 128> gain_oncc;
		optional<int> xfin_lokey; optional<int> xfin_hikey;
		optional<int> xfout_lokey; optional<int> xfout_hikey;
		curve_t xf_keycurve;
		optional<int> xfin_lovel; optional<int> xfin_hivel;
		optional<int> xfout_lovel; optional<int> xfout_hivel;
		curve_t xf_velcurve;
		boost::array<optional<int>, 128> xfin_locc; boost::array<optional<int>, 128> xfin_hicc;
		boost::array<optional<int>, 128> xfout_locc; boost::array<optional<int>, 128> xfout_hicc;
		curve_t xf_cccurve;

		// pitch
		int transpose;
		int tune;
		optional<int> pitch_keycenter; int pitch_keytrack; int pitch_veltrack; int pitch_random;
		int bend_up; int bend_down; int bend_step;

		// filter
		filter_t fil_type; filter_t fil2_type;
		optional<float> cutoff; optional<float> cutoff2;
		boost::array<int, 128> cutoff_oncc; boost::array<int, 128> cutoff2_oncc;
		boost::array<int, 128> cutoff_smoothcc; boost::array<int, 128> cutoff2_smoothcc;
		boost::array<int, 128> cutoff_stepcc; boost::array<int, 128> cutoff2_stepcc;
		boost::array<int, 128> cutoff_curvecc; boost::array<int, 128> cutoff2_curvecc;
		int cutoff_chanaft; int cutoff2_chanaft;
		int cutoff_polyaft; int cutoff2_polyaft;
		float resonance; float resonance2;
		boost::array<int, 128> resonance_oncc; boost::array<int, 128> resonance2_oncc;
		boost::array<int, 128> resonance_smoothcc; boost::array<int, 128> resonance2_smoothcc;
		boost::array<int, 128> resonance_stepcc; boost::array<int, 128> resonance2_stepcc;
		boost::array<int, 128> resonance_curvecc; boost::array<int, 128> resonance2_curvecc;
		int fil_keytrack; int fil2_keytrack;
		int fil_keycenter; int fil2_keycenter;
		int fil_veltrack; int fil2_veltrack;
		int fil_random; int fil2_random;

		// per voice equalizer
		float eq1_freq; float eq2_freq; float eq3_freq;
		boost::array<float, 128> eq1_freq_oncc; boost::array<float, 128> eq2_freq_oncc; boost::array<float, 128> eq3_freq_oncc;
		float eq1_vel2freq; float eq2_vel2freq; float eq3_vel2freq;
		float eq1_bw; float eq2_bw; float eq3_bw;
		boost::array<float, 128> eq1_bw_oncc; boost::array<float, 128> eq2_bw_oncc; boost::array<float, 128> eq3_bw_oncc;
		float eq1_gain; float eq2_gain; float eq3_gain;
		boost::array<float, 128> eq1_gain_oncc; boost::array<float, 128> eq2_gain_oncc; boost::array<float, 128> eq3_gain_oncc;
		float eq1_vel2gain; float eq2_vel2gain; float eq3_vel2gain;
	};
	
	/////////////////////////////////////////////////////////////
	// class Region

	/// Defines Region information of an Instrument
	class Region :
		public Definition
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
	class Group :
		public Definition
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
		enum section_t { UNKNOWN, GROUP, REGION, CONTROL };
		section_t _current_section;
		Region* _current_region;
		Group* _current_group;

		// control header directives
		std::string default_path;
		int octave_offset;
		int note_offset;
	};

} // !namespace sfz

#endif // !LIBSFZ_SFZ_H
