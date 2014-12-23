
#include "alsaprint.h"

alsaprint alsaprint::single;
const char* alsaprint::EVENT_TO_STR[255];


std::ostream& operator<< (std::ostream& stream, snd_seq_event_t &ev)
{
  stream << "event " << (int) ev.source.client << ":" << (int) ev.source.port;
  stream << "->" << (int) ev.dest.client << ":" << (int) ev.dest.port << " ";
  stream << alsaprint::EVENT_TO_STR[ev.type] << " ";
  switch (ev.type) {
  case SND_SEQ_EVENT_SYSTEM: // system status; event data type = snd_seq_result_t
    stream << "SND_SEQ_EVENT_SYSTEM";
    break;

  case SND_SEQ_EVENT_RESULT: // returned result status; event data type = snd_seq_result_t
    stream << "SND_SEQ_EVENT_RESULT";
    break;

  case SND_SEQ_EVENT_NOTE: // note on and off with duration; event data type = snd_seq_ev_note_t
  case SND_SEQ_EVENT_NOTEON: // note on; event data type = snd_seq_ev_note_t
  case SND_SEQ_EVENT_NOTEOFF: // note off; event data type = snd_seq_ev_note_t
  case SND_SEQ_EVENT_KEYPRESS 	: //key pressure change (aftertouch); event data type = snd_seq_ev_note_t
    stream << ev.data.note;
    break;

  case SND_SEQ_EVENT_CONTROLLER : //controller; event data type = snd_seq_ev_ctrl_t
  case SND_SEQ_EVENT_PGMCHANGE 	: //program change; event data type = snd_seq_ev_ctrl_t
    stream << ev.data.control << " ";
    break;

  case SND_SEQ_EVENT_CHANPRESS 	: //channel pressure; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_CHANPRESS";
    break;

  case SND_SEQ_EVENT_PITCHBEND 	: //pitchwheel; event data type = snd_seq_ev_ctrl_t; data is from -8192 to 8191)
    stream << "SND_SEQ_EVENT_PITCHBEND";
    break;

  case SND_SEQ_EVENT_CONTROL14 	: //14 bit controller value; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_CONTROL14";
    break;

  case SND_SEQ_EVENT_NONREGPARAM 	: //14 bit NRPN; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_NONREGPARAM";
    break;

  case SND_SEQ_EVENT_REGPARAM 	: //14 bit RPN; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_REGPARAM";
    break;

  case SND_SEQ_EVENT_SONGPOS 	: //SPP with LSB and MSB values; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_SONGPOS";
    break;

  case SND_SEQ_EVENT_SONGSEL 	: //Song Select with song ID number; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_SONGSEL";
    break;

  case SND_SEQ_EVENT_QFRAME 	: //midi time code quarter frame; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_QFRAME";
    break;

  case SND_SEQ_EVENT_TIMESIGN 	: //SMF Time Signature event; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_TIMESIGN";
    break;

  case SND_SEQ_EVENT_KEYSIGN 	: //SMF Key Signature event; event data type = snd_seq_ev_ctrl_t
    stream << "SND_SEQ_EVENT_KEYSIGN";
    break;

  case SND_SEQ_EVENT_START 	: //MIDI Real Time Start message; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_START";
    break;

  case SND_SEQ_EVENT_CONTINUE 	: //MIDI Real Time Continue message; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_CONTINUE";
    break;

  case SND_SEQ_EVENT_STOP 	: //MIDI Real Time Stop message; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_STOP";
    break;

  case SND_SEQ_EVENT_SETPOS_TICK 	: //Set tick queue position; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_SETPOS_TICK";
    break;

  case SND_SEQ_EVENT_SETPOS_TIME 	: //Set real-time queue position; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_SETPOS_TIME";
    break;

  case SND_SEQ_EVENT_TEMPO 	: //(SMF) Tempo event; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_TEMPO";
    break;

  case SND_SEQ_EVENT_CLOCK 	: //MIDI Real Time Clock message; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_CLOCK";
    break;

  case SND_SEQ_EVENT_TICK 	: //MIDI Real Time Tick message; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_TICK";
    break;

  case SND_SEQ_EVENT_QUEUE_SKEW 	: //Queue timer skew; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_QUEUE_SKEW";
    break;

  case SND_SEQ_EVENT_SYNC_POS 	: //Sync position changed; event data type = snd_seq_ev_queue_control_t
    stream << "SND_SEQ_EVENT_SYNC_POS";
    break;

  case SND_SEQ_EVENT_TUNE_REQUEST 	: //Tune request; event data type = none
    stream << "SND_SEQ_EVENT_TUNE_REQUEST";
    break;

  case SND_SEQ_EVENT_RESET 	: //Reset to power-on state; event data type = none
    stream << "SND_SEQ_EVENT_RESET";
    break;

  case SND_SEQ_EVENT_SENSING 	: //Active sensing event; event data type = none
    stream << "SND_SEQ_EVENT_SENSING";
    break;

  case SND_SEQ_EVENT_ECHO 	: //Echo-back event; event data type = any type
    stream << "SND_SEQ_EVENT_ECHO";
    break;

  case SND_SEQ_EVENT_OSS 	: //OSS emulation raw event; event data type = any type
    stream << "SND_SEQ_EVENT_OSS";
    break;

  case SND_SEQ_EVENT_CLIENT_START 	: //New client has connected; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_CLIENT_START";
    break;

  case SND_SEQ_EVENT_CLIENT_EXIT 	: //Client has left the system; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_CLIENT_EXIT";
    break;

  case SND_SEQ_EVENT_CLIENT_CHANGE 	: //Client status/info has changed; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_CLIENT_CHANGE";
    break;

  case SND_SEQ_EVENT_PORT_START 	: //New port was created; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_PORT_START";
    break;

  case SND_SEQ_EVENT_PORT_EXIT 	: //Port was deleted from system; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_PORT_EXIT";
    break;

  case SND_SEQ_EVENT_PORT_CHANGE 	: //Port status/info has changed; event data type = snd_seq_addr_t
    stream << "SND_SEQ_EVENT_PORT_CHANGE";
    break;

  case SND_SEQ_EVENT_PORT_SUBSCRIBED 	: //Ports connected; event data type = snd_seq_connect_t
    break;

  case SND_SEQ_EVENT_PORT_UNSUBSCRIBED 	: //Ports disconnected; event data type = snd_seq_connect_t
    break;

  case SND_SEQ_EVENT_USR0 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR0";
    break;

  case SND_SEQ_EVENT_USR1 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR1";
    break;

  case SND_SEQ_EVENT_USR2 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR2";
    break;

  case SND_SEQ_EVENT_USR3 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR3";
    break;

  case SND_SEQ_EVENT_USR4 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR4";
    break;

  case SND_SEQ_EVENT_USR5 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR5";
    break;

  case SND_SEQ_EVENT_USR6 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR6";
    break;

  case SND_SEQ_EVENT_USR7 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR7";
    break;

  case SND_SEQ_EVENT_USR8 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR8";
    break;

  case SND_SEQ_EVENT_USR9 	: //user-defined event; event data type = any (fixed size)
    stream << "SND_SEQ_EVENT_USR9";
    break;

  case SND_SEQ_EVENT_SYSEX 	: //system exclusive data (variable length); event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_SYSEX";
    break;

  case SND_SEQ_EVENT_BOUNCE 	: //error event; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_BOUNCE";
    break;

  case SND_SEQ_EVENT_USR_VAR0 	: //reserved for user apps; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_USR_VAR0";
    break;

  case SND_SEQ_EVENT_USR_VAR1 	: //reserved for user apps; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_USR_VAR1";
    break;

  case SND_SEQ_EVENT_USR_VAR2 	: //reserved for user apps; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_USR_VAR2";
    break;

  case SND_SEQ_EVENT_USR_VAR3 	: //reserved for user apps; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_USR_VAR3";
    break;

  case SND_SEQ_EVENT_USR_VAR4 	: //reserved for user apps; event data type = snd_seq_ev_ext_t
    stream << "SND_SEQ_EVENT_USR_VAR4";
    break;

  case SND_SEQ_EVENT_NONE 	: //NOP; ignored in any case
    stream << "SND_SEQ_EVENT_NONE";
    break;
  }

  stream << std::endl;
  return stream;
}

std::ostream& operator<< (std::ostream& stream, snd_seq_ev_ctrl_t &ev)
{
  stream << "(ch:"     << (int)ev.channel;
  stream << ",param:"  << (int)ev.param;
  stream << ",value: " << (int)ev.value;
  stream << ") ";
  return stream;
}

std::ostream& operator<< (std::ostream& stream, snd_seq_ev_note_t &ev)
{
  stream << "(ch:"           << (int)ev.channel;
  stream << ",note:"         << (int)ev.note;
  stream << ",velocity:"     << (int)ev.velocity;
  stream << ",off_velocity:" << (int)ev.off_velocity;
  stream << ",duration:"     << (int)ev.duration;
  stream << ") ";
  return stream;
}



alsaprint::alsaprint()
{
  for(int i=0; i<256; i++) {
    EVENT_TO_STR[i] = "noval";
  }

  EVENT_TO_STR[SND_SEQ_EVENT_SYSTEM] = "SND_SEQ_EVENT_SYSTEM";
  EVENT_TO_STR[SND_SEQ_EVENT_RESULT] = "SND_SEQ_EVENT_RESULT";
  EVENT_TO_STR[SND_SEQ_EVENT_NOTE] = "SND_SEQ_EVENT_NOTE";
  EVENT_TO_STR[SND_SEQ_EVENT_NOTEON] = "SND_SEQ_EVENT_NOTEON";
  EVENT_TO_STR[SND_SEQ_EVENT_NOTEOFF] = "SND_SEQ_EVENT_NOTEOFF";
  EVENT_TO_STR[SND_SEQ_EVENT_KEYPRESS] = "SND_SEQ_EVENT_KEYPRESS";
  EVENT_TO_STR[SND_SEQ_EVENT_CONTROLLER] = "SND_SEQ_EVENT_CONTROLLER";
  EVENT_TO_STR[SND_SEQ_EVENT_PGMCHANGE] = "SND_SEQ_EVENT_PGMCHANGE";
  EVENT_TO_STR[SND_SEQ_EVENT_CHANPRESS] = "SND_SEQ_EVENT_CHANPRESS";
  EVENT_TO_STR[SND_SEQ_EVENT_PITCHBEND] = "SND_SEQ_EVENT_PITCHBEND";
  EVENT_TO_STR[SND_SEQ_EVENT_CONTROL14] = "SND_SEQ_EVENT_CONTROL14";
  EVENT_TO_STR[SND_SEQ_EVENT_NONREGPARAM] = "SND_SEQ_EVENT_NONREGPARAM";
  EVENT_TO_STR[SND_SEQ_EVENT_REGPARAM] = "SND_SEQ_EVENT_REGPARAM";
  EVENT_TO_STR[SND_SEQ_EVENT_SONGPOS] = "SND_SEQ_EVENT_SONGPOS";
  EVENT_TO_STR[SND_SEQ_EVENT_SONGSEL] = "SND_SEQ_EVENT_SONGSEL";
  EVENT_TO_STR[SND_SEQ_EVENT_QFRAME] = "SND_SEQ_EVENT_QFRAME";
  EVENT_TO_STR[SND_SEQ_EVENT_TIMESIGN] = "SND_SEQ_EVENT_TIMESIGN";
  EVENT_TO_STR[SND_SEQ_EVENT_KEYSIGN] = "SND_SEQ_EVENT_KEYSIGN";
  EVENT_TO_STR[SND_SEQ_EVENT_START] = "SND_SEQ_EVENT_START";
  EVENT_TO_STR[SND_SEQ_EVENT_CONTINUE] = "SND_SEQ_EVENT_CONTINUE";
  EVENT_TO_STR[SND_SEQ_EVENT_STOP] = "SND_SEQ_EVENT_STOP";
  EVENT_TO_STR[SND_SEQ_EVENT_SETPOS_TICK] = "SND_SEQ_EVENT_SETPOS_TICK";
  EVENT_TO_STR[SND_SEQ_EVENT_SETPOS_TIME] = "SND_SEQ_EVENT_SETPOS_TIME";
  EVENT_TO_STR[SND_SEQ_EVENT_TEMPO] = "SND_SEQ_EVENT_TEMPO";
  EVENT_TO_STR[SND_SEQ_EVENT_CLOCK] = "SND_SEQ_EVENT_CLOCK";
  EVENT_TO_STR[SND_SEQ_EVENT_TICK] = "SND_SEQ_EVENT_TICK";
  EVENT_TO_STR[SND_SEQ_EVENT_QUEUE_SKEW] = "SND_SEQ_EVENT_QUEUE_SKEW";
  EVENT_TO_STR[SND_SEQ_EVENT_SYNC_POS] = "SND_SEQ_EVENT_SYNC_POS";
  EVENT_TO_STR[SND_SEQ_EVENT_TUNE_REQUEST] = "SND_SEQ_EVENT_TUNE_REQUEST";
  EVENT_TO_STR[SND_SEQ_EVENT_RESET] = "SND_SEQ_EVENT_RESET";
  EVENT_TO_STR[SND_SEQ_EVENT_SENSING] = "SND_SEQ_EVENT_SENSING";
  EVENT_TO_STR[SND_SEQ_EVENT_ECHO] = "SND_SEQ_EVENT_ECHO";
  EVENT_TO_STR[SND_SEQ_EVENT_OSS] = "SND_SEQ_EVENT_OSS";
  EVENT_TO_STR[SND_SEQ_EVENT_CLIENT_START] = "SND_SEQ_EVENT_CLIENT_START";
  EVENT_TO_STR[SND_SEQ_EVENT_CLIENT_EXIT] = "SND_SEQ_EVENT_CLIENT_EXIT";
  EVENT_TO_STR[SND_SEQ_EVENT_CLIENT_CHANGE] = "SND_SEQ_EVENT_CLIENT_CHANGE";
  EVENT_TO_STR[SND_SEQ_EVENT_PORT_START] = "SND_SEQ_EVENT_PORT_START";
  EVENT_TO_STR[SND_SEQ_EVENT_PORT_EXIT] = "SND_SEQ_EVENT_PORT_EXIT";
  EVENT_TO_STR[SND_SEQ_EVENT_PORT_CHANGE] = "SND_SEQ_EVENT_PORT_CHANGE";
  EVENT_TO_STR[SND_SEQ_EVENT_PORT_SUBSCRIBED] = "SND_SEQ_EVENT_PORT_SUBSCRIBED";
  EVENT_TO_STR[SND_SEQ_EVENT_PORT_UNSUBSCRIBED] = "SND_SEQ_EVENT_PORT_UNSUBSCRIBED";
  EVENT_TO_STR[SND_SEQ_EVENT_USR0] = "SND_SEQ_EVENT_USR0";
  EVENT_TO_STR[SND_SEQ_EVENT_USR1] = "SND_SEQ_EVENT_USR1";
  EVENT_TO_STR[SND_SEQ_EVENT_USR2] = "SND_SEQ_EVENT_USR2";
  EVENT_TO_STR[SND_SEQ_EVENT_USR3] = "SND_SEQ_EVENT_USR3";
  EVENT_TO_STR[SND_SEQ_EVENT_USR4] = "SND_SEQ_EVENT_USR4";
  EVENT_TO_STR[SND_SEQ_EVENT_USR5] = "SND_SEQ_EVENT_USR5";
  EVENT_TO_STR[SND_SEQ_EVENT_USR6] = "SND_SEQ_EVENT_USR6";
  EVENT_TO_STR[SND_SEQ_EVENT_USR7] = "SND_SEQ_EVENT_USR7";
  EVENT_TO_STR[SND_SEQ_EVENT_USR8] = "SND_SEQ_EVENT_USR8";
  EVENT_TO_STR[SND_SEQ_EVENT_USR9] = "SND_SEQ_EVENT_USR9";
  EVENT_TO_STR[SND_SEQ_EVENT_SYSEX] = "SND_SEQ_EVENT_SYSEX";
  EVENT_TO_STR[SND_SEQ_EVENT_BOUNCE] = "SND_SEQ_EVENT_BOUNCE";
  EVENT_TO_STR[SND_SEQ_EVENT_USR_VAR0] = "SND_SEQ_EVENT_USR_VAR0";
  EVENT_TO_STR[SND_SEQ_EVENT_USR_VAR1] = "SND_SEQ_EVENT_USR_VAR1";
  EVENT_TO_STR[SND_SEQ_EVENT_USR_VAR2] = "SND_SEQ_EVENT_USR_VAR2";
  EVENT_TO_STR[SND_SEQ_EVENT_USR_VAR3] = "SND_SEQ_EVENT_USR_VAR3";
  EVENT_TO_STR[SND_SEQ_EVENT_USR_VAR4] = "SND_SEQ_EVENT_USR_VAR4";
  EVENT_TO_STR[SND_SEQ_EVENT_NONE] = "SND_SEQ_EVENT_NONE";


}
