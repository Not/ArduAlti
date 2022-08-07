

#include <Arduino.h>
#include <Note.h>
#include <Globals.h>
#include <Queue.h>
#include <BuzzerPlayer.h>

    BuzzerPlayer::BuzzerPlayer(){}
    bool BuzzerPlayer::add_note(Note note){
      if (notes.count()<16){
        notes.push(note);
        return true;
      }
      return false;
    }
    bool BuzzerPlayer::add_instant_note(Note note){
      notes.clear();
      return add_note(note);
    }
    
    void BuzzerPlayer::play_tone(int frequency, int duration, bool wait_for_completion){
      tone(BUZZER_PIN, frequency, duration );
      if (wait_for_completion){
        delay(duration);
      }
    }

    void BuzzerPlayer::play_tone(Note note){
      tone(BUZZER_PIN, note.frequency, note.duration);
    }
    void BuzzerPlayer::run(){
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
    void BuzzerPlayer::run_all(){
      while(notes.count()>0){
        run();
      }
    }