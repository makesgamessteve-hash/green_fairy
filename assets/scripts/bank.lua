-- Edit or add a patch table, then press R in the desktop app or refresh the web page.
-- Use a WAV filename under assets/sounds to layer a sample tuned to C4.
return {
    {
        name = "Glass Canopy", wave_a = "triangle", wave_b = "sine",
        attack = .085, decay = .55, sustain = .65, release = 1.4,
        morph = .36, color = .64, echo = .43, detune = 9, fm = .45,
        lfo = .27, motion = .19, sample = .26, sample_file = "canopy.wav"
    },
    {
        name = "Afterimage", wave_a = "sine", wave_b = "saw",
        attack = .36, decay = .91, sustain = .67, release = 2.5,
        morph = .27, color = .49, echo = .67, detune = 13, fm = 1.5,
        lfo = .12, motion = .26
    },
    {
        name = "Copper Moth", wave_a = "square", wave_b = "triangle",
        attack = .007, decay = .18, sustain = .28, release = .27,
        morph = .44, color = .54, echo = .22, detune = -15, fm = .3,
        lfo = 1.7, motion = .28
    },
    {
        name = "Night Orchard", wave_a = "sine", wave_b = "noise",
        attack = .48, decay = .85, sustain = .75, release = 3,
        morph = .19, color = .34, echo = .62, detune = -7, fm = .8,
        lfo = .21, motion = .15
    },
    {
        name = "Vesper Bass", wave_a = "saw", wave_b = "sine",
        attack = .006, decay = .24, sustain = .65, release = .22,
        morph = .54, color = .43, echo = .13, detune = -1200, fm = .18,
        lfo = .8, motion = .1
    }
}
