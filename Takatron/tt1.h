/* PSX Takatron Stuff */

#define ONE 0x1000
#define OTHER 0
#define SPRITE 1

/* temporarily unused
#define SAMP 2
#define MIDI 3
*/
#define SAMP 0
#define MIDI 0


#define NUM_DATA 163
#define NUM_SPRITE 128
#define NUM_MIDI 3
#define NUM_SAMP 28

struct {
	int type,number;
       } data[NUM_DATA];

struct {
	int x,y, w,h, rotation;
       } sprite[NUM_SPRITE];

struct {
	void *address;
       } sample[NUM_SAMP];

struct {
        void *address;
       } midi[NUM_MIDI];


/* PSX Takatron Data Header */

#define AllClear                         0        /* SAMP */
#define BePrepared                       1        /* MIDI */
#define BOOM                             2        /* SAMP */
#define BraveLion                        3        /* SAMP */
#define Buzzard1                         4        /* BMP  */
#define Buzzard2                         5        /* BMP  */
#define Buzzard3                         6        /* BMP  */
#define BuzzardR1                        7        /* BMP  */
#define BuzzardR2                        8        /* BMP  */
#define BuzzardR3                        9        /* BMP  */
#define Challenge                        10       /* SAMP */
#define CircleLife                       11       /* SAMP */
#define ComesBack                        12       /* SAMP */
#define essdown1                         13       /* BMP  */
#define essdown2                         14       /* BMP  */
#define essdown3                         15       /* BMP  */
#define essleft1                         16       /* BMP  */
#define essleft2                         17       /* BMP  */
#define essleft3                         18       /* BMP  */
#define essright1                        19       /* BMP  */
#define essright2                        20       /* BMP  */
#define essright3                        21       /* BMP  */
#define essup1                           22       /* BMP  */
#define essup2                           23       /* BMP  */
#define essup3                           24       /* BMP  */
#define FEELLOVE                         25       /* SAMP */
#define FinalBoss                        26       /* MIDI */
#define FScarBossA                       27       /* BMP  */
#define FScarBossB                       28       /* BMP  */
#define Get_Lioness                      29       /* SAMP */
#define GetOut                           30       /* SAMP */
#define GO                               31       /* SAMP */
#define Got_1up                          32       /* SAMP */
#define Got_Continue                     33       /* SAMP */
#define GROWL                            34       /* SAMP */
#define Growldown                        35       /* BMP  */
#define Growlleft                        36       /* BMP  */
#define Growlright                       37       /* BMP  */
#define Growlup                          38       /* BMP  */
#define HesAlive                         39       /* SAMP */
#define HIT                              40       /* SAMP */
#define Humiliating                      41       /* SAMP */
#define Hyenadown1                       42       /* BMP  */
#define Hyenadown2                       43       /* BMP  */
#define Hyenaleft1                       44       /* BMP  */
#define Hyenaleft2                       45       /* BMP  */
#define Hyenaright1                      46       /* BMP  */
#define Hyenaright2                      47       /* BMP  */
#define Hyenaup1                         48       /* BMP  */
#define Hyenaup2                         49       /* BMP  */
#define Jag1                             50       /* BMP  */
#define Jag2                             51       /* BMP  */
#define Jag3                             52       /* BMP  */
#define Jag4                             53       /* BMP  */
#define JagL1                            54       /* BMP  */
#define JagL2                            55       /* BMP  */
#define JagL3                            56       /* BMP  */
#define JagL4                            57       /* BMP  */
#define Kitu1                            58       /* BMP  */
#define Kitu2                            59       /* BMP  */
#define Kitu3                            60       /* BMP  */
#define LiveKing                         61       /* SAMP */
#define Love_Heart                       62       /* BMP  */
#define LOVEMIDI                         63       /* MIDI */
#define MyFont                           64       /* FONT */
#define MyKingdom                        65       /* SAMP */
#define NiceOne                          66       /* SAMP */
#define NotFair                          67       /* SAMP */
#define obstacle1                        68       /* BMP  */
#define obstacle2                        69       /* BMP  */
#define palette                          70       /* PAL  */
#define PWR_1up                          71       /* BMP  */
#define PWR_Continue                     72       /* BMP  */
#define PWR_Love                         73       /* BMP  */
#define PWR_Roar                         74       /* BMP  */
#define PWR_Warp                         75       /* BMP  */
#define QuiverFear                       76       /* SAMP */
#define Rhinodown1                       77       /* BMP  */
#define Rhinodown2                       78       /* BMP  */
#define Rhinoleft1                       79       /* BMP  */
#define Rhinoleft2                       80       /* BMP  */
#define Rhinoright1                      81       /* BMP  */
#define Rhinoright2                      82       /* BMP  */
#define Rhinoup1                         83       /* BMP  */
#define Rhinoup2                         84       /* BMP  */
#define ROAR                             85       /* SAMP */
#define Run                              86       /* SAMP */
#define RunAway                          87       /* SAMP */
#define ScarAJump1                       88       /* BMP  */
#define ScarAJump2                       89       /* BMP  */
#define ScarALand                        90       /* BMP  */
#define ScarASlash1                      91       /* BMP  */
#define ScarASlash2                      92       /* BMP  */
#define ScarASlash3                      93       /* BMP  */
#define Scardown1                        94       /* BMP  */
#define Scardown2                        95       /* BMP  */
#define Scardown3                        96       /* BMP  */
#define Scarleft1                        97       /* BMP  */
#define Scarleft2                        98       /* BMP  */
#define Scarleft3                        99       /* BMP  */
#define Scarright1                       100      /* BMP  */
#define Scarright2                       101      /* BMP  */
#define Scarright3                       102      /* BMP  */
#define Scarup1                          103      /* BMP  */
#define Scarup2                          104      /* BMP  */
#define Scarup3                          105      /* BMP  */
#define ShotD1                           106      /* BMP  */
#define ShotD2                           107      /* BMP  */
#define ShotLR                           108      /* BMP  */
#define ShotUD                           109      /* BMP  */
#define simadown                         110      /* BMP  */
#define simadown1                        111      /* BMP  */
#define simadown2                        112      /* BMP  */
#define simadown3                        113      /* BMP  */
#define simadown4                        114      /* BMP  */
#define simaleft                         115      /* BMP  */
#define simaleft1                        116      /* BMP  */
#define simaleft2                        117      /* BMP  */
#define simaleft3                        118      /* BMP  */
#define simaleft4                        119      /* BMP  */
#define simaright                        120      /* BMP  */
#define simaright1                       121      /* BMP  */
#define simaright2                       122      /* BMP  */
#define simaright3                       123      /* BMP  */
#define simaright4                       124      /* BMP  */
#define simaup                           125      /* BMP  */
#define simaup1                          126      /* BMP  */
#define simaup2                          127      /* BMP  */
#define simaup3                          128      /* BMP  */
#define simaup4                          129      /* BMP  */
#define SIMBADIE                         130      /* SAMP */
#define simdown                          131      /* BMP  */
#define simdown2                         132      /* BMP  */
#define simdown3                         133      /* BMP  */
#define simdown4                         134      /* BMP  */
#define simleft                          135      /* BMP  */
#define simleft2                         136      /* BMP  */
#define simleft3                         137      /* BMP  */
#define simleft4                         138      /* BMP  */
#define simright                         139      /* BMP  */
#define simright2                        140      /* BMP  */
#define simright3                        141      /* BMP  */
#define simright4                        142      /* BMP  */
#define simup                            143      /* BMP  */
#define simup2                           144      /* BMP  */
#define simup3                           145      /* BMP  */
#define simup4                           146      /* BMP  */
#define Small_Font                       147      /* FONT */
#define Start_Buzzard                    148      /* SAMP */
#define Surprised                        149      /* SAMP */
#define Times                            150      /* FONT */
#define Title1                           151      /* BMP  */
#define Title2                           152      /* BMP  */
#define Title3                           153      /* BMP  */
#define Title4                           154      /* BMP  */
#define Title5                           155      /* BMP  */
#define Title6                           156      /* BMP  */
#define Title7                           157      /* BMP  */
#define Title8                           158      /* BMP  */
#define TitleDate                        159      /* BMP  */
#define WileyA                           160      /* BMP  */
#define WileyB                           161      /* BMP  */
#define WOW                              162      /* SAMP */

