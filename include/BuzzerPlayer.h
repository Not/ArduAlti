#ifndef BUZZER_PLAYER_H
#define BUZZER_PLAYER_H

#include <Arduino.h>
#include <Note.h>
#include <Globals.h>

class BuzzerPlayer{
  public:
    BuzzerPlayer(){}
    Queue<Note> notes = Queue<Note>(16);
    Note current_note;
    long current_note_start;
    bool add_note(Note note){
      if (notes.count()<16){
        notes.push(note);
        return true;
      }
      return false;
    }
    bool add_instant_note(Note note){
      notes.clear();
      return add_note(note);
    }
    
    void play_tone(int frequency, int duration, bool wait_for_completion=false){
      tone(BUZZER_PIN, frequency, duration );
      if (wait_for_completion){
        delay(duration);
      }
    }

    void play_tone(Note note){
      tone(BUZZER_PIN, note.frequency, note.duration);
    }
    void run(){
      unsigned long current_time = millis();
      if (notes.count()>0){

        if (!initialized){
          initialized = true;
          current_note = notes.pop();
          current_note_start = current_time;
          play_tone(current_note);
          DEBUG("playing note "); current_note.print(&Serial); DEBUG(" at "); DEBUGln(millis());
          return;
        }
        if (current_time>current_note_start + current_note.duration + current_note.pause_after){
          current_note = notes.pop();
          current_note_start = current_time;
          play_tone(current_note);
          DEBUG("playing note "); current_note.print(&Serial); DEBUG(" at "); DEBUGln(millis());
        }

      }
    }
    void run_all(){
      while(notes.count()>0){
        run();
      }
    }
  private:
    bool initialized;
};

#endif