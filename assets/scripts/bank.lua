-- Edit or add a patch table, then press F5 in the desktop app or refresh the web page.
-- Use a WAV filename under assets/sounds to layer a sample tuned to C4.
local patches = {
    {
        id = "glass-canopy", type = "pads",
        name = "Glass Canopy", wave_a = "triangle", wave_b = "sine",
        attack = .085, decay = .55, sustain = .65, release = 1.4,
        morph = .36, color = .64, echo = .43, detune = 9, fm = .45,
        lfo = .27, motion = .19, sample = .26, sample_file = "canopy.wav",
        filter = {resonance = 1.25, depth = 1.4, velocity = 1.4, keytrack = .35,
                  attack = .14, decay = 1.08, sustain = .21, release = 1.2},
        shape = {loop = true, {time = .85, level = .68}, {time = 1.40, level = -.43},
                 {time = 1.10, level = .12}},
        mod = {{source = "shape", destination = "morph", amount = .21},
               {source = "lfo", destination = "pitch", amount = .09}},
        scene = {mesh = "prism.obj", tint = 0x8eb861, accent = 0xcce77e, spin = 0.48, tilt = 0.17, aspect = 1, satellites = 3}
    },
    {
        id = "afterimage", type = "pads",
        name = "Afterimage", wave_a = "sine", wave_b = "saw",
        attack = .36, decay = .91, sustain = .67, release = 2.5,
        morph = .27, color = .49, echo = .67, detune = 13, fm = 1.5,
        lfo = .12, motion = .26,
        filter = {resonance = .9, depth = 1.6, velocity = .48, keytrack = .25,
                  attack = .92, decay = 1.2, sustain = .30, release = 1.4},
        mod = {{source = "lfo", destination = "cutoff", amount = .48},
               {source = "amp_env", destination = "fm", amount = -.48}},
        scene = {mesh = "helix.obj", tint = 0x9a78e5, accent = 0x63b9ff, spin = -0.29, tilt = 0.1, aspect = 1.1, satellites = 2}
    },
    {
        id = "copper-moth", type = "plucks",
        name = "Copper Moth", wave_a = "square", wave_b = "triangle",
        attack = .007, decay = .18, sustain = .28, release = .27,
        morph = .44, color = .54, echo = .22, detune = -15, fm = .3,
        lfo = 1.7, motion = .28,
        filter = {mode = "bandpass", resonance = 1.05, depth = 1.75, velocity = .85,
                  attack = .009, decay = .17, sustain = .12, release = .23},
        mod = {{source = "velocity", destination = "fm", amount = .52}},
        scene = {mesh = "fan.obj", tint = 0xe48a52, accent = 0xf9ca83, spin = 0.6, tilt = -0.14, aspect = 1.18, satellites = 3}
    },
    {
        id = "night-orchard", type = "pads",
        name = "Night Orchard", wave_a = "sine", wave_b = "noise",
        attack = .48, decay = .85, sustain = .75, release = 3,
        morph = .19, color = .34, echo = .62, detune = -7, fm = .8,
        lfo = .21, motion = .15,
        sample = .43, sample_file = "orchard-air.wav",
        grain = {size = .16, density = 31, position = .47, spray = .19},
        filter = {resonance = 1.02, depth = .8, keytrack = .22,
                  attack = .42, decay = 1.4, sustain = .44, release = 1.8},
        shape = {loop = true, {time = 1.45, level = .88}, {time = 1.25, level = -.84}},
        mod = {{source = "shape", destination = "grain_position", amount = .31},
               {source = "lfo", destination = "cutoff", amount = .38}},
        scene = {mesh = "arch.obj", tint = 0x725cba, accent = 0xb2e590, spin = 0.4, tilt = 0.49, aspect = 1.15, satellites = 1}
    },
    {
        id = "vesper-bass", type = "bass",
        name = "Vesper Bass", wave_a = "saw", wave_b = "sine",
        attack = .006, decay = .24, sustain = .65, release = .22,
        morph = .54, color = .32, echo = .13, detune = -1200, fm = .18,
        lfo = .8, motion = .1,
        filter = {resonance = 2.85, depth = 2.15, velocity = 1.72, keytrack = .46,
                  attack = .004, decay = .255, sustain = .05, release = .14},
        mod = {{source = "filter_env", destination = "morph", amount = -.21}},
        scene = {mesh = "anvil.obj", tint = 0xb74b73, accent = 0xf99075, spin = 0.66, tilt = -0.05, aspect = 1.04, satellites = 1}
    },
    -- Evolving pads
    {
        id = "aerial-fern", type = "pads",
        name = "Aerial Fern", wave_a = "triangle", wave_b = "saw",
        attack = .44, decay = 1.28, sustain = .74, release = 2.7,
        morph = .27, color = .68, echo = .44, detune = 13, fm = .28,
        lfo = .19, motion = .23,
        filter = {resonance = 1.29, depth = 1.14, velocity = .65, keytrack = .46,
                  attack = .61, decay = 1.36, sustain = .30, release = 1.84},
        shape = {loop = true, {time = 1.25, level = .52}, {time = 1.5, level = -.58},
                 {time = 1.05, level = .12}},
        mod = {{source = "shape", destination = "cutoff", amount = .46},
               {source = "lfo", destination = "pitch", amount = .08}},
        scene = {mesh = "petal.obj", tint = 0x85cf85, accent = 0xddf398, spin = -0.3, tilt = -0.53, aspect = 0.85, satellites = 5}
    },
    {
        id = "lumen-drift", type = "pads",
        name = "Lumen Drift", wave_a = "sine", wave_b = "triangle",
        attack = 1.18, decay = 2.30, sustain = .88, release = 3.6,
        morph = .47, color = .51, echo = .55, detune = -21, fm = .62,
        lfo = .08, motion = .20,
        filter = {resonance = .91, depth = .67, velocity = .42, keytrack = .33,
                  attack = 1.56, decay = 2.20, sustain = .47, release = 2.90},
        shape = {loop = true, {time = 2.2, level = .75}, {time = 3.1, level = -.55}},
        sample = .28, sample_file = "orchard-air.wav",
        grain = {size = .19, density = 26, position = .45, spray = .12},
        mod = {{source = "shape", destination = "grain_position", amount = .27},
               {source = "lfo", destination = "fm", amount = .26}},
        scene = {mesh = "prism.obj", tint = 0x66baed, accent = 0xf3de99, spin = 0.31, tilt = -0.25, aspect = 1.06, satellites = 4}
    },
    {
        id = "marsh-lanterns", type = "pads",
        name = "Marsh Lanterns", wave_a = "sine", wave_b = "square",
        attack = .78, decay = 1.60, sustain = .65, release = 2.6,
        morph = .21, color = .39, echo = .63, detune = 8, fm = .36,
        lfo = .22, motion = .19,
        filter = {resonance = 1.46, depth = 1.57, velocity = .78, keytrack = .45,
                  attack = .96, decay = 1.63, sustain = .34, release = 2.20},
        shape = {loop = true, {time = .86, level = .45}, {time = 1.38, level = -.68},
                 {time = 1.15, level = .16}},
        mod = {{source = "shape", destination = "morph", amount = .18},
               {source = "lfo", destination = "cutoff", amount = .32}},
        scene = {mesh = "relay.obj", tint = 0xa3b759, accent = 0xffd17f, spin = 0.41, tilt = 0.3, aspect = 0.92, satellites = 2}
    },
    {
        id = "silver-murmur", type = "pads",
        name = "Silver Murmur", wave_a = "triangle", wave_b = "noise",
        attack = .83, decay = 1.85, sustain = .75, release = 2.2,
        morph = .17, color = .22, echo = .35, detune = 4, fm = .82,
        lfo = .34, motion = .20,
        filter = {mode = "bandpass", resonance = .63, depth = .69, velocity = .25, keytrack = .88,
                  attack = 1.10, decay = 1.80, sustain = .39, release = 1.84},
        shape = {loop = true, {time = 1.6, level = -.62}, {time = 2.1, level = .76}},
        sample = .35, sample_file = "orchard-air.wav",
        grain = {size = .14, density = 33, position = .55, spray = .20},
        mod = {{source = "shape", destination = "grain_position", amount = .24},
               {source = "lfo", destination = "morph", amount = .12}},
        scene = {mesh = "fan.obj", tint = 0xaebdc8, accent = 0xe9c5f4, spin = 0.32, tilt = -0.34, aspect = 0.88, satellites = 5}
    },
    {
        id = "frost-arch", type = "pads",
        name = "Frost Arch", wave_a = "saw", wave_b = "sine",
        attack = 1.42, decay = 1.67, sustain = .78, release = 4.3,
        morph = .62, color = .45, echo = .49, detune = 17, fm = .93,
        lfo = .10, motion = .16,
        filter = {mode = "highpass", resonance = 1.23, depth = -.93, keytrack = .32,
                  attack = 1.04, decay = 1.46, sustain = .68, release = 2.1},
        mod = {{source = "lfo", destination = "cutoff", amount = .69}},
        scene = {mesh = "arch.obj", tint = 0x679ac7, accent = 0xdaf3ff, spin = -0.41, tilt = -0.5, aspect = 0.96, satellites = 1}
    },
    {
        id = "auburn-tide", type = "pads",
        name = "Auburn Tide", wave_a = "triangle", wave_b = "square",
        attack = .62, decay = 2.10, sustain = .70, release = 2.8,
        morph = .33, color = .59, echo = .41, detune = -16, fm = .17,
        lfo = .28, motion = .26,
        filter = {resonance = 1.12, depth = 1.03, velocity = .45, keytrack = .42,
                  attack = .89, decay = 2.0, sustain = .34, release = 2.10},
        shape = {loop = true, {time = 1.46, level = .65}, {time = 1.30, level = -.57},
                 {time = .9, level = .16}},
        mod = {{source = "shape", destination = "cutoff", amount = .52},
               {source = "velocity", destination = "fm", amount = .26}},
        scene = {mesh = "helix.obj", tint = 0xc8756e, accent = 0xffd37d, spin = 0.38, tilt = -0.4, aspect = 1.11, satellites = 4}
    },
    {
        id = "distant-arches", type = "pads",
        name = "Distant Arches", wave_a = "sine", wave_b = "saw",
        attack = 1.51, decay = 2.50, sustain = .57, release = 4.8,
        morph = .31, color = .37, echo = .77, detune = -11, fm = 1.15,
        lfo = .14, motion = .31,
        filter = {resonance = 1.07, depth = .92, velocity = .38, keytrack = .52,
                  attack = 1.86, decay = 2.35, sustain = .54, release = 3.35},
        shape = {loop = true, {time = 2.4, level = .84}, {time = 3.0, level = -.65}},
        sample = .32, sample_file = "filament.wav",
        grain = {size = .21, density = 25, position = .48, spray = .17},
        mod = {{source = "shape", destination = "grain_position", amount = .34},
               {source = "lfo", destination = "cutoff", amount = .29}},
        scene = {mesh = "relay.obj", tint = 0x8974c0, accent = 0xa9c5f4, spin = 0.5, tilt = 0.1, aspect = 0.96, satellites = 3}
    },
    {
        id = "lichen-bloom", type = "pads",
        name = "Lichen Bloom", wave_a = "sine", wave_b = "triangle",
        attack = .31, decay = 1.70, sustain = .80, release = 2.5,
        morph = .66, color = .61, echo = .42, detune = 27, fm = .71,
        lfo = .46, motion = .29,
        filter = {resonance = 1.18, depth = 1.52, velocity = .69, keytrack = .43,
                  attack = .67, decay = 1.27, sustain = .38, release = 2.12},
        shape = {loop = true, {time = .74, level = .63}, {time = 1.18, level = -.55},
                 {time = .92, level = .05}},
        mod = {{source = "shape", destination = "fm", amount = .42},
               {source = "velocity", destination = "morph", amount = -.11}},
        scene = {mesh = "petal.obj", tint = 0x80b45d, accent = 0xf1bfe3, spin = 0.33, tilt = -0.24, aspect = 0.97, satellites = 4}
    },
    {
        id = "polar-terrace", type = "pads",
        name = "Polar Terrace", wave_a = "square", wave_b = "sine",
        attack = .91, decay = 2.40, sustain = .70, release = 3.9,
        morph = .68, color = .33, echo = .59, detune = -1200, fm = .29,
        lfo = .12, motion = .18,
        filter = {resonance = 1.54, depth = 1.22, velocity = .52, keytrack = .51,
                  attack = 1.13, decay = 1.75, sustain = .26, release = 2.68},
        shape = {loop = true, {time = 1.7, level = .51}, {time = 2.3, level = -.70}},
        mod = {{source = "lfo", destination = "cutoff", amount = .58},
               {source = "shape", destination = "morph", amount = .16}},
        scene = {mesh = "anvil.obj", tint = 0x628da8, accent = 0xbee9f0, spin = 0.42, tilt = 0.24, aspect = 0.98, satellites = 4}
    },
    {
        id = "opaline-wake", type = "pads",
        name = "Opaline Wake", wave_a = "triangle", wave_b = "sine",
        attack = .56, decay = 1.33, sustain = .85, release = 2.1,
        morph = .43, color = .70, echo = .52, detune = -7, fm = 1.55,
        lfo = .24, motion = .34,
        filter = {resonance = .92, depth = .78, velocity = .64, keytrack = .37,
                  attack = .78, decay = 1.22, sustain = .31, release = 1.71},
        shape = {loop = true, {time = 1.68, level = .64}, {time = 2.0, level = -.59}},
        sample = .26, sample_file = "canopy.wav",
        grain = {size = .105, density = 31, position = .38, spray = .16},
        mod = {{source = "shape", destination = "grain_position", amount = .26},
               {source = "velocity", destination = "sample", amount = .14}},
        scene = {mesh = "prism.obj", tint = 0x89d4c9, accent = 0xf5b4e2, spin = 0.63, tilt = -0.11, aspect = 0.85, satellites = 2}
    },
    {
        id = "ember-veil", type = "pads",
        name = "Ember Veil", wave_a = "saw", wave_b = "triangle",
        attack = .68, decay = 1.54, sustain = .59, release = 3.0,
        morph = .53, color = .52, echo = .33, detune = 19, fm = .44,
        lfo = .37, motion = .16,
        filter = {resonance = 1.58, depth = 1.15, velocity = .71, keytrack = .58,
                  attack = .88, decay = 1.46, sustain = .28, release = 2.07},
        shape = {loop = true, {time = 1.1, level = -.42}, {time = 1.5, level = .71},
                 {time = .88, level = .02}},
        mod = {{source = "shape", destination = "cutoff", amount = .54},
               {source = "lfo", destination = "fm", amount = .22}},
        scene = {mesh = "spire.obj", tint = 0xde7953, accent = 0xfccf86, spin = 0.54, tilt = -0.47, aspect = 1.11, satellites = 4}
    },
    {
        id = "moon-reservoir", type = "pads",
        name = "Moon Reservoir", wave_a = "sine", wave_b = "noise",
        attack = 1.68, decay = 2.00, sustain = .76, release = 5.2,
        morph = .09, color = .63, echo = .71, detune = 1200, fm = .19,
        lfo = .06, motion = .08,
        filter = {resonance = .84, depth = .54, velocity = .34, keytrack = .30,
                  attack = 2.10, decay = 2.37, sustain = .58, release = 3.98},
        shape = {loop = true, {time = 2.8, level = .76}, {time = 3.6, level = -.75}},
        sample = .44, sample_file = "orchard-air.wav",
        grain = {size = .23, density = 23, position = .52, spray = .18},
        mod = {{source = "shape", destination = "grain_position", amount = .36},
               {source = "lfo", destination = "sample", amount = .11}},
        scene = {mesh = "arch.obj", tint = 0x546f9e, accent = 0xd0b3f0, spin = -0.59, tilt = 0.32, aspect = 1.04, satellites = 5}
    },

    -- Strings and voices
    {
        id = "amber-ensemble", type = "strings",
        name = "Amber Ensemble", wave_a = "saw", wave_b = "saw",
        attack = .28, decay = 1.20, sustain = .84, release = 1.60,
        morph = .46, color = .58, echo = .28, detune = 12, fm = .08,
        lfo = .66, motion = .21,
        filter = {resonance = 1.02, depth = .89, velocity = .67, keytrack = .71,
                  attack = .33, decay = .78, sustain = .43, release = 1.12},
        mod = {{source = "lfo", destination = "pitch", amount = .16},
               {source = "amp_env", destination = "morph", amount = .16}},
        scene = {mesh = "fan.obj", tint = 0xce9a57, accent = 0xf5d99b, spin = 0.45, tilt = -0.51, aspect = 1.05, satellites = 4}
    },
    {
        id = "birch-quartet", type = "strings",
        name = "Birch Quartet", wave_a = "triangle", wave_b = "saw",
        attack = .19, decay = .93, sustain = .79, release = 1.1,
        morph = .55, color = .64, echo = .20, detune = -9, fm = .34,
        lfo = .87, motion = .13,
        filter = {resonance = .92, depth = .81, velocity = .76, keytrack = .78,
                  attack = .24, decay = .70, sustain = .37, release = .87},
        mod = {{source = "lfo", destination = "pitch", amount = .12},
               {source = "velocity", destination = "morph", amount = .17}},
        scene = {mesh = "kite.obj", tint = 0xaeb384, accent = 0xe0e3b1, spin = 0.48, tilt = 0.32, aspect = 0.95, satellites = 1}
    },
    {
        id = "hollow-choir", type = "choir",
        name = "Hollow Choir", wave_a = "sine", wave_b = "square",
        attack = .47, decay = 1.71, sustain = .77, release = 2.7,
        morph = .21, color = .24, echo = .52, detune = 7, fm = 1.13,
        lfo = .39, motion = .11,
        filter = {mode = "bandpass", resonance = .76, depth = .68, velocity = .46, keytrack = .79,
                  attack = .64, decay = 1.32, sustain = .44, release = 2.10},
        shape = {loop = true, {time = 1.63, level = .57}, {time = 1.96, level = -.62}},
        sample = .31, sample_file = "orchard-air.wav",
        grain = {size = .18, density = 30, position = .29, spray = .12},
        mod = {{source = "shape", destination = "grain_position", amount = .29},
               {source = "lfo", destination = "pitch", amount = .06}},
        scene = {mesh = "arch.obj", tint = 0xa08ac6, accent = 0xd1a2be, spin = 0.55, tilt = 0.23, aspect = 1.04, satellites = 5}
    },
    {
        id = "violet-octaves", type = "strings",
        name = "Violet Octaves", wave_a = "saw", wave_b = "triangle",
        attack = .39, decay = 1.13, sustain = .83, release = 1.65,
        morph = .59, color = .55, echo = .37, detune = 1200, fm = .10,
        lfo = .55, motion = .10,
        filter = {resonance = 1.13, depth = .64, velocity = .61, keytrack = .70,
                  attack = .48, decay = .97, sustain = .49, release = 1.32},
        mod = {{source = "lfo", destination = "pitch", amount = .14},
               {source = "filter_env", destination = "morph", amount = .18}},
        scene = {mesh = "helix.obj", tint = 0x8962ce, accent = 0xdbacfb, spin = 0.57, tilt = 0.01, aspect = 0.97, satellites = 3}
    },
    {
        id = "silk-bow", type = "strings",
        name = "Silk Bow", wave_a = "triangle", wave_b = "triangle",
        attack = .16, decay = .84, sustain = .81, release = 1.05,
        morph = .51, color = .72, echo = .22, detune = 7, fm = .05,
        lfo = 1.18, motion = .29,
        filter = {resonance = .84, depth = .75, velocity = .52, keytrack = .69,
                  attack = .20, decay = .57, sustain = .42, release = .78},
        mod = {{source = "lfo", destination = "pitch", amount = .21},
               {source = "velocity", destination = "cutoff", amount = .29}},
        scene = {mesh = "petal.obj", tint = 0xdf95ab, accent = 0xf7d7a0, spin = 0.56, tilt = 0.25, aspect = 1.09, satellites = 4}
    },
    {
        id = "grove-vox", type = "choir",
        name = "Grove Vox", wave_a = "sine", wave_b = "triangle",
        attack = .52, decay = 1.31, sustain = .73, release = 2.75,
        morph = .35, color = .25, echo = .43, detune = 18, fm = 2.14,
        lfo = .58, motion = .22,
        filter = {mode = "bandpass", resonance = .7, depth = .89, velocity = .45, keytrack = .84,
                  attack = .62, decay = 1.30, sustain = .36, release = 2.03},
        shape = {loop = true, {time = 1.28, level = .51}, {time = 1.6, level = -.57}},
        mod = {{source = "shape", destination = "cutoff", amount = .37},
               {source = "lfo", destination = "pitch", amount = .09}},
        scene = {mesh = "bell.obj", tint = 0x769b68, accent = 0xc8db9b, spin = -0.52, tilt = -0.02, aspect = 1.06, satellites = 4}
    },
    {
        id = "reed-chamber", type = "strings",
        name = "Reed Chamber", wave_a = "square", wave_b = "triangle",
        attack = .09, decay = .54, sustain = .75, release = .86,
        morph = .69, color = .17, echo = .19, detune = -4, fm = .41,
        lfo = 1.25, motion = .11,
        filter = {mode = "bandpass", resonance = .67, depth = .61, velocity = .52, keytrack = 1.01,
                  attack = .087, decay = .39, sustain = .36, release = .69},
        mod = {{source = "lfo", destination = "pitch", amount = .15},
               {source = "amp_env", destination = "fm", amount = .23}},
        scene = {mesh = "spire.obj", tint = 0xc3ab56, accent = 0xe1e9a5, spin = -0.55, tilt = -0.36, aspect = 0.85, satellites = 4}
    },
    {
        id = "hushed-altos", type = "choir",
        name = "Hushed Altos", wave_a = "triangle", wave_b = "noise",
        attack = .66, decay = 1.60, sustain = .76, release = 2.62,
        morph = .07, color = .64, echo = .30, detune = 0, fm = .36,
        lfo = .49, motion = .13,
        filter = {resonance = .79, depth = .74, velocity = .43, keytrack = .55,
                  attack = .86, decay = 1.24, sustain = .44, release = 1.85},
        shape = {loop = true, {time = 1.82, level = .58}, {time = 2.27, level = -.44}},
        sample = .25, sample_file = "orchard-air.wav",
        grain = {size = .17, density = 26, position = .55, spray = .11},
        mod = {{source = "shape", destination = "grain_position", amount = .21},
               {source = "lfo", destination = "morph", amount = .06}},
        scene = {mesh = "fan.obj", tint = 0x9c8ebd, accent = 0xe0bdd9, spin = -0.55, tilt = -0.52, aspect = 1.09, satellites = 4}
    },
    {
        id = "winter-violins", type = "strings",
        name = "Winter Violins", wave_a = "saw", wave_b = "square",
        attack = .38, decay = 1.47, sustain = .78, release = 1.64,
        morph = .23, color = .67, echo = .36, detune = 11, fm = .15,
        lfo = .79, motion = .29,
        filter = {resonance = 1.08, depth = 1.27, velocity = .59, keytrack = .82,
                  attack = .42, decay = 1.03, sustain = .37, release = 1.16},
        mod = {{source = "lfo", destination = "pitch", amount = .21},
               {source = "filter_env", destination = "morph", amount = -.16}},
        scene = {mesh = "kite.obj", tint = 0x79a8d4, accent = 0xf4e7c9, spin = 0.66, tilt = -0.37, aspect = 1.0, satellites = 3}
    },
    {
        id = "ghost-chorale", type = "choir",
        name = "Ghost Chorale", wave_a = "sine", wave_b = "sine",
        attack = .90, decay = 1.75, sustain = .82, release = 3.36,
        morph = .31, color = .30, echo = .64, detune = 1200, fm = 2.65,
        lfo = .19, motion = .32,
        filter = {mode = "bandpass", resonance = .77, depth = .67, velocity = .44, keytrack = .86,
                  attack = 1.16, decay = 1.51, sustain = .58, release = 2.78},
        shape = {loop = true, {time = 1.9, level = .61}, {time = 2.4, level = -.60}},
        sample = .37, sample_file = "orchard-air.wav",
        grain = {size = .19, density = 28, position = .43, spray = .16},
        mod = {{source = "shape", destination = "grain_position", amount = .28},
               {source = "lfo", destination = "fm", amount = .51}},
        scene = {mesh = "relay.obj", tint = 0x829ace, accent = 0xdfbce8, spin = 0.35, tilt = -0.17, aspect = 1.03, satellites = 4}
    },

    -- Keys and bells
    {
        id = "porcelain-keys", type = "keys",
        name = "Porcelain Keys", wave_a = "triangle", wave_b = "sine",
        attack = .007, decay = .65, sustain = .32, release = .64,
        morph = .59, color = .74, echo = .16, detune = 1200, fm = .64,
        lfo = .46, motion = .08, sample = .24, sample_file = "canopy.wav",
        filter = {resonance = .95, depth = 1.43, velocity = .94, keytrack = .68,
                  attack = .004, decay = .24, sustain = .11, release = .30},
        mod = {{source = "filter_env", destination = "fm", amount = .47},
               {source = "velocity", destination = "sample", amount = .14}},
        scene = {mesh = "bell.obj", tint = 0xc2d8d8, accent = 0xf6cea8, spin = 0.53, tilt = -0.34, aspect = 0.9, satellites = 5}
    },
    {
        id = "smoked-ep", type = "keys",
        name = "Smoked EP", wave_a = "sine", wave_b = "sine",
        attack = .005, decay = .91, sustain = .40, release = .53,
        morph = .17, color = .58, echo = .22, detune = 1200, fm = 1.33,
        lfo = .89, motion = .08,
        filter = {resonance = .87, depth = 1.23, velocity = 1.22, keytrack = .61,
                  attack = .005, decay = .30, sustain = .14, release = .36},
        mod = {{source = "filter_env", destination = "fm", amount = 1.12},
               {source = "velocity", destination = "fm", amount = .63}},
        scene = {mesh = "anvil.obj", tint = 0xa27a73, accent = 0xe0b68a, spin = 0.62, tilt = 0.06, aspect = 0.93, satellites = 2}
    },
    {
        id = "mirror-tines", type = "keys",
        name = "Mirror Tines", wave_a = "sine", wave_b = "triangle",
        attack = .003, decay = .56, sustain = .24, release = .86,
        morph = .23, color = .78, echo = .30, detune = 973, fm = 2.48,
        lfo = .16, motion = .07,
        filter = {resonance = 1.13, depth = 1.42, velocity = .89, keytrack = .79,
                  attack = .003, decay = .22, sustain = .07, release = .40},
        mod = {{source = "filter_env", destination = "fm", amount = 1.24},
               {source = "velocity", destination = "morph", amount = -.13}},
        scene = {mesh = "spire.obj", tint = 0xa3bbdc, accent = 0xe1edff, spin = -0.48, tilt = -0.53, aspect = 0.88, satellites = 3}
    },
    {
        id = "brass-celesta", type = "keys",
        name = "Brass Celesta", wave_a = "sine", wave_b = "square",
        attack = .004, decay = 1.06, sustain = .08, release = 1.40,
        morph = .10, color = .73, echo = .41, detune = 1200, fm = 1.70,
        lfo = .35, motion = .11, sample = .31, sample_file = "canopy.wav",
        filter = {resonance = 1.38, depth = 1.37, velocity = .74, keytrack = .83,
                  attack = .004, decay = .34, sustain = .08, release = .51},
        mod = {{source = "filter_env", destination = "fm", amount = .82},
               {source = "velocity", destination = "sample", amount = .16}},
        scene = {mesh = "bell.obj", tint = 0xd3a14c, accent = 0xffdba1, spin = 0.52, tilt = -0.5, aspect = 0.88, satellites = 1}
    },
    {
        id = "orbit-bells", type = "keys",
        name = "Orbit Bells", wave_a = "sine", wave_b = "sine",
        attack = .002, decay = 1.38, sustain = .04, release = 2.30,
        morph = .13, color = .82, echo = .52, detune = 777, fm = 3.45,
        lfo = .13, motion = .05,
        sample = .49, sample_file = "canopy.wav",
        grain = {size = .075, density = 35, position = .19, spray = .15},
        mod = {{source = "lfo", destination = "grain_position", amount = .12},
               {source = "amp_env", destination = "fm", amount = .86}},
        filter = {resonance = 1.18, depth = .72, velocity = .44, keytrack = .76,
                  attack = .018, decay = .74, sustain = .23, release = 1.05},
        scene = {mesh = "relay.obj", tint = 0x7a92dd, accent = 0xd0c3fa, spin = 0.38, tilt = 0.04, aspect = 1.02, satellites = 2}
    },
    {
        id = "moss-organ", type = "keys",
        name = "Moss Organ", wave_a = "triangle", wave_b = "square",
        attack = .016, decay = .21, sustain = .91, release = .11,
        morph = .43, color = .54, echo = .18, detune = 1200, fm = .15,
        lfo = 2.10, motion = .19,
        filter = {resonance = .79, depth = .37, velocity = .37, keytrack = .71,
                  attack = .019, decay = .23, sustain = .72, release = .11},
        mod = {{source = "lfo", destination = "pitch", amount = .13},
               {source = "velocity", destination = "morph", amount = .14}},
        scene = {mesh = "cage.obj", tint = 0x79964e, accent = 0xd3cb84, spin = 0.66, tilt = 0.3, aspect = 0.92, satellites = 4}
    },
    {
        id = "prism-mallets", type = "keys",
        name = "Prism Mallets", wave_a = "triangle", wave_b = "sine",
        attack = .002, decay = .33, sustain = .06, release = .43,
        morph = .47, color = .67, echo = .21, detune = 1200, fm = 1.76,
        lfo = .25, motion = .06, sample = .16, sample_file = "canopy.wav",
        filter = {resonance = 1.48, depth = 1.21, velocity = .98, keytrack = .90,
                  attack = .003, decay = .15, sustain = .05, release = .27},
        mod = {{source = "filter_env", destination = "fm", amount = .96},
               {source = "velocity", destination = "sample", amount = .14}},
        scene = {mesh = "prism.obj", tint = 0x74bed4, accent = 0xffb8a6, spin = 0.47, tilt = -0.47, aspect = 0.89, satellites = 5}
    },
    {
        id = "opal-wires", type = "keys",
        name = "Opal Wires", wave_a = "sine", wave_b = "sine",
        attack = .004, decay = 1.20, sustain = .22, release = 1.08,
        morph = .25, color = .76, echo = .32, detune = 1104, fm = 2.89,
        lfo = .64, motion = .20,
        filter = {resonance = 1.20, depth = 1.33, velocity = .58, keytrack = .73,
                  attack = .006, decay = .35, sustain = .09, release = .59},
        mod = {{source = "filter_env", destination = "fm", amount = 1.17},
               {source = "lfo", destination = "pitch", amount = .12}},
        scene = {mesh = "helix.obj", tint = 0x72d4ba, accent = 0xe4bbec, spin = -0.38, tilt = 0.25, aspect = 1.08, satellites = 1}
    },
    {
        id = "rain-on-glass", type = "keys",
        name = "Rain on Glass", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .73, sustain = .13, release = 1.40,
        morph = .66, color = .79, echo = .49, detune = 608, fm = 1.32,
        lfo = .29, motion = .12,
        filter = {resonance = .94, depth = .92, velocity = .54, keytrack = .60,
                  attack = .004, decay = .42, sustain = .20, release = .77},
        sample = .42, sample_file = "filament.wav",
        grain = {size = .095, density = 34, position = .22, spray = .23},
        mod = {{source = "lfo", destination = "grain_position", amount = .14},
               {source = "velocity", destination = "sample", amount = .15}},
        scene = {mesh = "kite.obj", tint = 0x74a4db, accent = 0xc2f1f4, spin = 0.54, tilt = -0.33, aspect = 1.03, satellites = 5}
    },
    {
        id = "slate-piano", type = "keys",
        name = "Slate Piano", wave_a = "triangle", wave_b = "saw",
        attack = .008, decay = .78, sustain = .41, release = .41,
        morph = .28, color = .63, echo = .26, detune = 17, fm = .54,
        lfo = .18, motion = .09, sample = .19, sample_file = "canopy.wav",
        filter = {resonance = .89, depth = 1.46, velocity = 1.11, keytrack = .72,
                  attack = .003, decay = .29, sustain = .18, release = .37},
        mod = {{source = "filter_env", destination = "fm", amount = .39},
               {source = "velocity", destination = "sample", amount = .16}},
        scene = {mesh = "anvil.obj", tint = 0x8498ae, accent = 0xf4c6b8, spin = 0.59, tilt = 0.28, aspect = 1.1, satellites = 4}
    },
    {
        id = "tin-music-box", type = "keys",
        name = "Tin Music Box", wave_a = "square", wave_b = "sine",
        attack = .002, decay = .52, sustain = .10, release = .62,
        morph = .71, color = .82, echo = .33, detune = 1200, fm = .91,
        lfo = 1.60, motion = .12,
        filter = {resonance = 1.20, depth = 1.35, velocity = .59, keytrack = .94,
                  attack = .002, decay = .20, sustain = .06, release = .25},
        mod = {{source = "filter_env", destination = "fm", amount = .63},
               {source = "lfo", destination = "pitch", amount = .095}},
        scene = {mesh = "cage.obj", tint = 0x9eafbf, accent = 0xecc685, spin = -0.72, tilt = -0.27, aspect = 1.01, satellites = 1}
    },
    {
        id = "autumn-chimes", type = "keys",
        name = "Autumn Chimes", wave_a = "sine", wave_b = "triangle",
        attack = .009, decay = 1.88, sustain = .05, release = 2.44,
        morph = .31, color = .69, echo = .67, detune = 702, fm = 3.30,
        lfo = .23, motion = .22,
        filter = {resonance = 1.07, depth = .85, velocity = .52, keytrack = .76,
                  attack = .008, decay = .62, sustain = .18, release = 1.39},
        sample = .31, sample_file = "canopy.wav",
        grain = {size = .13, density = 24, position = .16, spray = .16},
        mod = {{source = "lfo", destination = "grain_position", amount = .23},
               {source = "filter_env", destination = "fm", amount = 1.04}},
        scene = {mesh = "bell.obj", tint = 0xdf9956, accent = 0xe66d56, spin = 0.56, tilt = -0.45, aspect = 1.14, satellites = 3}
    },

    -- Plucks
    {
        id = "fern-mandolin", type = "plucks",
        name = "Fern Mandolin", wave_a = "triangle", wave_b = "square",
        attack = .002, decay = .22, sustain = .15, release = .33,
        morph = .35, color = .72, echo = .23, detune = 16, fm = .37,
        lfo = .45, motion = .14,
        filter = {resonance = 1.34, depth = 1.55, velocity = 1.01, keytrack = .65,
                  attack = .002, decay = .095, sustain = .04, release = .18},
        mod = {{source = "velocity", destination = "morph", amount = -.18},
               {source = "filter_env", destination = "fm", amount = .54}},
        scene = {mesh = "petal.obj", tint = 0x83b84d, accent = 0xe8cf71, spin = 0.65, tilt = -0.42, aspect = 1.09, satellites = 2}
    },
    {
        id = "rivet-harp", type = "plucks",
        name = "Rivet Harp", wave_a = "sine", wave_b = "saw",
        attack = .004, decay = .37, sustain = .12, release = .68,
        morph = .27, color = .61, echo = .31, detune = 1200, fm = 1.61,
        lfo = .31, motion = .07,
        filter = {resonance = 1.23, depth = 1.72, velocity = .88, keytrack = .82,
                  attack = .003, decay = .18, sustain = .09, release = .32},
        mod = {{source = "filter_env", destination = "fm", amount = .88},
               {source = "velocity", destination = "morph", amount = .10}},
        scene = {mesh = "cage.obj", tint = 0xbb8d76, accent = 0xeee3a3, spin = 0.34, tilt = 0.29, aspect = 0.97, satellites = 2}
    },
    {
        id = "satin-pick", type = "plucks",
        name = "Satin Pick", wave_a = "triangle", wave_b = "sine",
        attack = .001, decay = .30, sustain = .16, release = .29,
        morph = .48, color = .69, echo = .14, detune = -11, fm = .60,
        lfo = .52, motion = .12, sample = .12, sample_file = "canopy.wav",
        filter = {resonance = .82, depth = .91, velocity = .75, keytrack = .59,
                  attack = .003, decay = .12, sustain = .08, release = .17},
        mod = {{source = "filter_env", destination = "fm", amount = .44},
               {source = "velocity", destination = "sample", amount = .13}},
        scene = {mesh = "kite.obj", tint = 0xd68eab, accent = 0xf6cdbc, spin = 0.5, tilt = 0.0, aspect = 1.12, satellites = 3}
    },
    {
        id = "reed-lute", type = "plucks",
        name = "Reed Lute", wave_a = "square", wave_b = "triangle",
        attack = .003, decay = .32, sustain = .09, release = .35,
        morph = .46, color = .55, echo = .26, detune = -8, fm = .21,
        lfo = .82, motion = .13,
        filter = {resonance = 1.38, depth = 1.69, velocity = .87, keytrack = .77,
                  attack = .005, decay = .16, sustain = .05, release = .25},
        mod = {{source = "filter_env", destination = "morph", amount = -.26},
               {source = "velocity", destination = "fm", amount = .37}},
        scene = {mesh = "fan.obj", tint = 0xb1af67, accent = 0xe9c883, spin = -0.59, tilt = -0.25, aspect = 0.84, satellites = 2}
    },
    {
        id = "quartz-droplet", type = "plucks",
        name = "Quartz Droplet", wave_a = "sine", wave_b = "triangle",
        attack = .005, decay = .27, sustain = .07, release = .90,
        morph = .20, color = .23, echo = .60, detune = 690, fm = 2.22,
        lfo = .48, motion = .16,
        filter = {mode = "bandpass", resonance = .92, depth = 1.16, keytrack = .86,
                  attack = .004, decay = .19, sustain = .08, release = .32},
        mod = {{source = "filter_env", destination = "fm", amount = 1.12}},
        scene = {mesh = "spire.obj", tint = 0x73cde7, accent = 0xb8eef5, spin = -0.51, tilt = 0.17, aspect = 1.07, satellites = 4}
    },
    {
        id = "sundial-strings", type = "plucks",
        name = "Sundial Strings", wave_a = "saw", wave_b = "sine",
        attack = .004, decay = .48, sustain = .17, release = .46,
        morph = .56, color = .55, echo = .29, detune = 12, fm = .48,
        lfo = .27, motion = .19,
        filter = {resonance = 1.21, depth = 1.57, velocity = .64, keytrack = .76,
                  attack = .004, decay = .19, sustain = .11, release = .28},
        mod = {{source = "filter_env", destination = "morph", amount = -.19},
               {source = "lfo", destination = "cutoff", amount = .24}},
        scene = {mesh = "relay.obj", tint = 0xc8884b, accent = 0xf8b568, spin = 0.31, tilt = 0.47, aspect = 1.07, satellites = 1}
    },
    {
        id = "spruce-koto", type = "plucks",
        name = "Spruce Koto", wave_a = "triangle", wave_b = "saw",
        attack = .002, decay = .45, sustain = .10, release = .57,
        morph = .26, color = .78, echo = .29, detune = -22, fm = 1.12,
        lfo = .56, motion = .15, sample = .15, sample_file = "canopy.wav",
        filter = {resonance = 1.55, depth = 1.64, velocity = .86, keytrack = .90,
                  attack = .003, decay = .15, sustain = .08, release = .31},
        mod = {{source = "filter_env", destination = "fm", amount = 1.2},
               {source = "velocity", destination = "sample", amount = .10}},
        scene = {mesh = "petal.obj", tint = 0x599f78, accent = 0xd0d693, spin = 0.61, tilt = -0.19, aspect = 1.05, satellites = 2}
    },
    {
        id = "coral-pop", type = "plucks",
        name = "Coral Pop", wave_a = "square", wave_b = "sine",
        attack = .003, decay = .14, sustain = .12, release = .21,
        morph = .32, color = .73, echo = .27, detune = 1200, fm = .33,
        lfo = 1.20, motion = .13,
        filter = {resonance = 1.57, depth = 1.38, velocity = .94, keytrack = .53,
                  attack = .003, decay = .072, sustain = .05, release = .14},
        mod = {{source = "filter_env", destination = "fm", amount = .83},
               {source = "velocity", destination = "morph", amount = -.18}},
        scene = {mesh = "prism.obj", tint = 0xeb857f, accent = 0xffd36d, spin = 0.5, tilt = -0.15, aspect = 1.05, satellites = 3}
    },
    {
        id = "raindrop-zither", type = "plucks",
        name = "Raindrop Zither", wave_a = "sine", wave_b = "square",
        attack = .003, decay = .42, sustain = .13, release = .77,
        morph = .17, color = .76, echo = .49, detune = 841, fm = 1.87,
        lfo = .73, motion = .22,
        filter = {resonance = 1.05, depth = 1.10, velocity = .72, keytrack = .77,
                  attack = .004, decay = .24, sustain = .15, release = .55},
        sample = .29, sample_file = "filament.wav",
        grain = {size = .065, density = 27, position = .22, spray = .19},
        mod = {{source = "lfo", destination = "grain_position", amount = .17},
               {source = "filter_env", destination = "fm", amount = .64}},
        scene = {mesh = "helix.obj", tint = 0x61b3d1, accent = 0xe4bdeb, spin = -0.4, tilt = -0.09, aspect = 1.17, satellites = 4}
    },
    {
        id = "charcoal-plectrum", type = "plucks",
        name = "Charcoal Plectrum", wave_a = "triangle", wave_b = "noise",
        attack = .001, decay = .28, sustain = .19, release = .26,
        morph = .12, color = .62, echo = .12, detune = 0, fm = .44,
        lfo = .39, motion = .08,
        filter = {resonance = 1.04, depth = 1.54, velocity = .77, keytrack = .61,
                  attack = .002, decay = .11, sustain = .09, release = .20},
        mod = {{source = "filter_env", destination = "morph", amount = .32},
               {source = "velocity", destination = "fm", amount = .38}},
        scene = {mesh = "kite.obj", tint = 0x8d879f, accent = 0xe2a576, spin = 0.53, tilt = 0.39, aspect = 0.97, satellites = 5}
    },

    -- Basses
    {
        id = "subterrane", type = "bass",
        name = "Subterrane", wave_a = "sine", wave_b = "sine",
        attack = .011, decay = .34, sustain = .82, release = .21,
        morph = .72, color = .31, echo = .12, detune = -1200, fm = .22,
        lfo = .43, motion = .12,
        filter = {resonance = .78, depth = .59, velocity = .54, keytrack = .34,
                  attack = .019, decay = .24, sustain = .32, release = .15},
        mod = {{source = "filter_env", destination = "fm", amount = .46},
               {source = "velocity", destination = "morph", amount = -.09}},
        scene = {mesh = "anvil.obj", tint = 0x6f789e, accent = 0xb489de, spin = 0.65, tilt = -0.25, aspect = 0.87, satellites = 3}
    },
    {
        id = "pitch-black", type = "bass",
        name = "Pitch Black", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .29, sustain = .70, release = .14,
        morph = .42, color = .30, echo = .10, detune = -1200, fm = .15,
        lfo = .56, motion = .14,
        filter = {resonance = 1.57, depth = 1.56, velocity = 1.00, keytrack = .60,
                  attack = .003, decay = .16, sustain = .06, release = .14},
        mod = {{source = "filter_env", destination = "morph", amount = -.21},
               {source = "velocity", destination = "fm", amount = .19}},
        scene = {mesh = "cage.obj", tint = 0x586d85, accent = 0xf490bc, spin = -0.46, tilt = -0.13, aspect = 1.18, satellites = 4}
    },
    {
        id = "magma-saw", type = "bass",
        name = "Magma Saw", wave_a = "saw", wave_b = "saw",
        attack = .003, decay = .38, sustain = .67, release = .20,
        morph = .41, color = .56, echo = .05, detune = 8, fm = .26,
        lfo = .62, motion = .12,
        filter = {resonance = 3.35, depth = 2.8, velocity = 1.05, keytrack = .51,
                  attack = .012, decay = .3, sustain = .08, release = .13},
        mod = {{source = "lfo", destination = "cutoff", amount = .26},
               {source = "key", destination = "pitch", amount = .065}},
        scene = {mesh = "anvil.obj", tint = 0xdb575a, accent = 0xffb46c, spin = 0.64, tilt = -0.16, aspect = 1.05, satellites = 4}
    },
    {
        id = "velvet-sub", type = "bass",
        name = "Velvet Sub", wave_a = "triangle", wave_b = "sine",
        attack = .017, decay = .52, sustain = .83, release = .22,
        morph = .62, color = .35, echo = .09, detune = -1200, fm = .74,
        lfo = .25, motion = .05,
        filter = {resonance = .79, depth = .74, velocity = .52, keytrack = .40,
                  attack = .033, decay = .33, sustain = .30, release = .17},
        mod = {{source = "filter_env", destination = "fm", amount = .32},
               {source = "lfo", destination = "cutoff", amount = .21}},
        scene = {mesh = "prism.obj", tint = 0x8b628f, accent = 0xe992be, spin = 0.31, tilt = -0.09, aspect = 1.0, satellites = 3}
    },
    {
        id = "cobalt-punch", type = "bass",
        name = "Cobalt Punch", wave_a = "square", wave_b = "triangle",
        attack = .002, decay = .18, sustain = .42, release = .14,
        morph = .54, color = .40, echo = .12, detune = -1200, fm = .90,
        lfo = 1.15, motion = .15,
        filter = {resonance = 2.24, depth = 2.02, velocity = 1.27, keytrack = .59,
                  attack = .003, decay = .101, sustain = .07, release = .14},
        mod = {{source = "filter_env", destination = "fm", amount = .68},
               {source = "velocity", destination = "morph", amount = -.15}},
        scene = {mesh = "anvil.obj", tint = 0x4d88d2, accent = 0x72dbee, spin = 0.67, tilt = -0.32, aspect = 0.89, satellites = 2}
    },
    {
        id = "silicon-funk", type = "bass",
        name = "Silicon Funk", wave_a = "saw", wave_b = "square",
        attack = .004, decay = .26, sustain = .46, release = .18,
        morph = .28, color = .39, echo = .18, detune = -13, fm = 1.39,
        lfo = 2.22, motion = .24,
        filter = {resonance = 2.61, depth = 1.94, velocity = 1.16, keytrack = .62,
                  attack = .008, decay = .20, sustain = .13, release = .17},
        mod = {{source = "lfo", destination = "cutoff", amount = .53},
               {source = "filter_env", destination = "fm", amount = .66}},
        scene = {mesh = "relay.obj", tint = 0x73ba84, accent = 0xd4ed85, spin = 0.72, tilt = 0.02, aspect = 0.93, satellites = 3}
    },
    {
        id = "deepwater-bass", type = "bass",
        name = "Deepwater Bass", wave_a = "sine", wave_b = "triangle",
        attack = .047, decay = .60, sustain = .79, release = .37,
        morph = .55, color = .27, echo = .16, detune = -1200, fm = .83,
        lfo = .77, motion = .24,
        filter = {resonance = 1.22, depth = 1.12, velocity = .63, keytrack = .44,
                  attack = .058, decay = .38, sustain = .41, release = .26},
        mod = {{source = "lfo", destination = "cutoff", amount = .39},
               {source = "filter_env", destination = "fm", amount = .34}},
        scene = {mesh = "arch.obj", tint = 0x458ca7, accent = 0x87e4ce, spin = 0.48, tilt = -0.35, aspect = 1.08, satellites = 2}
    },
    {
        id = "neon-growl", type = "bass",
        name = "Neon Growl", wave_a = "saw", wave_b = "sine",
        attack = .010, decay = .45, sustain = .66, release = .26,
        morph = .37, color = .40, echo = .24, detune = -1200, fm = 3.35,
        lfo = 1.16, motion = .26,
        filter = {resonance = 2.75, depth = 1.56, velocity = .91, keytrack = .55,
                  attack = .013, decay = .29, sustain = .23, release = .22},
        shape = {loop = true, {time = .27, level = .65}, {time = .40, level = -.73},
                 {time = .19, level = .09}},
        mod = {{source = "shape", destination = "fm", amount = 1.26},
               {source = "lfo", destination = "cutoff", amount = .64}},
        scene = {mesh = "cage.obj", tint = 0xb56bba, accent = 0xffa774, spin = 0.5, tilt = 0.13, aspect = 1.16, satellites = 4}
    },
    {
        id = "iron-pulse", type = "bass",
        name = "Iron Pulse", wave_a = "square", wave_b = "square",
        attack = .004, decay = .33, sustain = .65, release = .19,
        morph = .36, color = .34, echo = .07, detune = 20, fm = .05,
        lfo = .84, motion = .21,
        filter = {resonance = 1.66, depth = 1.86, velocity = .79, keytrack = .65,
                  attack = .004, decay = .15, sustain = .11, release = .16},
        mod = {{source = "filter_env", destination = "morph", amount = .19},
               {source = "lfo", destination = "cutoff", amount = .38}},
        scene = {mesh = "anvil.obj", tint = 0x9e8190, accent = 0xea9277, spin = -0.51, tilt = -0.04, aspect = 1.13, satellites = 5}
    },
    {
        id = "mercury-bass", type = "bass",
        name = "Mercury Bass", wave_a = "triangle", wave_b = "saw",
        attack = .006, decay = .44, sustain = .71, release = .23,
        morph = .47, color = .41, echo = .16, detune = -1200, fm = .58,
        lfo = 1.68, motion = .20,
        filter = {resonance = 1.72, depth = 1.31, velocity = .99, keytrack = .54,
                  attack = .008, decay = .24, sustain = .20, release = .20},
        mod = {{source = "lfo", destination = "cutoff", amount = .55},
               {source = "filter_env", destination = "fm", amount = .63}},
        scene = {mesh = "bell.obj", tint = 0x849bc6, accent = 0xc5baf0, spin = 0.65, tilt = -0.41, aspect = 0.98, satellites = 2}
    },
    {
        id = "mothwing-sub", type = "bass",
        name = "Mothwing Sub", wave_a = "sine", wave_b = "noise",
        attack = .019, decay = .62, sustain = .75, release = .31,
        morph = .09, color = .37, echo = .10, detune = 0, fm = .25,
        lfo = .37, motion = .11,
        filter = {resonance = .94, depth = 1.07, velocity = .53, keytrack = .47,
                  attack = .035, decay = .43, sustain = .32, release = .22},
        mod = {{source = "filter_env", destination = "morph", amount = .15},
               {source = "lfo", destination = "cutoff", amount = .21}},
        scene = {mesh = "fan.obj", tint = 0x567b8e, accent = 0xbcbd80, spin = 0.33, tilt = 0.24, aspect = 1.02, satellites = 2}
    },
    {
        id = "terminal-bass", type = "bass",
        name = "Terminal Bass", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .22, sustain = .58, release = .20,
        morph = .46, color = .43, echo = .19, detune = 1200, fm = 1.55,
        lfo = 1.26, motion = .22,
        filter = {resonance = 2.22, depth = 1.95, velocity = 1.08, keytrack = .64,
                  attack = .005, decay = .14, sustain = .08, release = .16},
        mod = {{source = "filter_env", destination = "fm", amount = .89},
               {source = "velocity", destination = "morph", amount = -.17}},
        scene = {mesh = "cage.obj", tint = 0xbd6856, accent = 0xf7ca77, spin = 0.44, tilt = -0.3, aspect = 1.12, satellites = 3}
    },

    -- Leads
    {
        id = "foxfire", type = "leads",
        name = "Foxfire", wave_a = "saw", wave_b = "triangle",
        attack = .014, decay = .33, sustain = .79, release = .33,
        morph = .35, color = .62, echo = .32, detune = 14, fm = .50,
        lfo = 1.49, motion = .17,
        filter = {resonance = 1.47, depth = 1.38, velocity = .77, keytrack = .80,
                  attack = .015, decay = .23, sustain = .32, release = .24},
        mod = {{source = "lfo", destination = "pitch", amount = .18},
               {source = "filter_env", destination = "morph", amount = -.19}},
        scene = {mesh = "kite.obj", tint = 0xef8455, accent = 0xffe58f, spin = -0.65, tilt = -0.27, aspect = 0.98, satellites = 1}
    },
    {
        id = "phosphor-reed", type = "leads",
        name = "Phosphor Reed", wave_a = "square", wave_b = "sine",
        attack = .012, decay = .48, sustain = .76, release = .42,
        morph = .37, color = .56, echo = .34, detune = -8, fm = .65,
        lfo = 1.94, motion = .19,
        filter = {resonance = 1.29, depth = .97, velocity = .58, keytrack = .86,
                  attack = .022, decay = .31, sustain = .38, release = .32},
        mod = {{source = "lfo", destination = "pitch", amount = .20},
               {source = "filter_env", destination = "fm", amount = .62}},
        scene = {mesh = "spire.obj", tint = 0xa8d45f, accent = 0xddf583, spin = 0.5, tilt = 0.4, aspect = 0.97, satellites = 2}
    },
    {
        id = "starling", type = "leads",
        name = "Starling", wave_a = "triangle", wave_b = "sine",
        attack = .029, decay = .62, sustain = .83, release = .48,
        morph = .58, color = .81, echo = .39, detune = 1200, fm = .72,
        lfo = 2.25, motion = .15,
        filter = {resonance = .95, depth = .68, velocity = .52, keytrack = .78,
                  attack = .041, decay = .36, sustain = .46, release = .36},
        mod = {{source = "lfo", destination = "pitch", amount = .18},
               {source = "amp_env", destination = "fm", amount = .23}},
        scene = {mesh = "fan.obj", tint = 0x7baecb, accent = 0xeedae0, spin = 0.73, tilt = -0.14, aspect = 0.9, satellites = 4}
    },
    {
        id = "solar-needle", type = "leads",
        name = "Solar Needle", wave_a = "saw", wave_b = "square",
        attack = .004, decay = .38, sustain = .68, release = .30,
        morph = .34, color = .69, echo = .30, detune = 11, fm = .31,
        lfo = 1.20, motion = .16,
        filter = {resonance = 1.96, depth = 1.24, velocity = .72, keytrack = .87,
                  attack = .007, decay = .19, sustain = .31, release = .21},
        mod = {{source = "lfo", destination = "pitch", amount = .16},
               {source = "velocity", destination = "fm", amount = .42}},
        scene = {mesh = "spire.obj", tint = 0xf0ad4e, accent = 0xfff486, spin = 0.73, tilt = 0.01, aspect = 1.07, satellites = 2}
    },
    {
        id = "tungsten-horn", type = "leads",
        name = "Tungsten Horn", wave_a = "saw", wave_b = "sine",
        attack = .083, decay = .43, sustain = .83, release = .31,
        morph = .25, color = .48, echo = .17, detune = 0, fm = .83,
        lfo = .89, motion = .17,
        filter = {resonance = 1.49, depth = 1.67, velocity = 1.03, keytrack = .77,
                  attack = .111, decay = .34, sustain = .38, release = .24},
        mod = {{source = "velocity", destination = "fm", amount = .49},
               {source = "amp_env", destination = "morph", amount = -.16}},
        scene = {mesh = "bell.obj", tint = 0xc1a37b, accent = 0xfff4b0, spin = 0.64, tilt = -0.29, aspect = 0.88, satellites = 1}
    },
    {
        id = "copper-flute", type = "leads",
        name = "Copper Flute", wave_a = "sine", wave_b = "noise",
        attack = .051, decay = .66, sustain = .81, release = .39,
        morph = .07, color = .74, echo = .29, detune = 0, fm = .24,
        lfo = 2.34, motion = .13,
        filter = {resonance = .79, depth = .63, velocity = .41, keytrack = .82,
                  attack = .063, decay = .37, sustain = .48, release = .30},
        mod = {{source = "lfo", destination = "pitch", amount = .18},
               {source = "velocity", destination = "morph", amount = .09}},
        scene = {mesh = "helix.obj", tint = 0xcd8558, accent = 0x98dacb, spin = -0.7, tilt = -0.26, aspect = 1.05, satellites = 1}
    },
    {
        id = "bit-comet", type = "leads",
        name = "Bit Comet", wave_a = "square", wave_b = "square",
        attack = .005, decay = .37, sustain = .70, release = .22,
        morph = .33, color = .76, echo = .36, detune = 1200, fm = .15,
        lfo = 1.81, motion = .20,
        filter = {resonance = 1.33, depth = .73, velocity = .66, keytrack = .85,
                  attack = .006, decay = .20, sustain = .39, release = .17},
        mod = {{source = "lfo", destination = "pitch", amount = .16},
               {source = "filter_env", destination = "morph", amount = -.18}},
        scene = {mesh = "kite.obj", tint = 0xd472a3, accent = 0xf6c970, spin = -0.53, tilt = -0.35, aspect = 0.87, satellites = 4}
    },
    {
        id = "lacquer-lead", type = "leads",
        name = "Lacquer Lead", wave_a = "triangle", wave_b = "saw",
        attack = .023, decay = .41, sustain = .75, release = .33,
        morph = .54, color = .54, echo = .34, detune = -14, fm = 1.84,
        lfo = 1.24, motion = .18,
        filter = {resonance = 1.53, depth = 1.18, velocity = .89, keytrack = .79,
                  attack = .031, decay = .32, sustain = .34, release = .26},
        mod = {{source = "lfo", destination = "pitch", amount = .15},
               {source = "filter_env", destination = "fm", amount = .61}},
        scene = {mesh = "prism.obj", tint = 0xc95d74, accent = 0xf7ba90, spin = 0.41, tilt = 0.41, aspect = 0.89, satellites = 5}
    },
    {
        id = "signal-pike", type = "leads",
        name = "Signal Pike", wave_a = "sine", wave_b = "square",
        attack = .008, decay = .30, sustain = .72, release = .31,
        morph = .28, color = .71, echo = .36, detune = 596, fm = 2.65,
        lfo = .83, motion = .18,
        filter = {resonance = 1.69, depth = 1.14, velocity = .62, keytrack = .88,
                  attack = .010, decay = .22, sustain = .24, release = .22},
        mod = {{source = "filter_env", destination = "fm", amount = 1.01},
               {source = "lfo", destination = "pitch", amount = .13}},
        scene = {mesh = "spire.obj", tint = 0x5ac8d4, accent = 0xc4f4df, spin = 0.51, tilt = -0.13, aspect = 0.9, satellites = 4}
    },
    {
        id = "lantern-piper", type = "leads",
        name = "Lantern Piper", wave_a = "triangle", wave_b = "square",
        attack = .049, decay = .56, sustain = .80, release = .45,
        morph = .36, color = .30, echo = .27, detune = 1200, fm = .52,
        lfo = 1.58, motion = .23,
        filter = {mode = "bandpass", resonance = .78, depth = .49, velocity = .46, keytrack = .95,
                  attack = .059, decay = .37, sustain = .36, release = .32},
        mod = {{source = "lfo", destination = "pitch", amount = .19},
               {source = "velocity", destination = "morph", amount = .13}},
        scene = {mesh = "petal.obj", tint = 0xe9b05c, accent = 0xf7dcab, spin = 0.57, tilt = -0.53, aspect = 1.03, satellites = 3}
    },
    {
        id = "ribbon-rocket", type = "leads",
        name = "Ribbon Rocket", wave_a = "saw", wave_b = "saw",
        attack = .007, decay = .36, sustain = .80, release = .49,
        morph = .47, color = .61, echo = .49, detune = -24, fm = .43,
        lfo = 2.07, motion = .28,
        filter = {resonance = 1.48, depth = 1.32, velocity = .78, keytrack = .87,
                  attack = .012, decay = .29, sustain = .35, release = .35},
        mod = {{source = "lfo", destination = "pitch", amount = .19},
               {source = "filter_env", destination = "morph", amount = -.16}},
        scene = {mesh = "helix.obj", tint = 0xd179ca, accent = 0xf2a176, spin = -0.83, tilt = 0.45, aspect = 1.15, satellites = 5}
    },
    {
        id = "fever-ray", type = "leads",
        name = "Fever Ray", wave_a = "sine", wave_b = "triangle",
        attack = .010, decay = .33, sustain = .75, release = .54,
        morph = .28, color = .59, echo = .45, detune = 890, fm = 3.92,
        lfo = 2.68, motion = .27,
        filter = {resonance = 1.63, depth = 1.19, velocity = .74, keytrack = .80,
                  attack = .016, decay = .22, sustain = .40, release = .38},
        mod = {{source = "filter_env", destination = "fm", amount = 1.34},
               {source = "lfo", destination = "pitch", amount = .24}},
        scene = {mesh = "fan.obj", tint = 0xd86483, accent = 0xf5d376, spin = 0.8, tilt = 0.34, aspect = 1.09, satellites = 5}
    },
    {
        id = "gilt-resistor", type = "leads",
        name = "Gilt Resistor", wave_a = "square", wave_b = "triangle",
        attack = .021, decay = .42, sustain = .79, release = .28,
        morph = .61, color = .61, echo = .20, detune = 19, fm = .23,
        lfo = .72, motion = .32,
        filter = {resonance = 1.36, depth = 1.02, velocity = .68, keytrack = .83,
                  attack = .026, decay = .27, sustain = .33, release = .24},
        mod = {{source = "lfo", destination = "pitch", amount = .12},
               {source = "velocity", destination = "morph", amount = .17}},
        scene = {mesh = "cage.obj", tint = 0xcfa353, accent = 0xbfe297, spin = 0.51, tilt = -0.04, aspect = 1.0, satellites = 1}
    },
    {
        id = "star-scanner", type = "leads",
        name = "Star Scanner", wave_a = "saw", wave_b = "sine",
        attack = .026, decay = .62, sustain = .77, release = .68,
        morph = .48, color = .63, echo = .55, detune = 1200, fm = 1.22,
        lfo = 1.11, motion = .28,
        filter = {resonance = 1.54, depth = 1.22, velocity = .66, keytrack = .82,
                  attack = .047, decay = .41, sustain = .33, release = .51},
        shape = {loop = true, {time = .45, level = .66}, {time = .67, level = -.62},
                 {time = .37, level = .13}},
        mod = {{source = "shape", destination = "cutoff", amount = .46},
               {source = "lfo", destination = "pitch", amount = .18}},
        scene = {mesh = "relay.obj", tint = 0x5e9ddd, accent = 0xafebf9, spin = 0.48, tilt = -0.13, aspect = 0.97, satellites = 2}
    },

    -- Rhythmic and morphing
    {
        id = "clock-coral", type = "rhythms",
        name = "Clock Coral", wave_a = "square", wave_b = "sine",
        attack = .015, decay = .71, sustain = .69, release = .57,
        morph = .51, color = .62, echo = .44, detune = 1200, fm = 1.30,
        lfo = 4.00, motion = .45,
        filter = {resonance = 1.2, depth = .62, keytrack = .43},
        shape = {loop = true, {time = .07, level = 1}, {time = .15, level = -.94},
                 {time = .07, level = .09}, {time = .15, level = -.67}},
        mod = {{source = "shape", destination = "cutoff", amount = 1.46},
               {source = "shape", destination = "morph", amount = .28}},
        scene = {mesh = "relay.obj", tint = 0xe18181, accent = 0xffc495, spin = 0.93, tilt = -0.2, aspect = 1.03, satellites = 2}
    },
    {
        id = "lattice-pump", type = "rhythms",
        name = "Lattice Pump", wave_a = "saw", wave_b = "triangle",
        attack = .028, decay = .90, sustain = .72, release = .84,
        morph = .54, color = .43, echo = .37, detune = -19, fm = .77,
        lfo = 3.22, motion = .51,
        filter = {resonance = 1.79, depth = 1.61, velocity = .59, keytrack = .52,
                  attack = .04, decay = .36, sustain = .28, release = .43},
        shape = {loop = true, {time = .045, level = .91}, {time = .20, level = -.88},
                 {time = .055, level = .29}, {time = .22, level = -.73}},
        mod = {{source = "shape", destination = "cutoff", amount = 1.39},
               {source = "shape", destination = "morph", amount = .27}},
        scene = {mesh = "cage.obj", tint = 0x59b4ab, accent = 0xc2eeb9, spin = -0.7, tilt = 0.05, aspect = 1.13, satellites = 4}
    },
    {
        id = "tern-signal", type = "rhythms",
        name = "Tern Signal", wave_a = "sine", wave_b = "noise",
        attack = .018, decay = .62, sustain = .76, release = .65,
        morph = .25, color = .56, echo = .42, detune = 0, fm = .88,
        lfo = 6.00, motion = .28,
        filter = {resonance = 1.41, depth = .95, velocity = .63, keytrack = .53,
                  attack = .029, decay = .30, sustain = .25, release = .47},
        shape = {loop = true, {time = .055, level = .76}, {time = .12, level = -.84},
                 {time = .048, level = .44}, {time = .14, level = -.68}},
        mod = {{source = "shape", destination = "morph", amount = .24},
               {source = "shape", destination = "cutoff", amount = .87}},
        scene = {mesh = "kite.obj", tint = 0x8facb7, accent = 0xf6d09d, spin = -1.1, tilt = -0.26, aspect = 1.16, satellites = 1}
    },
    {
        id = "neon-weave", type = "rhythms",
        name = "Neon Weave", wave_a = "sine", wave_b = "saw",
        attack = .038, decay = 1.03, sustain = .71, release = 1.07,
        morph = .49, color = .51, echo = .59, detune = 27, fm = 1.66,
        lfo = 2.68, motion = .57,
        filter = {resonance = 1.43, depth = 1.31, velocity = .63, keytrack = .58,
                  attack = .063, decay = .47, sustain = .27, release = .61},
        shape = {loop = true, {time = .16, level = .87}, {time = .30, level = -.81},
                 {time = .08, level = .22}, {time = .24, level = -.61}},
        sample = .27, sample_file = "filament.wav",
        grain = {size = .125, density = 33, position = .37, spray = .15},
        mod = {{source = "shape", destination = "cutoff", amount = 1.10},
               {source = "shape", destination = "grain_position", amount = .29}},
        scene = {mesh = "helix.obj", tint = 0x93d66d, accent = 0xf3adec, spin = -0.85, tilt = 0.14, aspect = 0.93, satellites = 4}
    },
    {
        id = "cinder-rotor", type = "rhythms",
        name = "Cinder Rotor", wave_a = "saw", wave_b = "square",
        attack = .052, decay = .76, sustain = .77, release = .92,
        morph = .39, color = .51, echo = .43, detune = 1200, fm = .35,
        lfo = 7.00, motion = .39,
        filter = {resonance = 1.87, depth = 1.34, velocity = .58, keytrack = .64,
                  attack = .050, decay = .36, sustain = .26, release = .64},
        shape = {loop = true, {time = .048, level = .88}, {time = .13, level = -.79},
                 {time = .065, level = .51}, {time = .15, level = -.61}},
        mod = {{source = "shape", destination = "cutoff", amount = 1.32},
               {source = "shape", destination = "fm", amount = .61}},
        scene = {mesh = "fan.obj", tint = 0xcb6e64, accent = 0xffb282, spin = -1.2, tilt = 0.15, aspect = 0.84, satellites = 3}
    },
    {
        id = "phantom-rhythm", type = "rhythms",
        name = "Phantom Rhythm", wave_a = "triangle", wave_b = "noise",
        attack = .088, decay = 1.04, sustain = .73, release = 1.19,
        morph = .20, color = .48, echo = .61, detune = 0, fm = .42,
        lfo = 3.48, motion = .25,
        filter = {resonance = 1.34, depth = 1.08, velocity = .55, keytrack = .57,
                  attack = .10, decay = .41, sustain = .34, release = .81},
        shape = {loop = true, {time = .13, level = .72}, {time = .21, level = -.75},
                 {time = .072, level = .49}, {time = .29, level = -.67}},
        sample = .36, sample_file = "orchard-air.wav",
        grain = {size = .14, density = 33, position = .54, spray = .20},
        mod = {{source = "shape", destination = "cutoff", amount = 1.08},
               {source = "shape", destination = "grain_position", amount = .30}},
        scene = {mesh = "relay.obj", tint = 0x8b79d0, accent = 0xa4e6dd, spin = -0.9, tilt = -0.4, aspect = 0.92, satellites = 5}
    },
    {
        id = "darkroom-sweep", type = "rhythms",
        name = "Darkroom Sweep", wave_a = "sine", wave_b = "square",
        attack = .32, decay = 1.28, sustain = .83, release = 1.87,
        morph = .42, color = .36, echo = .54, detune = -12, fm = 2.43,
        lfo = 1.43, motion = .61,
        filter = {resonance = 1.06, depth = 1.24, velocity = .49, keytrack = .47,
                  attack = .37, decay = .55, sustain = .42, release = 1.22},
        shape = {loop = true, {time = .43, level = -.62}, {time = .35, level = .91},
                 {time = .58, level = -.77}},
        sample = .31, sample_file = "orchard-air.wav",
        grain = {size = .17, density = 25, position = .45, spray = .15},
        mod = {{source = "shape", destination = "cutoff", amount = 1.32},
               {source = "shape", destination = "grain_position", amount = .29}},
        scene = {mesh = "arch.obj", tint = 0xa063a3, accent = 0xfc8fae, spin = 0.5, tilt = 0.39, aspect = 0.94, satellites = 2}
    },
    {
        id = "cascade-relay", type = "rhythms",
        name = "Cascade Relay", wave_a = "triangle", wave_b = "saw",
        attack = .110, decay = .91, sustain = .76, release = 1.23,
        morph = .57, color = .44, echo = .64, detune = 1200, fm = .92,
        lfo = 4.64, motion = .52,
        filter = {resonance = 1.58, depth = 1.60, velocity = .63, keytrack = .61,
                  attack = .13, decay = .44, sustain = .24, release = .80},
        shape = {loop = true, {time = .08, level = .86}, {time = .15, level = -.89},
                 {time = .085, level = .52}, {time = .16, level = -.68}},
        mod = {{source = "shape", destination = "cutoff", amount = 1.31},
               {source = "shape", destination = "fm", amount = .73}},
        scene = {mesh = "relay.obj", tint = 0x67a9e2, accent = 0xb7f5ec, spin = 1.08, tilt = -0.45, aspect = 1.06, satellites = 1}
    },
    {
        id = "transistor-tide", type = "rhythms",
        name = "Transistor Tide", wave_a = "square", wave_b = "triangle",
        attack = .074, decay = 1.15, sustain = .68, release = .81,
        morph = .56, color = .43, echo = .29, detune = 17, fm = .26,
        lfo = 5.24, motion = .55,
        filter = {resonance = 1.67, depth = 1.24, velocity = .56, keytrack = .62,
                  attack = .090, decay = .48, sustain = .29, release = .60},
        shape = {loop = true, {time = .093, level = .80}, {time = .18, level = -.65},
                 {time = .071, level = .33}, {time = .19, level = -.85}},
        mod = {{source = "shape", destination = "cutoff", amount = 1.28},
               {source = "shape", destination = "morph", amount = .25}},
        scene = {mesh = "cage.obj", tint = 0x6ab29a, accent = 0xe7db91, spin = 1.17, tilt = -0.52, aspect = 1.02, satellites = 1}
    },
    {
        id = "aurora-gate", type = "rhythms",
        name = "Aurora Gate", wave_a = "sine", wave_b = "triangle",
        attack = .18, decay = 1.76, sustain = .83, release = 1.73,
        morph = .45, color = .51, echo = .62, detune = 1200, fm = 2.01,
        lfo = 2.12, motion = .48,
        filter = {resonance = 1.19, depth = 1.29, velocity = .50, keytrack = .45,
                  attack = .24, decay = .67, sustain = .38, release = 1.21},
        shape = {loop = true, {time = .105, level = .72}, {time = .27, level = -.73},
                 {time = .09, level = .34}, {time = .29, level = -.64}},
        sample = .37, sample_file = "filament.wav",
        grain = {size = .16, density = 30, position = .41, spray = .16},
        mod = {{source = "shape", destination = "cutoff", amount = 1.17},
               {source = "shape", destination = "grain_position", amount = .34}},
        scene = {mesh = "arch.obj", tint = 0x79cbb8, accent = 0xd2a6f5, spin = -0.67, tilt = 0.24, aspect = 1.04, satellites = 2}
    },

    -- Drums and percussion
    {
        id = "thump-coil", type = "percussion",
        name = "Thump Coil", wave_a = "sine", wave_b = "triangle",
        attack = .001, decay = .15, sustain = .01, release = .08,
        morph = .32, color = .28, echo = .06, detune = -1200, fm = 1.05,
        lfo = .30, motion = .04,
        filter = {resonance = 1.13, depth = 1.31, velocity = .54, keytrack = .57,
                  attack = .002, decay = .088, sustain = .03, release = .05},
        shape = {{time = .005, level = 1}, {time = .061, level = 0}},
        mod = {{source = "shape", destination = "pitch", amount = 20},
               {source = "filter_env", destination = "fm", amount = .72}},
        scene = {mesh = "anvil.obj", tint = 0xeb9268, accent = 0xffd6a0, spin = -0.51, tilt = -0.2, aspect = 1.03, satellites = 5}
    },
    {
        id = "dust-snare", type = "percussion",
        name = "Dust Snare", wave_a = "triangle", wave_b = "noise",
        attack = .002, decay = .15, sustain = .03, release = .11,
        morph = .79, color = .54, echo = .17, detune = 0, fm = .40,
        lfo = 3.5, motion = .13,
        filter = {mode = "highpass", resonance = .82, depth = -.85, velocity = .36, keytrack = .17,
                  attack = .002, decay = .092, sustain = .04, release = .07},
        mod = {{source = "filter_env", destination = "morph", amount = .18},
               {source = "velocity", destination = "fm", amount = .41}},
        scene = {mesh = "fan.obj", tint = 0xc4a888, accent = 0xf2dbaf, spin = 0.97, tilt = -0.32, aspect = 1.17, satellites = 1}
    },
    {
        id = "circuit-hat", type = "percussion",
        name = "Circuit Hat", wave_a = "square", wave_b = "noise",
        attack = .001, decay = .05, sustain = .01, release = .04,
        morph = .92, color = .78, echo = .10, detune = 1100, fm = .20,
        lfo = 3.0, motion = .07,
        filter = {mode = "highpass", resonance = .76, depth = -.68, velocity = .31, keytrack = .22,
                  attack = .003, decay = .046, sustain = .08, release = .035},
        mod = {{source = "velocity", destination = "morph", amount = .07},
               {source = "filter_env", destination = "fm", amount = .5}},
        scene = {mesh = "cage.obj", tint = 0x9cc8ba, accent = 0xefecad, spin = 0.74, tilt = 0.17, aspect = 0.92, satellites = 3}
    },
    {
        id = "glacial-rim", type = "percussion",
        name = "Glacial Rim", wave_a = "square", wave_b = "sine",
        attack = .001, decay = .08, sustain = .01, release = .15,
        morph = .23, color = .51, echo = .27, detune = 851, fm = 2.38,
        lfo = .60, motion = .14,
        filter = {mode = "highpass", resonance = 1.11, depth = -.80, velocity = .44, keytrack = .34,
                  attack = .002, decay = .061, sustain = .04, release = .09},
        shape = {{time = .005, level = 1}, {time = .037, level = 0}},
        mod = {{source = "shape", destination = "pitch", amount = 6},
               {source = "filter_env", destination = "fm", amount = .81}},
        scene = {mesh = "relay.obj", tint = 0x7dbce8, accent = 0xe3f9ff, spin = -0.43, tilt = -0.47, aspect = 0.84, satellites = 4}
    },
    {
        id = "soft-frame-drum", type = "percussion",
        name = "Soft Frame Drum", wave_a = "sine", wave_b = "noise",
        attack = .003, decay = .23, sustain = .01, release = .15,
        morph = .25, color = .49, echo = .18, detune = -1200, fm = 1.10,
        lfo = .20, motion = .04,
        filter = {resonance = .94, depth = 1.49, velocity = .52, keytrack = .55,
                  attack = .004, decay = .14, sustain = .11, release = .10},
        shape = {{time = .006, level = 1}, {time = .084, level = 0}},
        mod = {{source = "shape", destination = "pitch", amount = 12},
               {source = "filter_env", destination = "morph", amount = .16}},
        scene = {mesh = "bell.obj", tint = 0xc6a47e, accent = 0xedbca2, spin = 0.55, tilt = -0.14, aspect = 1.13, satellites = 4}
    },
    {
        id = "foundry-tom", type = "percussion",
        name = "Foundry Tom", wave_a = "sine", wave_b = "triangle",
        attack = .001, decay = .28, sustain = .02, release = .16,
        morph = .21, color = .51, echo = .22, detune = 484, fm = 2.10,
        lfo = .77, motion = .10,
        filter = {resonance = 1.17, depth = 1.19, velocity = .64, keytrack = .53,
                  attack = .003, decay = .17, sustain = .10, release = .13},
        shape = {{time = .005, level = 1}, {time = .103, level = 0}},
        mod = {{source = "shape", destination = "pitch", amount = 18},
               {source = "filter_env", destination = "fm", amount = 1.02}},
        scene = {mesh = "anvil.obj", tint = 0xca7873, accent = 0xf5a976, spin = -0.44, tilt = -0.5, aspect = 1.11, satellites = 4}
    },
    {
        id = "tin-shaker", type = "percussion",
        name = "Tin Shaker", wave_a = "sine", wave_b = "noise",
        attack = .003, decay = .08, sustain = .04, release = .09,
        morph = .87, color = .73, echo = .16, detune = 0, fm = .63,
        lfo = 14, motion = .10,
        filter = {mode = "highpass", resonance = .83, depth = -.79, velocity = .27, keytrack = .18,
                  attack = .004, decay = .054, sustain = .08, release = .064},
        mod = {{source = "filter_env", destination = "morph", amount = .11},
               {source = "velocity", destination = "fm", amount = .33}},
        scene = {mesh = "petal.obj", tint = 0xa4b6bf, accent = 0xece2aa, spin = -1.47, tilt = -0.4, aspect = 1.16, satellites = 2}
    },
    {
        id = "ring-anvil", type = "percussion",
        name = "Ring Anvil", wave_a = "sine", wave_b = "square",
        attack = .001, decay = .45, sustain = .01, release = .52,
        morph = .23, color = .54, echo = .47, detune = 948, fm = 5.11,
        lfo = .48, motion = .07,
        filter = {mode = "highpass", resonance = 1.13, depth = -.89, velocity = .40, keytrack = .28,
                  attack = .002, decay = .27, sustain = .12, release = .31},
        shape = {{time = .005, level = 1}, {time = .049, level = 0}},
        mod = {{source = "shape", destination = "pitch", amount = 7},
               {source = "filter_env", destination = "fm", amount = 1.24}},
        scene = {mesh = "bell.obj", tint = 0xac8ab5, accent = 0xf5b7a0, spin = 0.52, tilt = 0.11, aspect = 0.96, satellites = 5}
    },

    -- Drones and sound effects
    {
        id = "aether-vents", type = "drones",
        name = "Aether Vents", wave_a = "sine", wave_b = "noise",
        attack = .72, decay = 1.70, sustain = .88, release = 3.6,
        morph = .32, color = .62, echo = .60, detune = 0, fm = 1.43,
        lfo = .52, motion = .37,
        sample = .77, sample_file = "orchard-air.wav",
        grain = {size = .21, density = 29, position = .40, spray = .20},
        filter = {mode = "highpass", resonance = 1.35, depth = -.87, keytrack = .45,
                  attack = .91, decay = 1.33, sustain = .7, release = 1.9},
        shape = {loop = true, {time = 1.7, level = .91}, {time = 2.3, level = -.70}},
        mod = {{source = "shape", destination = "grain_position", amount = .35},
               {source = "color", destination = "sample", amount = .19}},
        scene = {mesh = "helix.obj", tint = 0x7d91d7, accent = 0xb5c4f0, spin = -0.35, tilt = 0.25, aspect = 0.9, satellites = 1}
    },
    {
        id = "orbital-beacon", type = "drones",
        name = "Orbital Beacon", wave_a = "sine", wave_b = "square",
        attack = .095, decay = .64, sustain = .68, release = 1.08,
        morph = .18, color = .67, echo = .57, detune = 802, fm = 4.38,
        lfo = 5.30, motion = .42,
        filter = {resonance = 1.26, depth = 1.10, velocity = .49, keytrack = .63,
                  attack = .13, decay = .36, sustain = .47, release = .77},
        shape = {loop = true, {time = .10, level = .74}, {time = .21, level = -.83},
                 {time = .16, level = .17}},
        mod = {{source = "shape", destination = "pitch", amount = 3.2},
               {source = "lfo", destination = "fm", amount = .9}},
        scene = {mesh = "relay.obj", tint = 0xc49f57, accent = 0xf7de8d, spin = -0.9, tilt = -0.32, aspect = 1.18, satellites = 2}
    },
    {
        id = "hearth-static", type = "drones",
        name = "Hearth Static", wave_a = "triangle", wave_b = "noise",
        attack = .90, decay = 2.14, sustain = .83, release = 3.37,
        morph = .37, color = .71, echo = .49, detune = 0, fm = .15,
        lfo = .27, motion = .36,
        filter = {resonance = .93, depth = .81, velocity = .54, keytrack = .40,
                  attack = 1.04, decay = 1.60, sustain = .51, release = 2.41},
        shape = {loop = true, {time = 2.1, level = .63}, {time = 2.7, level = -.60}},
        sample = .54, sample_file = "orchard-air.wav",
        grain = {size = .19, density = 27, position = .48, spray = .23},
        mod = {{source = "shape", destination = "grain_position", amount = .33},
               {source = "lfo", destination = "sample", amount = .11}},
        scene = {mesh = "petal.obj", tint = 0xdb7968, accent = 0xf7b3a0, spin = -0.35, tilt = 0.34, aspect = 0.99, satellites = 1}
    },
    {
        id = "magnetic-rain", type = "drones",
        name = "Magnetic Rain", wave_a = "square", wave_b = "sine",
        attack = .35, decay = 1.57, sustain = .67, release = 2.60,
        morph = .55, color = .33, echo = .64, detune = 976, fm = 5.49,
        lfo = 3.38, motion = .53,
        filter = {mode = "highpass", resonance = .87, depth = -.79, velocity = .43, keytrack = .51,
                  attack = .45, decay = .95, sustain = .55, release = 1.64},
        shape = {loop = true, {time = .79, level = .87}, {time = 1.2, level = -.66},
                 {time = .61, level = .16}},
        sample = .40, sample_file = "filament.wav",
        grain = {size = .105, density = 39, position = .42, spray = .22},
        mod = {{source = "shape", destination = "grain_position", amount = .38},
               {source = "lfo", destination = "fm", amount = 1.31}},
        scene = {mesh = "cage.obj", tint = 0x8292cd, accent = 0xc7d9f6, spin = -0.8, tilt = 0.31, aspect = 1.0, satellites = 5}
    },
    {
        id = "void-siren", type = "drones",
        name = "Void Siren", wave_a = "sine", wave_b = "saw",
        attack = 1.10, decay = 1.42, sustain = .79, release = 4.41,
        morph = .43, color = .47, echo = .68, detune = 638, fm = 4.70,
        lfo = .64, motion = .36,
        filter = {resonance = 1.36, depth = .99, velocity = .41, keytrack = .36,
                  attack = 1.28, decay = 1.64, sustain = .53, release = 3.1},
        shape = {loop = true, {time = 1.8, level = -.65}, {time = 1.6, level = .96},
                 {time = 1.1, level = -.13}},
        mod = {{source = "shape", destination = "pitch", amount = 3.75},
               {source = "shape", destination = "fm", amount = 1.18}},
        scene = {mesh = "arch.obj", tint = 0x635fae, accent = 0xc08ae3, spin = 0.37, tilt = -0.04, aspect = 1.11, satellites = 3}
    },
    {
        id = "doppler-hull", type = "drones",
        name = "Doppler Hull", wave_a = "square", wave_b = "triangle",
        attack = .69, decay = 1.52, sustain = .84, release = 2.86,
        morph = .49, color = .43, echo = .56, detune = -608, fm = 3.08,
        lfo = .95, motion = .44,
        filter = {resonance = 1.48, depth = 1.20, velocity = .45, keytrack = .51,
                  attack = .82, decay = 1.24, sustain = .44, release = 2.11},
        shape = {loop = true, {time = .83, level = .66}, {time = 1.62, level = -.85},
                 {time = .95, level = .14}},
        mod = {{source = "shape", destination = "pitch", amount = 4.5},
               {source = "shape", destination = "cutoff", amount = .71}},
        scene = {mesh = "kite.obj", tint = 0x5e99bd, accent = 0xabcbf0, spin = 0.63, tilt = 0.43, aspect = 0.84, satellites = 5}
    },
    {
        id = "blacklight", type = "drones",
        name = "Blacklight", wave_a = "sine", wave_b = "triangle",
        attack = .24, decay = 1.11, sustain = .79, release = 2.75,
        morph = .42, color = .72, echo = .54, detune = 1200, fm = 5.36,
        lfo = .54, motion = .29,
        sample = .44, sample_file = "canopy.wav",
        grain = {size = .11, density = 40, position = .24, spray = .22},
        filter = {resonance = 1.16, depth = 1.12, keytrack = .48,
                  attack = .16, decay = .88, sustain = .36, release = 1.58},
        shape = {loop = true, {time = .58, level = .93}, {time = .89, level = -.62},
                 {time = .82, level = .29}},
        mod = {{source = "shape", destination = "grain_position", amount = .37},
               {source = "shape", destination = "fm", amount = 1.2},
               {source = "lfo", destination = "pitch", amount = .12}},
        scene = {mesh = "helix.obj", tint = 0xbc70ee, accent = 0xf6a4ef, spin = 0.62, tilt = 0.4, aspect = 0.99, satellites = 3}
    },
}

-- Voices define an instrument's envelope, cutoff and harmonic structure. The
-- treatments below give each voice a different role in a science-fiction or
-- fantasy score, with changes to timing, register, space and modulation.
-- Voice fields: name, oscillator A/B, attack, decay, sustain, release,
-- color, morph, FM, resonance, optional WAV source.
local families = {
    {"pads", "arch.obj", {101, 146, 179}, {
        {"Nebula Veil", "sine", "triangle", .92, 1.9, .76, 3.9, .49, .36, .72, .96, "orchard-air.wav"},
        {"Auric Canopy", "triangle", "saw", .42, 1.3, .82, 2.7, .57, .44, .38, 1.1},
        {"Frost Halo", "sine", "sine", 1.3, 2.1, .65, 4.2, .54, .19, 1.9, .83, "canopy.wav"},
        {"Aether Forest", "triangle", "noise", .77, 1.6, .79, 3.0, .47, .20, .22, .9, "orchard-air.wav"},
        {"Sunken Temple", "sine", "square", 1.1, 2.3, .73, 4.1, .36, .33, .77, 1.24},
        {"Ion Pillars", "saw", "triangle", .58, 1.2, .78, 2.9, .59, .52, .44, 1.25},
        {"Moonlit Moor", "sine", "noise", 1.36, 2.1, .81, 4.6, .51, .14, .32, .86, "orchard-air.wav"},
        {"Solar Bloom", "triangle", "sine", .71, 1.5, .74, 3.6, .62, .25, 1.55, 1.12, "filament.wav"},
        {"Crystal Wake", "triangle", "square", .33, 1.48, .62, 2.4, .58, .47, .47, 1.23},
        {"Mist Citadel", "sine", "saw", 1.15, 2.4, .71, 4.7, .45, .24, 1.2, 1.14, "orchard-air.wav"},
    }},
    {"leads", "kite.obj", {199, 117, 81}, {
        {"Photon Blade", "saw", "square", .009, .32, .71, .34, .61, .36, .39, 1.55},
        {"Wizard's Reed", "square", "triangle", .042, .46, .79, .39, .52, .46, .61, 1.13},
        {"Dawn Horn", "saw", "sine", .083, .48, .74, .46, .49, .18, 1.20, 1.39},
        {"Comet Whistle", "sine", "triangle", .021, .37, .86, .45, .73, .34, 1.45, .80},
        {"Rune Fanfare", "square", "saw", .013, .27, .72, .28, .53, .55, .28, 1.85},
        {"Star Forger", "triangle", "sine", .014, .31, .75, .57, .69, .40, 2.83, 1.17},
        {"Spectral Piper", "sine", "noise", .063, .64, .83, .45, .61, .12, .21, .89},
        {"Plasma Aria", "sine", "square", .028, .38, .78, .52, .65, .24, 3.01, 1.3},
        {"Dragon Flare", "saw", "saw", .037, .33, .76, .40, .58, .43, .63, 1.63},
        {"Tungsten Arrow", "triangle", "square", .007, .24, .67, .31, .72, .60, .94, 1.36},
    }},
    {"strings", "fan.obj", {157, 135, 189}, {
        {"Frost Viols", "triangle", "saw", .22, .86, .79, 1.37, .49, .46, .18, .86, "bowed-alloy.wav"},
        {"Ember Bows", "saw", "triangle", .13, .64, .81, 1.1, .57, .52, .38, 1.13},
        {"Orbital Cellos", "saw", "sine", .29, .86, .77, 1.9, .42, .64, .28, 1.01, "bowed-alloy.wav"},
        {"Siege Pizzicato", "saw", "triangle", .008, .18, .14, .31, .54, .31, .44, 1.1},
        {"Sable Quartet", "triangle", "saw", .17, .70, .75, 1.3, .55, .57, .22, 1.0},
        {"Windborne Violas", "triangle", "triangle", .31, 1.1, .80, 1.6, .63, .55, .09, .82, "bowed-alloy.wav"},
        {"Silver Tremolo", "saw", "square", .082, .57, .71, 1.0, .58, .31, .17, 1.25},
        {"Glass Bows", "sine", "triangle", .36, 1.15, .76, 2.2, .65, .25, 1.19, .9, "bowed-alloy.wav"},
        {"Rift Orchestra", "saw", "saw", .39, 1.3, .84, 2.1, .50, .51, .12, 1.18},
        {"Crown Spiccato", "triangle", "square", .013, .15, .20, .34, .58, .37, .38, 1.29},
    }},
    {"choir", "bell.obj", {119, 137, 185}, {
        {"Vault Aahs", "sine", "triangle", .49, 1.3, .76, 2.8, .32, .25, 1.24, .76, "choir-air.wav"},
        {"Oracle Oohs", "sine", "sine", .66, 1.4, .82, 3.1, .28, .38, 1.52, .80, "choir-air.wav"},
        {"Spirit Breaths", "triangle", "noise", .82, 1.5, .74, 3.8, .44, .19, .23, .78, "choir-air.wav"},
        {"Abbey Altos", "triangle", "sine", .41, 1.2, .77, 2.5, .37, .49, .87, .83, "choir-air.wav"},
        {"Moon Cantors", "sine", "square", 1.03, 1.9, .71, 4.2, .29, .23, 1.19, .86, "choir-air.wav"},
        {"Shade Chorists", "sine", "noise", .64, 1.7, .69, 3.3, .35, .09, .54, .74, "choir-air.wav"},
        {"Temple Tenors", "square", "sine", .30, .88, .78, 2.6, .31, .49, .31, .88, "choir-air.wav"},
        {"Star Hymnal", "sine", "triangle", .72, 1.5, .86, 4.4, .40, .42, 1.96, .90, "choir-air.wav"},
        {"Forest Choir", "triangle", "square", .52, 1.2, .79, 2.8, .33, .56, .52, .82, "choir-air.wav"},
        {"Ashen Vowels", "triangle", "sine", .24, 1.1, .71, 2.4, .37, .29, 1.57, .93, "choir-air.wav"},
    }},
    {"bass", "anvil.obj", {177, 85, 112}, {
        {"Siege Engine", "saw", "sine", .008, .34, .68, .24, .31, .50, .41, 2.48},
        {"Void Reactor", "square", "sine", .015, .54, .84, .37, .27, .66, .47, 1.4},
        {"Gravity Coil", "sine", "triangle", .019, .48, .79, .29, .32, .36, .72, 1.0},
        {"Coal Titan", "saw", "square", .004, .25, .55, .16, .39, .44, .33, 2.72},
        {"Hull Rumble", "triangle", "sine", .034, .55, .77, .40, .26, .56, 1.28, 1.39},
        {"Deep Forge", "square", "triangle", .006, .19, .60, .24, .38, .47, .82, 2.01},
        {"Crawler Unit", "triangle", "saw", .018, .37, .64, .27, .36, .37, 1.62, 2.2},
        {"Abyss Engine", "sine", "noise", .039, .65, .86, .41, .24, .105, .21, .94},
        {"Dread Runner", "saw", "saw", .003, .22, .63, .18, .40, .52, .39, 2.6},
        {"Obsidian Sub", "sine", "sine", .009, .46, .79, .23, .28, .61, .37, .81},
    }},
    {"keys", "cage.obj", {156, 172, 190}, {
        {"Relic Piano", "triangle", "saw", .008, .69, .33, .69, .61, .31, .55, .86, "canopy.wav"},
        {"Mirror Carillon", "sine", "sine", .002, 1.27, .12, 1.7, .65, .19, 3.09, 1.19, "filament.wav"},
        {"Ice Celesta", "triangle", "sine", .004, .67, .17, .95, .69, .49, 1.65, .91, "canopy.wav"},
        {"Aether Chimes", "sine", "square", .006, 1.37, .11, 1.8, .71, .18, 2.74, 1.15, "filament.wav"},
        {"Chronicle Organ", "triangle", "square", .013, .24, .91, .19, .55, .47, .21, .86},
        {"Fog Mallets", "triangle", "sine", .011, .37, .13, .40, .54, .30, .92, .92},
        {"Amber Tines", "sine", "triangle", .005, .83, .35, .61, .63, .27, 1.95, 1.06},
        {"Copper Gamelan", "sine", "square", .003, .65, .09, .82, .74, .20, 2.84, 1.16, "filament.wav"},
        {"Porcelain Console", "triangle", "sine", .017, .89, .42, .74, .58, .55, .71, .88, "canopy.wav"},
        {"Astrarium Bells", "sine", "sine", .004, 1.46, .07, 1.99, .72, .24, 3.22, 1.19, "canopy.wav"},
    }},
    {"plucks", "petal.obj", {133, 166, 119}, {
        {"Warden's Lute", "triangle", "square", .004, .22, .16, .40, .51, .38, .43, 1.11},
        {"Ion Harp", "sine", "triangle", .003, .32, .11, .70, .62, .25, 1.65, 1.16, "filament.wav"},
        {"Grove Koto", "triangle", "saw", .003, .26, .13, .39, .64, .45, .82, 1.31},
        {"Quartz Plectrum", "sine", "square", .002, .23, .10, .44, .51, .18, 1.77, 1.08},
        {"Starlit Dulcimer", "triangle", "sine", .006, .41, .17, .62, .62, .44, .75, 1.13, "canopy.wav"},
        {"Vault Mandolin", "square", "triangle", .002, .18, .13, .33, .56, .54, .46, 1.34},
        {"Spellbound Zither", "sine", "triangle", .007, .36, .19, .58, .53, .32, 1.12, 1.05, "filament.wav"},
        {"Silver Spindle", "triangle", "noise", .005, .22, .14, .34, .55, .12, .36, 1.11},
        {"Reef Pick", "square", "sine", .003, .16, .12, .26, .66, .28, .55, 1.30},
        {"Sundial Lyre", "saw", "sine", .008, .36, .20, .54, .48, .48, .71, 1.27},
    }},
    {"rhythms", "relay.obj", {114, 173, 168}, {
        {"Time Relay", "square", "sine", .032, .63, .69, .58, .46, .48, .84, 1.54},
        {"Glyph Sequence", "triangle", "saw", .044, .89, .70, .98, .49, .35, .38, 1.47},
        {"Flux Lattice", "sine", "saw", .084, 1.1, .77, 1.4, .46, .42, 2.13, 1.38, "filament.wav"},
        {"War Drum Grid", "saw", "square", .018, .64, .65, .44, .37, .42, .44, 2.05},
        {"Sentinel Pulse", "square", "triangle", .056, .90, .72, 1.1, .48, .53, .33, 1.71},
        {"Frost Gate", "triangle", "noise", .14, .77, .72, 1.5, .50, .26, .53, 1.29, "orchard-air.wav"},
        {"Beacon Code", "sine", "square", .018, .61, .65, .52, .54, .23, 2.08, 1.51},
        {"Clockwork Tide", "saw", "triangle", .043, .78, .68, .73, .47, .51, .45, 1.84},
        {"Starfall Stutter", "sine", "triangle", .060, .84, .78, 1.0, .48, .33, 2.61, 1.62, "filament.wav"},
        {"Rampart Current", "square", "saw", .093, 1.07, .71, .93, .41, .49, .56, 1.88},
    }},
    {"percussion", "prism.obj", {184, 145, 106}, {
        {"Titan Kick", "sine", "triangle", .002, .18, .013, .11, .26, .37, 1.05, 1.20},
        {"Foundry Snare", "triangle", "noise", .003, .14, .023, .13, .56, .81, .34, .87},
        {"Stardust Hat", "square", "noise", .001, .07, .012, .058, .66, .91, .26, .76},
        {"Monolith Tom", "sine", "triangle", .002, .26, .019, .17, .39, .24, 1.72, 1.16},
        {"Relic Gong", "sine", "square", .005, .58, .025, .42, .51, .27, 3.64, .95, "filament.wav"},
        {"Rune Clack", "square", "triangle", .002, .095, .013, .10, .54, .36, 1.16, 1.11},
        {"Siege Frame Drum", "sine", "noise", .003, .23, .023, .14, .35, .22, .91, 1.13},
        {"Silver Shaker", "sine", "noise", .006, .095, .032, .078, .63, .87, .35, .79},
        {"Hull Rim", "square", "sine", .001, .10, .014, .16, .54, .28, 2.07, 1.19},
        {"Crucible Crash", "square", "noise", .002, .47, .03, .35, .57, .67, .65, .98, "filament.wav"},
    }},
    {"drones", "helix.obj", {109, 133, 200}, {
        {"Alien Beacon", "sine", "square", .21, .82, .72, 1.4, .53, .19, 3.01, 1.25},
        {"Glass Singularity", "sine", "triangle", .92, 1.7, .78, 3.1, .59, .29, 4.45, 1.19, "filament.wav"},
        {"Specter Engine", "square", "saw", .67, 1.4, .75, 2.9, .46, .44, 2.57, 1.66},
        {"Witchwind", "sine", "noise", 1.1, 2.1, .84, 4.2, .45, .32, 1.34, .95, "orchard-air.wav"},
        {"Vault Alarm", "saw", "sine", .18, .83, .71, 1.6, .52, .42, 3.85, 1.43},
        {"Eclipse Static", "triangle", "noise", 1.2, 2.2, .79, 4.4, .59, .26, .87, .8, "orchard-air.wav"},
        {"Mirror Rift", "sine", "square", .74, 1.6, .74, 3.7, .47, .29, 4.85, 1.27, "bowed-alloy.wav"},
        {"Gate Descent", "square", "triangle", .85, 1.8, .82, 3.1, .39, .52, 2.51, 1.63},
        {"Siren of Glass", "sine", "saw", 1.23, 2.1, .73, 4.1, .46, .31, 4.22, 1.42, "filament.wav"},
        {"Sunken Radio", "triangle", "noise", .56, 1.4, .78, 2.9, .42, .33, 1.5, 1.0, "orchard-air.wav"},
    }},
}

-- Prefix, speed, brightness offset, resonance multiplier, echo, pitch color,
-- motion rate, pitch motion, texture position and color accent.
local treatments = {
    {"Orbital",   1.12, .03, .93, .52,  9, .82, .16, .32, 0xa8dbfa},
    {"Runic",      .83, .05, 1.21, .43, 23, 1.4, .42, .25, 0xf3c987},
    {"Colossal",  1.45,-.06, 1.13, .39,-1200, .64, .18, .55, 0xf1a388},
    {"Fractured",  .65,-.01, 1.19, .34,-31, 2.6, .31, .22, 0xbdadf1},
    {"Luminous",  1.31, .08, .89, .65, 1200, .55, .10, .62, 0xdcefb0},
    {"Obscured",  1.16,-.09, 1.09, .47,-16, .67, .21, .49, 0xabbedc},
    {"Hallowed",  1.56, .015,.83, .71,  7, .47, .12, .68, 0xf4d8ad},
    {"Tempest",    .76, .06, 1.33, .43, 41, 3.4, .46, .37, 0x8fe2dc},
    {"Farshore",  1.68,-.025,.96, .72, 15, .39, .15, .77, 0xbdc7fa},
    {"Scorched",   .70,-.035,1.26, .26,-27, 1.8, .29, .18, 0xf1ac72},
}

local function bound(value, lower, upper)
    return math.min(upper, math.max(lower, value))
end

local function cinematic(family, voice_index, treatment_index, family_index)
    local kind, mesh, color, voices = family[1], family[2], family[3], family[4]
    local v = voices[voice_index]
    local t = treatments[treatment_index]
    local atmosphere = kind == "pads" or kind == "drones"
    local voice = kind == "choir"
    local bowed = kind == "strings"
    local bass = kind == "bass"
    local rhythm = kind == "rhythms"
    local drum = kind == "percussion"
    local transient = drum or kind == "plucks" or (bowed and v[4] < .02)
    local slow = atmosphere or voice or (bowed and not transient)
    local time = t[2]
    local cutoff = bound(v[8] + t[3], .21, .82)
    local mode = "lowpass"
    if voice and (treatment_index == 2 or treatment_index == 6 or treatment_index == 8) then
        mode = "bandpass"
        cutoff = bound(cutoff, .24, .32)
    elseif drum and v[3] == "noise" and voice_index ~= 7 then
        mode = "highpass"
    end
    local attack = bound(v[4] * time, .001, 2.5)
    local decay = v[5] * time
    local release = v[7] * time
    local motion = t[7]
    local fm_amount = (0.18 + .035 * treatment_index) * (drum and .75 or 1)
    local pitch_wander = t[8] * ((bowed or kind == "leads") and .8 or .32)
    local detune = bass and -1200 or t[6]
    if voice or bowed then detune = bound(detune, -29, 29) end
    if drum and voice_index < 5 then detune = voice_index == 2 and 0 or -1200 end

    local red = bound(color[1] + 4 * voice_index - 2 * treatment_index, 72, 221)
    local green = bound(color[2] + 3 * treatment_index - 2 * voice_index, 62, 224)
    local blue = bound(color[3] + (voice_index * 11 + treatment_index * 7) % 39 - 18, 69, 230)
    local patch = {
        id = "score-" .. kind .. "-" .. voice_index .. "-" .. treatment_index,
        name = t[1] .. " " .. v[1], type = kind,
        wave_a = v[2], wave_b = v[3], attack = attack, decay = decay,
        sustain = v[6], release = release, color = cutoff,
        morph = bound(v[9] + (treatment_index - 5) * .011, .07, .92),
        echo = t[5] * (bass and .40 or drum and .46 or transient and .72 or 1),
        detune = detune,
        fm = v[10] * (.86 + treatment_index * .044),
        lfo = motion * (rhythm and 2 or slow and .38 or 1.18),
        motion = rhythm and .32 or drum and .06 or slow and .14 or .10,
        filter = {
            mode = mode,
            resonance = bound(v[11] * t[4], .64, 3.8),
            depth = mode == "highpass" and -.64 or bass and 1.72 or transient and 1.18 or .77,
            velocity = transient and .92 or .54, keytrack = voice and .88 or .61,
            attack = attack * (slow and 1.36 or .66), decay = decay * .67,
            sustain = mode == "highpass" and .12 or slow and .47 or .22,
            release = release * .81
        },
        mod = {
            {source = "velocity", destination = "fm", amount = fm_amount},
            {source = "lfo", destination = slow and "cutoff" or "pitch",
             amount = slow and .32 or pitch_wander},
        },
        scene = {
            mesh = mesh, tint = red * 65536 + green * 256 + blue, accent = t[10],
            spin = .18 + motion * .28 + voice_index * .019,
            tilt = (treatment_index - 5) * .076, aspect = .85 + voice_index * .032,
            satellites = (family_index + voice_index + treatment_index) % 5 + 1
        }
    }

    if rhythm then
        patch.shape = {loop = true, {time = .053 * time, level = .85},
                       {time = .17 * time, level = -.80}, {time = .085 * time, level = .37},
                       {time = .19 * time, level = -.62}}
        patch.mod[3] = {source = "shape", destination = "cutoff", amount = 1.33}
        patch.mod[4] = {source = "shape", destination = "morph", amount = .23}
    elseif atmosphere or (voice and treatment_index % 2 == 0) then
        patch.shape = {loop = true, {time = 1.45 * time, level = .64},
                       {time = 2.28 * time, level = -.66}, {time = 1.06 * time, level = .17}}
        patch.mod[3] = {source = "shape", destination = "morph", amount = .19}
        if kind == "drones" then
            patch.mod[4] = {source = "shape", destination = "pitch", amount = .65 + .22 * treatment_index}
        end
    elseif drum and (voice_index == 1 or voice_index == 4 or voice_index == 7) then
        patch.shape = {{time = .005, level = .94}, {time = .053 * time, level = 0}}
        patch.mod[3] = {source = "shape", destination = "pitch", amount = 9 + treatment_index}
    elseif transient or kind == "keys" then
        patch.mod[3] = {source = "filter_env", destination = "fm", amount = .48 + .057 * voice_index}
    end

    if v[12] then
        patch.sample_file = v[12]
        patch.sample = voice and .39 or atmosphere and .31 or drum and .19 or .24
        if voice or atmosphere or rhythm or ((bowed or kind == "keys") and treatment_index % 3 == 1) then
            patch.grain = {size = bound(.12 * time, .065, .23),
                           density = 20 + treatment_index + 2 * (voice_index % 4),
                           position = t[9], spray = .09 + .012 * voice_index}
            patch.mod[#patch.mod + 1] = {source = patch.shape and "shape" or "lfo",
                                        destination = "grain_position", amount = .20}
        end
    end
    return patch
end

local counts = {}
for i = 1, #patches do
    local kind = patches[i].type
    counts[kind] = (counts[kind] or 0) + 1
end
for family_index = 1, #families do
    local family = families[family_index]
    local missing = 100 - (counts[family[1]] or 0)
    for recipe = 0, missing - 1 do
        local pair = (recipe * 37) % 100
        local voice_index = pair % #family[4] + 1
        local treatment_index = math.floor(pair / #family[4]) + 1
        patches[#patches + 1] = cinematic(family, voice_index, treatment_index, family_index)
    end
end
return patches
