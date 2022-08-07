#ifndef BUZZER_PLAYER_H
#define BUZZER_PLAYER_H

#include <Arduino.h>
#include <Note.h>
#include <Globals.h>
#include <Queue.h>

class BuzzerPlayer{
  public:
    BuzzerPlayer();
    Queue<Note> notes = Queue<Note>(16);
    Note current_note;
    long current_note_start;
    bool add_note(Note note);
    bool add_instant_note(Note note);
    void play_tone(int frequency, int duration, bool wait_for_completion=false);
    void play_tone(Note note);
    void run();
    void run_all();
  private:
    bool initialized;
};

#endif