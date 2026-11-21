-- Edit or add a patch table, then press R in the desktop app or refresh the web page.
-- Use a WAV filename under assets/sounds to layer a sample tuned to C4.
return {
    {
        name = "Glass Canopy", wave_a = "triangle", wave_b = "sine",
        attack = .085, decay = .55, sustain = .65, release = 1.4,
        morph = .36, color = .64, echo = .43, detune = 9, fm = .45,
        lfo = .27, motion = .19, sample = .26, sample_file = "canopy.wav",
        scene = {mesh = "prism.obj", tint = 0x8eb861, accent = 0xcce77e, spin = 0.48, tilt = 0.17, aspect = 1, satellites = 3}
    },
    {
        name = "Afterimage", wave_a = "sine", wave_b = "saw",
        attack = .36, decay = .91, sustain = .67, release = 2.5,
        morph = .27, color = .49, echo = .67, detune = 13, fm = 1.5,
        lfo = .12, motion = .26,
        scene = {mesh = "helix.obj", tint = 0x9a78e5, accent = 0x63b9ff, spin = -0.29, tilt = 0.1, aspect = 1.1, satellites = 2}
    },
    {
        name = "Copper Moth", wave_a = "square", wave_b = "triangle",
        attack = .007, decay = .18, sustain = .28, release = .27,
        morph = .44, color = .54, echo = .22, detune = -15, fm = .3,
        lfo = 1.7, motion = .28,
        scene = {mesh = "fan.obj", tint = 0xe48a52, accent = 0xf9ca83, spin = 0.6, tilt = -0.14, aspect = 1.18, satellites = 3}
    },
    {
        name = "Night Orchard", wave_a = "sine", wave_b = "noise",
        attack = .48, decay = .85, sustain = .75, release = 3,
        morph = .19, color = .34, echo = .62, detune = -7, fm = .8,
        lfo = .21, motion = .15,
        scene = {mesh = "arch.obj", tint = 0x725cba, accent = 0xb2e590, spin = 0.4, tilt = 0.49, aspect = 1.15, satellites = 1}
    },
    {
        name = "Vesper Bass", wave_a = "saw", wave_b = "sine",
        attack = .006, decay = .24, sustain = .65, release = .22,
        morph = .54, color = .43, echo = .13, detune = -1200, fm = .18,
        lfo = .8, motion = .1,
        scene = {mesh = "anvil.obj", tint = 0xb74b73, accent = 0xf99075, spin = 0.66, tilt = -0.05, aspect = 1.04, satellites = 1}
    },
    -- Evolving pads
    {
        name = "Aerial Fern", wave_a = "triangle", wave_b = "saw",
        attack = .44, decay = 1.28, sustain = .74, release = 2.7,
        morph = .27, color = .68, echo = .44, detune = 13, fm = .28,
        lfo = .19, motion = .23,
        scene = {mesh = "petal.obj", tint = 0x85cf85, accent = 0xddf398, spin = -0.3, tilt = -0.53, aspect = 0.85, satellites = 5}
    },
    {
        name = "Lumen Drift", wave_a = "sine", wave_b = "triangle",
        attack = 1.18, decay = 2.30, sustain = .88, release = 3.6,
        morph = .47, color = .51, echo = .55, detune = -21, fm = .62,
        lfo = .08, motion = .20,
        scene = {mesh = "prism.obj", tint = 0x66baed, accent = 0xf3de99, spin = 0.31, tilt = -0.25, aspect = 1.06, satellites = 4}
    },
    {
        name = "Marsh Lanterns", wave_a = "sine", wave_b = "square",
        attack = .78, decay = 1.60, sustain = .65, release = 2.6,
        morph = .21, color = .39, echo = .63, detune = 8, fm = .36,
        lfo = .22, motion = .19,
        scene = {mesh = "relay.obj", tint = 0xa3b759, accent = 0xffd17f, spin = 0.41, tilt = 0.3, aspect = 0.92, satellites = 2}
    },
    {
        name = "Silver Murmur", wave_a = "triangle", wave_b = "noise",
        attack = .83, decay = 1.85, sustain = .75, release = 2.2,
        morph = .17, color = .58, echo = .35, detune = 4, fm = .82,
        lfo = .34, motion = .20,
        scene = {mesh = "fan.obj", tint = 0xaebdc8, accent = 0xe9c5f4, spin = 0.32, tilt = -0.34, aspect = 0.88, satellites = 5}
    },
    {
        name = "Frost Arch", wave_a = "saw", wave_b = "sine",
        attack = 1.42, decay = 1.67, sustain = .78, release = 4.3,
        morph = .62, color = .45, echo = .49, detune = 17, fm = .93,
        lfo = .10, motion = .16,
        scene = {mesh = "arch.obj", tint = 0x679ac7, accent = 0xdaf3ff, spin = -0.41, tilt = -0.5, aspect = 0.96, satellites = 1}
    },
    {
        name = "Auburn Tide", wave_a = "triangle", wave_b = "square",
        attack = .62, decay = 2.10, sustain = .70, release = 2.8,
        morph = .33, color = .59, echo = .41, detune = -16, fm = .17,
        lfo = .28, motion = .26,
        scene = {mesh = "helix.obj", tint = 0xc8756e, accent = 0xffd37d, spin = 0.38, tilt = -0.4, aspect = 1.11, satellites = 4}
    },
    {
        name = "Distant Arches", wave_a = "sine", wave_b = "saw",
        attack = 1.51, decay = 2.50, sustain = .57, release = 4.8,
        morph = .31, color = .37, echo = .77, detune = -11, fm = 1.15,
        lfo = .14, motion = .31,
        scene = {mesh = "relay.obj", tint = 0x8974c0, accent = 0xa9c5f4, spin = 0.5, tilt = 0.1, aspect = 0.96, satellites = 3}
    },
    {
        name = "Lichen Bloom", wave_a = "sine", wave_b = "triangle",
        attack = .31, decay = 1.70, sustain = .80, release = 2.5,
        morph = .66, color = .61, echo = .42, detune = 27, fm = .71,
        lfo = .46, motion = .29,
        scene = {mesh = "petal.obj", tint = 0x80b45d, accent = 0xf1bfe3, spin = 0.33, tilt = -0.24, aspect = 0.97, satellites = 4}
    },
    {
        name = "Polar Terrace", wave_a = "square", wave_b = "sine",
        attack = .91, decay = 2.40, sustain = .70, release = 3.9,
        morph = .68, color = .33, echo = .59, detune = -1200, fm = .29,
        lfo = .12, motion = .18,
        scene = {mesh = "anvil.obj", tint = 0x628da8, accent = 0xbee9f0, spin = 0.42, tilt = 0.24, aspect = 0.98, satellites = 4}
    },
    {
        name = "Opaline Wake", wave_a = "triangle", wave_b = "sine",
        attack = .56, decay = 1.33, sustain = .85, release = 2.1,
        morph = .43, color = .70, echo = .52, detune = -7, fm = 1.55,
        lfo = .24, motion = .34, sample = .15, sample_file = "canopy.wav",
        scene = {mesh = "prism.obj", tint = 0x89d4c9, accent = 0xf5b4e2, spin = 0.63, tilt = -0.11, aspect = 0.85, satellites = 2}
    },
    {
        name = "Ember Veil", wave_a = "saw", wave_b = "triangle",
        attack = .68, decay = 1.54, sustain = .59, release = 3.0,
        morph = .53, color = .52, echo = .33, detune = 19, fm = .44,
        lfo = .37, motion = .16,
        scene = {mesh = "spire.obj", tint = 0xde7953, accent = 0xfccf86, spin = 0.54, tilt = -0.47, aspect = 1.11, satellites = 4}
    },
    {
        name = "Moon Reservoir", wave_a = "sine", wave_b = "noise",
        attack = 1.68, decay = 2.00, sustain = .76, release = 5.2,
        morph = .09, color = .63, echo = .71, detune = 1200, fm = .19,
        lfo = .06, motion = .08,
        scene = {mesh = "arch.obj", tint = 0x546f9e, accent = 0xd0b3f0, spin = -0.59, tilt = 0.32, aspect = 1.04, satellites = 5}
    },

    -- Strings and voices
    {
        name = "Amber Ensemble", wave_a = "saw", wave_b = "saw",
        attack = .28, decay = 1.20, sustain = .84, release = 1.60,
        morph = .46, color = .58, echo = .28, detune = 12, fm = .08,
        lfo = .66, motion = .21,
        scene = {mesh = "fan.obj", tint = 0xce9a57, accent = 0xf5d99b, spin = 0.45, tilt = -0.51, aspect = 1.05, satellites = 4}
    },
    {
        name = "Birch Quartet", wave_a = "triangle", wave_b = "saw",
        attack = .19, decay = .93, sustain = .79, release = 1.1,
        morph = .55, color = .64, echo = .20, detune = -9, fm = .34,
        lfo = .87, motion = .13,
        scene = {mesh = "kite.obj", tint = 0xaeb384, accent = 0xe0e3b1, spin = 0.48, tilt = 0.32, aspect = 0.95, satellites = 1}
    },
    {
        name = "Hollow Choir", wave_a = "sine", wave_b = "square",
        attack = .47, decay = 1.71, sustain = .77, release = 2.7,
        morph = .21, color = .47, echo = .52, detune = 7, fm = 1.13,
        lfo = .39, motion = .11,
        scene = {mesh = "arch.obj", tint = 0xa08ac6, accent = 0xd1a2be, spin = 0.55, tilt = 0.23, aspect = 1.04, satellites = 5}
    },
    {
        name = "Violet Octaves", wave_a = "saw", wave_b = "triangle",
        attack = .39, decay = 1.13, sustain = .83, release = 1.65,
        morph = .59, color = .55, echo = .37, detune = 1200, fm = .10,
        lfo = .55, motion = .10,
        scene = {mesh = "helix.obj", tint = 0x8962ce, accent = 0xdbacfb, spin = 0.57, tilt = 0.01, aspect = 0.97, satellites = 3}
    },
    {
        name = "Silk Bow", wave_a = "triangle", wave_b = "triangle",
        attack = .16, decay = .84, sustain = .81, release = 1.05,
        morph = .51, color = .72, echo = .22, detune = 7, fm = .05,
        lfo = 1.18, motion = .29,
        scene = {mesh = "petal.obj", tint = 0xdf95ab, accent = 0xf7d7a0, spin = 0.56, tilt = 0.25, aspect = 1.09, satellites = 4}
    },
    {
        name = "Grove Vox", wave_a = "sine", wave_b = "triangle",
        attack = .52, decay = 1.31, sustain = .73, release = 2.75,
        morph = .35, color = .53, echo = .43, detune = 18, fm = 2.14,
        lfo = .58, motion = .22,
        scene = {mesh = "bell.obj", tint = 0x769b68, accent = 0xc8db9b, spin = -0.52, tilt = -0.02, aspect = 1.06, satellites = 4}
    },
    {
        name = "Reed Chamber", wave_a = "square", wave_b = "triangle",
        attack = .09, decay = .54, sustain = .75, release = .86,
        morph = .69, color = .49, echo = .19, detune = -4, fm = .41,
        lfo = 1.25, motion = .11,
        scene = {mesh = "spire.obj", tint = 0xc3ab56, accent = 0xe1e9a5, spin = -0.55, tilt = -0.36, aspect = 0.85, satellites = 4}
    },
    {
        name = "Hushed Altos", wave_a = "triangle", wave_b = "noise",
        attack = .66, decay = 1.60, sustain = .76, release = 2.62,
        morph = .07, color = .64, echo = .30, detune = 0, fm = .36,
        lfo = .49, motion = .13,
        scene = {mesh = "fan.obj", tint = 0x9c8ebd, accent = 0xe0bdd9, spin = -0.55, tilt = -0.52, aspect = 1.09, satellites = 4}
    },
    {
        name = "Winter Violins", wave_a = "saw", wave_b = "square",
        attack = .38, decay = 1.47, sustain = .78, release = 1.64,
        morph = .23, color = .67, echo = .36, detune = 11, fm = .15,
        lfo = .79, motion = .29,
        scene = {mesh = "kite.obj", tint = 0x79a8d4, accent = 0xf4e7c9, spin = 0.66, tilt = -0.37, aspect = 1.0, satellites = 3}
    },
    {
        name = "Ghost Chorale", wave_a = "sine", wave_b = "sine",
        attack = .90, decay = 1.75, sustain = .82, release = 3.36,
        morph = .31, color = .56, echo = .64, detune = 1200, fm = 2.65,
        lfo = .19, motion = .32,
        scene = {mesh = "relay.obj", tint = 0x829ace, accent = 0xdfbce8, spin = 0.35, tilt = -0.17, aspect = 1.03, satellites = 4}
    },

    -- Keys and bells
    {
        name = "Porcelain Keys", wave_a = "triangle", wave_b = "sine",
        attack = .007, decay = .65, sustain = .32, release = .64,
        morph = .59, color = .74, echo = .16, detune = 1200, fm = .64,
        lfo = .46, motion = .08, sample = .24, sample_file = "canopy.wav",
        scene = {mesh = "bell.obj", tint = 0xc2d8d8, accent = 0xf6cea8, spin = 0.53, tilt = -0.34, aspect = 0.9, satellites = 5}
    },
    {
        name = "Smoked EP", wave_a = "sine", wave_b = "sine",
        attack = .005, decay = .91, sustain = .40, release = .53,
        morph = .17, color = .58, echo = .22, detune = 1200, fm = 1.33,
        lfo = .89, motion = .08,
        scene = {mesh = "anvil.obj", tint = 0xa27a73, accent = 0xe0b68a, spin = 0.62, tilt = 0.06, aspect = 0.93, satellites = 2}
    },
    {
        name = "Mirror Tines", wave_a = "sine", wave_b = "triangle",
        attack = .003, decay = .56, sustain = .24, release = .86,
        morph = .23, color = .78, echo = .30, detune = 973, fm = 2.48,
        lfo = .16, motion = .07,
        scene = {mesh = "spire.obj", tint = 0xa3bbdc, accent = 0xe1edff, spin = -0.48, tilt = -0.53, aspect = 0.88, satellites = 3}
    },
    {
        name = "Brass Celesta", wave_a = "sine", wave_b = "square",
        attack = .004, decay = 1.06, sustain = .08, release = 1.40,
        morph = .10, color = .73, echo = .41, detune = 1200, fm = 1.70,
        lfo = .35, motion = .11, sample = .31, sample_file = "canopy.wav",
        scene = {mesh = "bell.obj", tint = 0xd3a14c, accent = 0xffdba1, spin = 0.52, tilt = -0.5, aspect = 0.88, satellites = 1}
    },
    {
        name = "Orbit Bells", wave_a = "sine", wave_b = "sine",
        attack = .002, decay = 1.38, sustain = .04, release = 2.30,
        morph = .13, color = .82, echo = .52, detune = 777, fm = 3.45,
        lfo = .13, motion = .05,
        scene = {mesh = "relay.obj", tint = 0x7a92dd, accent = 0xd0c3fa, spin = 0.38, tilt = 0.04, aspect = 1.02, satellites = 2}
    },
    {
        name = "Moss Organ", wave_a = "triangle", wave_b = "square",
        attack = .016, decay = .21, sustain = .91, release = .11,
        morph = .43, color = .54, echo = .18, detune = 1200, fm = .15,
        lfo = 2.10, motion = .19,
        scene = {mesh = "cage.obj", tint = 0x79964e, accent = 0xd3cb84, spin = 0.66, tilt = 0.3, aspect = 0.92, satellites = 4}
    },
    {
        name = "Prism Mallets", wave_a = "triangle", wave_b = "sine",
        attack = .002, decay = .33, sustain = .06, release = .43,
        morph = .47, color = .67, echo = .21, detune = 1200, fm = 1.76,
        lfo = .25, motion = .06, sample = .16, sample_file = "canopy.wav",
        scene = {mesh = "prism.obj", tint = 0x74bed4, accent = 0xffb8a6, spin = 0.47, tilt = -0.47, aspect = 0.89, satellites = 5}
    },
    {
        name = "Opal Wires", wave_a = "sine", wave_b = "sine",
        attack = .004, decay = 1.20, sustain = .22, release = 1.08,
        morph = .25, color = .76, echo = .32, detune = 1104, fm = 2.89,
        lfo = .64, motion = .20,
        scene = {mesh = "helix.obj", tint = 0x72d4ba, accent = 0xe4bbec, spin = -0.38, tilt = 0.25, aspect = 1.08, satellites = 1}
    },
    {
        name = "Rain on Glass", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .73, sustain = .13, release = 1.40,
        morph = .66, color = .79, echo = .49, detune = 608, fm = 1.32,
        lfo = .29, motion = .12, sample = .35, sample_file = "canopy.wav",
        scene = {mesh = "kite.obj", tint = 0x74a4db, accent = 0xc2f1f4, spin = 0.54, tilt = -0.33, aspect = 1.03, satellites = 5}
    },
    {
        name = "Slate Piano", wave_a = "triangle", wave_b = "saw",
        attack = .008, decay = .78, sustain = .41, release = .41,
        morph = .28, color = .63, echo = .26, detune = 17, fm = .54,
        lfo = .18, motion = .09, sample = .19, sample_file = "canopy.wav",
        scene = {mesh = "anvil.obj", tint = 0x8498ae, accent = 0xf4c6b8, spin = 0.59, tilt = 0.28, aspect = 1.1, satellites = 4}
    },
    {
        name = "Tin Music Box", wave_a = "square", wave_b = "sine",
        attack = .002, decay = .52, sustain = .10, release = .62,
        morph = .71, color = .82, echo = .33, detune = 1200, fm = .91,
        lfo = 1.60, motion = .12,
        scene = {mesh = "cage.obj", tint = 0x9eafbf, accent = 0xecc685, spin = -0.72, tilt = -0.27, aspect = 1.01, satellites = 1}
    },
    {
        name = "Autumn Chimes", wave_a = "sine", wave_b = "triangle",
        attack = .009, decay = 1.88, sustain = .05, release = 2.44,
        morph = .31, color = .69, echo = .67, detune = 702, fm = 3.30,
        lfo = .23, motion = .22, sample = .18, sample_file = "canopy.wav",
        scene = {mesh = "bell.obj", tint = 0xdf9956, accent = 0xe66d56, spin = 0.56, tilt = -0.45, aspect = 1.14, satellites = 3}
    },

    -- Plucks
    {
        name = "Fern Mandolin", wave_a = "triangle", wave_b = "square",
        attack = .002, decay = .22, sustain = .15, release = .33,
        morph = .35, color = .72, echo = .23, detune = 16, fm = .37,
        lfo = .45, motion = .14,
        scene = {mesh = "petal.obj", tint = 0x83b84d, accent = 0xe8cf71, spin = 0.65, tilt = -0.42, aspect = 1.09, satellites = 2}
    },
    {
        name = "Rivet Harp", wave_a = "sine", wave_b = "saw",
        attack = .004, decay = .37, sustain = .12, release = .68,
        morph = .27, color = .61, echo = .31, detune = 1200, fm = 1.61,
        lfo = .31, motion = .07,
        scene = {mesh = "cage.obj", tint = 0xbb8d76, accent = 0xeee3a3, spin = 0.34, tilt = 0.29, aspect = 0.97, satellites = 2}
    },
    {
        name = "Satin Pick", wave_a = "triangle", wave_b = "sine",
        attack = .001, decay = .30, sustain = .16, release = .29,
        morph = .48, color = .69, echo = .14, detune = -11, fm = .60,
        lfo = .52, motion = .12, sample = .12, sample_file = "canopy.wav",
        scene = {mesh = "kite.obj", tint = 0xd68eab, accent = 0xf6cdbc, spin = 0.5, tilt = 0.0, aspect = 1.12, satellites = 3}
    },
    {
        name = "Reed Lute", wave_a = "square", wave_b = "triangle",
        attack = .003, decay = .32, sustain = .09, release = .35,
        morph = .46, color = .55, echo = .26, detune = -8, fm = .21,
        lfo = .82, motion = .13,
        scene = {mesh = "fan.obj", tint = 0xb1af67, accent = 0xe9c883, spin = -0.59, tilt = -0.25, aspect = 0.84, satellites = 2}
    },
    {
        name = "Quartz Droplet", wave_a = "sine", wave_b = "triangle",
        attack = .005, decay = .27, sustain = .07, release = .90,
        morph = .20, color = .89, echo = .60, detune = 690, fm = 2.22,
        lfo = .48, motion = .16,
        scene = {mesh = "spire.obj", tint = 0x73cde7, accent = 0xb8eef5, spin = -0.51, tilt = 0.17, aspect = 1.07, satellites = 4}
    },
    {
        name = "Sundial Strings", wave_a = "saw", wave_b = "sine",
        attack = .004, decay = .48, sustain = .17, release = .46,
        morph = .56, color = .55, echo = .29, detune = 12, fm = .48,
        lfo = .27, motion = .19,
        scene = {mesh = "relay.obj", tint = 0xc8884b, accent = 0xf8b568, spin = 0.31, tilt = 0.47, aspect = 1.07, satellites = 1}
    },
    {
        name = "Spruce Koto", wave_a = "triangle", wave_b = "saw",
        attack = .002, decay = .45, sustain = .10, release = .57,
        morph = .26, color = .78, echo = .29, detune = -22, fm = 1.12,
        lfo = .56, motion = .15, sample = .15, sample_file = "canopy.wav",
        scene = {mesh = "petal.obj", tint = 0x599f78, accent = 0xd0d693, spin = 0.61, tilt = -0.19, aspect = 1.05, satellites = 2}
    },
    {
        name = "Coral Pop", wave_a = "square", wave_b = "sine",
        attack = .003, decay = .14, sustain = .12, release = .21,
        morph = .32, color = .73, echo = .27, detune = 1200, fm = .33,
        lfo = 1.20, motion = .13,
        scene = {mesh = "prism.obj", tint = 0xeb857f, accent = 0xffd36d, spin = 0.5, tilt = -0.15, aspect = 1.05, satellites = 3}
    },
    {
        name = "Raindrop Zither", wave_a = "sine", wave_b = "square",
        attack = .003, decay = .42, sustain = .13, release = .77,
        morph = .17, color = .76, echo = .49, detune = 841, fm = 1.87,
        lfo = .73, motion = .22, sample = .26, sample_file = "canopy.wav",
        scene = {mesh = "helix.obj", tint = 0x61b3d1, accent = 0xe4bdeb, spin = -0.4, tilt = -0.09, aspect = 1.17, satellites = 4}
    },
    {
        name = "Charcoal Plectrum", wave_a = "triangle", wave_b = "noise",
        attack = .001, decay = .28, sustain = .19, release = .26,
        morph = .12, color = .62, echo = .12, detune = 0, fm = .44,
        lfo = .39, motion = .08,
        scene = {mesh = "kite.obj", tint = 0x8d879f, accent = 0xe2a576, spin = 0.53, tilt = 0.39, aspect = 0.97, satellites = 5}
    },

    -- Basses
    {
        name = "Subterrane", wave_a = "sine", wave_b = "sine",
        attack = .011, decay = .34, sustain = .82, release = .21,
        morph = .72, color = .31, echo = .12, detune = -1200, fm = .22,
        lfo = .43, motion = .12,
        scene = {mesh = "anvil.obj", tint = 0x6f789e, accent = 0xb489de, spin = 0.65, tilt = -0.25, aspect = 0.87, satellites = 3}
    },
    {
        name = "Pitch Black", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .29, sustain = .70, release = .14,
        morph = .42, color = .45, echo = .10, detune = -1200, fm = .15,
        lfo = .56, motion = .14,
        scene = {mesh = "cage.obj", tint = 0x586d85, accent = 0xf490bc, spin = -0.46, tilt = -0.13, aspect = 1.18, satellites = 4}
    },
    {
        name = "Magma Saw", wave_a = "saw", wave_b = "saw",
        attack = .003, decay = .38, sustain = .67, release = .20,
        morph = .41, color = .56, echo = .05, detune = 8, fm = .26,
        lfo = .62, motion = .12,
        scene = {mesh = "anvil.obj", tint = 0xdb575a, accent = 0xffb46c, spin = 0.64, tilt = -0.16, aspect = 1.05, satellites = 4}
    },
    {
        name = "Velvet Sub", wave_a = "triangle", wave_b = "sine",
        attack = .017, decay = .52, sustain = .83, release = .22,
        morph = .62, color = .35, echo = .09, detune = -1200, fm = .74,
        lfo = .25, motion = .05,
        scene = {mesh = "prism.obj", tint = 0x8b628f, accent = 0xe992be, spin = 0.31, tilt = -0.09, aspect = 1.0, satellites = 3}
    },
    {
        name = "Cobalt Punch", wave_a = "square", wave_b = "triangle",
        attack = .002, decay = .18, sustain = .42, release = .14,
        morph = .54, color = .65, echo = .12, detune = -1200, fm = .90,
        lfo = 1.15, motion = .15,
        scene = {mesh = "anvil.obj", tint = 0x4d88d2, accent = 0x72dbee, spin = 0.67, tilt = -0.32, aspect = 0.89, satellites = 2}
    },
    {
        name = "Silicon Funk", wave_a = "saw", wave_b = "square",
        attack = .004, decay = .26, sustain = .46, release = .18,
        morph = .28, color = .62, echo = .18, detune = -13, fm = 1.39,
        lfo = 2.22, motion = .24,
        scene = {mesh = "relay.obj", tint = 0x73ba84, accent = 0xd4ed85, spin = 0.72, tilt = 0.02, aspect = 0.93, satellites = 3}
    },
    {
        name = "Deepwater Bass", wave_a = "sine", wave_b = "triangle",
        attack = .047, decay = .60, sustain = .79, release = .37,
        morph = .55, color = .27, echo = .16, detune = -1200, fm = .83,
        lfo = .77, motion = .24,
        scene = {mesh = "arch.obj", tint = 0x458ca7, accent = 0x87e4ce, spin = 0.48, tilt = -0.35, aspect = 1.08, satellites = 2}
    },
    {
        name = "Neon Growl", wave_a = "saw", wave_b = "sine",
        attack = .010, decay = .45, sustain = .66, release = .26,
        morph = .37, color = .53, echo = .24, detune = -1200, fm = 3.35,
        lfo = 1.16, motion = .26,
        scene = {mesh = "cage.obj", tint = 0xb56bba, accent = 0xffa774, spin = 0.5, tilt = 0.13, aspect = 1.16, satellites = 4}
    },
    {
        name = "Iron Pulse", wave_a = "square", wave_b = "square",
        attack = .004, decay = .33, sustain = .65, release = .19,
        morph = .36, color = .43, echo = .07, detune = 20, fm = .05,
        lfo = .84, motion = .21,
        scene = {mesh = "anvil.obj", tint = 0x9e8190, accent = 0xea9277, spin = -0.51, tilt = -0.04, aspect = 1.13, satellites = 5}
    },
    {
        name = "Mercury Bass", wave_a = "triangle", wave_b = "saw",
        attack = .006, decay = .44, sustain = .71, release = .23,
        morph = .47, color = .54, echo = .16, detune = -1200, fm = .58,
        lfo = 1.68, motion = .20,
        scene = {mesh = "bell.obj", tint = 0x849bc6, accent = 0xc5baf0, spin = 0.65, tilt = -0.41, aspect = 0.98, satellites = 2}
    },
    {
        name = "Mothwing Sub", wave_a = "sine", wave_b = "noise",
        attack = .019, decay = .62, sustain = .75, release = .31,
        morph = .09, color = .37, echo = .10, detune = 0, fm = .25,
        lfo = .37, motion = .11,
        scene = {mesh = "fan.obj", tint = 0x567b8e, accent = 0xbcbd80, spin = 0.33, tilt = 0.24, aspect = 1.02, satellites = 2}
    },
    {
        name = "Terminal Bass", wave_a = "square", wave_b = "sine",
        attack = .004, decay = .22, sustain = .58, release = .20,
        morph = .46, color = .58, echo = .19, detune = 1200, fm = 1.55,
        lfo = 1.26, motion = .22,
        scene = {mesh = "cage.obj", tint = 0xbd6856, accent = 0xf7ca77, spin = 0.44, tilt = -0.3, aspect = 1.12, satellites = 3}
    },

    -- Leads
    {
        name = "Foxfire", wave_a = "saw", wave_b = "triangle",
        attack = .014, decay = .33, sustain = .79, release = .33,
        morph = .35, color = .73, echo = .32, detune = 14, fm = .50,
        lfo = 1.49, motion = .17,
        scene = {mesh = "kite.obj", tint = 0xef8455, accent = 0xffe58f, spin = -0.65, tilt = -0.27, aspect = 0.98, satellites = 1}
    },
    {
        name = "Phosphor Reed", wave_a = "square", wave_b = "sine",
        attack = .012, decay = .48, sustain = .76, release = .42,
        morph = .37, color = .65, echo = .34, detune = -8, fm = .65,
        lfo = 1.94, motion = .19,
        scene = {mesh = "spire.obj", tint = 0xa8d45f, accent = 0xddf583, spin = 0.5, tilt = 0.4, aspect = 0.97, satellites = 2}
    },
    {
        name = "Starling", wave_a = "triangle", wave_b = "sine",
        attack = .029, decay = .62, sustain = .83, release = .48,
        morph = .58, color = .81, echo = .39, detune = 1200, fm = .72,
        lfo = 2.25, motion = .15,
        scene = {mesh = "fan.obj", tint = 0x7baecb, accent = 0xeedae0, spin = 0.73, tilt = -0.14, aspect = 0.9, satellites = 4}
    },
    {
        name = "Solar Needle", wave_a = "saw", wave_b = "square",
        attack = .004, decay = .38, sustain = .68, release = .30,
        morph = .34, color = .82, echo = .30, detune = 11, fm = .31,
        lfo = 1.20, motion = .16,
        scene = {mesh = "spire.obj", tint = 0xf0ad4e, accent = 0xfff486, spin = 0.73, tilt = 0.01, aspect = 1.07, satellites = 2}
    },
    {
        name = "Tungsten Horn", wave_a = "saw", wave_b = "sine",
        attack = .083, decay = .43, sustain = .83, release = .31,
        morph = .25, color = .62, echo = .17, detune = 0, fm = .83,
        lfo = .89, motion = .17,
        scene = {mesh = "bell.obj", tint = 0xc1a37b, accent = 0xfff4b0, spin = 0.64, tilt = -0.29, aspect = 0.88, satellites = 1}
    },
    {
        name = "Copper Flute", wave_a = "sine", wave_b = "noise",
        attack = .051, decay = .66, sustain = .81, release = .39,
        morph = .07, color = .74, echo = .29, detune = 0, fm = .24,
        lfo = 2.34, motion = .13,
        scene = {mesh = "helix.obj", tint = 0xcd8558, accent = 0x98dacb, spin = -0.7, tilt = -0.26, aspect = 1.05, satellites = 1}
    },
    {
        name = "Bit Comet", wave_a = "square", wave_b = "square",
        attack = .005, decay = .37, sustain = .70, release = .22,
        morph = .33, color = .76, echo = .36, detune = 1200, fm = .15,
        lfo = 1.81, motion = .20,
        scene = {mesh = "kite.obj", tint = 0xd472a3, accent = 0xf6c970, spin = -0.53, tilt = -0.35, aspect = 0.87, satellites = 4}
    },
    {
        name = "Lacquer Lead", wave_a = "triangle", wave_b = "saw",
        attack = .023, decay = .41, sustain = .75, release = .33,
        morph = .54, color = .64, echo = .34, detune = -14, fm = 1.84,
        lfo = 1.24, motion = .18,
        scene = {mesh = "prism.obj", tint = 0xc95d74, accent = 0xf7ba90, spin = 0.41, tilt = 0.41, aspect = 0.89, satellites = 5}
    },
    {
        name = "Signal Pike", wave_a = "sine", wave_b = "square",
        attack = .008, decay = .30, sustain = .72, release = .31,
        morph = .28, color = .83, echo = .36, detune = 596, fm = 2.65,
        lfo = .83, motion = .18,
        scene = {mesh = "spire.obj", tint = 0x5ac8d4, accent = 0xc4f4df, spin = 0.51, tilt = -0.13, aspect = 0.9, satellites = 4}
    },
    {
        name = "Lantern Piper", wave_a = "triangle", wave_b = "square",
        attack = .049, decay = .56, sustain = .80, release = .45,
        morph = .36, color = .56, echo = .27, detune = 1200, fm = .52,
        lfo = 1.58, motion = .23,
        scene = {mesh = "petal.obj", tint = 0xe9b05c, accent = 0xf7dcab, spin = 0.57, tilt = -0.53, aspect = 1.03, satellites = 3}
    },
    {
        name = "Ribbon Rocket", wave_a = "saw", wave_b = "saw",
        attack = .007, decay = .36, sustain = .80, release = .49,
        morph = .47, color = .72, echo = .49, detune = -24, fm = .43,
        lfo = 2.07, motion = .28,
        scene = {mesh = "helix.obj", tint = 0xd179ca, accent = 0xf2a176, spin = -0.83, tilt = 0.45, aspect = 1.15, satellites = 5}
    },
    {
        name = "Fever Ray", wave_a = "sine", wave_b = "triangle",
        attack = .010, decay = .33, sustain = .75, release = .54,
        morph = .28, color = .71, echo = .45, detune = 890, fm = 3.92,
        lfo = 2.68, motion = .27,
        scene = {mesh = "fan.obj", tint = 0xd86483, accent = 0xf5d376, spin = 0.8, tilt = 0.34, aspect = 1.09, satellites = 5}
    },
    {
        name = "Gilt Resistor", wave_a = "square", wave_b = "triangle",
        attack = .021, decay = .42, sustain = .79, release = .28,
        morph = .61, color = .70, echo = .20, detune = 19, fm = .23,
        lfo = .72, motion = .32,
        scene = {mesh = "cage.obj", tint = 0xcfa353, accent = 0xbfe297, spin = 0.51, tilt = -0.04, aspect = 1.0, satellites = 1}
    },
    {
        name = "Star Scanner", wave_a = "saw", wave_b = "sine",
        attack = .026, decay = .62, sustain = .77, release = .68,
        morph = .48, color = .75, echo = .55, detune = 1200, fm = 1.22,
        lfo = 1.11, motion = .28,
        scene = {mesh = "relay.obj", tint = 0x5e9ddd, accent = 0xafebf9, spin = 0.48, tilt = -0.13, aspect = 0.97, satellites = 2}
    },

    -- Rhythmic and morphing
    {
        name = "Clock Coral", wave_a = "square", wave_b = "sine",
        attack = .015, decay = .71, sustain = .69, release = .57,
        morph = .51, color = .62, echo = .44, detune = 1200, fm = 1.30,
        lfo = 4.00, motion = .45,
        scene = {mesh = "relay.obj", tint = 0xe18181, accent = 0xffc495, spin = 0.93, tilt = -0.2, aspect = 1.03, satellites = 2}
    },
    {
        name = "Lattice Pump", wave_a = "saw", wave_b = "triangle",
        attack = .028, decay = .90, sustain = .72, release = .84,
        morph = .54, color = .52, echo = .37, detune = -19, fm = .77,
        lfo = 3.22, motion = .51,
        scene = {mesh = "cage.obj", tint = 0x59b4ab, accent = 0xc2eeb9, spin = -0.7, tilt = 0.05, aspect = 1.13, satellites = 4}
    },
    {
        name = "Tern Signal", wave_a = "sine", wave_b = "noise",
        attack = .018, decay = .62, sustain = .76, release = .65,
        morph = .25, color = .70, echo = .42, detune = 0, fm = .88,
        lfo = 6.00, motion = .28,
        scene = {mesh = "kite.obj", tint = 0x8facb7, accent = 0xf6d09d, spin = -1.1, tilt = -0.26, aspect = 1.16, satellites = 1}
    },
    {
        name = "Neon Weave", wave_a = "sine", wave_b = "saw",
        attack = .038, decay = 1.03, sustain = .71, release = 1.07,
        morph = .49, color = .62, echo = .59, detune = 27, fm = 1.66,
        lfo = 2.68, motion = .57,
        scene = {mesh = "helix.obj", tint = 0x93d66d, accent = 0xf3adec, spin = -0.85, tilt = 0.14, aspect = 0.93, satellites = 4}
    },
    {
        name = "Cinder Rotor", wave_a = "saw", wave_b = "square",
        attack = .052, decay = .76, sustain = .77, release = .92,
        morph = .39, color = .65, echo = .43, detune = 1200, fm = .35,
        lfo = 7.00, motion = .39,
        scene = {mesh = "fan.obj", tint = 0xcb6e64, accent = 0xffb282, spin = -1.2, tilt = 0.15, aspect = 0.84, satellites = 3}
    },
    {
        name = "Phantom Rhythm", wave_a = "triangle", wave_b = "noise",
        attack = .088, decay = 1.04, sustain = .73, release = 1.19,
        morph = .20, color = .60, echo = .61, detune = 0, fm = .42,
        lfo = 3.48, motion = .25,
        scene = {mesh = "relay.obj", tint = 0x8b79d0, accent = 0xa4e6dd, spin = -0.9, tilt = -0.4, aspect = 0.92, satellites = 5}
    },
    {
        name = "Darkroom Sweep", wave_a = "sine", wave_b = "square",
        attack = .32, decay = 1.28, sustain = .83, release = 1.87,
        morph = .42, color = .41, echo = .54, detune = -12, fm = 2.43,
        lfo = 1.43, motion = .61,
        scene = {mesh = "arch.obj", tint = 0xa063a3, accent = 0xfc8fae, spin = 0.5, tilt = 0.39, aspect = 0.94, satellites = 2}
    },
    {
        name = "Cascade Relay", wave_a = "triangle", wave_b = "saw",
        attack = .110, decay = .91, sustain = .76, release = 1.23,
        morph = .57, color = .55, echo = .64, detune = 1200, fm = .92,
        lfo = 4.64, motion = .52,
        scene = {mesh = "relay.obj", tint = 0x67a9e2, accent = 0xb7f5ec, spin = 1.08, tilt = -0.45, aspect = 1.06, satellites = 1}
    },
    {
        name = "Transistor Tide", wave_a = "square", wave_b = "triangle",
        attack = .074, decay = 1.15, sustain = .68, release = .81,
        morph = .56, color = .53, echo = .29, detune = 17, fm = .26,
        lfo = 5.24, motion = .55,
        scene = {mesh = "cage.obj", tint = 0x6ab29a, accent = 0xe7db91, spin = 1.17, tilt = -0.52, aspect = 1.02, satellites = 1}
    },
    {
        name = "Aurora Gate", wave_a = "sine", wave_b = "triangle",
        attack = .18, decay = 1.76, sustain = .83, release = 1.73,
        morph = .45, color = .64, echo = .62, detune = 1200, fm = 2.01,
        lfo = 2.12, motion = .48,
        scene = {mesh = "arch.obj", tint = 0x79cbb8, accent = 0xd2a6f5, spin = -0.67, tilt = 0.24, aspect = 1.04, satellites = 2}
    },

    -- Drums and percussion
    {
        name = "Thump Coil", wave_a = "sine", wave_b = "triangle",
        attack = .001, decay = .15, sustain = .01, release = .08,
        morph = .32, color = .32, echo = .06, detune = -1200, fm = 1.05,
        lfo = .30, motion = .04,
        scene = {mesh = "anvil.obj", tint = 0xeb9268, accent = 0xffd6a0, spin = -0.51, tilt = -0.2, aspect = 1.03, satellites = 5}
    },
    {
        name = "Dust Snare", wave_a = "triangle", wave_b = "noise",
        attack = .002, decay = .15, sustain = .03, release = .11,
        morph = .79, color = .69, echo = .17, detune = 0, fm = .40,
        lfo = 3.5, motion = .13,
        scene = {mesh = "fan.obj", tint = 0xc4a888, accent = 0xf2dbaf, spin = 0.97, tilt = -0.32, aspect = 1.17, satellites = 1}
    },
    {
        name = "Circuit Hat", wave_a = "square", wave_b = "noise",
        attack = .001, decay = .05, sustain = .01, release = .04,
        morph = .92, color = .89, echo = .10, detune = 1100, fm = .20,
        lfo = 3.0, motion = .07,
        scene = {mesh = "cage.obj", tint = 0x9cc8ba, accent = 0xefecad, spin = 0.74, tilt = 0.17, aspect = 0.92, satellites = 3}
    },
    {
        name = "Glacial Rim", wave_a = "square", wave_b = "sine",
        attack = .001, decay = .08, sustain = .01, release = .15,
        morph = .23, color = .77, echo = .27, detune = 851, fm = 2.38,
        lfo = .60, motion = .14,
        scene = {mesh = "relay.obj", tint = 0x7dbce8, accent = 0xe3f9ff, spin = -0.43, tilt = -0.47, aspect = 0.84, satellites = 4}
    },
    {
        name = "Soft Frame Drum", wave_a = "sine", wave_b = "noise",
        attack = .003, decay = .23, sustain = .01, release = .15,
        morph = .25, color = .49, echo = .18, detune = -1200, fm = 1.10,
        lfo = .20, motion = .04,
        scene = {mesh = "bell.obj", tint = 0xc6a47e, accent = 0xedbca2, spin = 0.55, tilt = -0.14, aspect = 1.13, satellites = 4}
    },
    {
        name = "Foundry Tom", wave_a = "sine", wave_b = "triangle",
        attack = .001, decay = .28, sustain = .02, release = .16,
        morph = .21, color = .51, echo = .22, detune = 484, fm = 2.10,
        lfo = .77, motion = .10,
        scene = {mesh = "anvil.obj", tint = 0xca7873, accent = 0xf5a976, spin = -0.44, tilt = -0.5, aspect = 1.11, satellites = 4}
    },
    {
        name = "Tin Shaker", wave_a = "sine", wave_b = "noise",
        attack = .003, decay = .08, sustain = .04, release = .09,
        morph = .87, color = .82, echo = .16, detune = 0, fm = .63,
        lfo = 14, motion = .10,
        scene = {mesh = "petal.obj", tint = 0xa4b6bf, accent = 0xece2aa, spin = -1.47, tilt = -0.4, aspect = 1.16, satellites = 2}
    },
    {
        name = "Ring Anvil", wave_a = "sine", wave_b = "square",
        attack = .001, decay = .45, sustain = .01, release = .52,
        morph = .23, color = .81, echo = .47, detune = 948, fm = 5.11,
        lfo = .48, motion = .07,
        scene = {mesh = "bell.obj", tint = 0xac8ab5, accent = 0xf5b7a0, spin = 0.52, tilt = 0.11, aspect = 0.96, satellites = 5}
    },

    -- Drones and sound effects
    {
        name = "Aether Vents", wave_a = "sine", wave_b = "noise",
        attack = .72, decay = 1.70, sustain = .88, release = 3.6,
        morph = .32, color = .62, echo = .60, detune = 0, fm = 1.43,
        lfo = .52, motion = .37,
        scene = {mesh = "helix.obj", tint = 0x7d91d7, accent = 0xb5c4f0, spin = -0.35, tilt = 0.25, aspect = 0.9, satellites = 1}
    },
    {
        name = "Orbital Beacon", wave_a = "sine", wave_b = "square",
        attack = .095, decay = .64, sustain = .68, release = 1.08,
        morph = .18, color = .67, echo = .57, detune = 802, fm = 4.38,
        lfo = 5.30, motion = .42,
        scene = {mesh = "relay.obj", tint = 0xc49f57, accent = 0xf7de8d, spin = -0.9, tilt = -0.32, aspect = 1.18, satellites = 2}
    },
    {
        name = "Hearth Static", wave_a = "triangle", wave_b = "noise",
        attack = .90, decay = 2.14, sustain = .83, release = 3.37,
        morph = .37, color = .71, echo = .49, detune = 0, fm = .15,
        lfo = .27, motion = .36,
        scene = {mesh = "petal.obj", tint = 0xdb7968, accent = 0xf7b3a0, spin = -0.35, tilt = 0.34, aspect = 0.99, satellites = 1}
    },
    {
        name = "Magnetic Rain", wave_a = "square", wave_b = "sine",
        attack = .35, decay = 1.57, sustain = .67, release = 2.60,
        morph = .55, color = .56, echo = .64, detune = 976, fm = 5.49,
        lfo = 3.38, motion = .53, sample = .22, sample_file = "canopy.wav",
        scene = {mesh = "cage.obj", tint = 0x8292cd, accent = 0xc7d9f6, spin = -0.8, tilt = 0.31, aspect = 1.0, satellites = 5}
    },
    {
        name = "Void Siren", wave_a = "sine", wave_b = "saw",
        attack = 1.10, decay = 1.42, sustain = .79, release = 4.41,
        morph = .43, color = .47, echo = .68, detune = 638, fm = 4.70,
        lfo = .64, motion = .36,
        scene = {mesh = "arch.obj", tint = 0x635fae, accent = 0xc08ae3, spin = 0.37, tilt = -0.04, aspect = 1.11, satellites = 3}
    },
    {
        name = "Doppler Hull", wave_a = "square", wave_b = "triangle",
        attack = .69, decay = 1.52, sustain = .84, release = 2.86,
        morph = .49, color = .43, echo = .56, detune = -608, fm = 3.08,
        lfo = .95, motion = .44,
        scene = {mesh = "kite.obj", tint = 0x5e99bd, accent = 0xabcbf0, spin = 0.63, tilt = 0.43, aspect = 0.84, satellites = 5}
    },
    {
        name = "Blacklight", wave_a = "sine", wave_b = "triangle",
        attack = .24, decay = 1.11, sustain = .79, release = 2.75,
        morph = .42, color = .72, echo = .54, detune = 1200, fm = 5.36,
        lfo = .54, motion = .29,
        scene = {mesh = "helix.obj", tint = 0xbc70ee, accent = 0xf6a4ef, spin = 0.62, tilt = 0.4, aspect = 0.99, satellites = 3}
    },
}
