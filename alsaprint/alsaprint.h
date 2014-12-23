
#ifndef ALSAPRINT_HH
#define ALSAPRINT_HH
#include <ostream>
#include <alsa/asoundlib.h>

std::ostream& operator<< (std::ostream& stream, snd_seq_event_t &ev);
std::ostream& operator<< (std::ostream& stream, snd_seq_ev_ctrl_t &ev);
std::ostream& operator<< (std::ostream& stream, snd_seq_ev_note_t &ev);

class alsaprint {
 public:
  alsaprint();

  static alsaprint single;

  static const char* EVENT_TO_STR[255];
};

#endif
