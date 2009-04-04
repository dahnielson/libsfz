/* -*- Mode: C++ ; c-basic-offset: 8 -*- */

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

#include "sfz.h"

#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>

namespace sfz
{

	/////////////////////////////////////////////////////////////
	// class optional

	const optional_base::nothing_t optional_base::nothing;

	/////////////////////////////////////////////////////////////
	// class Articulation

	Articulation::Articulation()
	{
	}

	Articulation::~Articulation()
	{
	}

	/////////////////////////////////////////////////////////////
	// class Region

	Region::Region()
	{
	}

	Region::~Region()
	{
	}

	bool
	Region::OnKey(uint8_t chan, uint8_t key, uint8_t vel,
		      int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
		      uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
		      float timer, uint8_t seq, bool* sw, uint8_t last_sw_key, uint8_t prev_sw_key)
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
	Region::OnControl(uint8_t chan, uint8_t cont, uint8_t val,
			  int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft,
			  uint8_t prog, float rand, trigger_t trig, uint8_t* cc,
			  float timer, uint8_t seq, bool* sw, uint8_t last_sw_key, uint8_t prev_sw_key)
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
 			chan    >= lochan           &&  chan    <= hichan             &&
			((val   >= on_locc[cont]    &&  val     <= on_hicc[cont])     ||
			 (val   >= start_locc[cont] &&  val     <= start_hicc[cont])) &&
 			bend    >= lobend           &&  bend    <= hibend             &&
 			bpm     >= lobpm            &&  bpm     <= hibpm              &&
 			chanaft >= lochanaft        &&  chanaft <= hichanaft          &&
 			polyaft >= lopolyaft        &&  polyaft <= hipolyaft          &&
			prog    >= loprog           &&  prog    <= hiprog             &&
 			rand    >= lorand           &&  rand    <= hirand             &&
			timer   >= lotimer          &&  timer   <= hitimer            &&
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

	Articulation* 
	Region::GetArticulation(int bend, uint8_t bpm, uint8_t chanaft, uint8_t polyaft, uint8_t* cc)
	{
		return new Articulation(); //todo: implement GetArticulation()
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
	Group::Reset()
	{
		// sample definition default
		sample = "";

		// input control defaults
		lochan = 1; hichan = 16;
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

		group.unset();
		off_by.unset();
		off_mode = OFF_FAST;

		// sample player defaults
		count.unset();
		delay.unset(); delay_random.unset(); 
		delay_beats.unset(); stop_beats.unset();
		delay_samples.unset();
		end.unset();
		loop_crossfade.unset();
		offset.unset(); offset_random.unset();
		loop_mode = NO_LOOP;
		loop_start.unset(); loop_end.unset();
		sync_beats.unset(); sync_offset.unset();

		// amplifier defaults
		volume = 0;
		pan = 0;
		width = 100;
		position = 0;
		amp_keytrack = 0;
		amp_keycenter.unset();
		amp_veltrack = 0;
		amp_random = 0;
		rt_decay = 0;
		xfin_lokey.unset();
		xfin_hikey.unset();
		xfout_lokey.unset();
		xfout_hikey.unset();
		xf_keycurve = POWER;
		xfin_lovel.unset();
		xfin_hivel.unset();
		xfout_lovel.unset();
		xfout_hivel.unset();
		xf_velcurve = POWER;
		xf_cccurve = POWER;

		// pitch defaults
		transpose = 0;
		tune = 0;
		pitch_keycenter.unset();
		pitch_keytrack = 100;
		pitch_veltrack = 0;
		pitch_random = 0;
		bend_up = 0;
		bend_down = 0;
		bend_step = 0;

		// CCs defaults
		for (int i = 0; i < 128; ++i)
		{
			// input control
			locc[i] = 0;
			hicc[i] = 127;
			start_locc[i] = -1;
			start_hicc[i] = -1;
			stop_locc[i] = -1;
			stop_hicc[i] = -1;
			on_locc[i] = -1;
			on_hicc[i] = -1;

			// sample player
			delay_oncc[i].unset();
			delay_samples_oncc[i].unset();
			offset_oncc[i].unset();

			// amplifier
			amp_velcurve_[i] = 0;
			gain_oncc[i] = 0;
			xfin_locc[i].unset();
			xfin_hicc[i].unset();
			xfout_locc[i].unset();
			xfout_hicc[i].unset();
		}
	}

	Region*
	Group::RegionFactory()
	{
		Region* region = new Region();

		region->id = id++;

		// sample definition
		region->sample = sample;

		// input control
		region->lochan = lochan;
		region->hichan = hichan;
		region->lokey = lokey;
		region->hikey = hikey;
		region->lovel = lovel;
		region->hivel = hivel;
		region->locc = locc;
		region->hicc = hicc;
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
		region->start_locc = start_locc;
		region->start_hicc = start_hicc;
		region->stop_locc = stop_locc;
		region->stop_hicc = stop_hicc;
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
		region->on_locc = on_locc;
		region->on_hicc = on_hicc;

		// sample player
		region->count = count;
		region->delay = delay;
		region->delay_random = delay_random;
		region->delay_oncc = delay_oncc;
		region->delay_beats = delay_beats;
		region->stop_beats = stop_beats;
		region->delay_samples = delay_samples;
		region->delay_samples_oncc = delay_samples_oncc;
		region->end = end;
		region->loop_crossfade = loop_crossfade;
		region->offset = offset;
		region->offset_random = offset_random;
		region->offset_oncc = offset_oncc;
		region->loop_mode = loop_mode;
		region->loop_start = loop_start;
		region->loop_end = loop_end;
		region->sync_beats = sync_beats;
		region->sync_offset = sync_offset;

		// amplifier
		region->volume = volume;
		region->pan = pan;
		region->width = width;
		region->position = position;
		region->amp_keytrack = amp_keytrack;
		region->amp_keycenter = amp_keycenter;
		region->amp_veltrack = amp_veltrack;
		region->amp_velcurve_ = amp_velcurve_;
		region->amp_random = amp_random;
		region->rt_decay = rt_decay;
		region->gain_oncc = gain_oncc;
		region->xfin_lokey = xfin_lokey;
		region->xfin_hikey = xfin_hikey;
		region->xfout_lokey = xfout_lokey;
		region->xfout_hikey = xfout_hikey;
		region->xf_keycurve = xf_keycurve;
		region->xfin_lovel = xfin_lovel;
		region->xfin_hivel = xfin_lovel;
		region->xfout_lovel = xfout_lovel;
		region->xfout_hivel = xfout_hivel;
		region->xf_velcurve = xf_velcurve;
		region->xfin_locc = xfin_locc;
		region->xfin_hicc = xfin_hicc;
		region->xfout_locc = xfout_locc;
		region->xfout_hicc = xfout_hicc;
		region->xf_cccurve = xf_cccurve;

		// pitch
		region->transpose = transpose;
		region->tune = tune;
		region->pitch_keycenter = pitch_keycenter;
		region->pitch_keytrack = pitch_keytrack;
		region->pitch_veltrack = pitch_veltrack;
		region->pitch_random = pitch_random;
		region->bend_up = bend_up;
		region->bend_down = bend_down;
		region->bend_step = bend_step;

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

		// sample definition
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

		// input controls
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

		// sample player
		else if ("count" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->count = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->count = boost::lexical_cast<int>(value);
			}
		}
		else if ("delay" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->delay = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->delay = boost::lexical_cast<float>(value);
			}
		}
		else if ("delay_random" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->delay_random = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->delay_random = boost::lexical_cast<float>(value);
			}
		}
		else if ("delay_beats" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->delay_beats = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->delay_beats = boost::lexical_cast<int>(value);
			}
		}
		else if ("stop_beats" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->stop_beats = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->stop_beats = boost::lexical_cast<int>(value);
			}
		}
		else if ("delay_samples" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->delay_samples = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->delay_samples = boost::lexical_cast<int>(value);
			}
		}
		else if ("end" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->end = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->end = boost::lexical_cast<int>(value);
			}
		}
		else if ("loop_crossfade" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->loop_crossfade = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->loop_crossfade = boost::lexical_cast<int>(value);
			}
		}
		else if ("offset_random" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->offset_random = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->offset_random = boost::lexical_cast<int>(value);
			}
		}
		else if ("loop_mode" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "no_loop")
					_current_region->loop_mode = NO_LOOP;
				else if (value == "one_shot")
					_current_region->loop_mode = ONE_SHOT;
				else if (value == "loop_continous")
					_current_region->loop_mode = LOOP_CONTINOUS;
				else if (value == "loop_sustain")
					_current_region->loop_mode = LOOP_SUSTAIN;
			case GROUP:
				if (value == "no_loop")
					_current_group->loop_mode = NO_LOOP;
				else if (value == "one_shot")
					_current_group->loop_mode = ONE_SHOT;
				else if (value == "loop_continous")
					_current_group->loop_mode = LOOP_CONTINOUS;
				else if (value == "loop_sustain")
					_current_group->loop_mode = LOOP_SUSTAIN;
			}
		}
		else if ("loop_start" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->loop_start = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->loop_start = boost::lexical_cast<int>(value);
			}
		}
		else if ("loop_end" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->loop_end = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->loop_end = boost::lexical_cast<int>(value);
			}
		}
		else if ("sync_beats" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sync_beats = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->sync_beats = boost::lexical_cast<float>(value);
			}
		}
		else if ("sync_offset" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->sync_offset = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->sync_offset = boost::lexical_cast<float>(value);
			}
		}

		// amplifier
		else if ("volume" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->volume = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->volume = boost::lexical_cast<float>(value);
			}
		}
		else if ("pan" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->pan = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->pan = boost::lexical_cast<float>(value);
			}
		}
		else if ("width" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->width = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->width = boost::lexical_cast<float>(value);
			}
		}
		else if ("position" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->position = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->position = boost::lexical_cast<float>(value);
			}
		}
		else if ("amp_keytrack" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->amp_keytrack = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->amp_keytrack = boost::lexical_cast<float>(value);
			}
		}
		else if ("amp_keycenter" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->amp_keycenter = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->amp_keycenter = boost::lexical_cast<int>(value);
			}
		}
		else if ("amp_veltrack" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->amp_veltrack = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->amp_veltrack = boost::lexical_cast<float>(value);
			}
		}
		else if ("amp_random" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->amp_random = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->amp_random = boost::lexical_cast<float>(value);
			}
		}
		else if ("rt_decay" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->rt_decay = boost::lexical_cast<float>(value);
			case GROUP:
				_current_group->rt_decay = boost::lexical_cast<float>(value);
			}
		}
		else if ("xfin_lokey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfin_lokey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfin_lokey = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfin_hikey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfin_hikey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfin_hikey = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfout_lokey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfout_lokey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfout_lokey = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfout_hikey" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfout_hikey = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfout_hikey = boost::lexical_cast<int>(value);
			}
		}
		else if ("xf_keycurve" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "gain")
					_current_region->xf_keycurve = GAIN;
				else if (value == "power")
					_current_region->xf_keycurve = POWER;
			case GROUP:
				if (value == "gain")
					_current_group->xf_keycurve = GAIN;
				else if (value == "power")
					_current_group->xf_keycurve = POWER;
			}
		}
		else if ("xfin_lovel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfin_lovel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfin_lovel = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfin_hivel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfin_hivel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfin_hivel = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfout_lovel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfout_lovel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfout_lovel = boost::lexical_cast<int>(value);
			}
		}
		else if ("xfout_hivel" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->xfout_hivel = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->xfout_hivel = boost::lexical_cast<int>(value);
			}
		}
		else if ("xf_velcurve" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "gain")
					_current_region->xf_velcurve = GAIN;
				else if (value == "power")
					_current_region->xf_velcurve = POWER;
			case GROUP:
				if (value == "gain")
					_current_group->xf_velcurve = GAIN;
				else if (value == "power")
					_current_group->xf_velcurve = POWER;
			}
		}
		else if ("xf_cccurve" == key)
		{
			switch (_current_section)
			{
			case REGION:
				if (value == "gain")
					_current_region->xf_cccurve = GAIN;
				else if (value == "power")
					_current_region->xf_cccurve = POWER;
			case GROUP:
				if (value == "gain")
					_current_group->xf_cccurve = GAIN;
				else if (value == "power")
					_current_group->xf_cccurve = POWER;
			}
		}
		
		// pitch
		else if ("transpose" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->transpose = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->transpose = boost::lexical_cast<int>(value);
			}
		}
		else if ("tune" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->tune = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->tune = boost::lexical_cast<int>(value);
			}
		}
		else if ("pitch_keycenter" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->pitch_keycenter = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->pitch_keycenter = boost::lexical_cast<int>(value);
			}
		}
		else if ("pitch_keytrack" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->pitch_keytrack = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->pitch_keytrack = boost::lexical_cast<int>(value);
			}
		}
		else if ("pitch_veltrack" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->pitch_veltrack = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->pitch_veltrack = boost::lexical_cast<int>(value);
			}
		}
		else if ("pitch_random" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->pitch_random = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->pitch_random = boost::lexical_cast<int>(value);
			}
		}
		else if ("bend_up" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->bend_up = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->bend_up = boost::lexical_cast<int>(value);
			}
		}
		else if ("bend_down" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->bend_down = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->bend_down = boost::lexical_cast<int>(value);
			}
		}
		else if ("bend_step" == key)
		{
			switch (_current_section)
			{
			case REGION:
				_current_region->bend_step = boost::lexical_cast<int>(value);
			case GROUP:
				_current_group->bend_step = boost::lexical_cast<int>(value);
			}
		}

		// CCs
		else
		{
			std::string::size_type delimiter_index = key.find("cc");
			std::string key_cc = key.substr(0, delimiter_index);
			int num_cc = boost::lexical_cast<int>(key.substr(delimiter_index + 2));

			// input controls
			if ("lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("start_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->start_locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->start_locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("start_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->start_hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->start_hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("stop_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->stop_locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->stop_locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("stop_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->stop_hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->stop_hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("on_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->on_locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->on_locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("on_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->on_hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->on_hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}

			// sample player
			else if ("delay_on" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->delay_oncc[num_cc] = boost::lexical_cast<float>(value);
				case GROUP:
					_current_group->delay_oncc[num_cc] = boost::lexical_cast<float>(value);
				}
			}
			else if ("delay_samples_on" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->delay_samples_oncc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->delay_samples_oncc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("offset_on" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->offset_oncc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->offset_oncc[num_cc] = boost::lexical_cast<int>(value);
				}
			}

			// amplifier
			else if ("gain_on" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->gain_oncc[num_cc] = boost::lexical_cast<float>(value);
				case GROUP:
					_current_group->gain_oncc[num_cc] = boost::lexical_cast<float>(value);
				}
			}
			else if ("xfin_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->xfin_locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->xfin_locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("xfin_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->xfin_hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->xfin_hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("xfout_lo" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->xfout_locc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->xfout_locc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
			else if ("xfout_hi" == key_cc)
			{
				switch (_current_section)
				{
				case REGION:
					_current_region->xfout_hicc[num_cc] = boost::lexical_cast<int>(value);
				case GROUP:
					_current_group->xfout_hicc[num_cc] = boost::lexical_cast<int>(value);
				}
			}
		}
	}
	

} // !namespace sfz
