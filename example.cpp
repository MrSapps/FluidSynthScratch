#include <fluidsynth.h>
#include <fluidsynth/ramsfont.h>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>

static void AddSoundFont(fluid_synth_t* synth)
{
    std::string soundFontName = "Oddworld.sf2";
    if (fluid_is_soundfont(soundFontName.c_str()))
    {
        fluid_synth_sfload(synth, soundFontName.c_str(), 1);
    }
    else
    {
        abort();
    }
}

static void AddRamSoundFont(fluid_synth_t* synth)
{
    fluid_sfont_t* sfont = fluid_ramsfont_create_sfont();
    fluid_ramsfont_t* rFont = reinterpret_cast<fluid_ramsfont_t*>(sfont->data);
    fluid_ramsfont_set_name(rFont, "TestRamFont");

    fluid_sample_t* sample = new_fluid_ramsample();
    fluid_sample_set_name(sample, "TestRamSample");

    std::ifstream rawFile("Tambourine.raw", std::ios::binary);
    if (!rawFile.is_open())
    {
        abort();
    }
    std::vector<unsigned char> buffer(31024); // hard coded to the file size in bytes
    rawFile.read((char*)buffer.data(), buffer.size());

    // How do I calculate number of audio frames?
    int numFrames = 2;
    fluid_sample_set_sound_data(sample, (short int*)buffer.data(), numFrames, 1, 60);

    // The example midi only plays stuff using programs 17 and 26
    // the sample should play for any key in the whole midi keyboard range
    fluid_ramsfont_add_izone(rFont, 0, 17, sample, 0, 127);
    fluid_ramsfont_add_izone(rFont, 0, 26, sample, 0, 127);
}

int main(int argc, char** argv)
{
    fluid_settings_t* settings = new_fluid_settings();

    // audio.driver alsa, oss, pulseaudio
    fluid_settings_setstr(settings, "audio.driver", "alsa");
    //fluid_settings_setnum(settings, "synth.polyphony", 64);
    //fluid_settings_setnum(settings, "audio.period-size", 1649);


    fluid_synth_t* synth = new_fluid_synth(settings);
    fluid_player_t* player = new_fluid_player(synth);
    fluid_audio_driver_t* adriver = new_fluid_audio_driver(settings, synth);

    // Plays expected sound
    //AddSoundFont(synth);

    // Should play all notes with the same ram sample, instead nothing - however this appears in stdout:
    /*
    fluidsynth: warning: No preset found on channel 6 [bank=0 prog=17]
    fluidsynth: warning: No preset found on channel 7 [bank=0 prog=26]
    fluidsynth: warning: No preset found on channel 6 [bank=0 prog=17]
    fluidsynth: warning: No preset found on channel 7 [bank=0 prog=26]
     */
    AddRamSoundFont(synth);

    std::string midiFileName = "MISEQ24_f.mid";
    if (fluid_is_midifile(midiFileName.c_str()))
    {
        fluid_player_add(player,midiFileName.c_str());
    }
    else
    {
        abort();
    }

    /* play the midi files, if any */
    fluid_player_play(player);

    /* wait for playback termination */
    fluid_player_join(player);

    /* cleanup */
    delete_fluid_audio_driver(adriver);
    delete_fluid_player(player);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
    return 0;
}
