/* -*- Mode: C++ ; c-basic-offset: 8 -*- */

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

#include "sfz.h"

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>

namespace sfz
{

	// seed the random number generator
	void initrand()
	{
		srand((unsigned)(time(0)));
	} 
	
	// generates a psuedo-random float between 0.0 and 0.999...
	inline float randfloat()
	{
		return rand()/(float(RAND_MAX)+1);
	} 

	/////////////////////////////////////////////////////////////
	// class Region

	Region::Region(Group* parent) :
		_group(parent),

		// counters
		_seq_position_counter(1),

		// keys
		_last_sw_key(-1)
	{
	}

	Region::~Region()
	{
	}

	Group*
	Region::get_group()
	{
		return _group;
	}

	bool 
	Region::triggered(int chan, int note, int vel, float rand, trigger_t trig)
	{
		// values from instrument...
		int bend = get_group()->get_instrument()->_bend;
		int chanaft = get_group()->get_instrument()->_chanaft;
		int polyaft = get_group()->get_instrument()->_polyaft;
		int bpm = get_group()->get_instrument()->_bpm;

		// override velocity if...
		if (sw_vel == VEL_PREVIOUS)
			vel = get_group()->get_instrument()->_last_vel;

		// region will play if...
		bool is_triggered = (
			chan    >= lochan     &&  chan    <= hichan     &&
			note    >= lokey      &&  note    <= hikey      &&
			vel     >= lovel      &&  vel     <= hivel      &&
			bend    >= lobend     &&  bend    <= hibend     &&
			chanaft >= lochanaft  &&  chanaft <= hichanaft  &&
			polyaft >= lopolyaft  &&  polyaft <= hipolyaft  &&
			rand    >= lorand     &&  rand    <= hirand     &&
			bpm     >= lobpm      &&  bpm     <= hibpm      &&
			_seq_position_counter == seq_position           &&
			((sw_last >= sw_lokey && sw_last <= sw_hikey) ? (_last_sw_key == sw_last)                                 : true)  &&
			((sw_down >= sw_lokey && sw_down <= sw_hikey) ? (get_group()->get_instrument()->_key[sw_down])            : true)  &&
			((sw_up   >= sw_lokey && sw_up   <= sw_hikey) ? (!get_group()->get_instrument()->_key[sw_up])             : true)  &&
			((sw_previous != -1)                          ? (get_group()->get_instrument()->_last_key == sw_previous) : true)  &&
			((trigger && trig) != 0)
			);

		// keep track of last pressed key in switch range...
		if (note >= sw_lokey && note <= sw_hikey)
			_last_sw_key = note;

		// advance seq_position counter on note-on...
		if ((trig && TRIGGER_RELEASE) == 0)
			(_seq_position_counter < seq_length) ? _seq_position_counter++ : _seq_position_counter = 1;

		return is_triggered;
	}

	/////////////////////////////////////////////////////////////
	// class Group

	Group::Group(Instrument* parent) :
		_instrument(parent),

		// input control defaults
		lochan(1),     hichan(16),
		lokey(0),      hikey(127),
		lovel(0),      hivel(127),
		lobend(-8192), hibend(8192),
		lochanaft(0),  hichanaft(127),
		lopolyaft(0),  hipolyaft(127),
		lorand(0.0),   hirand(1.0),
		lobpm(0),      hibpm(500),

		seq_length(1), 
		seq_position(1),

		sw_lokey(-1),  sw_hikey(-1),
		sw_last(-1), 
		sw_down(-1),
		sw_up(-1),
		sw_previous(-1),
		sw_vel(VEL_CURRENT),

		trigger(TRIGGER_ATTACK),

		group(-1),
		off_by(-1),
		off_mode(OFF_FAST)
	{
	}

	Group::~Group()
	{
	}

	Region*
	Group::add_region()
	{
		Region* region = new Region(this);

		region->lochan = lochan;
		region->hichan = hichan;
		region->lokey = lokey;
		region->hikey = hikey;
		region->lovel = lovel;
		region->hivel = hivel;
		region->lobend = lobend;
		region->hibend = hibend;
		region->lochanaft = lochanaft;
		region->hichanaft = hichanaft;
		region->lopolyaft = lopolyaft;
		region->hipolyaft = hipolyaft;
		region->lorand = lorand;
		region->hirand = hirand;
		region->lobpm = lobpm;
		region->hibpm = hibpm;
		region->seq_length = seq_length;
		region->seq_position = seq_position;
		region->sw_lokey = sw_lokey;
		region->sw_hikey = sw_hikey;
		region->sw_last = sw_last;
		region->sw_down = sw_down;
		region->sw_up = sw_up;
		region->sw_previous = sw_previous;
		region->sw_vel = sw_vel;
		region->trigger = trigger;
		region->group = group;
		region->off_by = off_by;
		region->off_mode = off_mode;

		_regions.push_back(region);
		return region;
	}

	Instrument*
	Group::get_instrument()
	{
		return _instrument;
	}

	Region*
	Group::get_first_region() //fixme: implement
	{
		return NULL;
	}

	Region*
	Group::get_next_region() //fixme: implement
	{
		return NULL;
	}

	/////////////////////////////////////////////////////////////
	// class Instrument

	Instrument::Instrument() :
		_bend(0),
		_chanaft(0),
		_polyaft(0),
		_bpm(120),

		_last_key(-1),
		_last_vel(-1)
	{
		initrand();
		for (int i = 0; i < 128; ++i)
		{
			_key[i] = false;
			_vel[i] = 0;
			_controller[i] = 0;
		}
	}

	Instrument::~Instrument()
	{
	}

	Group*
	Instrument::add_group()
	{
		Group* group = new Group(this);
		_groups.push_back(group);
		return group;
	}

	Group*
	Instrument::get_first_group() //fixme: implement
	{
		return NULL;
	}

	Group*
	Instrument::get_next_group() //fixme: implement
	{
		return NULL;
	}

	void 
	Instrument::pitch_bend(const int value)
	{
		_bend = value;
	}

	void 
	Instrument::channel_pressure(const int value)
	{
		_chanaft = value;
	}

	void 
	Instrument::aftertouch(const int value)
	{
		_polyaft = value;
	}

	void 
	Instrument::set_bpm(const int value)
	{
		_bpm = value;
	}

	std::vector<Region*>
	Instrument::continous_controller(const int controller, const int value) //fixme: implement
	{
		_controller[controller] = value;

		std::vector<Region*> triggered_regions;
		return triggered_regions;
	}

	std::vector<Region*> 
	Instrument::note_on(const int channel, const int note, const int velocity)
	{
		// figure out trigger type...
		trigger_t trigger = TRIGGER_ATTACK | ((_keys_pressed > 0) ? TRIGGER_LEGATO :  TRIGGER_FIRST);

		// keep track of key states...
		_key[note] = true;
		_vel[note] = note;
		_keys_pressed++;

		// keep track of last pressed key and velocity...
		_last_key = note;
		_last_vel = velocity;

		// find triggered regions...
		float rand = randfloat();
		std::vector<Region*> triggered_regions;
		for (Group* group = get_first_group(); group != 0; group = get_next_group())
		{
			for (Region* region = group->get_first_region(); region != 0; region = group->get_next_region())
			{
				if (region->triggered(channel, note, velocity, rand, trigger))
					triggered_regions.push_back(region);
			}
		}
		return triggered_regions;
	}

	std::vector<Region*> 
	Instrument::note_off(const int channel, const int note)
	{
		// keep track of key states...
		_key[note] = false;
		if (_keys_pressed > 0)
			_keys_pressed--;

		// find triggered regions...
		float rand = randfloat();
		std::vector<Region*> triggered_regions;
		for (Group* group = get_first_group(); group != 0; group = get_next_group())
		{
			for (Region* region = group->get_first_region(); region != 0; region = group->get_next_region())
			{
				if (region->triggered(channel, note, _vel[note], rand, TRIGGER_RELEASE))
					triggered_regions.push_back(region);
			}
		}
		return triggered_regions;
	}

	/////////////////////////////////////////////////////////////
	// class File

	File::File(std::ifstream file) :
		_instrument(new Instrument()),
		_current_section(GROUP)
	{
		enum token_type_t { HEADER, OPCODE };
		token_type_t token_type;
		std::string token_string;

		std::string token;
		std::string line;

		while (std::getline(file, line))
		{
			// COMMENT
			std::string::size_type slash_index = line.find("//");
			if (slash_index != std::string::npos)
				line.resize(slash_index);

			// DEFINITION
			std::stringstream linestream(line);
			while (linestream >> token)
			{
				if (token[0] == '<' and token[token.size()-1] == '>')
				{
					// HEAD
					if (!token_string.empty())
					{
						switch (token_type)
						{
						case HEADER:
							push_header(token_string);
							break;
						case OPCODE:
							push_opcode(token_string);
							break;
						}
						token_string.erase();
					}
					token_string.append(token);
					token_type = HEADER;
				}
				else if (token.find('=') != std::string::npos)
				{
					// HEAD
					if (!token_string.empty())
					{
						switch (token_type)
						{
						case HEADER:
							push_header(token_string);
							break;
						case OPCODE:
							push_opcode(token_string);
							break;
						}
						token_string.erase();
					}
					token_string.append(token);
					token_type = OPCODE;
				}
				else
				{
					// TAIL
					token_string.append(" ");
					token_string.append(token);
				}
			}

			// EOL
			if (!token_string.empty())
			{
				switch (token_type)
				{
				case HEADER:
					push_header(token_string);
					break;
				case OPCODE:
					push_opcode(token_string);
					break;
				}
				token_string.erase();
			}
		}
	}

	File::~File()
	{
	}

	Instrument*
	File::get_instrument()
	{
		return _instrument;
	}

	void 
	File::push_header(std::string token)
	{
		if (token == "<group>")
		{
			_current_section = GROUP;
			_current_group = _instrument->add_group();
		}
		else if (token == "<region>")
		{
			_current_section = REGION;
			_current_region = _current_group->add_region();
		}
		else 
		{
			_current_section = UNKNOWN;
		}
	}
	
	void 
	File::push_opcode(std::string token)
	{
		if (_current_section == UNKNOWN)
			return;

		std::string::size_type delimiter_index = token.find('=');
		std::string key = token.substr(0, delimiter_index);
		std::string value = token.substr(delimiter_index + 1);

		if ("sample" == key) 
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sample = value;
			case GROUP:
				_current_group->sample = value;
			}
		}
		else if ("lochan" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lochan = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lochan = boost::lexical_cast<int>(value);
			}
		}
		else if ("hichan" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hichan = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hichan = boost::lexical_cast<int>(value);
			}
		}
		else if ("lokey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lokey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lokey = boost::lexical_cast<int>(value);
			}
		}
		else if ("hikey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hikey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hikey = boost::lexical_cast<int>(value);
			}
		}
		else if ("lovel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lovel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lovel = boost::lexical_cast<int>(value);
			}
		}
		else if ("hivel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hivel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hivel = boost::lexical_cast<int>(value);
			}
		}
		else if ("lobend" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lobend = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lobend = boost::lexical_cast<int>(value);
			}
		}
		else if ("hibend" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hibend = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hibend = boost::lexical_cast<int>(value);
			}
		}
		else if ("lochanaft" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lochanaft = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lochanaft = boost::lexical_cast<int>(value);
			}
		}
		else if ("hichanaft" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hichanaft = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hichanaft = boost::lexical_cast<int>(value);
			}
		}
		else if ("lopolyaft" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lopolyaft = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lopolyaft = boost::lexical_cast<int>(value);
			}
		}
		else if ("hipolyaft" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hipolyaft = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hipolyaft = boost::lexical_cast<int>(value);
			}
		}
		else if ("lorand" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lorand = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->lorand = boost::lexical_cast<float>(value);
			}
		}
		else if ("hirand" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hirand = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->hirand = boost::lexical_cast<float>(value);
			}
		}
		else if ("lobpm" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lobpm = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lobpm = boost::lexical_cast<int>(value);
			}
		}
		else if ("hibpm" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hibpm = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hibpm = boost::lexical_cast<int>(value);
			}
		}
		else if ("seq_length" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->seq_length = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->seq_length = boost::lexical_cast<int>(value);
			}
		}
		else if ("seq_position" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->seq_position = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->seq_position = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_lokey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_lokey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_lokey = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_hikey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_hikey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_hikey = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_last" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_last = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_last = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_down" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_down = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_down = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_up" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_up = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_up = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_previous" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sw_previous = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->sw_previous = boost::lexical_cast<int>(value);
			}
		}
		else if ("sw_vel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "current") 
					_current_region->sw_vel = VEL_CURRENT;
				else if (value == "previous") 
					_current_region->sw_vel = VEL_PREVIOUS;
			case GROUP:
				if (value == "current") 
					_current_group->sw_vel = VEL_CURRENT;
				else if (value == "previous") 
					_current_group->sw_vel = VEL_PREVIOUS;
			}
		}
		else if ("trigger" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "attack") 
					_current_region->trigger = TRIGGER_ATTACK;
				else if (value == "release") 
					_current_region->trigger = TRIGGER_RELEASE;
				else if (value == "first") 
					_current_region->trigger = TRIGGER_FIRST;
				else if (value == "legato") 
					_current_region->trigger = TRIGGER_LEGATO;
			case GROUP:
				if (value == "attack") 
					_current_group->trigger = TRIGGER_ATTACK;
				else if (value == "release") 
					_current_group->trigger = TRIGGER_RELEASE;
				else if (value == "first") 
					_current_group->trigger = TRIGGER_FIRST;
				else if (value == "legato") 
					_current_group->trigger = TRIGGER_LEGATO;
			}
		}
		else if ("group" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->group = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->group = boost::lexical_cast<int>(value);
			}
		}
		else if ("off_by" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->off_by = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->off_by = boost::lexical_cast<int>(value);
			}
		}
		else if ("off_mode" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "fast") 
					_current_region->off_mode = OFF_FAST;
				else if (value == "normal") 
					_current_region->off_mode = OFF_NORMAL;
			case GROUP:
				if (value == "fast") 
					_current_group->off_mode = OFF_FAST;
				else if (value == "normal") 
					_current_group->off_mode = OFF_NORMAL;
			}
		}
	}
	

} // !namespace sfz
