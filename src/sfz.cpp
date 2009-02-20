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

	/////////////////////////////////////////////////////////////
	// class Region

	Region::Region()
	{
	}

	Region::~Region()
	{
	}

	bool
	Instrument::OnKey(uint8_t chan, uint8_t key, uint8_t vel,
			  uint8_t bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
			  uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
			  float timer, uint8_t seq, uint8_t* sw, uint8_t last_sw_key, uint8_t prev_sw_key)
	{
		// chan        (MIDI channel)
		// key         (MIDI note)
		// vel         (MIDI velocity)

		// bend        (MIDI pitch bend)
		// bpm         (host BPM)
		// chanaft     (MIDI channel pressure)
		// polyaft     (MIDI polyphonic aftertouch)
		// prog        (MIDI program change)
		// rand        (generated random number)
		// trigger     (how it was triggered)
		// cc          (all 128 CC values)

		// timer       (time since previous region in the group was triggered)
		// seq         (the state of the region sequence counter)
		// sw          (the state of region key switches, 128 possible values)
		// last_sw_key (the last key pressed in the key switch range)
		// prev_sw_key (the previous note value)

 		bool is_triggered (
 			chan    >= lochan     &&  chan    <= hichan     &&
 			key     >= lokey      &&  key     <= hikey      &&
 			vel     >= lovel      &&  vel     <= hivel      &&
 			bend    >= lobend     &&  bend    <= hibend     &&
 			bpm     >= lobpm      &&  bpm     <= hibpm      &&
 			chanaft >= lochanaft  &&  chanaft <= hichanaft  &&
 			polyaft >= lopolyaft  &&  polyaft <= hipolyaft  &&
			prog    >= loprog     &&  prog    <= hiprog     &&
 			rand    >= lorand     &&  rand    <= hirand     &&
			timer   >= lotimer    &&  timer   <= hitimer    &&
 			seq == seq_position   &&
			((sw_last >= sw_lokey && sw_last <= sw_hikey) ? (last_sw_key == sw_last) : true)  &&
                        ((sw_down >= sw_lokey && sw_down <= sw_hikey) ? (sw[sw_down]) : true)  &&
                        ((sw_up   >= sw_lokey && sw_up   <= sw_hikey) ? (!sw[sw_up])  : true)  &&
			((sw_previous != -1)                          ? (prev_sw_key == sw_previous) : true)  &&
 			((trigger && trig) != 0)
 			);

		if (!is_triggered)
			return false;

		for (int i = 0; i < 128; ++i)
		{
			if (!(cc[i] >= locc[i] && cc[i] <= hicc[i]))
				return false;
		}

		return true;
	}

	bool
	Instrument::OnControl(uint8_t chan, uint8_t cont, uint8_t val,
			      uint8_t bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
			      uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
			      float timer, uint8_t seq, uint8_t* sw, uint8_t last_sw_key, uint8_t prev_sw_key)
	{
		// chan      (MIDI channel)
		// cont      (MIDI controller)
		// val       (MIDI controller value)

		// bend      (MIDI pitch bend)
		// bpm       (host BPM)
		// chanaft   (MIDI channel pressure)
		// polyaft   (MIDI polyphonic aftertouch)
		// prog      (MIDI program change)
		// rand      (generated random number)
		// trigger   (how it was triggered)
		// cc        (all CC values)

		// timer       (time since previous region in the group was triggered)
		// seq         (the state of the region sequence counter)
		// sw          (the state of region key switches, 128 possible values)
		// last_sw_key (the last key pressed in the key switch range)
		// prev_sw_key (the previous note value)

		bool is_triggered = (
 			chan    >= lochan           &&  chan    <= hichan           &&
			val     >= on_locc[cont]    &&  val     <= on_hicc[cont]    &&
			val     >= start_locc[cont] &&  val     <= start_hicc[cont] &&
 			bend    >= lobend           &&  bend    <= hibend           &&
 			bpm     >= lobpm            &&  bpm     <= hibpm            &&
 			chanaft >= lochanaft        &&  chanaft <= hichanaft        &&
 			polyaft >= lopolyaft        &&  polyaft <= hipolyaft        &&
			prog    >= loprog           &&  prog    <= hiprog           &&
 			rand    >= lorand           &&  rand    <= hirand           &&
			timer   >= lotimer          &&  timer   <= hitimer          &&
 			seq == seq_position   &&
			((sw_last >= sw_lokey && sw_last <= sw_hikey) ? (last_sw_key == sw_last) : true)  &&
                        ((sw_down >= sw_lokey && sw_down <= sw_hikey) ? (sw[sw_down]) : true)  &&
                        ((sw_up   >= sw_lokey && sw_up   <= sw_hikey) ? (!sw[sw_up])  : true)  &&
			((sw_previous != -1)                          ? (prev_sw_key == sw_previous) : true)  &&
 			((trigger && trig) != 0)
 			);

		if (!is_triggered)
			return false;

		for (int i = 0; i < 128; ++i)
		{
			if (!(cc[i] >= locc[i] && cc[i] <= hicc[i]))
				return false;
		}

		return true;
	}

	/////////////////////////////////////////////////////////////
	// class Instrument

	Instrument::Instrument()
	{
	}

	Instrument::~Instrument()
	{
	}

	/////////////////////////////////////////////////////////////
	// class Group

	Group::Group() :
		id(0)
	{
		Reset();
	}

	Group::~Group()
	{
	}

	void
	Reset()
	{
		// sample definition
		sample = "";

		// input control defaults
		lohan = 1; hichan = 16;
		lokey = 0; hikey = 127;
		lovel = 0; hivel = 127;
		lobend = -8192; hibend = 8192;
		lobpm = 0; hibpm = 500;
		lochanaft = 0; hichanaft = 127;
		lopolyaft = 0; hipolyaft = 127;
		loprog = 0; hiprog = 127;
		lorand = 0.0; hirand = 1.0;
		lotimer = 0.0; hitimer = 0.0;

		seq_length = 1;
		seq_position = 1;

		sw_lokey = -1; sw_hikey = -1;
		sw_last = -1;
		sw_down = -1;
		sw_up = -1;
		sw_previous = -1;
		sw_vel = VEL_CURRENT;

		trigger = TRIGGER_ATTACK;

		group = -1;
		off_by = -1;
		off_mode = OFF_FAST;

		for (int i = 0; i < 128; ++i)
		{
			locc[i] = 0;
			hicc[i] = 127;
			start_locc[i] = -1;
			start_hicc[i] = -1;
			stop_locc[i] = -1;
			stop_hicc[i] = -1;
			on_locc[i] = -1;
			on_hicc[i] = -1;
		}
	}

	Region*
	Group::RegionFactory()
	{
		Region* region = new Region(this);

		region->id = id++;
		region->sample = sample;
		region->lochan = lochan;
		region->hichan = hichan;
		region->lokey = lokey;
		region->hikey = hikey;
		region->lovel = lovel;
		region->hivel = hivel;
		region->lobend = lobend;
		region->hibend = hibend;
		region->lobpm = lobpm;
		region->hibpm = hibpm;
		region->lochanaft = lochanaft;
		region->hichanaft = hichanaft;
		region->lopolyaft = lopolyaft;
		region->hipolyaft = hipolyaft;
		region->loprog = loprog;
		region->hiprog = hiprog;
		region->lorand = lorand;
		region->hirand = hirand;
		region->lotimer = lotimer;
		region->hitimer = hitimer;
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

		for (int i = 0; i < 128; ++i)
		{
			region->locc[i] = locc[i];
			region->hicc[i] = hicc[i];
			region->start_locc[i] = start_locc[i];
			region->start_hicc[i] = start_hicc[i];
			region->stop_locc[i] = stop_locc[i];
			region->stop_hicc[i] = stop_hicc[i];
			region->on_locc[i] = on_locc[i];
			region->on_hicc[i] = on_hicc[i];
		}

		return region;
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
	File::GetInstrument()
	{
		return _instrument;
	}

	void 
	File::push_header(std::string token)
	{
		if (token == "<group>")
		{
			_current_section = GROUP;
			_current_group->Reset();
		}
		else if (token == "<region>")
		{
			_current_section = REGION;
			_current_region = _current_group->RegionFactory();
			_instrument->regions.push_back(_current_region);
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
		else if ("key" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lokey = boost::lexical_cast<int>(value);
				_current_region->hikey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->lokey = boost::lexical_cast<int>(value);
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
		else if ("loprog" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->loprog = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->loprog = boost::lexical_cast<int>(value);
			}
		}
		else if ("hiprog" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hiprog = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->hiprog = boost::lexical_cast<int>(value);
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
		else if ("lotimer" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->lotimer = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->lotimer = boost::lexical_cast<float>(value);
			}
		}
		else if ("hitimer" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->hitimer = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->hitimer = boost::lexical_cast<float>(value);
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
		else
		{
			std::string::size_type delimiter_index = key.find("cc");
			std::string key_cc = key.substr(0, delimiter_index);
			std::string num_cc = key.substr(delimiter_index + 2);

			if ("lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->locc[num_cc] = value;
				case GROUP:
					_current_group->locc[num_cc] = value;
				}
			}
			else if ("hi" = key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->hicc[num_cc] = value;
				case GROUP:
					_current_group->hicc[num_cc] = value;
				}
			}
			else if ("start_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->start_locc[num_cc] = value;
				case GROUP:
					_current_group->start_locc[num_cc] = value;
				}
			}
			else if ("start_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->start_hicc[num_cc] = value;
				case GROUP:
					_current_group->start_hicc[num_cc] = value;
				}
			}
			else if ("stop_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->stop_locc[num_cc] = value;
				case GROUP:
					_current_group->stop_locc[num_cc] = value;
				}
			}
			else if ("stop_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->stop_hicc[num_cc] = value;
				case GROUP:
					_current_group->stop_hicc[num_cc] = value;
				}
			}
			else if ("on_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->on_locc[num_cc] = value;
				case GROUP:
					_current_group->on_locc[num_cc] = value;
				}
			}
			else if ("on_hi" = key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->on_hicc[num_cc] = value;
				case GROUP:
					_current_group->on_hicc[num_cc] = value;
				}
			}
		}
	}
	

} // !namespace sfz
